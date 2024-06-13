#include "RenderManager.h"

#include "Core/ShaderManager.h"
#include "DefaultTextures.h"
#include "GameWindow.h"

#include "Framework/Engine.h"
#include "Editer/EditerManager.h"

static bool useGrayscale = false;

RenderManager* RenderManager::GetInstance() {
    static RenderManager instance;
    return &instance;
}

void RenderManager::Initialize() {
    graphics_ = Graphics::GetInstance();

    auto shaderManager = ShaderManager::GetInstance();
    shaderManager->Initialize();
    shaderManager->SetDirectory(std::filesystem::current_path() / SHADER_DIRECTORY);

    auto window = GameWindow::GetInstance();
    swapChain_.Create(window->GetHWND());

    DefaultTexture::Initialize();

    auto& swapChainBuffer = swapChain_.GetColorBuffer(0);
    finalImageBuffer_.Create(L"FinalImageBuffer", swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), swapChainBuffer.GetFormat(), swapChainBuffer.IsSRGB());

    skinningManager_.Initialize();
    geometryRenderingPass_.Initialize(swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight());
    lightingRenderingPass_.Initialize(swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight());
    skybox_.Initialize(lightingRenderingPass_.GetResult().GetRTVFormat(), geometryRenderingPass_.GetDepth().GetFormat());
    lineDrawer_.Initialize(lightingRenderingPass_.GetResult().GetRTVFormat());

    //bloom_.Initialize(&lightingRenderingPass_.GetResult());
    fxaa_.Initialize(&lightingRenderingPass_.GetResult());
    postEffect_.Initialize(finalImageBuffer_);

    //    modelRenderer.Initialize(mainColorBuffer_, mainDepthBuffer_);
    transition_.Initialize();
    raytracingRenderer_.Create(swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight());

    //raymarchingRenderer_.Create(mainColorBuffer_.GetWidth(), mainColorBuffer_.GetHeight());

    //computeShaderTester_.Initialize(1024, 1024);
    //commandContext_.Start(D3D12_COMMAND_LIST_TYPE_DIRECT);
    //computeShaderTester_.Dispatch(commandContext_);
    //commandContext_.Finish(true);

    timer_.Initialize();

    frameCount_ = 0;
}

void RenderManager::Finalize() {
    DefaultTexture::Finalize();
}

void RenderManager::Render() {

    uint32_t targetSwapChainBufferIndex = (swapChain_.GetCurrentBackBufferIndex() + 1) % SwapChain::kNumBuffers;

    auto camera = camera_.lock();
    auto sunLight = sunLight_.lock();

    commandContext_.Start(D3D12_COMMAND_LIST_TYPE_DIRECT);

    skinningManager_.Update(commandContext_);

    if (camera && sunLight) {
        // 影、スペキュラ
        modelSorter_.Sort(*camera);;
        //    raytracingRenderer_.Render(commandContext_, *camera, *sunLight);
        geometryRenderingPass_.Render(commandContext_, *camera, modelSorter_);
        lightingRenderingPass_.Render(commandContext_, geometryRenderingPass_, *camera, *sunLight);

        commandContext_.TransitionResource(lightingRenderingPass_.GetResult(), D3D12_RESOURCE_STATE_RENDER_TARGET);
        commandContext_.TransitionResource(geometryRenderingPass_.GetDepth(), D3D12_RESOURCE_STATE_DEPTH_READ);
        commandContext_.SetViewportAndScissorRect(0, 0, lightingRenderingPass_.GetResult().GetWidth(), lightingRenderingPass_.GetResult().GetHeight());
        commandContext_.SetRenderTarget(lightingRenderingPass_.GetResult().GetRTV(), geometryRenderingPass_.GetDepth().GetDSV());
        skybox_.SetWorldMatrix(Matrix4x4::MakeAffineTransform({ 1.0f, 1.0f, 1.0f }, Quaternion::identity, camera->GetPosition()));
        skybox_.Render(commandContext_, *camera);

        commandContext_.SetRenderTarget(lightingRenderingPass_.GetResult().GetRTV());
        commandContext_.SetViewportAndScissorRect(0, 0, lightingRenderingPass_.GetResult().GetWidth(), lightingRenderingPass_.GetResult().GetHeight());
        lineDrawer_.Render(commandContext_, *camera);
    }

    //bloom_.Render(commandContext_);
    fxaa_.Render(commandContext_);

    commandContext_.TransitionResource(finalImageBuffer_, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext_.SetRenderTarget(finalImageBuffer_.GetRTV());
    commandContext_.SetViewportAndScissorRect(0, 0, finalImageBuffer_.GetWidth(), finalImageBuffer_.GetHeight());

    postEffect_.Render(commandContext_, fxaa_.GetResult());

    auto& swapChainBuffer = swapChain_.GetColorBuffer(targetSwapChainBufferIndex);
    commandContext_.CopyBuffer(swapChainBuffer, finalImageBuffer_);


#ifdef ENABLE_IMGUI
    // スワップチェーンに描画
    commandContext_.TransitionResource(swapChainBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext_.FlushResourceBarriers();
    commandContext_.SetRenderTarget(swapChainBuffer.GetRTV());
    //commandContext_.ClearColor(swapChainBuffer);
    commandContext_.SetViewportAndScissorRect(0, 0, swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight());

   /* commandContext_.TransitionResource(finalImageBuffer_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandContext_.FlushResourceBarriers();
    ImGui::Begin("Game", 0, ImGuiWindowFlags_NoScrollbar);
    ImVec2 windowSize = ImGui::GetWindowSize();
    ImTextureID image = reinterpret_cast<ImTextureID>(finalImageBuffer_.GetSRV().GetGPU().ptr);
    ImVec2 imageSize = CalcAspectFitSize(windowSize, { (float)finalImageBuffer_.GetWidth(), (float)finalImageBuffer_.GetHeight() });
    ImVec2 imageOffset = { (windowSize.x - imageSize.x) * 0.5f, (windowSize.y - imageSize.y) * 0.5f};
    ImGui::SetCursorPos(imageOffset);
    ImGui::Image(image, imageSize);
    ImGui::End();
    
    ImGui::Begin("Profile");
    auto io = ImGui::GetIO();
    ImGui::Text("Framerate : %f", io.Framerate);
    ImGui::Text("FrameCount : %d", frameCount_);
    postEffect_.DrawImGui("PostEffect");
    
    ImGui::End();*/

    Engine::GetEditerManager()->RenderToColorBuffer(commandContext_);
#endif // ENABLE_IMGUI

    commandContext_.TransitionResource(swapChainBuffer, D3D12_RESOURCE_STATE_PRESENT);

    // コマンドリスト完成(クローズ)
    commandContext_.Close();

    // バックバッファをフリップ
    swapChain_.Present();
    frameCount_++;
    // シグナルを発行し待つ
    auto& commandQueue = graphics_->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
    commandQueue.WaitForIdle();

    commandContext_.Finish(false);

    graphics_->GetReleasedObjectTracker().FrameIncrementForRelease();

    timer_.KeepFrameRate(60);

}
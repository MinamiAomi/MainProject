#pragma once

#include "Core/RootSignature.h"
#include "Core/PipelineState.h"
#include "Core/TextureResource.h"

class CommandContext;
class ColorBuffer;
class DepthBuffer;
class Camera;
class DirectionalLight;

class ModelRenderer {
public:
    enum RootIndex {
        Scene = 0,
        Instance,
        Material,
        Texture,
        Sampler,

        NumParameters
    };

    void Initialize(const ColorBuffer& colorBuffer, const DepthBuffer& depthBuffer);
    void Render(CommandContext& commandContext, const Camera& camera, const DirectionalLight& sunLight);

private:
    void InitializeRootSignature();
    void InitializePipelineState(DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat);

    RootSignature rootSignature_;
    PipelineState pipelineState_;
};
#pragma once

#include <map>
#include <string>

#include "../Core/ColorBuffer.h"
#include "../Core/RootSignature.h"
#include "../../Math/Camera.h"
#include "../Core/TextureResource.h"

#include "StateObject.h"
#include "TLAS.h"
#include "ShaderTable.h"

class CommandContext;
class ModelSorter;

class TestRTRenderer {
public:
    void Create(uint32_t width, uint32_t height);

    void Render(CommandContext& commandContext, const Camera& camera, const ModelSorter& modelSorter);

    void SetSkybox(const std::shared_ptr<TextureResource>& texture) { skyboxTexture_ = texture; }
    ColorBuffer& GetResult() { return colorBuffer_; }

private:
    void CreateRootSignature();
    void CreateStateObject();
    void CreateShaderTables();
    void BuildScene(CommandContext& commandContext, const ModelSorter& modelSorter);

    StateObject stateObject_;
    RootSignature globalRootSignature_;
    RootSignature hitGroupLocalRootSignature_;

    TLAS tlas_;

    ShaderTable rayGenerationShaderTable_;
    ShaderTable hitGroupShaderTable_;
    ShaderTable missShaderTable_;

    std::map<std::wstring, void*> identifierMap_;
    std::shared_ptr<TextureResource> skyboxTexture_;
    ColorBuffer colorBuffer_;
};
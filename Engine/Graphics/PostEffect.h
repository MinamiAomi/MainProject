#pragma once

#include "Core/RootSignature.h"
#include "Core/PipelineState.h"
#include "Core/ColorBuffer.h"
#include "Math/MathUtils.h"

class CommandContext;

class PostEffect {
public:
    struct Grayscale {
        Vector3 color = Vector3::one;
        bool isActive = false;
    };
    struct Vignette {
        float intensity = 16.0f;
        float power = 0.8f;
        bool isActive = false;
    };

    void Initialize(const ColorBuffer& target);
    void Render(CommandContext& commandContext, ColorBuffer& texture);

    void DrawImGui(const char* label);

private:
    RootSignature rootSignature_;
    PipelineState pipelineState_;
    Grayscale grayscale_;
    Vignette vignette_;
};
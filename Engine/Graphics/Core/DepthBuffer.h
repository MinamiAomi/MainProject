#pragma once

#include "PixelBuffer.h"

#include "DescriptorHandle.h"

namespace LIEngine {

    class DepthBuffer : public PixelBuffer {
    public:
        void Create(const std::wstring& name, uint32_t width, uint32_t height, DXGI_FORMAT format);
        void CreateArray(const std::wstring& name, uint32_t width, uint32_t height, uint32_t arraySize, DXGI_FORMAT format);

        void SetClearValue(float clearValue) { clearValue_ = clearValue; }
        float GetClearValue() const { return clearValue_; }

        const DescriptorHandle& GetSRV() const { return srvHandle_; }
        const DescriptorHandle& GetDSV() const { return dsvHandle_; }

    private:
        void CreateViews();

        float clearValue_ = 1.0f;
        DescriptorHandle dsvHandle_;
        DescriptorHandle srvHandle_;
    };

}
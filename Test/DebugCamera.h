#pragma once
#include "Collision/GameObject.h"

#include <memory>

#include "Math/MathUtils.h"
#include "Math/Transform.h"
#include "Math/Camera.h"

class DebugCamera :
    public GameObject {
public:
    void Initialize();
    void Update();

    const std::shared_ptr<Camera>& GetCamera() const { return camera_; }

private:
    std::shared_ptr<Camera> camera_;
    Vector3 eulerAngle_;
};
#pragma once
#include "Collision/GameObject.h"

#include <memory>

#include "Graphics/Model.h"

class FollowCamera;

class Player :
    public GameObject {
public:
    void Initialize();
    void Update();

    void SetFollowCamera(const std::shared_ptr<FollowCamera>& followCamera) { followCamera_ = followCamera; }

private:
    
    std::unique_ptr<ModelInstance> model_;
    std::weak_ptr<FollowCamera> followCamera_;
};
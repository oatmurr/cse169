#pragma once

#include "Skin.h"
#include "Pose.h"

class Rig {

private:
    Skeleton* skeleton;
    Skin* skin;

public:
    Rig();
    Rig(Skeleton* skeleton, Skin* skin = nullptr);
    ~Rig();

    Skeleton* GetSkeleton();

    void ApplyPose(Pose& pose);
};
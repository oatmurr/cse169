#pragma once

#include "Skin.h"
#include "Pose.h"

class Rig {

private:
    Skeleton* skeleton;
    Skin* skin;

public:
    Rig(char* skelFilename, char* skinFilename = nullptr);
    ~Rig();

    void ApplyPose(Pose& pose);
};
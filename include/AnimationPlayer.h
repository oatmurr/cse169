#pragma once

#include "AnimationClip.h"
#include "Rig.h"

// pointer to animation, pointer to skeleton, keeps track of time, accesses animation data & poses the skeleton.
class AnimationPlayer
{    
private:
    float time;
    AnimationClip *clip;
    Pose currentPose;
    Rig *rig;

public:
    AnimationPlayer();
    ~AnimationPlayer();

    // getters and setters
    void SetClip(AnimationClip* clip);
    const Pose& GetCurrentPose();
    void SetRig(Rig* rig);

    // update animation (call each frame)
    void Update(float deltaTime);
};

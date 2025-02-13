#include "AnimationPlayer.h"

AnimationPlayer::AnimationPlayer() {
    time = 0.0f;
    clip = nullptr;
}

AnimationPlayer::~AnimationPlayer() {
    delete clip;
    delete rig;
}

void AnimationPlayer::SetClip(AnimationClip* clip) {
    this->clip = clip;
}

const Pose& AnimationPlayer::GetCurrentPose() {
    return currentPose;
}

void AnimationPlayer::SetRig(Rig* rig) {
    this->rig = rig;
}

void AnimationPlayer::Update(float deltaTime) {
    
    time += deltaTime;

    // clamp time to range
    if (time < clip->GetStartTime()) {
        time = clip->GetStartTime();
    }

    // loop animation
    if (time > clip->GetEndTime()) {
        time = 0.0f;
    }

    // evaluate animation at current time
    clip->Evaluate(time, currentPose);

    // apply pose to rig
    rig->ApplyPose(currentPose);
}
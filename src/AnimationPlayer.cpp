#include "AnimationPlayer.h"

AnimationPlayer::AnimationPlayer() {
    // time = 0.0f;
    time = -5.0f;
    clip = nullptr;
    rig = nullptr;
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

    // initialise pose with skeleton
    currentPose = Pose(rig->GetSkeleton());
}

void AnimationPlayer::Update(float deltaTime) {
    
    if (!clip) {
        printf("AnimationPlayer::Update - no clip\n");
        return;
    }

    if (!rig) {
        printf("AnimationPlayer::Update - no rig\n");
        return;
    }
    
    time += deltaTime;
    // printf("AnimationPlayer::Update - time: %f\n", time);

    // bounds check before pose evaluation
    if (currentPose.GetNumDOFs() != clip->GetChannels().size()) {
        printf("AnimationPlayer::Update - pose size mismatch\n");
        currentPose.Resize(clip->GetChannels().size());
    }

    // clamp time to range
    // if (time < clip->GetStartTime()) {
    //     time = clip->GetStartTime();
    // }

    // loop animation
    // if (time > clip->GetEndTime()) {
    //     time = 0.0f;
    // }

    // evaluate animation at current time
    clip->Evaluate(time, currentPose);
    // printf("AnimationPlayer::Update - current pose DOFs: ");
    // for(int i = 0; i < currentPose.GetNumDOFs(); i++) {
    //     printf("%f ", currentPose.GetDOF(i).GetValue());
    // }

    // apply pose to rig
    rig->ApplyPose(currentPose);
}
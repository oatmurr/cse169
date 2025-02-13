#pragma once

#include "Channel.h"
#include "Pose.h"

class AnimationClip {

private:
    float start, end;
    std::vector<Channel> channels;

public:
    AnimationClip();

    // getters and setters
    float GetStartTime() const;
    float GetEndTime() const;
    
    // evaluate animation at a given time and update pose
    void Evaluate(float time, Pose& pose);

    // load animation from .anim file
    bool Load(const char *filename);
};
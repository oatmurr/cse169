#include "Channel.h"
#include "Pose.h"

class AnimationClip {

private:
    float startTime, endTime;
    std::vector<Channel> channels;

public:
    AnimationClip();
    ~AnimationClip();
    
    // evaluate animation at a given time and update pose
    void Evaluate(float time, Pose& pose);

    // load animation from .anim file
    bool Load(const char *filename);
};
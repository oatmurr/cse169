#include "AnimationClip.h"
#include "Skeleton.h"

// pointer to animation, pointer to skeleton, keeps track of time, accesses animation data & poses the skeleton.
class AnimationPlayer {
    
private:
    float time;
    AnimationClip *clip;
    Pose currentPose;

public:
    AnimationPlayer();
    ~AnimationPlayer();

    // getters and setters
    void SetClip(AnimationClip* c) {clip = c;}
    const Pose& GetCurrentPose() {return currentPose;}

    // update animation (call each frame)
    void Update(float deltaTime);
};

#include "Skeleton.h"
#include "Skin.h"
#include "Pose.h"

// simple container for a skeleton, skin, and morphs
class Rig {

private:
    Skeleton skeleton;
    Skin skin;
    // std::vector<Morph> morphs; ???

public:
    Rig();
    ~Rig();

    void ApplyPose(const Pose& pose);
};
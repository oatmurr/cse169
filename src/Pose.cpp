#include "Pose.h"

Pose::Pose() {
    DOFs.resize(0);
}

Pose::Pose(Skeleton* skeleton) {

    // should be equal to the number of channels
    DOFs.resize(skeleton->GetJointList().size() * 3);

    for (int i = 0; i < DOFs.size(); i++) {
        DOFs[i].SetValue(0.0f);
    }
}
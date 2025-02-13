#include "Pose.h"

Pose::Pose() {
    DOFs.resize(0);
}

void Pose::SetDOF(int index, float DOF) {
    DOFs[index].SetValue(DOF);
}

DOF& Pose::GetDOF(int index) {
    return DOFs[index];
}

int Pose::GetNumDOFs() {
    return DOFs.size();
}

Pose::Pose(Skeleton* skeleton) {

    // should be equal to the number of channels
    DOFs.resize(skeleton->GetJointList().size() * 3);

    for (int i = 0; i < DOFs.size(); i++) {
        DOFs[i].SetValue(0.0f);
    }
}

void Pose::Resize(int size) {
    DOFs.resize(size);
}
#include "Rig.h"

Rig::Rig() {
    skeleton = nullptr;
    skin = nullptr;
}

Rig::Rig(Skeleton* skeleton, Skin* skin) {
    this->skeleton = skeleton;
    this->skin = skin;
}

Rig::~Rig() {
    delete skeleton;
    delete skin;
}

void Rig::ApplyPose(Pose& pose) {
    std::vector<Joint*> jointList = skeleton->GetJointList();

    for (int i = 0; i < jointList.size(); i++) {
        float x = pose.GetDOF(i * 3).GetValue();
        float y = pose.GetDOF(i * 3 + 1).GetValue();
        float z = pose.GetDOF(i * 3 + 2).GetValue();

        jointList[i]->SetPose(x, y, z);
    }

    skeleton->Update();
    if (skin) {
        skin->Update();
    }
}
#include "Rig.h"

Rig::Rig(char* skelFilename, char* skinFilename) {
    skeleton = new Skeleton();
    skeleton->Load(skelFilename);
    skeleton->PopulateJointList();

    if (skinFilename) {
        skin = new Skin();
        skin->Load(skinFilename, skeleton);
    } else {
        skin = nullptr;
    }
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
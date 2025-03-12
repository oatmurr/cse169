#include "Rig.h"

Rig::Rig()
{
    skeleton = nullptr;
    skin = nullptr;
}

Rig::Rig(Skeleton* skeleton, Skin* skin)
{
    this->skeleton = skeleton;
    this->skin = skin;
}

Rig::~Rig()
{
    delete skeleton;
    delete skin;
}

Skeleton* Rig::GetSkeleton()
{
    return skeleton;
}

void Rig::ApplyPose(Pose& pose)
{
    if (!skeleton)
    {
        printf("Rig::ApplyPose - skeleton is null\n");
        return;
    }
    
    std::vector<Joint*> jointList = skeleton->GetJointList();

    // first three channels are root translation (x, y, z)
    float rootTranslationX = pose.GetDOF(0).GetValue();
    float rootTranslationY = pose.GetDOF(1).GetValue();
    float rootTranslationZ = pose.GetDOF(2).GetValue();

    // set root translation
    if (jointList.size() > 0)
    {
        jointList[0]->SetOffset(rootTranslationX, rootTranslationY, rootTranslationZ);
    }

    for (int i = 0; i < jointList.size(); i++)
    {
        if (!jointList[i])
        {
            printf("Rig::ApplyPose - jointList[%d] is null\n", i);
            continue;
        }
        
        // after that there will be three rotational channels for every joint
        float jointRotationX = pose.GetDOF((i * 3) + 3).GetValue();
        float jointRotationY = pose.GetDOF((i * 3) + 4).GetValue();
        float jointRotationZ = pose.GetDOF((i * 3) + 5).GetValue();

        jointList[i]->SetPose(jointRotationX, jointRotationY, jointRotationZ);

        // printf("Joint %s pose values - x: %f, y: %f, z: %f\n", 
        //    jointList[i]->GetName().c_str(), jointRotationX, jointRotationY, jointRotationZ);
    }

    skeleton->Update();
    if (skin)
    {
        skin->Update();
    }
}
#pragma once

#include "Skeleton.h"

// A pose is an array of values that maps to a rig
// If the rig contains only simple independent DOFs, the pose can just be an array of floats
// 𝚽 = [𝜑1, 𝜑2, ..., 𝜑𝑁]

class Pose {
    
private:
    std::vector<DOF> DOFs;

public:
    Pose();
    Pose(Skeleton* skeleton);
    
    // getters and setters
    void SetDOF(int index, float DOF);
    DOF& GetDOF(int index);
};

#include "Keyframe.h"
#include <vector>

class Channel {

private:
    std::string extrapolationIn, extrapolationOut;

    std::vector<Keyframe> keys;

public:
    Channel();
    ~Channel();

    // main runtime function
    float Evaluate(float time);

    // load (called by AnimationClip)
    bool Load(Tokenizer& token);

    // computes tangent rules/values and cubic coefficients for each keyframe
    void Precompute();

    // find keyframe span for interpolation (binary search)
    int FindSpan(float time);

    // extrapolation over infinite time range
    float ExtrapolateIn(float time);
    float ExtrapolateOut(float time);   
};
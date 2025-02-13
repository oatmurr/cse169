#pragma once

#include "core.h"
#include "Tokenizer.h"
#include <string>

// stores time, value, tangents, cubics...
class Keyframe {

private:
    float time;
    float value;
    
    // tangent: rate of change of the DOF (velocity)
    float tangentIn, tangentOut;

    // - Flat (tangent = 0)
    // - Linear (tangent points to next/last key)
    // - Smooth (automatically adjust tangent for smooth results)
    std::string ruleIn, ruleOut;

public:
    Keyframe();

    // getters and setters
    void SetTime(float time);
    float GetTime() const;

    void SetValue(float value);
    float GetValue() const;

    void SetTangentIn(float tangent);
    float GetTangentIn() const;

    void SetTangentOut(float tangent);
    float GetTangentOut() const;

    void SetRuleIn(std::string rule);
    std::string GetRuleIn() const;

    void SetRuleOut(std::string rule);
    std::string GetRuleOut() const;

    // load
    bool Load(Tokenizer& token);

    // precompute cubic coefficients based on adjacent keyframe data
    void ComputeCoefficients(const Keyframe& nextKey);

    // cubic hermite curve coefficients that define keyframe span
    // • The shape of the span curve is defined by the value of the two keyframes
    // and the outgoing tangent of the first and incoming tangent of the second
    // • Those 4 geometric values are converted to the 4 coefficients of a cubic
    // equation, as we will see
    // • For simplicity, each keyframe can store the 4 coefficients for the next span
    // (the coefficients of the last keyframe are not used)
    float a, b, c, d;
};
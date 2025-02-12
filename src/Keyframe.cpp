#include  "Keyframe.h"

Keyframe::Keyframe() {
    time = 0.0f;
    value = 0.0f;
    tangentIn = 0.0f;
    tangentOut = 0.0f;
    ruleIn = "fixed";
    ruleOut = "fixed";
}

float Keyframe::GetTime() const {
    return time;
}

float Keyframe::GetValue() const {
    return value;
}

float Keyframe::GetTangentIn() const {
    return tangentIn;
}

float Keyframe::GetTangentOut() const {
    return tangentOut;
}

std::string Keyframe::GetRuleIn() const {
    return ruleIn;
}

std::string Keyframe::GetRuleOut() const {
    return ruleOut;
}

bool Keyframe::Load(Tokenizer& token) {
    
    // load time
    time = token.GetFloat();
    // load value
    value = token.GetFloat();

    // load tangent rules/values
    char in[256], out[256];
    token.GetToken(in);
    token.GetToken(out);

    // check if 'in' is float or string
    if (isdigit(in[0]) || in[0] == '-' || in[0] == '.') {
        // fixed
        ruleIn = std::string("fixed");
        // yes float
        tangentIn = std::stof(in);
    } else {
        // flat, linear, smooth
        ruleIn = std::string(in);
        // no float
        tangentIn = 0.0f;
    }

    // check if 'out' is float or string
    if (isdigit(out[0]) || out[0] == '-' || out[0] == '.') {
        // fixed
        ruleOut = std::string("fixed");
        // yes float
        tangentOut = std::stof(out);
    } else {
        // flat, linear, smooth
        ruleOut = std::string(out);
        // no float
        tangentOut = 0.0f;
    }

    return true;
}

void Keyframe::ComputeCoefficients(const Keyframe& nextKey) {
    
    float t0 = time;
    float t1 = nextKey.time;
    float dt = t1 - t0;

    float p0 = value;
    float p1 = nextKey.value;
    float v0 = tangentOut * dt;
    float v1 = nextKey.tangentIn * dt;

    // hermite basis matrix
    glm::mat4 hermiteBasis = {
        { 2.0f, -2.0f, 1.0f, 1.0f },
        { -3.0f, 3.0f, -2.0f, -1.0f },
        { 0.0f, 0.0f, 1.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f, 0.0f }
    };

    // geometry vector
    glm::vec4 geometry = { p0, p1, v0, v1 };

    // compute coefficients: c = B_Hrm * g_Hrm
    glm::vec4 coefficients = hermiteBasis * geometry;

    // store results
    a = coefficients.x;
    b = coefficients.y;
    c = coefficients.z;
    d = coefficients.w;
}
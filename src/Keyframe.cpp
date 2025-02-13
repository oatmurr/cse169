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

void Keyframe::SetTangentIn(float tangent) {
    tangentIn = tangent;
}

void Keyframe::SetTangentOut(float tangent) {
    tangentOut = tangent;
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
    
    // printf("Keyframe::ComputeCoefficients - computing for span (t0=%f -> t1=%f)\n", time, nextKey.time);

    float t0 = time;
    float t1 = nextKey.time;
    float dt = t1 - t0;

    if (dt == 0) {
        printf("Keyframe::ComputeCoefficients - dt is 0\n");
        return;
    }

    float p0 = value;
    float p1 = nextKey.value;
    float v0 = tangentOut * dt;
    float v1 = nextKey.tangentIn * dt;

    // printf("Keyframe::ComputeCoefficients - values: p0=%f, p1=%f, v0=%f, v1=%f, dt=%f\n", p0, p1, v0, v1, dt);
    // printf("Keyframe::ComputeCoefficients - tangents: v0=%f, v1=%f\n", tangentOut, nextKey.tangentIn);

    // hermite basis matrix
    // glm::mat4 hermiteBasis = {
    //     { 2.0f, -2.0f, 1.0f, 1.0f },
    //     { -3.0f, 3.0f, -2.0f, -1.0f },
    //     { 0.0f, 0.0f, 1.0f, 0.0f },
    //     { 1.0f, 0.0f, 0.0f, 0.0f }
    // };

    glm::mat4 hermiteBasis = {
        { 2.0f, -3.0f, 0.0f, 1.0f },
        { -2.0f, 3.0f, 0.0f, 0.0f },
        { 1.0f, -2.0f, 1.0f, 0.0f },
        { 1.0f, -1.0f, 0.0f, 0.0f }
    };

    // geometry vector
    glm::vec4 geometry = { p0, p1, v0, v1 };

    // printf("Keyframe::ComputeCoefficients - geometry vector: (%f, %f, %f, %f)\n", 
    //        geometry.x, geometry.y, geometry.z, geometry.w);

    // compute coefficients: c = B_Hrm * g_Hrm
    glm::vec4 coefficients = hermiteBasis * geometry;

    // printf("Keyframe::ComputeCoefficients - matrix multiplication:\n");
    // for (int i = 0; i < 4; i++) {
    //     float rowResult =
    //         hermiteBasis[i][0] * geometry.x +
    //         hermiteBasis[i][1] * geometry.y +
    //         hermiteBasis[i][2] * geometry.z +
    //         hermiteBasis[i][3] * geometry.w;

    //     printf("  row %d: (%f * %f) + (%f * %f) + (%f * %f) + (%f * %f) = %f\n",
    //         i,
    //         hermiteBasis[i][0], geometry.x,
    //         hermiteBasis[i][1], geometry.y,
    //         hermiteBasis[i][2], geometry.z,
    //         hermiteBasis[i][3], geometry.w,
    //         rowResult);
    // }

    // store results
    a = coefficients.x;
    b = coefficients.y;
    c = coefficients.z;
    d = coefficients.w;

    // printf("Keyframe::ComputeCoefficients - computed coefficients: a=%f, b=%f, c=%f, d=%f\n", a, b, c, d);
}
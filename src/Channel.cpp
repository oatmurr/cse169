#include "Channel.h"

Channel::Channel() {
    extrapolationIn = "constant";
    extrapolationOut = "constant";
}

Channel::~Channel() {
    keys.clear();
}

float Channel::Evaluate(float time) {
    
    // if there are no keys in the channel, it should just return 0
    if (keys.size() == 0) {
        return 0.0f;
    }

    // if there is just one key, it should return the value of that key
    if (keys.size() == 1) {
        return keys[0].GetValue();
    }

    // find the span between keyframes
    int span = FindSpan(time);

    // t is before the first key (use extrapolation in)
    if (span < 0) {
        return ExtrapolateIn(time);
    }

    // t is after the last key (use extrapolation out)
    if (span >= keys.size() - 1) {
        return ExtrapolateOut(time);
    }

    // t falls exactly on some key (return key value)
    if (time == keys[span].GetTime()) {
        return keys[span].GetValue();
    }

    // t falls on span between two keys (evaluate cubic equation)
    float t = (time - keys[span].GetTime()) / (keys[span + 1].GetTime() - keys[span].GetTime());
    float t2 = t * t;
    float t3 = t2 * t;

    return keys[span].a * t3 + keys[span].b * t2 + keys[span].c * t + keys[span].d;
}

bool Channel::Load(Tokenizer& token) {
    
    token.FindToken("{");

    while (true) {
        char temp[256];
        token.GetToken(temp);

        if (strcmp(temp, "extrapolation") == 0) {
            token.GetToken(temp);
            extrapolationIn = std::string(temp);
            token.GetToken(temp);
            extrapolationOut = std::string(temp);
        } else if (strcmp(temp, "keys") == 0) {
            int numKeys = token.GetInt();
            keys.resize(numKeys);
            token.FindToken("{");

            for (int i = 0; i < numKeys; i++) {
                keys[i].Load(token);
            }
            token.FindToken("}");
            break;

        } else {
            printf("Channel::Load - unrecognised token: %s\n", temp);
            token.SkipLine();
        }
    }

    Precompute();

    return true;
}

void Channel::Precompute() {
    
    for (int i = 0; i < keys.size() - 1; i++) {
        keys[i].ComputeCoefficients(keys[i + 1]);
    }
}

int Channel::FindSpan(float time) {
    
    int left = 0;
    int right = keys.size() - 1;

    // edge cases
    if (time < keys[0].GetTime()) {
        return -1;
    }

    if (time >= keys[keys.size() - 1].GetTime()) {
        return right;
    }

    // binary search
    while (left <= right) {
        int mid = (left + right) / 2;

        float midTime = keys[mid].GetTime();
        float nextTime = keys[mid + 1].GetTime();

        if (time >= midTime && time < nextTime) {
            return mid;
        }

        if (time < midTime) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }
    return left;
}

float Channel::ExtrapolateIn(float time) {
    
    // get key values and times needed for calculations
    float t0 = keys[0].GetTime();
    float p0 = keys[0].GetValue();

    float t1 = keys[1].GetTime();
    float p1 = keys[1].GetValue();

    float t2 = keys[2].GetTime();
    float p2 = keys[2].GetValue();

    float tN = keys[keys.size() - 1].GetTime();
    float pN = keys[keys.size() - 1].GetValue();

    // calculate time difference from start
    float dt = time - t0;
    
    // calculate incoming tangent (v) based on rule type
    float v;

    std::string ruleIn = keys[0].GetRuleIn();

    if (ruleIn == "fixed") {
        v = keys[0].GetTangentIn();
    } else if (ruleIn == "flat") {
        v = 0.0f;
    } else if (ruleIn == "linear") {
        v = (p1 - p0) / (t1 - t0);
    } else if (ruleIn == "smooth") {
        v = (p2 - p0) / (t2 - t0);
    } else {
        printf("Channel::ExtrapolateIn - unrecognised rule: %s\n", ruleIn.c_str());
    }

    // predefined cycle stuff
    float length = tN - t0;
    float cycles = floor(abs(dt) / length);
    float remainder = fmod(abs(dt), length);

    // extrapolation evaluation
    if (extrapolationIn == "constant") {
        // return first keyframe value
        return p0;
    } else if (extrapolationIn == "linear") {
        // linear extrapolation using tangent
        return p0 + dt * v;
    } else if (extrapolationIn == "cycle") {
        // adjust remainder for negative time
        if (dt < 0) {
            remainder = length - remainder;
        }
        // cycle animation
        return Evaluate(t0 + remainder);
    } else if (extrapolationIn == "cycle_offset") {
        // adjust remainder for negative time
        if (dt < 0) {
            remainder = length - remainder;
        }
        // calculate progressive offset
        float offset = (pN - p0) * cycles;
        // cycle animation with offset
        return Evaluate(t0 + remainder) - offset;
    } else if (extrapolationIn == "bounce") {
        // reverse direction on odd cycles
        if ((int)cycles % 2 == 1) {
            remainder = length - remainder;
        }
        // adjust remainder for negative time
        if (dt < 0) {
            remainder = length - remainder;
        }
        // bounce animation
        return Evaluate(t0 + remainder);
    } else {
        printf("Channel::ExtrapolateIn - unrecognised extrapolation: %s, defaulting to constant\n", extrapolationIn.c_str());
        return p0;
    }
}

float Channel::ExtrapolateOut(float time) {
    
    // get key values and times needed for calculations
    float t0 = keys[0].GetTime();
    float p0 = keys[0].GetValue();
    
    float tN = keys[keys.size() - 1].GetTime();
    float pN = keys[keys.size() - 1].GetValue();

    float tN1 = keys[keys.size() - 2].GetTime();
    float pN1 = keys[keys.size() - 2].GetValue();

    float tN2 = keys[keys.size() - 3].GetTime();
    float pN2 = keys[keys.size() - 3].GetValue();
    
    // calculate time difference from end
    float dt = time - tN;

    // calculate outgoing tangent based on rule type
    float v;

    std::string ruleOut = keys[keys.size() - 1].GetRuleOut();
    
    if (ruleOut == "fixed") {
        v = keys[keys.size() - 1].GetTangentOut();
    } else if (ruleOut == "flat") {
        v = 0.0f;
    } else if (ruleOut == "linear") {
        v = (pN1 - pN) / (tN1 - tN);
    } else if (ruleOut == "smooth") {
        v = (pN2 - pN) / (tN2 - tN);
    } else {
        printf("Channel::ExtrapolateOut - unrecognised rule: %s\n", ruleOut.c_str());
    }

    // predefined cycle stuff
    float length = tN - t0;
    float cycles = floor(dt / length);
    float remainder = fmod(dt, length);

    // extrapolation evaluation
    if (extrapolationOut == "constant") {
        // return last keyframe value
        return pN;
    } else if (extrapolationOut == "linear") {
        // linear extrapolation using tangent
        return pN + dt * v;
    } else if (extrapolationOut == "cycle") {
        // cycle animation
        return Evaluate(tN - length + remainder);
    } else if (extrapolationOut == "cycle_offset") {
        // calculate progressive offset
        float offset = (pN - p0) * cycles;
        // cycle animation with offset
        return Evaluate(tN - length + remainder) + offset;
    } else if (extrapolationOut == "bounce") {
        // reverse direction on odd cycles
        if ((int)cycles % 2 == 1) {
            remainder = length - remainder;
        }
        // bounce animation
        return Evaluate(tN - length + remainder);
    } else {
        printf("Channel::ExtrapolateOut - unrecognised extrapolation: %s, defaulting to constant\n", extrapolationOut.c_str());
        return pN;
    }
}

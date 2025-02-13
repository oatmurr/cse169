#include "AnimationClip.h"

AnimationClip::AnimationClip() {
    start = 0.0f;
    end = 0.0f;
}

float AnimationClip::GetStartTime() const {
    return start;
}

float AnimationClip::GetEndTime() const {
    return end;
}

const std::vector<Channel>& AnimationClip::GetChannels() const {
    return channels;
}

void AnimationClip::Evaluate(float time, Pose& pose) {
    // printf("AnimationClip::Evaluate - time: %f\n", time);
    for (int i = 0; i < channels.size(); i++) {
        float value = channels[i].Evaluate(time);
        // printf("AnimationClip::Evaluate - value: %f\n", value);
        // evaluate channel at time and update pose
        pose.SetDOF(i, value);
    }
}

bool AnimationClip::Load(const char *filename) {
    
    Tokenizer token;
    token.Open(filename);

    char temp[256];
    token.GetToken(temp);

    if (strcmp(temp, "animation") == 0) {
        
        token.FindToken("{");

        while (true) {
            
            token.GetToken(temp);
            // range
            if (strcmp(temp, "range") == 0) {
                
                start = token.GetFloat();
                end = token.GetFloat();
            // numchannels
            } else if (strcmp(temp, "numchannels") == 0) {
                
                int numChannels = token.GetInt();
                channels.resize(numChannels);

                // channels
                for (int i = 0; i < numChannels; i++) {
                    token.FindToken("channel");
                    channels[i].Load(token);
                }
            // end of animation block
            } else if (strcmp(temp, "}") == 0) {
                break;
            // unrecognised token
            } else {
                printf("AnimationClip::Load - unrecognised token: %s\n", temp);
                token.SkipLine();
            }
        }
    }
    
    token.Close();
    printf("AnimationClip::Load - finished loading animation\n");
    return true;
}
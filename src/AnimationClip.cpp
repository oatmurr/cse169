#include "AnimationClip.h"

bool AnimationClip::Load(const char *filename) {
    
    Tokenizer token;
    token.Open(filename);

    int currentChannel = 0;

    while (true) {
        char temp[256];
        token.GetToken(temp);

        if (strcmp(temp, "animation") == 0) {
            token.FindToken("{");
            // range
            if (strcmp(temp, "range") == 0) {
                startTime = token.GetFloat();
                endTime = token.GetFloat();
            // numchannels
            } else if (strcmp(temp, "numchannels") == 0) {
                int numChannels = token.GetInt();
                channels.resize(numChannels);
            // channels
            } else if (strcmp(temp, "channel") == 0) {
                if (currentChannel < channels.size()) {
                    channels[currentChannel].Load(token);
                    currentChannel++;
                } else {
                    printf("AnimationClip::Load - ERROR: too many channels\n");
                    return false;
                }
            }
            token.FindToken("}");
            break;

        } else {
            printf("AnimationClip::Load - unrecognised token: %s\n", temp);
            token.SkipLine();
        }
    }

    token.Close();
    printf("AnimationClip::Load - finished loading animation\n");
    return true;
}
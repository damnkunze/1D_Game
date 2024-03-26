float collectableAnimationStrength = 120;
float collectableAnimationLength = 17;
float collectableAnimationSpeed = 1;

float collectedAnimationStrength = 50;
float collectedAnimationLength = 22;
float collectedAnimationSpeed = 0.5;

namespace H {
    CRGB heartBaseColor = CRGB(255 - collectableAnimationStrength, 0, 0);

    class Heart {
    public:
        int pos;
        CRGB color;
        float currPointInAnimation;
        int currAnimationColorChange;
        bool collected;
        bool justCollected;

        // Empty placeholder
        Heart() {
            pos = -1;
            color = blankPixel;
            currPointInAnimation = 0;
            currAnimationColorChange = 0;
            collected = false;
            justCollected = false;
        }
        Heart(int pos, bool collected) {
            pos = pos;
            collected = collected;
            color = heartBaseColor;
            if (collected) {
                currPointInAnimation = random(-collectedAnimationLength + 1, collectedAnimationLength); 
            } else {
                currPointInAnimation = random(-collectableAnimationLength + 1, collectableAnimationLength); 
            }
            currAnimationColorChange = 0;
            justCollected = false;
        }

        bool getsCollected(int playerPos) {
            return pos <= playerPos && pos >= playerPos - playerSpeed;
        }

        // Imitate heartbeat effect
        void animateColor () {
            int animationFunctionRange = collectableAnimationLength;
            int animationStrength = collectableAnimationStrength;
            float animationSpeed = collectableAnimationSpeed;

            if (this->collected) {
                animationFunctionRange = collectedAnimationLength;
                animationStrength = collectedAnimationStrength;
                animationSpeed = collectedAnimationSpeed;
            }

            currPointInAnimation += animationSpeed;
            if (currPointInAnimation >= animationFunctionRange) {
                currPointInAnimation = -animationFunctionRange;
            }
            
            float animationFuncVal = max(0, -1 * pow(currPointInAnimation / 7.07, 4) + pow(currPointInAnimation / 5, 2));
            // -(x / 7.07)^4 + (x / 5)^2
            int changeRed = animationStrength * animationFuncVal;
            color = addValuesToColor(heartBaseColor, changeRed, 0, 0);
        }
    };
}
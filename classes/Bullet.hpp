namespace B {
    int bulletSpeed = 1;
    int bulletRange = 30;

    class Bullet {
    public:
        int pos;
        String shotBy;
        int velocity;
        CRGB color;
        int distanceMoved;
        bool stopped;

        // Empty placeholder
        Bullet() {
            pos = -1;
            shotBy = "none";
            velocity = 0;
            color = blankPixel;
            distanceMoved = 0;
            stopped = true;
        }
        Bullet(int pos, String shotBy, int velocity, CRGB color) {
            pos = pos;
            shotBy = shotBy;
            velocity = velocity;
            color = color;
            distanceMoved = 0;
            stopped = false;
        }

        void move() {
            pos += velocity;
            distanceMoved += velocity;
        }

        bool movedTooFar(int minPos, int maxPos) {
            if (distanceMoved > bulletRange || pos >= maxPos || pos <= minPos) {
                if (pos < minPos) pos = minPos;
                if (pos > maxPos) pos = maxPos;
                return true;
            }
            return false;
        }
    };
}
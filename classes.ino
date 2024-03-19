Class Bullet {
    public:
        int pos;
        CRGB color;
        int distanceMoved;
        Bullet(int pos, CRGB color) {
            this->pos = pos;
            this->color = color;
            this->distanceMoved = 0;
        }
}

Class Enemy {
    public:
        int pos;
        CRGB color;
        int lives;
        Enemy(int pos, CRGB color, int lives) {
            this->pos = pos;
            this->color = color;
            this->lives = lives;
        }
}
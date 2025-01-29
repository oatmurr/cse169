
class Skin {
private:

public:
    // constructors/destructors
    Skin();
    ~Skin();

    // functions
    bool Load(const char* filename);
    void Update();
    void Draw();
};
#pragma
#include <vector>
#include <unordered_map>
#include <math.h>

const float H             = 1.5f;
const float H2            = H * H;
const float H9            = H*H*H*H*H*H*H*H*H;
const float H6            = H*H*H*H*H*H;
const float REST_DENSITY  = 25.0f;
const float GAS_CONSTANT  = 2.0f;
const float VISCOSITY     = 0.05f;
const float PI            = 3.14159265f;

const float POLY6_COEFF   = 315.0f / (64.0f * PI * H9);
const float SPIKY_COEFF   = 45.0f  / (PI * H6);
const float VISC_COEFF    = 45.0f  / (PI * H6);

struct Vector3 {
    float x, y, z;
    Vector3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
    Vector3 operator+(const Vector3& v) const { return Vector3(x+v.x, y+v.y, z+v.z); }
    Vector3 operator-(const Vector3& v) const { return Vector3(x-v.x, y-v.y, z-v.z); }
    Vector3 operator*(float s)          const { return Vector3(x*s,   y*s,   z*s);   }
    Vector3& operator+=(const Vector3& v) { x+=v.x; y+=v.y; z+=v.z; return *this; }
    Vector3& operator-=(const Vector3& v) { x-=v.x; y-=v.y; z-=v.z; return *this; }
    Vector3& operator*=(float s)          { x*=s;   y*=s;   z*=s;   return *this; }
    float dot(const Vector3& v) const { return x*v.x + y*v.y + z*v.z; }
    Vector3 cross(const Vector3& v) const {
        return Vector3(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
    }
    float length() const { return sqrtf(x*x + y*y + z*z); }
    Vector3 normalized() const {
        float len = length();
        return len > 0 ? *this * (1.0f / len) : Vector3(0,0,0);
    }
};

struct SPHParticle {
    Vector3 position;
    Vector3 velocity;
    Vector3 force;
    float mass;
    float radius;
    bool pinned;
    float temperature;
    float density;
    float pressure;

    SPHParticle(Vector3 pos, float m, float r)
        : position(pos), velocity(0,0,0), force(0,0,0),
          mass(m), radius(r), pinned(false), temperature(0.5f),
          density(0.0f), pressure(0.0f) {}
};

struct Bounds {
    float minX, maxX;
    float minY, maxY;
    float minZ, maxZ;
};

struct IVec3 {
    int x, y, z;
    bool operator==(const IVec3& o) const { return x==o.x && y==o.y && z==o.z; }
};

struct IVec3Hash {
    size_t operator()(const IVec3& v) const {
        size_t hx = std::hash<int>{}(v.x);
        size_t hy = std::hash<int>{}(v.y);
        size_t hz = std::hash<int>{}(v.z);
        return hx ^ (hy * 2654435761u) ^ (hz * 805459861u);
    }
};

extern std::vector<SPHParticle*> bodies;
extern std::unordered_map<IVec3, std::vector<int>, IVec3Hash> grid;
extern Bounds bounds;
extern int frameCount; 

void initBalls();
void applyWallCollisions(SPHParticle* ball, const Bounds& b);
void updatePhysics(float dt);
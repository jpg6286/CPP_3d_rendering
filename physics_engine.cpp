#include "physics.h"
#include <iostream>
using namespace std;

std::vector<SPHParticle*> bodies;
std::unordered_map<IVec3, std::vector<int>, IVec3Hash> grid;

Vector3 gravity(0, -9.8f, 0);
float dragCoefficient = 0.999f;
int frameCount = 0;
Bounds bounds = { -10.0f, 10.0f, 0.0f, 12.0f, -10.0f, 10.0f };

void initBalls() {
    float ballRadius = 0.5f;
    float mass       = 1.0f;
    int   numBalls   = 500;

    for (int i = 0; i < numBalls; i++) {
        float x = bounds.minX + 0.6f + (float)(i % 18)        * 1.1f;
        float y = bounds.minY + 0.6f + (float)((i / 18) % 10) * 1.1f;
        float z = bounds.minZ + 0.6f + (float)(i / 180)        * 1.1f;

        SPHParticle* ball = new SPHParticle(Vector3(x, y, z), mass, ballRadius);
        ball->temperature = y / bounds.maxY;
        ball->velocity = Vector3(
            ((i * 7  + 3) % 11 - 5) * 0.3f,
            ((i * 13 + 1) % 7  - 3) * 0.3f,
            ((i * 5  + 9) % 9  - 4) * 0.3f);
        bodies.push_back(ball);
    }
}

void applyWallCollisions(SPHParticle* ball, const Bounds& b) {
    float r           = ball->radius;
    float restitution = 0.3f;

    if (ball->position.x - r < b.minX) { ball->position.x = b.minX + r; ball->velocity.x =  fabsf(ball->velocity.x) * restitution; }
    if (ball->position.x + r > b.maxX) { ball->position.x = b.maxX - r; ball->velocity.x = -fabsf(ball->velocity.x) * restitution; }
    if (ball->position.y - r < b.minY) { ball->position.y = b.minY + r; ball->velocity.y =  fabsf(ball->velocity.y) * restitution; }
    if (ball->position.y + r > b.maxY) { ball->position.y = b.maxY - r; ball->velocity.y = -fabsf(ball->velocity.y) * restitution; }
    if (ball->position.z - r < b.minZ) { ball->position.z = b.minZ + r; ball->velocity.z =  fabsf(ball->velocity.z) * restitution; }
    if (ball->position.z + r > b.maxZ) { ball->position.z = b.maxZ - r; ball->velocity.z = -fabsf(ball->velocity.z) * restitution; }
}

// Density 
float kernelPoly6(float r2) {
    if (r2 >= H2) return 0.0f;
    float diff = H2 - r2;
    return POLY6_COEFF * diff * diff * diff;
}

// Pressure
Vector3 kernelSpikyGrad(const Vector3& rVec, float r) {
    if (r <= 0.0f || r >= H) return Vector3(0,0,0);
    float diff = H - r;
    return rVec * (-SPIKY_COEFF * diff * diff / r);
}

// Viscosity 
float kernelViscLaplacian(float r) {
    if (r >= H) return 0.0f;
    return VISC_COEFF * (H - r);
}

IVec3 cellOf(const Vector3& pos, float cellSize) {
    return { (int)floorf(pos.x / cellSize),
             (int)floorf(pos.y / cellSize),
             (int)floorf(pos.z / cellSize) };
}

void updatePhysics(float dt) {
    const float heatingRate       = 1.5f;
    const float coolingRate       = 1.5f;
    const float boundaryThickness = 1.5f;
    const float buoyancyStrength  = 15.0f;
    const float cellSize          = H;

    grid.clear();
    grid.reserve(bodies.size() * 2);
    for (int i = 0; i < (int)bodies.size(); i++)
        grid[cellOf(bodies[i]->position, cellSize)].push_back(i);

    for (int i = 0; i < (int)bodies.size(); i++) {
        SPHParticle* pi = bodies[i];
        pi->density = 0.0f;

        IVec3 cell = cellOf(pi->position, cellSize);
        for (int dx = -1; dx <= 1; dx++)
        for (int dy = -1; dy <= 1; dy++)
        for (int dz = -1; dz <= 1; dz++) {
            auto it = grid.find({cell.x+dx, cell.y+dy, cell.z+dz});
            if (it == grid.end()) continue;
            for (int j : it->second) {
                Vector3 rVec = pi->position - bodies[j]->position;
                if (fabsf(rVec.x) > H || fabsf(rVec.y) > H || fabsf(rVec.z) > H) continue;
                float r2 = rVec.dot(rVec);
                pi->density += bodies[j]->mass * kernelPoly6(r2);
            }
        }
        pi->density  = fmaxf(pi->density, 0.0001f);
        pi->pressure = GAS_CONSTANT * (pi->density - REST_DENSITY);
    }

    for (int i = 0; i < (int)bodies.size(); i++) {
        SPHParticle* pi = bodies[i];
        if (pi->pinned) continue;

        Vector3 fPressure(0,0,0), fViscosity(0,0,0);

        IVec3 cell = cellOf(pi->position, cellSize);
        for (int dx = -1; dx <= 1; dx++)
        for (int dy = -1; dy <= 1; dy++)
        for (int dz = -1; dz <= 1; dz++) {
            auto it = grid.find({cell.x+dx, cell.y+dy, cell.z+dz});
            if (it == grid.end()) continue;
            for (int j : it->second) {
                if (j == i) continue;
                SPHParticle* pj = bodies[j];
                Vector3 rVec = pi->position - pj->position;
                if (fabsf(rVec.x) > H || fabsf(rVec.y) > H || fabsf(rVec.z) > H) continue;
                float r2 = rVec.dot(rVec);
                if (r2 >= H2 || r2 < 0.0001f) continue;
                float r = sqrtf(r2);

                float pTerm = (pi->pressure + pj->pressure) / (2.0f * pj->density);
                fPressure  += kernelSpikyGrad(rVec, r) * (pj->mass * pTerm);
                fViscosity += (pj->velocity - pi->velocity) *
                              (pj->mass * VISCOSITY * kernelViscLaplacian(r) / pj->density);
            }
        }

        Vector3 fGravity(0, -9.8f * pi->mass, 0);
        float   buoyancy  = (pi->temperature - 0.5f) * buoyancyStrength * pi->density;
        Vector3 fBuoyancy(0, buoyancy, 0);

        pi->force = fPressure + fViscosity + fGravity + fBuoyancy;
    }

    for (int i = 0; i < (int)bodies.size(); i++) {
        SPHParticle* p = bodies[i];
        if (p->pinned) continue;

        float floorInfluence = fmaxf(0.0f, 1.0f - (p->position.y - bounds.minY) / boundaryThickness);
        float ceilInfluence  = fmaxf(0.0f, 1.0f - (bounds.maxY - p->position.y) / boundaryThickness);
        p->temperature += heatingRate * floorInfluence * dt;
        p->temperature -= coolingRate * ceilInfluence  * dt;
        p->temperature  = fmaxf(0.0f, fminf(1.0f, p->temperature));

        IVec3 cell = cellOf(p->position, H);
        for (int dx = -1; dx <= 1; dx++)
        for (int dy = -1; dy <= 1; dy++)
        for (int dz = -1; dz <= 1; dz++) {
            auto it = grid.find({cell.x+dx, cell.y+dy, cell.z+dz});
            if (it == grid.end()) continue;
            for (int j : it->second) {
                if (j <= i) continue;
                SPHParticle* pj = bodies[j];
                Vector3 rVec = p->position - pj->position;
                float r2 = rVec.dot(rVec);
                if (r2 >= H2) continue;
                float transfer = (pj->temperature - p->temperature) * 0.1f * dt;
                p->temperature  = fmaxf(0.0f, fminf(1.0f, p->temperature  + transfer));
                pj->temperature = fmaxf(0.0f, fminf(1.0f, pj->temperature - transfer));
            }
        }

        // Euler-Cromer
        Vector3 accel = p->force * (1.0f / p->mass);
        p->velocity  += accel * dt;
        p->position  += p->velocity * dt;

        applyWallCollisions(p, bounds);
    }
}
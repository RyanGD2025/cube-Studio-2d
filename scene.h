#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "Entity.h"
#include "RenderSystem.h"
#include "InputSystem.h"
#include "UpdateSystem.h"
#include <cstdlib>
#include <ctime>

class Scene {
private:
    std::vector<Entity*> entities;
    RenderSystem* renderer;
    InputSystem* inputSys;
    UpdateSystem* updateSys;

public:
    Scene();
    ~Scene();
    void addPlayerCube();
    void gameLoop();
};

#endif // SCENE_H

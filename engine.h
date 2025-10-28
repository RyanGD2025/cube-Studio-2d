#ifndef ENGINE_H
#define ENGINE_H

#include "Scene.h"

class CubeEngine {
private:
    Scene* currentScene;

public:
    CubeEngine();
    ~CubeEngine();
    void run();
};

#endif // ENGINE_H

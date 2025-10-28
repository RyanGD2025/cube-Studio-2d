#include "Engine.h"
#include <iostream>

CubeEngine::CubeEngine() : currentScene(nullptr) {
    std::cout << "Iniciando Cube Studio 2D Engine...\n";
}

CubeEngine::~CubeEngine() {
    delete currentScene;
    std::cout << "Engine finalizado.\n";
}

void CubeEngine::run() {
    currentScene = new Scene();
    currentScene->gameLoop();
}

#include "Scene.h"
#include <iostream>

Scene::Scene() {
    renderer = new RenderSystem(20, 10);
    inputSys = new InputSystem();
    updateSys = new UpdateSystem();
    std::srand(std::time(nullptr));
}

Scene::~Scene() {
    for (auto* e : entities) {
        delete e->getComponent<Position>("position");
        delete e->getComponent<CubeComponent>("cube");
        delete e->getComponent<InputComponent>("input");
        delete e;
    }
    delete renderer;
    delete inputSys;
    delete updateSys;
}

void Scene::addPlayerCube() {
    Entity* player = new Entity(0);
    player->addComponent<Position>("position", new Position(10, 5));
    player->addComponent<CubeComponent>("cube", new CubeComponent('#', "red", 0));
    player->addComponent<InputComponent>("input", new InputComponent(true));
    entities.push_back(player);
}

void Scene::gameLoop() {
    addPlayerCube();

    while (true) {
        char input = inputSys->getInput();
        if (input == 'q' || input == 'Q') break;
        if (input == '+') updateSys->addRandomCube(entities);

        for (auto* e : entities) {
            updateSys->updateEntity(e, input);
        }

        renderer->clearGrid();
        for (auto* e : entities) {
            renderer->renderEntity(e);
        }
        renderer->drawGrid();

        // Delay simples
        for (int i = 0; i < 10000000; ++i) {}
    }
}

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cstdlib>  // Para rand() e system("clear")
#include <ctime>    // Para srand(time(NULL))
#include <conio.h>  // Para _kbhit() e _getch() (Windows); em Linux, use ncurses ou adapte

// Plataforma: Para Linux/Mac, substitua _kbhit/_getch por algo como termios (simplificado aqui)

using namespace std;

// ============================================
// COMPONENTES DO MOTOR (ECS-like: Entity-Component-System)
// ============================================

struct Position {
    int x, y;
    Position(int x = 0, int y = 0) : x(x), y(y) {}
};

struct CubeComponent {
    char symbol;  // Símbolo ASCII para o cubo (ex: '#', '@')
    string color; // Cor simulada (ANSI: "red", "green", etc.)
    int rotation; // Rotação em graus (0-360, afeta símbolo)
    CubeComponent(char sym = '#', string col = "white", int rot = 0) 
        : symbol(sym), color(col), rotation(rot) {}
};

struct InputComponent {
    // Para entidades que respondem a input (ex: player cube)
    bool movable;
    InputComponent(bool mov = false) : movable(mov) {}
};

// ============================================
// ENTIDADE
// ============================================

class Entity {
public:
    int id;
    map<string, void*> components;  // Chave: tipo (ex: "position"), valor: ponteiro pro componente

    Entity(int entityId) : id(entityId) {}

    template<typename T>
    T* getComponent(const string& type) {
        auto it = components.find(type);
        if (it != components.end()) {
            return static_cast<T*>(it->second);
        }
        return nullptr;
    }

    template<typename T>
    void addComponent(const string& type, T* comp) {
        components[type] = comp;
    }

    template<typename T>
    void removeComponent(const string& type) {
        auto it = components.find(type);
        if (it != components.end()) {
            delete static_cast<T*>(it->second);
            components.erase(it);
        }
    }
};

// ============================================
// SISTEMAS DO MOTOR
// ============================================

class RenderSystem {
private:
    vector<vector<char>> grid;  // Grid 2D para render (ex: 20x10)
    int width, height;

public:
    RenderSystem(int w = 20, int h = 10) : width(w), height(h) {
        grid.resize(height, vector<char>(width, ' '));  // Fundo vazio
    }

    void clearGrid() {
        for (auto& row : grid) {
            fill(row.begin(), row.end(), ' ');
        }
    }

    void renderEntity(Entity* entity) {
        auto* pos = entity->getComponent<Position>("position");
        auto* cube = entity->getComponent<CubeComponent>("cube");
        if (pos && cube && pos->x >= 0 && pos->x < width && pos->y >= 0 && pos->y < height) {
            char sym = cube->symbol;
            if (cube->rotation == 90 || cube->rotation == 270) sym = '/';  // Simula rotação simples
            else if (cube->rotation == 180) sym = '\\';
            grid[pos->y][pos->x] = sym;
        }
    }

    void drawGrid() {
        system("clear");  // Ou "cls" no Windows
        cout << "\n=== Cube Studio 2D Engine ===\n";
        cout << "Grid 2D de Cubos | Use WASD: Mover | R: Rotacionar | +: Adicionar Cubo | Q: Sair\n\n";
        for (const auto& row : grid) {
            for (char c : row) {
                cout << c << ' ';
            }
            cout << endl;
        }
        cout << "\n";
    }
};

class InputSystem {
public:
    char getInput() {
        if (_kbhit()) {  // Windows; para cross-platform, use uma lib ou poll
            return _getch();
        }
        return 0;
    }
};

class UpdateSystem {
public:
    void updateEntity(Entity* entity, char input) {
        auto* pos = entity->getComponent<Position>("position");
        auto* cube = entity->getComponent<CubeComponent>("cube");
        auto* inputComp = entity->getComponent<InputComponent>("input");
        if (!pos || !inputComp || !inputComp->movable) return;

        switch (input) {
            case 'w': pos->y = max(0, pos->y - 1); break;
            case 's': pos->y = min(9, pos->y + 1); break;  // Limite height-1
            case 'a': pos->x = max(0, pos->x - 1); break;
            case 'd': pos->x = min(19, pos->x + 1); break;  // Limite width-1
            case 'r': 
                if (cube) cube->rotation = (cube->rotation + 90) % 360; 
                break;
        }
    }

    void addRandomCube(vector<Entity*>& entities) {
        static int nextId = 1;
        Entity* newEntity = new Entity(nextId++);
        newEntity->addComponent<Position>("position", new Position(rand() % 20, rand() % 10));
        newEntity->addComponent<CubeComponent>("cube", new CubeComponent('@', "blue", 0));
        entities.push_back(newEntity);
    }
};

// ============================================
// CENA (Gerencia entidades)
// ============================================

class Scene {
private:
    vector<Entity*> entities;
    RenderSystem* renderer;
    InputSystem* inputSys;
    UpdateSystem* updateSys;

public:
    Scene() {
        renderer = new RenderSystem(20, 10);
        inputSys = new InputSystem();
        updateSys = new UpdateSystem();
        srand(time(NULL));  // Seed rand
    }

    ~Scene() {
        for (auto* e : entities) {
            // Limpa componentes
            delete e->getComponent<Position>("position");
            delete e->getComponent<CubeComponent>("cube");
            delete e->getComponent<InputComponent>("input");
            delete e;
        }
        delete renderer;
        delete inputSys;
        delete updateSys;
    }

    void addPlayerCube() {
        Entity* player = new Entity(0);
        player->addComponent<Position>("position", new Position(10, 5));
        player->addComponent<CubeComponent>("cube", new CubeComponent('#', "red", 0));
        player->addComponent<InputComponent>("input", new InputComponent(true));
        entities.push_back(player);
    }

    void gameLoop() {
        addPlayerCube();  // Adiciona cubo player inicial

        while (true) {
            char input = inputSys->getInput();
            if (input == 'q' || input == 'Q') break;
            if (input == '+') updateSys->addRandomCube(entities);

            // Update: Aplica input em entidades
            for (auto* e : entities) {
                updateSys->updateEntity(e, input);
            }

            // Render
            renderer->clearGrid();
            for (auto* e : entities) {
                renderer->renderEntity(e);
            }
            renderer->drawGrid();

            // Delay simples (em ms; use <chrono> pra precisão)
            for (int i = 0; i < 10000000; ++i) {}  // Placeholder pra ~0.1s
        }
    }
};

// ============================================
// MOTOR PRINCIPAL
// ============================================

class CubeEngine {
private:
    Scene* currentScene;

public:
    CubeEngine() : currentScene(nullptr) {}

    void run() {
        cout << "Iniciando Cube Studio 2D Engine...\n";
        currentScene = new Scene();
        currentScene->gameLoop();
        delete currentScene;
        cout << "Engine finalizado.\n";
    }
};

int main() {
    CubeEngine engine;
    engine.run();
    return 0;
}

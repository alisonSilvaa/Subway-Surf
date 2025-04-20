#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#endif

#define MAP_SIZE 20
#define MAX_OBSTACLES 3
#define MAX_ITEMS 3
#define LANES 3

// --- Estruturas principais --- //
typedef struct {
    int x, y;
    int lane;
    int isJumping;
    int isSliding;
    int score;
    int coins;
    int jumpTime;  // Tempo do pulo
    int slideTime; // Tempo do deslize
} Player;

typedef struct {
    int lane;
    int type;
    int active;
} Obstacle;

typedef struct {
    int lane;
    int type;
    int collected;
} Collectible;

typedef struct {
    Obstacle* obstacles[MAX_OBSTACLES];
    Collectible* items[MAX_ITEMS];
} Tile;

typedef struct {
    Player* player;
    Tile** map;
    int gameSpeed;
    int running;
    int mapPosition;
} GameState;

Tile* createTile() {
    Tile* t = (Tile*)malloc(sizeof(Tile));
    for (int i = 0; i < MAX_OBSTACLES; i++) t->obstacles[i] = NULL;
    for (int i = 0; i < MAX_ITEMS; i++) t->items[i] = NULL;
    return t;
}

Obstacle* createObstacle(int lane, int type) {
    Obstacle* o = (Obstacle*)malloc(sizeof(Obstacle));
    o->lane = lane;
    o->type = type;
    o->active = 1;
    return o;
}

Collectible* createItem(int lane, int type) {
    Collectible* c = (Collectible*)malloc(sizeof(Collectible));
    c->lane = lane;
    c->type = type;
    c->collected = 0;
    return c;
}

void shiftMap(Tile** map) {
    for (int j = 0; j < MAX_OBSTACLES; j++)
        if (map[0]->obstacles[j]) free(map[0]->obstacles[j]);
    for (int j = 0; j < MAX_ITEMS; j++)
        if (map[0]->items[j]) free(map[0]->items[j]);
    free(map[0]);

    for (int i = 0; i < MAP_SIZE - 1; i++)
        map[i] = map[i + 1];

    map[MAP_SIZE - 1] = createTile();
    if (rand() % 3 == 0)
        map[MAP_SIZE - 1]->obstacles[0] = createObstacle(rand() % LANES, rand() % 3);
    if (rand() % 3 == 0)
        map[MAP_SIZE - 1]->items[0] = createItem(rand() % LANES, rand() % 2);
}

int checkCollision(Player* p, Tile* t) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        Obstacle* o = t->obstacles[i];
        if (o && o->active && o->lane == p->lane && !p->isJumping && !p->isSliding)
            return 1;
    }
    return 0;
}

void collectItems(Player* p, Tile* t) {
    for (int i = 0; i < MAX_ITEMS; i++) {
        Collectible* c = t->items[i];
        if (c && !c->collected && c->lane == p->lane) {
            p->coins++;
            c->collected = 1;
        }
    }
}

GameState* initGame() {
    GameState* g = (GameState*)malloc(sizeof(GameState));
    g->player = (Player*)malloc(sizeof(Player));
    g->player->lane = 1;
    g->player->x = 0;
    g->player->y = 0;
    g->player->score = 0;
    g->player->coins = 0;
    g->player->isJumping = 0;
    g->player->isSliding = 0;
    g->player->jumpTime = 0; // Inicializa o tempo de pulo
    g->player->slideTime = 0; // Inicializa o tempo de deslize
    g->gameSpeed = 1;
    g->running = 1;
    g->map = (Tile**)malloc(sizeof(Tile*) * MAP_SIZE);
    for (int i = 0; i < MAP_SIZE; i++)
        g->map[i] = createTile();
    g->mapPosition = 1;
    return g;
}

void sleepMS(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

#ifndef _WIN32
int kbhit(void) {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}
#endif

void drawGame(GameState* g) {
    clearScreen();
    printf("\nPontuacao: %d | Moedas: %d\n", g->player->score, g->player->coins);
    printf("\n");

    // Imprime o mapa de baixo para cima
    for (int i = MAP_SIZE - 1; i >= 0; i--) {  // Mudamos para i >= 0 para inverter a ordem
        for (int l = 0; l < LANES; l++) {
            if (i == g->mapPosition && l == g->player->lane) {
                // Se o personagem está pulando ou deslizando
                if (g->player->isJumping) {
                    printf("PULO ");
                } else if (g->player->isSliding) {
                    printf("DESLIZOU ");
                } else {
                    printf("P ");  // Representa o personagem
                }
            } else {
                int printed = 0;
                for (int j = 0; j < MAX_OBSTACLES; j++) {
                    Obstacle* o = g->map[i]->obstacles[j];
                    if (o && o->lane == l && o->active) {
                        printf("X ");  // Representa o obstáculo
                        printed = 1;
                        break;
                    }
                }
                if (!printed) {
                    for (int j = 0; j < MAX_ITEMS; j++) {
                        Collectible* c = g->map[i]->items[j];
                        if (c && c->lane == l && !c->collected) {
                            printf("$ ");  // Representa o item
                            printed = 1;
                            break;
                        }
                    }
                }
                if (!printed) printf(". ");  // Espaço vazio
            }
        }
        printf("\n");
    }

    // Exibe os comandos abaixo do mapa
    printf("\nComandos:\n");
    printf("w - Pular\n");
    printf("s - Deslizar\n");
    printf("a - Mover para a esquerda\n");
    printf("d - Mover para a direita\n");
    printf("q - Sair do jogo\n");
}

void gameLoop(GameState* g) {
    char input;
    while (g->running) {
        drawGame(g);

#ifdef _WIN32
        if (_kbhit()) input = _getch();
        else input = 0;
#else
        if (kbhit()) input = getchar();
        else input = 0;
#endif

        g->player->isJumping = 0;
        g->player->isSliding = 0;

        switch (input) {
            case 'a':
                if (g->player->lane > 0) g->player->lane--;
                break;
            case 'd':
                if (g->player->lane < LANES - 1) g->player->lane++;
                break;
            case 'w':
                g->player->isJumping = 1;
                g->player->jumpTime = 10; // Tempo de duração do pulo
                break;
            case 's':
                g->player->isSliding = 1;
                g->player->slideTime = 10; // Tempo de duração do deslize
                break;
            case 'q':
                g->running = 0;
                break;
        }

        // Controla o tempo do pulo e deslize
        if (g->player->jumpTime > 0) {
            g->player->jumpTime--;
        } else {
            g->player->isJumping = 0;
        }

        if (g->player->slideTime > 0) {
            g->player->slideTime--;
        } else {
            g->player->isSliding = 0;
        }

        if (checkCollision(g->player, g->map[g->mapPosition])) {
            drawGame(g);
            printf("\nGame Over! Pontos: %d | Moedas: %d\n", g->player->score, g->player->coins);
            g->running = 0;
            break;
        }

        collectItems(g->player, g->map[g->mapPosition]);
        shiftMap(g->map);
        g->player->score++;
        sleepMS(150);
    }
}

void freeGame(GameState* g) {
    free(g->player);
    for (int i = 0; i < MAP_SIZE; i++) {
        for (int j = 0; j < MAX_OBSTACLES; j++)
            if (g->map[i]->obstacles[j]) free(g->map[i]->obstacles[j]);
        for (int j = 0; j < MAX_ITEMS; j++)
            if (g->map[i]->items[j]) free(g->map[i]->items[j]);
        free(g->map[i]);
    }
    free(g->map);
    free(g);
}

int main() {
    srand(time(NULL));
    GameState* game = initGame();
    gameLoop(game);
    freeGame(game);
    return 0;
}


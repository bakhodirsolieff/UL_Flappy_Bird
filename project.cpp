#include <SDL2/SDL.h>
#include <iostream>
#include <cstdlib>
#include <ctime>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int PIPE_WIDTH = 80;
const int PIPE_GAP = 150;

const float GRAVITY = 0.5f;
const float JUMP_FORCE = -7;
const float PIPE_SPEED = 5;
const int MAX_SCORE = 100;

float birdX = 100;
float birdY = SCREEN_HEIGHT / 2;
float birdVelocity = 0;
int pipeX = SCREEN_WIDTH;
int gapY;
int score = 0;
bool gameStarted = false;
bool gameOver = false;

void drawRect(SDL_Renderer* renderer, int x, int y, int w, int h, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(renderer, &rect);
}

void drawBird(SDL_Renderer* renderer) {
    SDL_Color birdColor = {255, 255, 0, 255}; // Yellow color
    drawRect(renderer, birdX, birdY, 30, 30, birdColor); // Body
    SDL_Color wingColor = {255, 165, 0, 255};            // Orange for wings
    drawRect(renderer, birdX + 10, birdY + 5, 15, 10, wingColor); // Top wing
    drawRect(renderer, birdX + 10, birdY + 15, 15, 10, wingColor); // Bottom wing
}

void resetGame() {
    birdY = SCREEN_HEIGHT / 2;
    birdVelocity = 0;
    pipeX = SCREEN_WIDTH;
    gapY = rand() % (SCREEN_HEIGHT - PIPE_GAP - 50) + 50; // Randomize gap position
    score = 0;
    gameStarted = false;
    gameOver = false;
}

void handleInput(SDL_Event& e) {
    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
        if (gameOver) {
            resetGame(); // Restart the game on spacebar after game over
        } else {
            if (!gameStarted) {
                gameStarted = true;
            }
            birdVelocity = JUMP_FORCE; // Make the bird jump
        }
    }
}

void updateGame() {
    if (gameOver) return;

    if (gameStarted) {
        birdVelocity += GRAVITY;
        birdY += birdVelocity;

        pipeX -= PIPE_SPEED;

        if (pipeX + PIPE_WIDTH < 0) {
            pipeX = SCREEN_WIDTH;
            gapY = rand() % (SCREEN_HEIGHT - PIPE_GAP - 50) + 50;
            score += 10;
        }

        if (score >= MAX_SCORE) {
            gameOver = true;
        }

        if (birdY < 0 || birdY + 30 > SCREEN_HEIGHT) {
            gameOver = true;
        }

        if (pipeX < birdX + 30 && pipeX + PIPE_WIDTH > birdX) {
            if (birdY < gapY || birdY + 30 > gapY + PIPE_GAP) {
                gameOver = true;
            }
        }
    }
}

void renderGame(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 135, 206, 235, 255); // Sky blue background
    SDL_RenderClear(renderer);

    SDL_Color pipeColor = {34, 139, 34, 255}; // Green pipes
    drawRect(renderer, pipeX, 0, PIPE_WIDTH, gapY, pipeColor);                          // Top pipe
    drawRect(renderer, pipeX, gapY + PIPE_GAP, PIPE_WIDTH, SCREEN_HEIGHT - gapY - PIPE_GAP, pipeColor); // Bottom pipe

    SDL_Color groundColor = {139, 69, 19, 255}; // Brown ground
    drawRect(renderer, 0, SCREEN_HEIGHT - 20, SCREEN_WIDTH, 20, groundColor);

    drawBird(renderer);

    SDL_Color batteryColor = {0, 255, 0, 255};
    int batteryWidth = 300;
    int batteryHeight = 20;
    int filledWidth = (score * batteryWidth) / MAX_SCORE;

    SDL_Color batteryBackground = {169, 169, 169, 255};
    drawRect(renderer, (SCREEN_WIDTH - batteryWidth) / 2, 20, batteryWidth, batteryHeight, batteryBackground);

    drawRect(renderer, (SCREEN_WIDTH - batteryWidth) / 2, 20, filledWidth, batteryHeight, batteryColor);

    if (gameOver) {
        SDL_Color gameOverColor = {255, 0, 0, 255};
        drawRect(renderer, SCREEN_WIDTH / 4, SCREEN_HEIGHT / 3, SCREEN_WIDTH / 2, 50, gameOverColor);
    }

    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Flappy Bird in SDL2",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (!window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    srand(static_cast<unsigned>(time(0)));
    resetGame();

    bool running = true;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            } else {
                handleInput(e);
            }
        }

        updateGame();
        renderGame(renderer);

        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

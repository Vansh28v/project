#include <stdio.h>
#include <stdbool.h>
#include <SDL3/SDL.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PADDLE_WIDTH 20
#define PADDLE_HEIGHT 100
#define BALL_SIZE 15
#define PADDLE_SPEED 10
#define BALL_SPEED 5

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

struct Game {
    SDL_FRect paddle1, paddle2;
    SDL_FRect ball;
    float ball_dx, ball_dy;
    int score1, score2;
    bool running;
};

void initializeGame(struct Game* game) {
    // Initialize paddles
    game->paddle1.x = 50.0f;
    game->paddle1.y = SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2;
    game->paddle1.w = PADDLE_WIDTH;
    game->paddle1.h = PADDLE_HEIGHT;

    game->paddle2.x = SCREEN_WIDTH - 50.0f - PADDLE_WIDTH;
    game->paddle2.y = SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2;
    game->paddle2.w = PADDLE_WIDTH;
    game->paddle2.h = PADDLE_HEIGHT;

    // Initialize ball
    game->ball.x = SCREEN_WIDTH / 2 - BALL_SIZE / 2;
    game->ball.y = SCREEN_HEIGHT / 2 - BALL_SIZE / 2;
    game->ball.w = BALL_SIZE;
    game->ball.h = BALL_SIZE;

    game->ball_dx = BALL_SPEED;
    game->ball_dy = BALL_SPEED;
    game->score1 = 0;
    game->score2 = 0;
    game->running = true;
}

bool initializeSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow("Ping Pong Game",
                              SCREEN_WIDTH,
                              SCREEN_HEIGHT,
                              SDL_WINDOW_OPENGL);

    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, NULL, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void handleInput(struct Game* game) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT) {
            game->running = false;
        }
    }

    const Uint8* keystates = SDL_GetKeyboardState(NULL);

    // Player 1 controls (W/S)
    if (keystates[SDL_SCANCODE_W] && game->paddle1.y > 0) {
        game->paddle1.y -= PADDLE_SPEED;
    }
    if (keystates[SDL_SCANCODE_S] && game->paddle1.y < SCREEN_HEIGHT - PADDLE_HEIGHT) {
        game->paddle1.y += PADDLE_SPEED;
    }

    // Player 2 controls (Up/Down arrows)
    if (keystates[SDL_SCANCODE_UP] && game->paddle2.y > 0) {
        game->paddle2.y -= PADDLE_SPEED;
    }
    if (keystates[SDL_SCANCODE_DOWN] && game->paddle2.y < SCREEN_HEIGHT - PADDLE_HEIGHT) {
        game->paddle2.y += PADDLE_SPEED;
    }

    // Quit on Escape key
    if (keystates[SDL_SCANCODE_ESCAPE]) {
        game->running = false;
    }
}

void updateGame(struct Game* game) {
    // Move ball
    game->ball.x += game->ball_dx;
    game->ball.y += game->ball_dy;

    // Ball collision with top and bottom walls
    if (game->ball.y <= 0 || game->ball.y >= SCREEN_HEIGHT - BALL_SIZE) {
        game->ball_dy = -game->ball_dy;
    }

    // Ball collision with paddles
    if (SDL_HasRectIntersectionFloat(&game->ball, &game->paddle1) ||
        SDL_HasRectIntersectionFloat(&game->ball, &game->paddle2)) {
        game->ball_dx = -game->ball_dx;
    }

    // Ball goes out of bounds (scoring)
    if (game->ball.x <= 0) {
        game->score2++;
        game->ball.x = SCREEN_WIDTH / 2 - BALL_SIZE / 2;
        game->ball.y = SCREEN_HEIGHT / 2 - BALL_SIZE / 2;
        game->ball_dx = BALL_SPEED;
    }
    if (game->ball.x >= SCREEN_WIDTH) {
        game->score1++;
        game->ball.x = SCREEN_WIDTH / 2 - BALL_SIZE / 2;
        game->ball.y = SCREEN_HEIGHT / 2 - BALL_SIZE / 2;
        game->ball_dx = -BALL_SPEED;
    }

    // Game over condition
    if (game->score1 >= 5 || game->score2 >= 5) {
        game->running = false;
    }
}

void renderGame(struct Game* game) {
    // Clear screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw center line
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int y = 0; y < SCREEN_HEIGHT; y += 20) {
        SDL_FRect line = {SCREEN_WIDTH / 2 - 5, (float)y, 10, 10};
        SDL_RenderFillRect(renderer, &line);
    }

    // Draw paddles
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &game->paddle1);
    SDL_RenderFillRect(renderer, &game->paddle2);

    // Draw ball
    SDL_RenderFillRect(renderer, &game->ball);

    // Update screen
    SDL_RenderPresent(renderer);
}

void closeSDL() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    if (!initializeSDL()) {
        printf("Failed to initialize SDL!\n");
        return -1;
    }

    struct Game game;
    initializeGame(&game);

    // Game loop
    while (game.running) {
        handleInput(&game);
        updateGame(&game);
        renderGame(&game);
        SDL_Delay(16); // ~60 FPS
    }

    // Display final score
    printf("Game Over!\n");
    printf("Final Score - Player 1: %d, Player 2: %d\n", game.score1, game.score2);
    
    if (game.score1 > game.score2) {
        printf("Player 1 Wins!\n");
    } else if (game.score2 > game.score1) {
        printf("Player 2 Wins!\n");
    } else {
        printf("It's a Tie!\n");
    }

    closeSDL();
    return 0;
}

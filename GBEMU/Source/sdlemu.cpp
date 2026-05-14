#include "sdlemu.hpp"

SDLEMU::SDLEMU(const char* title, int width, int height)
    : width(width), height(height) {
    SDL_strlcpy(this->title, title, sizeof(this->title));

    SDL_strlcpy(projectPath, SDL_GetBasePath(), sizeof(projectPath));
    SDL_Log("SDLEMU: project path: %s", projectPath);

    SDL_strlcpy(resourcePath, projectPath, sizeof(resourcePath));
    SDL_strlcat(resourcePath, "../../Resources/", sizeof(resourcePath));
    SDL_Log("SDLEMU: resource path: %s", resourcePath);
}

SDLEMU::~SDLEMU() {
    stop();
}

bool SDLEMU::start() {
    // Initialize SDL Subsystems
    if (SDL_Init(SDL_INIT_VIDEO) == false) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        stop();
        return false;
    }

    // Create the Window and Renderer
    if (!SDL_CreateWindowAndRenderer(title, 1024, 768, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("SDLEMU: create window and renderer failed: %s", SDL_GetError());
        stop();
        return false;
    }

    // Set the Logical Presentation Size
    if (!SDL_SetRenderLogicalPresentation(renderer, width, height, SDL_LOGICAL_PRESENTATION_LETTERBOX)) {
        SDL_Log("SDLEMU: render logical presentation failed: %s", SDL_GetError());
        stop();
        return false;
    }

    const char *filePath = "Assets/Zelda's Adventure/23322261-zeldas-adventure-game-boy-title-screen.png";
    char filename[256];
    SDL_strlcpy(filename, resourcePath, sizeof(filename));
    SDL_strlcat(filename, filePath, sizeof(filename));
    SDL_Log("SDLEMU: filename path: %s", filename);

    SDL_Surface *surface = SDL_LoadPNG(filename);
    if (!surface) {
        SDL_Log("SDLEMU: loading PNG as Surface failed: %s", SDL_GetError());
        stop();
        return false;
    }

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_Log("SDLEMU: loading PNG as Texture failed: %s", SDL_GetError());
        stop();
        return false;
    }
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    // Make it visible
    SDL_ShowWindow(window);

    // Bring it to the top
    SDL_RaiseWindow(window); 
    
    previousTime = SDL_GetTicksNS();
    running = true;
    return true; // Add missing return statement
}

    // Create the Window and Renderer
    if (!SDL_CreateWindowAndRenderer(title, 1024, 768, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("SDLEMU: create window and renderer failed: %s", SDL_GetError());
        stop();
        return false;
    }

    // Set the Logical Presentation Size
    if (!SDL_SetRenderLogicalPresentation(renderer, width, height, SDL_LOGICAL_PRESENTATION_LETTERBOX)) {
        SDL_Log("SDLEMU: render logical presentation failed: %s", SDL_GetError());
        stop();
        return false;
    }

    const char *filePath = "Assets/Zelda's Adventure/23322261-zeldas-adventure-game-boy-title-screen.png";
    char filename[256];
    SDL_strlcpy(filename, resourcePath, sizeof(filename));
    SDL_strlcat(filename, filePath, sizeof(filename));
    SDL_Log("SDLEMU: filename path: %s", filename);

    SDL_Surface *surface = SDL_LoadPNG(filename);
    if (!surface) {
        SDL_Log("SDLEMU: loading PNG as Surface failed: %s", SDL_GetError());
        stop();
        return false;
    }

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_Log("SDLEMU: loading PNG as Texture failed: %s", SDL_GetError());
        stop();
        return false;
    }
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    // Make it visible
    SDL_ShowWindow(window);

    // Bring it to the top
    SDL_RaiseWindow(window); 

    
    previousTime = SDL_GetTicksNS();
    running = true;
}

void SDLEMU::stop() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_Quit();
    running = false;
}

void SDLEMU::run() {
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_ESCAPE) {
                    running = false;
                }
            }
        }

        currentTime = SDL_GetTicksNS();
        deltaTime = currentTime - previousTime;

        update();
        render();

        previousTime = currentTime;
    }
}

void SDLEMU::update() {
    
}

void SDLEMU::render() {
     // Clear with black
     SDL_SetRenderDrawColor(renderer, 0x70, 0x01, 0x93, 255);
     SDL_RenderClear(renderer);

      // Render the loaded image
      if (texture) {
        SDL_RenderTexture(renderer, texture, NULL, NULL);
      }

     SDL_RenderPresent(renderer);
}


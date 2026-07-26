#include <SDL3/SDL.h>
#include <iostream>

int main()
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK))
    {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << '\n';
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Controller Test",
        640,
        480,
        0
    );

    int count = 0;
    SDL_JoystickID* joysticks = SDL_GetJoysticks(&count);

    if (count == 0)
    {
        std::cout << "No joysticks found.\n";
        return 1;
    }

    SDL_Joystick* joystick = SDL_OpenJoystick(joysticks[0]);

    if (!joystick)
    {
        std::cerr << "Failed to open joystick: "
                  << SDL_GetError() << '\n';
        return 1;
    }

    std::cout << "Opened: "
              << SDL_GetJoystickName(joystick)
              << '\n';

    SDL_free(joysticks);

    bool running = true;

    while (running)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                running = false;
                break;

            case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
                std::cout
                    << "BUTTON "
                    << static_cast<int>(event.jbutton.button)
                    << " PRESSED\n";
                break;

            case SDL_EVENT_JOYSTICK_BUTTON_UP:
                std::cout
                    << "BUTTON "
                    << static_cast<int>(event.jbutton.button)
                    << " RELEASED\n";
                break;

            case SDL_EVENT_JOYSTICK_AXIS_MOTION:
                if (event.jaxis.value > 10000 ||
                    event.jaxis.value < -10000)
                {
                    std::cout
                        << "AXIS "
                        << static_cast<int>(event.jaxis.axis)
                        << " = "
                        << event.jaxis.value
                        << '\n';
                }
                break;

            case SDL_EVENT_JOYSTICK_HAT_MOTION:
                std::cout
                    << "HAT "
                    << static_cast<int>(event.jhat.hat)
                    << " = "
                    << static_cast<int>(event.jhat.value)
                    << '\n';
                break;
            }
        }
    }

    SDL_CloseJoystick(joystick);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

#include "Game.h"
#include "pch.h"

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <iostream>
#include <stdlib.h>


int main()
{
#if (defined(_MSC_VER) && defined(_DEBUG))
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    try
    {
        auto game = std::make_unique<Game>();
        game->run();
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
        return -1;
    }

    return 0;
}

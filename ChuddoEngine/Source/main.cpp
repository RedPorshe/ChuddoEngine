// main.cpp
#include "Core/Engine.h"
#include <iostream>

int main ()
    {
    std::cout << "========================================\n";
    std::cout << "   CHUDDO ENGINE - TEST   \n";
    std::cout << "========================================\n";

    // 1. Инициализация движка
    if (!CEngine::InitializeEngine ())
        {
        std::cerr << "Failed to initialize engine!\n";
        return -1;
        }

        // 2. Получаем экземпляр движка
    auto & eng = CEngine::Get ();

    // 3. Запуск
    eng.Start ();

    // 4. Очистка
    CEngine::ShutdownEngine ();  // Вызовет delete Instance

    std::cout << "\n========================================\n";
    std::cout << "            TEST COMPLETE              \n";
    std::cout << "========================================\n";

    return 0;
    }
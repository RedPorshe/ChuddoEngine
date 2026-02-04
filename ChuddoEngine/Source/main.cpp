// main.cpp
#include "Core/Engine.h"
#include <iostream>

int main ()
    {
    std::cout << "========================================\n";
    std::cout << "   CHUDDO ENGINE - TEST   \n";
    std::cout << "========================================\n";
       
    if (!CEngine::InitializeEngine ())
        {
        std::cerr << "Failed to initialize engine!\n";
        return -1;
        }

    auto & eng = CEngine::Get ();

   
    eng.Start ();

    
    CEngine::ShutdownEngine (); 

    std::cout << "\n========================================\n";
    std::cout << "            TEST COMPLETE              \n";
    std::cout << "========================================\n";

    return 0;
    }
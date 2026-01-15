#include "CoreMinimal.h"



bool GIsRequestingExit = false;
FEngineLoop GEngine;



int main()
{
	setlocale(LC_ALL, "ru");
	int result = 0;
	try
	{
		if (GEngine.Init())
		{
			GEngine.Start();
		}
	result = EXIT_SUCCESS;
	}
	catch (const std::exception& e)
	{
		std::cout << ("Unhandled exception: {}", e.what());
		result = EXIT_FAILURE;
	}
	catch (...)
	{
		std::cout << "Unhandled unknown exception.";
		result = EXIT_FAILURE;
	}
	
	// Run test for CCharacter movement
	

	return result;
}
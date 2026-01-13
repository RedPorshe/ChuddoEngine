#include "CoreMinimal.h"
#include "Utils/Math/Matrix4.h"

bool GIsRequestingExit = false;
FEngineLoop GEngine;



int main()
{
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

	return result;
}
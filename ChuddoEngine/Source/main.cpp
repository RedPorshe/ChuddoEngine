#include "CoreMinimal.h"

#include <filesystem>

namespace fs = std::filesystem;

bool GIsRequestingExit = false;
FEngineLoop GEngine;



int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "ru");
	if (argc > 0)
	{
		fs::path exePath = fs::absolute(argv[0]);
		fs::current_path(exePath.parent_path());
		std::cout << "Working directory set to: " << fs::current_path().string() << std::endl;
	}
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
#include "Utils/MHBlueprintFunctionLibrary.h"

#include "Misc/PackageName.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"

bool UMHBlueprintFunctionLibrary::DoesLevelExist(const FString& LevelName)
{
	FString MapName = LevelName;

	// Ensure the package name is in the correct format
	if (FPackageName::IsShortPackageName(MapName))
	{
		// Prepend the /Game/ directory if it's a short package name
		MapName = "/Game/" + MapName;
	}
	else if (!MapName.StartsWith("/"))
	{
		// Ensure the package name starts with a '/'
		MapName = "/" + MapName;
	}

	// Remove any file extension
	MapName = FPaths::ChangeExtension(MapName, "");

	FString PackageFilePath;
	// Check if the package (level) exists
	return FPackageName::DoesPackageExist(MapName, &PackageFilePath);
}


#include "Error.h"
#include <iostream>

void Error(const char *type, const char *errorMsg)
{
	std::cout << "error - " << type << ": " << errorMsg << std::endl;

	exit(EXIT_FAILURE);
}
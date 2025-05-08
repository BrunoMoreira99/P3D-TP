#pragma once

#include "Roose/Core/Application.h"

#ifdef RS_PLATFORM_WINDOWS

inline int main(int argc, char** argv)
{
	const auto app = Roose::CreateApplication({ argc, argv });
	app->Run();
	delete app;
}

#endif

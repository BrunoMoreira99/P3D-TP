#pragma once

#include "Roose/Core/Application.h"

inline int main(int argc, char** argv)
{
	const auto app = Roose::CreateApplication({ argc, argv });
	app->Run();
	delete app;
}

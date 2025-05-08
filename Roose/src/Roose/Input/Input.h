#pragma once

#include "Roose/Input/KeyCode.h"
#include "Roose/Input/MouseCode.h"

#include <glm/glm.hpp>

namespace Roose {

	/**
	 * @class Input
	 * @brief Provides an interface for polling input states such as keyboard and mouse input.
	 */
	class Input
	{
	public:
		/**
		 * @brief Checks if the specific key is currently pressed.
		 * @param key The key code to check.
		 * @return True if the key is pressed, false otherwise.
		 */
		static bool IsKeyPressed(KeyCode key);

		/**
		 * @brief Checks if the specific mouse button is currently pressed.
		 * @param button The mouse button code to check.
		 * @return True if the mouse button is pressed, false otherwise.
		 */
		static bool IsMouseButtonPressed(MouseCode button);

		/**
		 * @brief Gets the current position of the mouse cursor.
		 * @return A glm::vec2 containing the x and y coordinates of the mouse cursor.
		 */
		static glm::vec2 GetMousePosition();

		/**
		 * @brief Gets the current x-coordinate of the mouse cursor.
		 * @return The x-coordinate of the mouse cursor as a float.
		 */
		static float GetMouseX();

		/**
		 * @brief Gets the current y-coordinate of the mouse cursor.
		 * @return The y-coordinate of the mouse cursor as a float.
		 */
		static float GetMouseY();
	};

}

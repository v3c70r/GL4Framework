#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <chrono>
//#include "Globals.hpp"
#include "imgui/imgui.h"



	void ImImpl_RenderDrawLists(ImDrawList** const cmd_lists, int cmd_lists_count);

	void scroll_callback(double xoffset, double yoffset);

	void key_callback(int key, int action);

	void char_callback(unsigned int c);

	void InitImGui(GLuint W, GLuint H);

	void UpdateImGui(GLFWwindow* pWindow);

#pragma once

//
// Precompiled Header
//

#include <Windows.h>
#include <assert.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <list>
#include <algorithm>
#include <map>
#include <vector>
#include <string>
#include <tuple>
#include <array>

#include "Utils.h"
#include "Log.h"

#include "Singleton.h"
#include "DynamicPool.h"

#include "TextManager.h"

#define GLEW_STATIC
#include <gl/glew.h>
#include <gl/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"

IncludeDir = {}
IncludeDir["GLEW"] = "%{wks.location}/Roose/vendor/GLEW/include"
IncludeDir["GLFW"] = "%{wks.location}/Roose/vendor/GLFW/include"
IncludeDir["glm"] = "%{wks.location}/Roose/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/Roose/vendor/stb_image"

Library = {}
Library["GLEW"] = "%{wks.location}/Roose/vendor/GLEW/lib/glew32s.lib"
Library["GLFW"] = "%{wks.location}/Roose/vendor/GLFW/lib/glfw3.lib"
Library["OpenGL"] = "opengl32.lib"

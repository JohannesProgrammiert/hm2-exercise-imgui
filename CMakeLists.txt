CMAKE_MINIMUM_REQUIRED(VERSION 3.12)

project(plottings LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

# --- library dependencies ------------------------------------------------------
find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)
# -------------------------------------------------------------------------------


# --- imgui library for UI drawing ----------------------------------------------
add_library(imgui
  libraries/imgui/imgui.cpp
  libraries/imgui/imgui_draw.cpp
  libraries/imgui/imgui_tables.cpp
  libraries/imgui/imgui_widgets.cpp
  libraries/imgui/backends/imgui_impl_glfw.cpp
  libraries/imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
  libraries/imgui
  libraries/imgui/backends
)
# -------------------------------------------------------------------------------


# --- implot addon for imgui for extended plotting ------------------------------
add_library(implot
  libraries/implot/implot.cpp
  libraries/implot/implot_items.cpp
)

target_include_directories(implot PUBLIC
  libraries/implot
)

target_link_libraries(implot PRIVATE
  imgui
)
# -------------------------------------------------------------------------------


# --- main application ----------------------------------------------------------
add_executable(${PROJECT_NAME}
  src/main.cpp
  src/ui.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  implot
  imgui
  glfw
  OpenGL::GL
)
# -------------------------------------------------------------------------------

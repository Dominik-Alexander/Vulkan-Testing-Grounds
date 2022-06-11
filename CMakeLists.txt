cmake_minimum_required(VERSION 3.10)

# Project name
project(Vulkan-Testing-Grounds)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

set(SOURCE_FILES src/main.cpp src/vulkan_base/vulkan_device.cpp src/vulkan_base/vulkan_swapchain.cpp src/vulkan_base/vulkan_renderpass.cpp src/vulkan_base/vulkan_pipeline.cpp)

# Find SDL2
add_subdirectory(libs/SDL)

# Find Vulkan
find_package(Vulkan REQUIRED)

# Compile shaders
if(UNIX)
add_custom_target(build_shaders ALL
	COMMAND "${PROJECT_SOURCE_DIR}/shaders/compile.sh"
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/shaders"
)
endif(UNIX)

if(WIN32)
add_custom_target(build_shaders ALL
	COMMAND "${PROJECT_SOURCE_DIR}/shaders/compile.bat"
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/shaders"
)
endif(WIN32)

# Vulkan-Testing_Grounds executable
add_executable(vulkan-testing-grounds ${SOURCE_FILES})
add_dependencies(vulkan-testing-grounds build_shaders)
target_include_directories(vulkan-testing-grounds PUBLIC libs/SDL/include)
target_link_libraries(vulkan-testing-grounds PUBLIC SDL2-static)
target_include_directories(vulkan-testing-grounds PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(vulkan-testing-grounds PUBLIC ${Vulkan_LIBRARIES})

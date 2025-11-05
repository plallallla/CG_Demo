cmake_minimum_required(VERSION 3.10.0)
project(CG_Demo VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(BaseOpenGL)

find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

file(GLOB TARGET_SOURCES "src/*/main.cpp")

foreach(main_file ${TARGET_SOURCES})
    get_filename_component(dir_name ${main_file} DIRECTORY)
    get_filename_component(target_name ${dir_name} NAME)

    # 创建可执行文件
    add_executable(${target_name} ${main_file})

    # 配置第三方库
    target_link_libraries(${target_name} PRIVATE glfw glad::glad assimp::assimp spdlog::spdlog_header_only imgui::imgui)
    target_include_directories(${target_name} PRIVATE ${Stb_INCLUDE_DIR})

endforeach()

cmake_minimum_required(VERSION 3.10)
project(MyFFmpegProject)

# 设置C++标准
set(CMAKE_CXX_STANDARD 14)

# 设置项目可执行文件输出的路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# 设置项目库文件输出的路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# 包含头文件
include_directories(/usr/local/ffmpeg4.2/include)
# 设置项目库文件搜索路径 -L
link_directories(/usr/local/ffmpeg4.2/lib)
# 设置源文件
set(SRC_LIST src/main.c)

# 指定可执行文件的名称和源文件
add_executable(remuxing ${SRC_LIST})
# 链接FFmpeg库
target_link_libraries(remuxing avcodec avformat avutil avfilter avdevice swresample swscale postproc)
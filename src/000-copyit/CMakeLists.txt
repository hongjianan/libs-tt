

########## cmake版本   ##########
#cmake_minmum_required(VERSION 2.5 FATAL_ERROR)

########## 工程   ##########
project(test)

########## 编译选项   ##########
##### c++版本  #####
set(CMAKE_CXX_STANDARD 11)
#add_definitions(-std=c++11)
##### 编译信息输出  #####
#set(CMAKE_VERBOSE_MAKEFILE ON)
##### 优化  #####
add_definitions(-O0  -g3)
##### -D #####

########## 源文件目录  ##########
##### 单独源文件  #####
#set(SOURCE_FILES xxx.cpp)
##### 源文件目录  #####
aux_source_directory(.  SOURCE_FILES)
message(STATUS "=============== compile file ===============")
message(STATUS ${SOURCE_FILES})
message(STATUS "============================================")

########## 头文件搜索路径  ##########
include_directories(${PROJECT_SOURCE_DIR}/..)

########## 链接库  ##########
##### 库路径  #####
##### 链接库  #####
#target_link_libraries(pthread  rt)

########## 输出   ##########
##### 路径 #####
### 执行文件
#set(EXECUTABLE_OUTPUT_PATH  PROJECT_BINARY_DIR)
### 链接库
#set(LIBRARY_OUTPUT_PATH  PROJECT_BINARY_DIR)

##### 输出文件  #####
add_executable(${PROJECT_NAME}  ${SOURCE_FILES})

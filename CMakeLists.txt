#指定cmake最低版本
cmake_minimum_required(VERSION 3.16)

# set(CMAKE_AR  "C:/msys64/mingw64/bin/ar.exe")

# 指定C编译器源
# set(CMAKE_C_COMPILER "C:/msys64/mingw64/bin/gcc.exe")

# 指定C++编译器源
# set(CMAKE_CXX_COMPILER "C:/msys64/mingw64/bin/g++.exe")

# set(CMAKE_CXX_COMPILER_FORCED TRUE)
# set(CMAKE_C_COMPILER_FORCED TRUE)

# set(CPLUS_INCLUDE_PATH "C:/msys64/mingw64/include/c++")
# set(ENV{CPLUS_INCLUDE_PATH} ${CPLUS_INCLUDE_PATH})

#project 会定义两个变量：PROJECT_BINARY_DIR 和 PROJECT_SOURCE_DIR
#PROJECT_BINARY_DIR cmake所在文件夹build 文件
#PROJECT_SOURCE_DIR cmake所在文件夹
project(FILE_BACKUP)

#可执行文件名变量
set(EXE_FILE_NAME file_backup)


# add_compile_options(-DENABLE_PRECOMPILED_HEADERS=OFF)
# add_compile_options(-DCMAKE_C_COMPILER="C:/msys64/mingw64/bin/gcc.exe")
# add_compile_options(-DCMAKE_CXX_COMPILER="C:/msys64/mingw64/bin/g++.exe")

#查找./src 下的所有源文件并保存到SRC_FILES变量名中 
aux_source_directory(./src SRC_FILES)
aux_source_directory(./test TEST_FILES)
include_directories(./include)


# openssl
set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)

#添加编译选项
add_compile_options(-w -O2 -std=c++17 -lcrypto)

#根据源文件生成可执行文件
add_executable(${EXE_FILE_NAME} ${SRC_FILES} ${TEST_FILES})

# find_package(OpenSSL REQUIRED)

#链接openSSL::Crypto库，用于加密解密
target_link_libraries(${EXE_FILE_NAME} OpenSSL::Crypto)

#链接头文件目录
# target_include_directories(${EXE_FILE_NAME} "C:/msys64/mingw64/include")
install(TARGETS ${EXE_FILE_NAME} DESTINATION bin)
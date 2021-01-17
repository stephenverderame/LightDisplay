# GLAD source directory
# Place glad files in this directory generated from https://glad.dav1d.de/
# File structure should be
# glad
#  -- include
#     -- glad
#        -- glad.h
#     -- KHR
#        -- khrplatform.h
#  -- src
#     -- glad.c

cmake_minimum_required (VERSION 3.8)
project ("glad")

add_library(glad "include/glad/glad.h" "include/KHR/khrplatform.h" "src/glad.c")
set(GLAD_INCLUDE_DIR "include")
target_include_directories(glad PUBLIC "${GLAD_INCLUDE_DIR}")
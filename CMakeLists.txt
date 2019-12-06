cmake_minimum_required(VERSION 2.6) # compatibilités de CMake
project(main) # nom du projet

set(CMAKE_BUILD_TYPE debug) # compilation en mode debug
set(CMAKE_CXX_FLAGS "-Wall -Wextra -std=c++14") # options

file( # liste des fichiers à compiler
	GLOB_RECURSE # recherche récursive
	source_files # fichiers listés dans une variable 
	src/*.[ch]pp # forme des fichiers à rechercher
)

add_executable( # création de l’exécutable binaire du projet
	main_exe # nom du binaire
	${source_files} # liste des fichiers compilés pour le linking
)

enable_testing()

add_subdirectory (tests)
include_directories (include/.)

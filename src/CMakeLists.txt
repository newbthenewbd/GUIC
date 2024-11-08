set(TARGET_NAME ${CMAKE_PROJECT_NAME})

#file(GLOB_RECURSE src CONFIGURE_DEPENDS "*.c" "*.cpp" "*.ui")

file(GLOB_RECURSE OPENCORR_SRC CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/ext/OpenCorr/src/*.c" "${CMAKE_SOURCE_DIR}/ext/OpenCorr/src/*.cpp")
file(GLOB_RECURSE OPENCORR_H CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/ext/OpenCorr/src/*.h" "${CMAKE_SOURCE_DIR}/ext/OpenCorr/src/*.hpp")

add_executable(${TARGET_NAME}
	${OPENCORR_SRC}
	main.cpp
	mainwindow.h mainwindow.cpp mainwindow.ui
	main.h
	projecttab.h projecttab.cpp projecttab.ui
	imagelistwidget.h
	imagelistitem.h
	tinycolormap.hpp
	colormapwidget.h
	colorpickerbutton.h
	sceneview.h
	sceneview.cpp
	colormapwidget.cpp
)

set_target_properties(${TARGET_NAME} PROPERTIES
	WIN32_EXECUTABLE ON
	MACOSX_BUNDLE ON
)

target_link_libraries(${TARGET_NAME} PRIVATE Qt5::Widgets)

target_include_directories(${TARGET_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/ext/verdigris/src")

target_link_libraries(${TARGET_NAME} PRIVATE opencv_core)
if(NOT GUIC_USE_SYSTEM_OPENCV)
	target_include_directories(${TARGET_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/ext/opencv-4.10.0/include" "${CMAKE_SOURCE_DIR}/ext/opencv-4.10.0/modules/core/include")
endif()

target_include_directories(${TARGET_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/ext/eigen" "${CMAKE_SOURCE_DIR}/ext/eigen/Eigen")

target_link_libraries(${TARGET_NAME} PRIVATE fftw3 fftw3f)
target_include_directories(${TARGET_NAME} PRIVATE "${fftw_SOURCE_DIR}/api")

find_package(Threads REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE nanoflann::nanoflann)

#if(APPLE)
#	if(CMAKE_C_COMPILER_ID MATCHES "AppleClang" OR CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
#		set(OpenMP_omp_LIBRARY omp)
#		set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp")
#		set(OpenMP_C_LIB_NAMES "omp")
#		set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp")
#		set(OpenMP_CXX_LIB_NAMES "omp")
#		find_path(OpenMP_LIBRARIES libomp.dylib "/opt/local/lib/libomp" "/usr/local/lib/libomp" "/opt/homebrew/lib/libomp")
#		find_path(OpenMP_INCLUDES omp.h "/opt/local/include/libomp" "/usr/local/include/libomp" "/opt/homebrew/include/libomp")
#	endif()
#endif()
#find_package(OpenMP REQUIRED)
#target_link_libraries(${TARGET_NAME} PRIVATE OpenMP::OpenMP_CXX)
#if(APPLE)
#	if(CMAKE_C_COMPILER_ID MATCHES "AppleClang" OR CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
#		target_include_directories(${TARGET_NAME} PRIVATE ${OpenMP_INCLUDES})
#		target_link_directories(${TARGET_NAME} PRIVATE ${OpenMP_LIBRARIES})
#	endif()
#endif()

target_include_directories(${TARGET_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/ext/OpenCorr/src")
target_precompile_headers(${TARGET_NAME} PRIVATE ${OPENCORR_H})

target_include_directories(${TARGET_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/src")
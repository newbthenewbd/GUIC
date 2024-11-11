set(TARGET_NAME ${CMAKE_PROJECT_NAME})

#file(GLOB_RECURSE src CONFIGURE_DEPENDS "*.c" "*.cpp" "*.ui")

add_executable(${TARGET_NAME}
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

target_link_libraries(${TARGET_NAME} PRIVATE opencv_core opencv_features2d opencv_flann opencv_imgcodecs opencv_imgproc)
if(NOT GUIC_USE_SYSTEM_OPENCV)
	target_include_directories(${TARGET_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/ext/opencv-4.10.0/include" "${CMAKE_SOURCE_DIR}/ext/opencv-4.10.0/modules/core/include" "${CMAKE_SOURCE_DIR}/ext/opencv-4.10.0/modules/features2d/include" "${CMAKE_SOURCE_DIR}/ext/opencv-4.10.0/modules/flann/include" "${CMAKE_SOURCE_DIR}/ext/opencv-4.10.0/modules/imgcodecs/include" "${CMAKE_SOURCE_DIR}/ext/opencv-4.10.0/modules/imgproc/include")
endif()

if(GUIC_USE_SYSTEM_QT)
	target_link_libraries(${TARGET_NAME} PRIVATE Qt5::Widgets)
elseif(TRUE)
	add_dependencies(${TARGET_NAME} Qt5)
	set_target_properties(${TARGET_NAME} PROPERTIES
		AUTOUIC_EXECUTABLE ${QT_BUILD_DIR}/bin/uic
	)
	if(WIN32)
		target_link_libraries(${TARGET_NAME} PRIVATE ws2_32)
	endif()
	target_link_directories(${TARGET_NAME} BEFORE PRIVATE ${QT_BUILD_DIR}/lib ${QT_BUILD_DIR}/plugins/platforms)
	#target_link_libraries(${TARGET_NAME} PRIVATE Qt5Widgets qwayland-egl wayland-egl qwayland-generic Qt5WaylandClient xkbcommon wayland-client wayland-cursor qxcb qxcb-egl-integration qxcb-glx-integration xcb-glx Qt5XcbQpa Xi SM ICE xcb-render xcb-image xcb-icccm xcb-sync xcb-xfixes xcb-shm xcb-randr xcb-shape xcb-keysyms qdds qicns qico qjp2 qmng qtga qtiff qwbmp qwebp qeglfs-x11-integration X11-xcb xcb Qt5EglDeviceIntegration Qt5PlatformSupport fontconfig freetype Qt5DBus Xrender Xext X11 Qt5Gui png qtharfbuzzng Qt5Core z icui18n icuuc icudata qtpcre m dl rt EGL GL pthread)
	# From https://bugreports.qt.io/browse/QTBUG-38913 (accessed 2024/11/11; thanks!)
	if(WIN32)
		target_link_libraries(${TARGET_NAME} PRIVATE Qt5Widgets qwindows Qt5PlatformSupport Qt5Gui Qt5Core qtpcre m opengl32 ws2_32)
	elseif(APPLE)
		target_link_libraries(${TARGET_NAME} PRIVATE Qt5Widgets Qt5PlatformSupport Qt5Gui Qt5Core qtpcre m)
	else()
		target_link_libraries(${TARGET_NAME} PRIVATE Qt5Widgets Qt5PlatformSupport Qt5Gui Qt5Core qtpcre m GL pthread)
	endif()
	target_compile_options(${TARGET_NAME} PRIVATE -I${QT_BUILD_DIR}/include -I${QT_BUILD_DIR}/include/QtCore -I${QT_BUILD_DIR}/include/QtGui -I${QT_BUILD_DIR}/include/QtWidgets)
else()
	add_dependencies(${TARGET_NAME} Qt5)
	set_target_properties(${TARGET_NAME} PROPERTIES
		AUTOUIC_EXECUTABLE ${QT_BUILD_DIR}/bin/uic
	)
	if(WIN32)
		set(QT_LIBS ${QT_BUILD_DIR}/bin/Qt5Core.dll ${QT_BUILD_DIR}/bin/Qt5Gui.dll ${QT_BUILD_DIR}/bin/Qt5Widgets.dll)
	else()
		find_library(QT_CORE NAMES Qt5Core QtCore PATHS ${QT_BUILD_DIR}/bin ${QT_BUILD_DIR}/lib NO_DEFAULT_PATH)
		find_library(QT_GUI NAMES Qt5Gui QtGui PATHS ${QT_BUILD_DIR}/bin ${QT_BUILD_DIR}/lib NO_DEFAULT_PATH)
		find_library(QT_WIDGETS NAMES Qt5Widgets QtWidgets PATHS ${QT_BUILD_DIR}/bin ${QT_BUILD_DIR}/lib NO_DEFAULT_PATH)
		set(QT_LIBS ${QT_CORE} ${QT_GUI} ${QT_WIDGETS})
	endif()
	target_link_libraries(${TARGET_NAME} PRIVATE ${QT_LIBS})
	if(APPLE)
		target_compile_options(${TARGET_NAME} PRIVATE -I${QT_BUILD_DIR}/lib/QtCore.framework/Headers -I${QT_BUILD_DIR}/lib/QtGui.framework/Headers -I${QT_BUILD_DIR}/lib/QtWidgets.framework/Headers)
	else()
		target_compile_options(${TARGET_NAME} PRIVATE -I${QT_BUILD_DIR}/include -I${QT_BUILD_DIR}/include/QtCore -I${QT_BUILD_DIR}/include/QtGui -I${QT_BUILD_DIR}/include/QtWidgets)
		foreach(lib ${QT_LIBS})
			add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_if_different
				${lib}
				${CMAKE_BINARY_DIR}/bin
			)
		endforeach()
	endif()
endif()

target_include_directories(${TARGET_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/ext/verdigris/src")

target_include_directories(${TARGET_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/ext/eigen" "${CMAKE_SOURCE_DIR}/ext/eigen/Eigen")

target_link_libraries(${TARGET_NAME} PRIVATE fftw3 fftw3f)
target_include_directories(${TARGET_NAME} PRIVATE "${fftw_SOURCE_DIR}/api")

find_package(Threads REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE nanoflann::nanoflann)

if(APPLE)
	if(CMAKE_C_COMPILER_ID MATCHES "AppleClang" OR CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
		set(OpenMP_omp_LIBRARY omp)
		set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp")
		set(OpenMP_C_LIB_NAMES "omp")
		set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp")
		set(OpenMP_CXX_LIB_NAMES "omp")
		find_path(OpenMP_LIBRARIES libomp.dylib "/opt/local/lib/libomp" "/usr/local/lib/libomp" "/opt/homebrew/lib/libomp")
		find_path(OpenMP_INCLUDES omp.h "/opt/local/include/libomp" "/usr/local/include/libomp" "/opt/homebrew/include/libomp")
	endif()
endif()
find_package(OpenMP REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE OpenMP::OpenMP_CXX)
if(APPLE)
	if(CMAKE_C_COMPILER_ID MATCHES "AppleClang" OR CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
		target_include_directories(${TARGET_NAME} PRIVATE ${OpenMP_INCLUDES})
		target_link_directories(${TARGET_NAME} PRIVATE ${OpenMP_LIBRARIES})
	endif()
endif()

target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)

file(GLOB_RECURSE OPENCORR_SRC CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/ext/OpenCorr/src/*.c" "${CMAKE_SOURCE_DIR}/ext/OpenCorr/src/*.cpp")
file(GLOB_RECURSE OPENCORR_H CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/ext/OpenCorr/src/*.h" "${CMAKE_SOURCE_DIR}/ext/OpenCorr/src/*.hpp")

target_sources(${TARGET_NAME} PRIVATE ${OPENCORR_SRC})


target_include_directories(${TARGET_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/ext/OpenCorr/src")
#target_precompile_headers(${TARGET_NAME} PRIVATE ${OPENCORR_H})

target_link_options(${TARGET_NAME} PRIVATE -static-libgcc -static-libstdc++)
if(MINGW)
	target_link_libraries(${TARGET_NAME} PRIVATE -static winpthread gomp -dynamic)
endif()
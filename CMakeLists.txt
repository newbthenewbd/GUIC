cmake_minimum_required(VERSION 3.16)

project(GUIC)

include(ExternalProject)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

if(NOT DEFINED CMAKE_BUILD_WITH_INSTALL_RPATH)
	set(CMAKE_BUILD_WITH_INSTALL_RPATH true)
endif()

if(NOT DEFINED CMAKE_INSTALL_RPATH)
	set(CMAKE_INSTALL_RPATH $ORIGIN)
endif()

if(NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

if(NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

option(GUIC_USE_SYSTEM_QT "Use system-provided Qt5" OFF)
if(GUIC_USE_SYSTEM_QT)
	find_package(Qt5 REQUIRED COMPONENTS Widgets)
else()
	# Version 5.6.3 unfortunately did not yet use CMake for building itself,
	# so it's a little bit harder
	set(QT_CONFIGURE ${CMAKE_SOURCE_DIR}/ext/qt-everywhere-opensource-src-5.6.3-modified/configure -static -release -opensource -confirm-license -no-openssl -no-harfbuzz -make libs -skip qt3d -skip qtactiveqt -skip qtandroidextras -skip qtcanvas3d -skip qtconnectivity -skip qtdeclarative -skip qtdoc -skip qtenginio -skip qtgraphicaleffects -skip qtimageformats -skip qtlocation -skip qtmacextras -skip qtmultimedia -skip qtquickcontrols -skip qtquickcontrols2 -skip qtscript -skip qtsensors -skip qtserialbus -skip qtserialport -skip qtsvg -skip qttools -skip qttranslations -skip qtwayland -skip qtwebchannel -skip qtwebengine -skip qtwebsockets -skip qtwebview -skip qtwinextras -skip qtx11extras -skip qtxmlpatterns -prefix ${CMAKE_BINARY_DIR}/ext/qt-everywhere-opensource-src-5.6.3-modified)
	include(ProcessorCount)
	ProcessorCount(NUM_PROCESSORS)
	if(WIN32)
		if(CMAKE_SIZEOF_VOID_P EQUAL 4)
			ExternalProject_Add(
				Qt5
				PREFIX ${CMAKE_BINARY_DIR}/ext/qt-everywhere-opensource-src-5.6.3-modified
				SOURCE_DIR ${CMAKE_SOURCE_DIR}/ext/qt-everywhere-opensource-src-5.6.3-modified
				CONFIGURE_COMMAND ${QT_CONFIGURE} -xplatform win32-g++ -device-option CROSS_COMPILE=i686-w64-mingw32-
				BUILD_COMMAND make -j${NUM_PROCESSORS}
				BUILD_BYPRODUCTS ${CMAKE_BINARY_DIR}/ext/qt-everywhere-opensource-src-5.6.3-modified/lib/libQt5Widgets.a
				INSTALL_COMMAND ""
			)
		else()
			ExternalProject_Add(
				Qt5
				PREFIX ${CMAKE_BINARY_DIR}/ext/qt-everywhere-opensource-src-5.6.3-modified
				SOURCE_DIR ${CMAKE_SOURCE_DIR}/ext/qt-everywhere-opensource-src-5.6.3-modified
				CONFIGURE_COMMAND ${QT_CONFIGURE} -xplatform win32-g++ -device-option CROSS_COMPILE=x86_64-w64-mingw32-
				BUILD_COMMAND make -j${NUM_PROCESSORS}
				BUILD_BYPRODUCTS ${CMAKE_BINARY_DIR}/ext/qt-everywhere-opensource-src-5.6.3-modified/lib/libQt5Widgets.a
				INSTALL_COMMAND ""
			)
		endif()
	elseif(NOT APPLE)
		# It's totally x86_64 Linux!
		ExternalProject_Add(
			Qt5
			PREFIX ${CMAKE_BINARY_DIR}/ext/qt-everywhere-opensource-src-5.6.3-modified
			SOURCE_DIR ${CMAKE_SOURCE_DIR}/ext/qt-everywhere-opensource-src-5.6.3-modified
			CONFIGURE_COMMAND ${QT_CONFIGURE} -xplatform linux-g++-64
			BUILD_COMMAND make -j${NUM_PROCESSORS}
			BUILD_BYPRODUCTS ${CMAKE_BINARY_DIR}/ext/qt-everywhere-opensource-src-5.6.3-modified/lib/libQt5Widgets.a
			INSTALL_COMMAND ""
		)
	else()
		# Not crosscompiling, it's Mac itself!
		ExternalProject_Add(
			Qt5
			PREFIX ${CMAKE_BINARY_DIR}/ext/qt-everywhere-opensource-src-5.6.3-modified
			SOURCE_DIR ${CMAKE_SOURCE_DIR}/ext/qt-everywhere-opensource-src-5.6.3-modified
			CONFIGURE_COMMAND ${QT_CONFIGURE} -platform macx-clang
			BUILD_COMMAND make -j${NUM_PROCESSORS}
			BUILD_BYPRODUCTS ${CMAKE_BINARY_DIR}/ext/qt-everywhere-opensource-src-5.6.3-modified/lib/libQt5Widgets.a
			INSTALL_COMMAND ""
		)
	endif()
	add_library(Qt5::Widgets INTERFACE IMPORTED)
	add_dependencies(Qt5::Widgets Qt5)
	set(QT_INCLUDE ${CMAKE_SOURCE_DIR}/ext/qt-everywhere-opensource-src-5.6.3-modified/qtbase/include)
	target_link_libraries(Qt5::Widgets INTERFACE ${CMAKE_BINARY_DIR}/ext/qt-everywhere-opensource-src-5.6.3-modified/lib/libQt5Widgets.a)
	target_include_directories(Qt5::Widgets INTERFACE ${QT_INCLUDE} ${QT_INCLUDE}/QtCore ${QT_INCLUDE}/QtWidgets)
	set(Qt5Core_VERSION_MAJOR 5)
	set(Qt5Core_VERSION_MINOR 6)
endif()

option(GUIC_USE_SYSTEM_OPENCV "Use system-provided OpenCV" OFF)
if(GUIC_USE_SYSTEM_OPENCV)
	find_path(OpenCV_DIR OpenCVConfig.cmake "/opt/local/libexec/opencv4/cmake")
	find_package(OpenCV REQUIRED)
else()
	set(BUILD_SHARED_LIBS OFF)
	set(BUILD_opencv_apps OFF)
	set(BUILD_EXAMPLES OFF)
	set(BUILD_PERF_TESTS OFF)
	set(BUILD_TESTS OFF)
	set(BUILD_LIST core features2d flann imgcodecs)
	add_subdirectory(ext/opencv-4.10.0)
endif()

# 64-bit floating point version of FFTW
#set(HAVE_STRING_H 1) # Why did I put this here again?
set(BUILD_SHARED_LIBS OFF)
set(BUILD_TESTS OFF)
set(ENABLE_SSE ON)
set(ENABLE_SSE2 ON)
set(ENABLE_THREADS ON)
set(DISABLE_FORTRAN ON)
add_subdirectory(ext/fftw-3.3.10 ${CMAKE_BINARY_DIR}/ext/fftw-3.3.10)

# Explicitly disable the unity build optimization for FFTW, as it breaks
# Also, if WIN32, use FFTW's builtin malloc
get_property(FFTW_TARGETS DIRECTORY ${CMAKE_BINARY_DIR}/ext/fftw-3.3.10 PROPERTY BUILDSYSTEM_TARGETS)
foreach(target ${FFTW_TARGETS})
	set_target_properties(${target} PROPERTIES UNITY_BUILD OFF)
	if(WIN32)
		target_compile_definitions(${target} PRIVATE WITH_OUR_MALLOC=1)
	endif()
endforeach()

# 32-bit floating point version of FFTW - options as above plus this
set(ENABLE_FLOAT ON)
add_subdirectory(ext/fftw-3.3.10 ${CMAKE_BINARY_DIR}/ext/fftwf-3.3.10)

# Explicitly disable the unity build optimization for the other FFTW too...
get_property(FFTWF_TARGETS DIRECTORY ${CMAKE_BINARY_DIR}/ext/fftwf-3.3.10 PROPERTY BUILDSYSTEM_TARGETS)
foreach(target ${FFTWF_TARGETS})
	set_target_properties(${target} PROPERTIES UNITY_BUILD OFF)
	if(WIN32)
		target_compile_definitions(${target} PRIVATE WITH_OUR_MALLOC=1)
	endif()
endforeach()

option(NANOFLANN_BUILD_EXAMPLES "" OFF)
option(NANOFLANN_BUILD_TESTS "" OFF)
option(MASTER_PROJECT_HAS_TARGET_UNINSTALL "" ON)
add_subdirectory(ext/nanoflann)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

add_subdirectory(src)
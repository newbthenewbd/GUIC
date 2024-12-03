cmake_minimum_required(VERSION 3.21)

project(GUIC)

include(ExternalProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

option(GUIC_USE_SYSTEM_OPENCV "Use system-provided OpenCV (may add shared dependency)" ON)
if(GUIC_USE_SYSTEM_OPENCV)
	find_path(OpenCV_DIR OpenCVConfig.cmake "/opt/local/libexec/opencv4/cmake")
	find_package(OpenCV REQUIRED)
else()
	include(cmake/ConfigOpenCV.cmake)
endif()

option(GUIC_USE_SYSTEM_QT "Use system-provided Qt5 (may add shared dependency)" ON)
if(GUIC_USE_SYSTEM_QT)
	find_package(Qt5 REQUIRED COMPONENTS Widgets)
else()
	# Version 5.6.3 unfortunately did not yet use CMake for building itself,
	# so it's a little bit harder
	set(QT_BUILD_DIR ${CMAKE_BINARY_DIR}/ext/qt-everywhere-opensource-src-5.6.3-modified)
	    set(QT_CONFIGURE ${CMAKE_SOURCE_DIR}/ext/qt-everywhere-opensource-src-5.6.3-modified/configure -static -release -opensource -confirm-license -no-sql-db2 -no-sql-ibase -no-sql-mysql -no-sql-oci -no-sql-odbc -no-sql-psql -no-sql-sqlite -no-sql-sqlite2 -no-sql-tds -no-qml-debug -qt-zlib -no-mtdev -no-gif -system-libpng -I ${CMAKE_SOURCE_DIR}/ext/qt-everywhere-opensource-src-5.6.3-modified/qtbase/src/3rdparty/libpng -no-libjpeg -no-openssl -no-libproxy -qt-pcre -no-xinput2 -no-pulseaudio -no-alsa -no-iconv -no-evdev -no-tslib -no-icu -no-pch -no-ltcg -no-dbus -no-eglfs -no-kms -no-gbm -no-directfb -no-linuxfb -no-mirclient -no-feature-codecs -no-feature-big_codecs -no-feature-ftp -no-feature-http -no-feature-udpsocket -no-feature-networkproxy -no-feature-socks5 -no-feature-networkinterface -no-feature-networkdiskcache -no-feature-bearermanagement -no-feature-localserver -no-libinput -no-gstreamer -no-system-proxies -no-securetransport -make libs -skip qt3d -skip qtactiveqt -skip qtandroidextras -skip qtcanvas3d -skip qtconnectivity -skip qtdeclarative -skip qtdoc -skip qtenginio -skip qtgraphicaleffects -skip qtimageformats -skip qtlocation -skip qtmacextras -skip qtmultimedia -skip qtquickcontrols -skip qtquickcontrols2 -skip qtscript -skip qtsensors -skip qtserialbus -skip qtserialport -skip qtsvg -skip qttools -skip qttranslations -skip qtwayland -skip qtwebchannel -skip qtwebengine -skip qtwebsockets -skip qtwebview -skip qtwinextras -skip qtx11extras -skip qtxmlpatterns -prefix ${QT_BUILD_DIR})
	include(ProcessorCount)
	ProcessorCount(NUM_PROCESSORS)
	if(WIN32)
		if(CMAKE_HOST_WIN32)
			# MinGW Makefiles and configure.bat, it's pretty limited relative to the others
			# so give it a special config, good for debugging
			ExternalProject_Add(
				Qt5
				PREFIX ${QT_BUILD_DIR}
				SOURCE_DIR ${CMAKE_SOURCE_DIR}/ext/qt-everywhere-opensource-src-5.6.3-modified
				CONFIGURE_COMMAND ${CMAKE_SOURCE_DIR}/ext/qt-everywhere-opensource-src-5.6.3-modified/configure -static -debug -opensource -confirm-license -no-sql-db2 -no-sql-ibase -no-sql-mysql -no-sql-oci -no-sql-odbc -no-sql-psql -no-sql-sqlite -no-sql-sqlite2 -no-sql-tds -no-qml-debug -qt-zlib -no-mtdev -no-gif -qt-libpng -no-libjpeg -no-openssl -no-libproxy -qt-pcre -no-iconv -no-evdev -no-icu -no-pch -no-ltcg -no-dbus -no-system-proxies -make libs -skip qt3d -skip qtactiveqt -skip qtandroidextras -skip qtcanvas3d -skip qtconnectivity -skip qtdeclarative -skip qtdoc -skip qtenginio -skip qtgraphicaleffects -skip qtimageformats -skip qtlocation -skip qtmacextras -skip qtmultimedia -skip qtquickcontrols -skip qtquickcontrols2 -skip qtscript -skip qtsensors -skip qtserialbus -skip qtserialport -skip qtsvg -skip qttools -skip qttranslations -skip qtwayland -skip qtwebchannel -skip qtwebengine -skip qtwebsockets -skip qtwebview -skip qtwinextras -skip qtx11extras -skip qtxmlpatterns -prefix ${QT_BUILD_DIR} -no-freetype -opengl desktop -platform win32-g++
				BUILD_COMMAND make -j${NUM_PROCESSORS}
				INSTALL_COMMAND make install
			)
	    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
			ExternalProject_Add(
				Qt5
				PREFIX ${QT_BUILD_DIR}
				SOURCE_DIR ${CMAKE_SOURCE_DIR}/ext/qt-everywhere-opensource-src-5.6.3-modified
				CONFIGURE_COMMAND ${QT_CONFIGURE} -no-freetype -xplatform win32-g++ -device-option CROSS_COMPILE=i686-w64-mingw32-
				BUILD_COMMAND make -j${NUM_PROCESSORS}
				INSTALL_COMMAND make install
			)
	    else()
			ExternalProject_Add(
				Qt5
				PREFIX ${QT_BUILD_DIR}
				SOURCE_DIR ${CMAKE_SOURCE_DIR}/ext/qt-everywhere-opensource-src-5.6.3-modified
				CONFIGURE_COMMAND ${QT_CONFIGURE} -no-freetype -xplatform win32-g++ -device-option CROSS_COMPILE=x86_64-w64-mingw32-
				BUILD_COMMAND make -j${NUM_PROCESSORS}
				INSTALL_COMMAND make install
			)
	    endif()
	elseif(NOT APPLE)
		# It's totally x86_64 Linux!
		ExternalProject_Add(
			Qt5
			PREFIX ${QT_BUILD_DIR}
			SOURCE_DIR ${CMAKE_SOURCE_DIR}/ext/qt-everywhere-opensource-src-5.6.3-modified
			CONFIGURE_COMMAND ${QT_CONFIGURE} -xplatform linux-g++-64
			BUILD_COMMAND make -j${NUM_PROCESSORS}
			INSTALL_COMMAND make install
		)
    else()
        # Not crosscompiling, it's Mac itself!
        ExternalProject_Add(
            Qt5
            PREFIX ${QT_BUILD_DIR}
            SOURCE_DIR ${CMAKE_SOURCE_DIR}/ext/qt-everywhere-opensource-src-5.6.3-modified
            CONFIGURE_COMMAND ${QT_CONFIGURE} -no-xinput2 -no-freetype -platform macx-clang
            BUILD_COMMAND make -j${NUM_PROCESSORS}
            INSTALL_COMMAND make install
        )
    endif()
    set(Qt5Core_VERSION_MAJOR 5)
    set(Qt5Core_VERSION_MINOR 6)
endif()

option(GUIC_USE_SYSTEM_FFTW "Use system-provided FFTW (may add shared dependency)" ON)
if(NOT GUIC_USE_SYSTEM_FFTW)
	# 64-bit floating point (double) version of FFTW
	# workaround OLD CMP0077 - see https://stackoverflow.com/a/62702897
	# (accessed 2024/11/11; thanks!)
	set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
	set(BUILD_TESTS OFF CACHE INTERNAL "")
	set(ENABLE_OPENMP OFF CACHE INTERNAL "")
	set(ENABLE_THREADS OFF CACHE INTERNAL "")
	set(WITH_COMBINED_THREADS OFF CACHE INTERNAL "")
	set(ENABLE_FLOAT OFF CACHE INTERNAL "")
	set(ENABLE_LONG_DOUBLE OFF CACHE INTERNAL "")
	set(ENABLE_QUAD_PRECISION OFF CACHE INTERNAL "")
	set(ENABLE_SSE ON CACHE INTERNAL "")
	set(ENABLE_SSE2 ON CACHE INTERNAL "")
	set(ENABLE_AVX OFF CACHE INTERNAL "")
	set(ENABLE_AVX2 OFF CACHE INTERNAL "")
	set(DISABLE_FORTRAN ON CACHE INTERNAL "")
	add_subdirectory(${CMAKE_SOURCE_DIR}/ext/fftw-3.3.10 ${CMAKE_BINARY_DIR}/ext/fftw-3.3.10)

	# Explicitly disable the unity build optimization for FFTW, as it breaks
	# Also, if WIN32, use FFTW's builtin malloc
	get_property(FFTW_TARGETS DIRECTORY ${CMAKE_BINARY_DIR}/ext/fftw-3.3.10 PROPERTY BUILDSYSTEM_TARGETS)
	foreach(target ${FFTW_TARGETS})
		set_target_properties(${target} PROPERTIES UNITY_BUILD OFF)
		if(WIN32)
			target_compile_definitions(${target} PRIVATE WITH_OUR_MALLOC=1)
		endif()
	endforeach()

	# 32-bit floating point (float) version of FFTW - options as above plus this
	set(ENABLE_FLOAT ON CACHE INTERNAL "" FORCE)
	add_subdirectory(${CMAKE_SOURCE_DIR}/ext/fftw-3.3.10 ${CMAKE_BINARY_DIR}/ext/fftwf-3.3.10)

	# Explicitly disable the unity build optimization for the other FFTW too...
	get_property(FFTWF_TARGETS DIRECTORY ${CMAKE_BINARY_DIR}/ext/fftwf-3.3.10 PROPERTY BUILDSYSTEM_TARGETS)
	foreach(target ${FFTWF_TARGETS})
		set_target_properties(${target} PROPERTIES UNITY_BUILD OFF)
		if(WIN32)
			target_compile_definitions(${target} PRIVATE WITH_OUR_MALLOC=1)
		endif()
	endforeach()
endif()

set(NANOFLANN_BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(NANOFLANN_BUILD_TESTS OFF CACHE INTERNAL "")
set(MASTER_PROJECT_HAS_TARGET_UNINSTALL ON CACHE INTERNAL "")
add_subdirectory(${CMAKE_SOURCE_DIR}/ext/nanoflann)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

add_subdirectory(${CMAKE_SOURCE_DIR}/src)

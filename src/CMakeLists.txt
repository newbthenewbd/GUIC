set(TARGET_NAME ${CMAKE_PROJECT_NAME})

# Add all the sources automatically, for the programmer to be happy
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "*.c" "*.cpp" "*.ui")
add_executable(${TARGET_NAME} ${SOURCES})
target_include_directories(${TARGET_NAME} PRIVATE "${CMAKE_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}/src")

# Add all the subdirectories as include dirs, for AUTOMOC/UIC to be happy
# with the happy programmer's includes... Otherwise, it will try to include
# e.g. "colormapwidget.h" from its custom directory
file(GLOB_RECURSE INCLUDE_DIRS CONFIGURE_DEPENDS LIST_DIRECTORIES true "*")
foreach(item ${INCLUDE_DIRS})
	if(IS_DIRECTORY ${item})
		target_include_directories(${TARGET_NAME} PRIVATE ${item})
	endif()
endforeach()

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
else()
	add_dependencies(${TARGET_NAME} Qt5)
	if(CMAKE_HOST_WIN32)
		set_target_properties(${TARGET_NAME} PROPERTIES
			AUTOMOC_EXECUTABLE ${QT_BUILD_DIR}/bin/moc.exe
			AUTOUIC_EXECUTABLE ${QT_BUILD_DIR}/bin/uic.exe
		)
	else()
		set_target_properties(${TARGET_NAME} PROPERTIES
			AUTOMOC_EXECUTABLE ${QT_BUILD_DIR}/bin/moc
			AUTOUIC_EXECUTABLE ${QT_BUILD_DIR}/bin/uic
		)
	endif()
	
	if(WIN32)
		target_link_libraries(${TARGET_NAME} PRIVATE ws2_32)
	endif()
	target_link_directories(${TARGET_NAME} BEFORE PRIVATE ${QT_BUILD_DIR}/lib ${QT_BUILD_DIR}/plugins/platforms)
	if(WIN32)
		target_link_libraries(${TARGET_NAME} PRIVATE Qt5Widgets qwindows winspool shlwapi Qt5PlatformSupport Qt5Gui comdlg32 oleaut32 imm32 winmm glu32 opengl32 gdi32 qtharfbuzzng Qt5Core ole32 uuid ws2_32 advapi32 shell32 user32 kernel32 mpr qtpcre m)
	elseif(APPLE)
		target_link_libraries(${TARGET_NAME} PRIVATE qcocoa "-framework Cocoa" "-framework Carbon" "-framework IOKit" cups Qt5PlatformSupport "-framework DiskArbitration" "-framework Foundation" "-framework CoreServices" "-framework AppKit" "-framework ApplicationServices" "-framework CoreFoundation" "-framework OpenGL" "-framework AGL" Qt5PrintSupport Qt5Widgets Qt5Gui qtharfbuzzng Qt5Core qtpcre m)
	else()
		target_link_libraries(${TARGET_NAME} PRIVATE Qt5Widgets Qt5PlatformSupport Qt5Gui Qt5Core qtpcre m GL pthread) # TODO make based on prl
	endif()
	if(GUIC_USE_SYSTEM_OPENCV)
		# Non-system OpenCV provides Qt with zlib and libpng...
		file(GLOB_RECURSE LIBPNG_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/ext/opencv-4.10.0/3rdparty/libpng/*.c")
		target_sources(${TARGET_NAME} PRIVATE ${LIBPNG_SOURCES})
		target_link_libraries(${TARGET_NAME} PRIVATE z)
	endif()
	target_compile_options(${TARGET_NAME} PRIVATE -I${QT_BUILD_DIR}/include -I${QT_BUILD_DIR}/include/QtCore -I${QT_BUILD_DIR}/include/QtGui -I${QT_BUILD_DIR}/include/QtWidgets)
endif()

target_include_directories(${TARGET_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/ext/eigen" "${CMAKE_SOURCE_DIR}/ext/eigen/Eigen")

target_link_libraries(${TARGET_NAME} PRIVATE fftw3 fftw3f)
if(GUIC_USE_SYSTEM_FFTW)
	find_path(FFTW_INCLUDE_DIR fftw3.h "/opt/local/include" "/usr/local/include" "/opt/homebrew/include")
	find_path(FFTW_LIBRARY_DIR libfftw3.a "/opt/local/lib" "/usr/local/lib" "/opt/homebrew/lib")
	target_include_directories(${TARGET_NAME} PRIVATE ${FFTW_INCLUDE_DIR})
	target_link_directories(${TARGET_NAME} PRIVATE ${FFTW_LIBRARY_DIR})
else()
	target_include_directories(${TARGET_NAME} PRIVATE "${fftw_SOURCE_DIR}/api")
endif()

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

file(GLOB_RECURSE OPENCORR_SRC CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/ext/OpenCorr/src/*.c" "${CMAKE_SOURCE_DIR}/ext/OpenCorr/src/*.cpp")
file(GLOB_RECURSE OPENCORR_H CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/ext/OpenCorr/src/*.h" "${CMAKE_SOURCE_DIR}/ext/OpenCorr/src/*.hpp")

target_sources(${TARGET_NAME} PRIVATE ${OPENCORR_SRC})


target_include_directories(${TARGET_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/ext/OpenCorr/src")
#target_precompile_headers(${TARGET_NAME} PRIVATE ${OPENCORR_H})

if(APPLE)
	target_link_libraries(${TARGET_NAME} PRIVATE "-framework Cocoa")
else()
	target_link_options(${TARGET_NAME} PRIVATE -static-libgcc -static-libstdc++)
endif()
if(MINGW)
	target_link_libraries(${TARGET_NAME} PRIVATE -static winpthread gomp -dynamic)
endif()

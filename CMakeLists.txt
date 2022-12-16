cmake_minimum_required(VERSION 3.18.0)
project(OLua VERSION 0.1.0)


set(ALL_SOURCE_FILE "")

set(ALL_DIR "src")
set(ALL_DIR ${ALL_DIR} "generated")
set(ALL_DIR ${ALL_DIR} "test")
set(ALL_DIR ${ALL_DIR} "thirdparty")

set(CMAKE_VS_PLATFORM_NAME_DEFAULT "Visual Studio 17 2022")
set(SEARCH_DIR ${ALL_DIR})
message("----------- init dir: ${SEARCH_DIR} ------  ${ALL_DIR}")

list(LENGTH SEARCH_DIR SOURCE_LIST_LEN)

while(${SOURCE_LIST_LEN} GREATER 0) 

	unset(CURR_SUB_DIR)
	foreach(CURR_DIR ${SEARCH_DIR})
		message("------- Search dir: ${CURR_DIR}")
		include_directories("${PROJECT_SOURCE_DIR}/${CURR_DIR}")
		file (GLOB CURR_DIR_SOURCE RELATIVE ${PROJECT_SOURCE_DIR} "${PROJECT_SOURCE_DIR}/${CURR_DIR}/*.cpp")
		file (GLOB CURR_DIR_HEADER RELATIVE ${PROJECT_SOURCE_DIR} "${PROJECT_SOURCE_DIR}/${CURR_DIR}/*.h")

		set(ALL_SOURCE_FILE ${ALL_SOURCE_FILE} ${CURR_DIR_SOURCE} ${CURR_DIR_HEADER})

		source_group(${CURR_DIR} FILES ${CURR_DIR_SOURCE} ${CURR_DIR_HEADER})

		file (GLOB CURR_ALL_FILE RELATIVE ${PROJECT_SOURCE_DIR} "${PROJECT_SOURCE_DIR}/${CURR_DIR}/*")
		foreach(CHECKING_SUB_DIR ${CURR_ALL_FILE})
			if(IS_DIRECTORY "${PROJECT_SOURCE_DIR}/${CHECKING_SUB_DIR}")
				set(CURR_SUB_DIR ${CURR_SUB_DIR} ${CHECKING_SUB_DIR})
			endif()
		endforeach()
		
	endforeach()
	set(SOURCE_LIST_LEN 0)
	if(DEFINED CURR_SUB_DIR)
		set(SEARCH_DIR ${CURR_SUB_DIR})
		list(LENGTH SEARCH_DIR SOURCE_LIST_LEN)
	endif()

endwhile()




add_definitions(-DNOUA_USE_MACOSX)
add_definitions(-D_HAS_EXCEPTIONS=0)

set(CMAKE_CXX_STANDARD 17)
message("CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")	
	add_definitions(-DENABLE_ENUM_TEXT=1)
	add_definitions(-DENABLE_RTTI=1)
	message("clang path : CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
	add_definitions(-DUSE_CLANG=1)
	add_compile_options(-Wno-switch)
	add_compile_options(-Wno-trigraphs)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	message("CMAKE_VS_PLATFORM_NAME: ${CMAKE_VS_PLATFORM_NAME}")
	message("CMAKE_VS_PLATFORM_TOOLSET: ${CMAKE_VS_PLATFORM_TOOLSET}")
	message("CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION: ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}")
	message("MSVC_VERSION:${MSVC_VERSION}")
	message("CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION_MAXIMUM:${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION_MAXIMUM}")
	message("CMAKE_GENERATOR:${CMAKE_GENERATOR}")
	message("CMAKE_VS_PLATFORM_TOOLSET_VERSION:${CMAKE_VS_PLATFORM_TOOLSET_VERSION}")
	add_definitions(-DENABLE_RTTI=1)
	if (${MSVC_VERSION} LESS 1925)
		message("MSVC_VERSION(${MSVC_VERSION}) is less than 1925, ENUM TEXT will not be enabled")
	else()
		message("MSVC_VERSION(${MSVC_VERSION}) is greater equal to 1925, ENUM TEXT enabled")
		add_definitions(-DENABLE_ENUM_TEXT=1)
	endif()

	add_compile_options(/wd4302)
	add_compile_options(/wd4311)
	add_compile_options(/wd5103)
	add_compile_options(/Zc:preprocessor)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)
	add_definitions(-DUSE_MSVC=1)

	if ("$ENV{MY_VSCODE_WORKAROUND}" STREQUAL "1")	
		message("Enable vscode workaround, force setting msvc include path")
		include_directories("C:\\Program Files\\Microsoft Visual Studio\\2022\\Enterprise\\VC\\Tools\\MSVC\\14.30.30705\\include")
	endif()
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	add_definitions(-DENABLE_ENUM_TEXT=1)
	add_definitions(-DENABLE_RTTI=1)
	add_definitions(-DUSE_GNU=1)
endif()

message("CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
	add_definitions(-DPLATFORM_WIN)
	add_definitions(-D_AMD64_)
	add_definitions(-DUSE_WCHAR)
	add_definitions(-D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
	SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
	message("Apply Windows platform")
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
	add_definitions(-DPLATFORM_MAC)
	add_definitions(-DUSE_WCHAR)
	message("Apply macOS platform")
	SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	add_definitions(-DPLATFORM_LINUX)
	add_definitions(-DUSE_WCHAR)
	message("Apply Linux platform")
	SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
else()
	message("This platform has not been tested. Possible errors exist: ${CMAKE_SYSTEM_NAME}") 
endif()


add_executable(OLua ${ALL_SOURCE_FILE})


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})


include(CPack)



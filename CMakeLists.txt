cmake_minimum_required(VERSION 3.13)

#设置工程
project(CopyFiles)

add_definitions(-DCOPYFILES_STACTIC)

#设置qt静态库路径
set(QT_STATIC_PATH C:/Qt/Qt5.7.1-msvc2013-x86-static-release)
set(CMAKE_PREFIX_PATH ${QT_STATIC_PATH}/lib/cmake)

#修改FLAGS
if (MSVC)
    set(CompilerFlags
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS_RELEASE
        )
    foreach(CompilerFlag ${CompilerFlags})
        string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
		message("${CMAKE_CXX_FLAGS_RELEASE} asdfasfasdf")
    endforeach()
endif(MSVC)

link_directories(${QT_STATIC_PATH}/lib
				${QT_STATIC_PATH}/plugins/imageformats
				${QT_STATIC_PATH}/plugins/platforms)

# 关闭控制台 ：+ WIN32
set(QT_USE_QTMAIN TRUE) 
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON) 
# 自动生成开关
set(CMAKE_AUTOMOC ON) 		
# 查找lib
find_package(Qt5Core QUIET) 
find_package(Qt5Gui QUIET) 
find_package(Qt5Widgets QUIET) 	
find_package(Qt5Xml QUIET) 	
find_package(Qt5Network QUIET) 	

# 查找 source
set(src_dir .)
foreach(dir ${src_dir})
	string(TOUPPER "${dir}" dir_u)			
	file(GLOB ${dir_u}_UI_FILES ${dir}/*.ui)
	file(GLOB ${dir_u}_H_FILES ${dir}/*.h)
	file(GLOB ${dir_u}_CXX_FILES ${dir}/*.cpp)	
	list(APPEND UI_FILES ${${dir_u}_UI_FILES})		
	list(APPEND ${PROJECT_NAME}_SRCS ${${dir_u}_H_FILES})
	list(APPEND HEADER_SRCS ${${dir_u}_CXX_FILES})
endforeach(dir ${src_dir})

#应用图标
file(GLOB RC_FILE *.rc)

# 自动生成的 mocs_compilation.cpp 包含所有配置的moc_*.cpp		
# ui_*.h 文件
qt5_wrap_ui( UIC_SRCS ${UI_FILES}) 
# qrc_*.cpp 文件
set(RESOURCE_FILES ${PROJECT_NAME}.qrc) 
qt5_add_resources(RCC_SRCS ${RESOURCE_FILES}) 

# 添加 source
add_executable(${PROJECT_NAME} WIN32
				${${PROJECT_NAME}_SRCS}
				${HEADER_SRCS}
				${UIC_SRCS} 
				${RCC_SRCS}
				${RC_FILE})
				
# 依赖
target_link_libraries(${PROJECT_NAME} 
				Qt5::Core	
				Qt5::Gui
				Qt5::Widgets	
				Qt5::Xml												
				Qt5PlatformSupport
				Qt5Svg
				qwindows
				Ws2_32
				opengl32	
				qtpng
				qtfreetype					
				qtharfbuzzng				
				qtpcre
				imm32
				Winmm
				)	
				
# Filter 设置				
source_group("Form Files" FILES ${UI_FILES})
source_group("Generated Files" FILES ${UIC_SRCS} ${RCC_SRCS} )
source_group("Resources" FILES ${RESOURCE_FILES})

	
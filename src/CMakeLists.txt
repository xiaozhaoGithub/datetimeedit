add_subdirectory(core)

if (WIN32)
	# Preserve folder structure in visual studio
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Sources})
elseif(UNIX)
endif()


add_library(${PROJECT_NAME}
	SHARED
	${Sources}
	${RCC_FILES}
)

target_link_libraries(${PROJECT_NAME} 
PRIVATE 
	${DATETIMEEDIT_DEMO_LIBRARIES}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE DATETIMEEDIT_LIBRARY)

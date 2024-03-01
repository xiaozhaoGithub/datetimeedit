set(Sources
    main.cc
    display_widget.cc
    display_widget.h
    stylemanager.h
    stylemanager.cc
)

add_executable(datetimeedit-example
	${Sources}
	datetimeedit-example.qrc
)

target_include_directories(datetimeedit-example 
	PUBLIC 
	${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(datetimeedit-example
PRIVATE 
	${DATETIMEEDIT_DEMO_LIBRARIES}
	datetimeedit
)







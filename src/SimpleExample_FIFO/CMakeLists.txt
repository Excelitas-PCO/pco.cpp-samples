set(PROJECT_NAME SimpleExample_FIFO)
set(PROJECT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.cpp
    ${PCO_FOLDER}/pco.camera/camera.cpp
    ${PCO_FOLDER}/pco.camera/cameraexception.cpp
    ${PCO_FOLDER}/pco.camera/image.cpp
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

include_directories(${PCO_FOLDER})
include_directories(${PCO_FOLDER}/include)

target_link_libraries(${PROJECT_NAME} PRIVATE pco_convert)
target_link_libraries(${PROJECT_NAME} PRIVATE sc2_cam)
target_link_libraries(${PROJECT_NAME} PRIVATE pco_recorder)

install(TARGETS ${PROJECT_NAME})

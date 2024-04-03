set(PROJECT_NAME ImageViewer)

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${QT_ROOT_DIR})
message("cmake prefix with qt dir: ${CMAKE_PREFIX_PATH}")

set(PROJECT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resource.qrc
    ${PCO_FOLDER}/pco.camera/camera.cpp
    ${PCO_FOLDER}/pco.camera/cameraexception.cpp
    ${PCO_FOLDER}/pco.camera/image.cpp
)

# This is only to get the var QT_VERSION_MAJOR
find_package(QT NAMES Qt6 REQUIRED COMPONENTS
    Widgets
    Gui
    Core)
#this does the actual package search
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
    Widgets
    Gui
    Core)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  qt_add_executable(${PROJECT_NAME} MANUAL_FINALIZATION ${PROJECT_SOURCES})
else()
  add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Gui)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core)

include_directories(${PCO_FOLDER})
include_directories(${PCO_FOLDER}/include)

target_link_libraries(${PROJECT_NAME} PRIVATE pco_convert)
target_link_libraries(${PROJECT_NAME} PRIVATE sc2_cam)
target_link_libraries(${PROJECT_NAME} PRIVATE pco_recorder)

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif(WIN32)

install(TARGETS ${PROJECT_NAME})

if(QT_VERSION_MAJOR EQUAL 6)
  qt_finalize_target(${PROJECT_NAME})
endif()

# Do the qt copying stuff
if(WIN32)
  if(CMAKE_BUILD_TYPE MATCHES Debug)
    file(COPY "${QT_ROOT_DIR}/bin/Qt${QT_VERSION_MAJOR}Cored.dll" DESTINATION "${CMAKE_BINARY_DIR}")
    file(COPY "${QT_ROOT_DIR}/bin/Qt${QT_VERSION_MAJOR}Guid.dll" DESTINATION "${CMAKE_BINARY_DIR}")
    file(COPY "${QT_ROOT_DIR}/bin/Qt${QT_VERSION_MAJOR}Widgetsd.dll" DESTINATION "${CMAKE_BINARY_DIR}")
    file(COPY "${QT_ROOT_DIR}/plugins/platforms/qwindowsd.dll" DESTINATION "${CMAKE_BINARY_DIR}/platforms")
  else()
    file(COPY "${QT_ROOT_DIR}/bin/Qt${QT_VERSION_MAJOR}Core.dll" DESTINATION "${CMAKE_BINARY_DIR}")
    file(COPY "${QT_ROOT_DIR}/bin/Qt${QT_VERSION_MAJOR}Gui.dll" DESTINATION "${CMAKE_BINARY_DIR}")
    file(COPY "${QT_ROOT_DIR}/bin/Qt${QT_VERSION_MAJOR}Widgets.dll" DESTINATION "${CMAKE_BINARY_DIR}")
    file(COPY "${QT_ROOT_DIR}/plugins/platforms/qwindows.dll" DESTINATION "${CMAKE_BINARY_DIR}/platforms")
  endif()
  file(COPY "${QT_ROOT_DIR}/plugins/imageformats" DESTINATION "${CMAKE_BINARY_DIR}")

  file(GLOB LIB_FILES "${CMAKE_BINARY_DIR}/Qt*.dll")
  install(FILES ${LIB_FILES} DESTINATION bin)
  install(DIRECTORY "${CMAKE_BINARY_DIR}/platforms" DESTINATION bin)
  install(DIRECTORY "${CMAKE_BINARY_DIR}/imageformats" DESTINATION bin)
else()
  file(GLOB LIB_FILES_CORE "${QT_ROOT_DIR}/lib/libQt${QT_VERSION_MAJOR}Core.so*")
  file(GLOB LIB_FILES_GUI "${QT_ROOT_DIR}/lib/libQt${QT_VERSION_MAJOR}Gui.so*")
  file(GLOB LIB_FILES_OPENGL "${QT_ROOT_DIR}/lib/libQt${QT_VERSION_MAJOR}OpenGL.so*")
  file(GLOB LIB_FILES_OPENGLWIDGETS "${QT_ROOT_DIR}/lib/libQt${QT_VERSION_MAJOR}OpenGLWidgets.so*")
  file(GLOB LIB_FILES_WIDGETS "${QT_ROOT_DIR}/lib/libQt${QT_VERSION_MAJOR}Widgets.so*")
  file(GLOB LIB_FILES_DBUS "${QT_ROOT_DIR}/lib/libQt${QT_VERSION_MAJOR}DBus.so*")
  file(GLOB LIB_FILES_XCB "${QT_ROOT_DIR}/lib/libQt${QT_VERSION_MAJOR}XcbQpa.so*")
  file(GLOB LIB_FILES_ICU "${QT_ROOT_DIR}/lib/libicu*.so*")
  file(COPY 
    ${LIB_FILES_CORE} 
    ${LIB_FILES_GUI} 
    ${LIB_FILES_OPENGL} 
    ${LIB_FILES_OPENGLWIDGETS} 
    ${LIB_FILES_WIDGETS} 
    ${LIB_FILES_DBUS} 
    ${LIB_FILES_XCB}
    ${LIB_FILES_ICU}
    DESTINATION "${CMAKE_BINARY_DIR}/lib")
  file(COPY "${QT_ROOT_DIR}/plugins/platforms" DESTINATION "${CMAKE_BINARY_DIR}/plugins")
  file(COPY "${QT_ROOT_DIR}/plugins/imageformats" DESTINATION "${CMAKE_BINARY_DIR}/plugins")

  message("so files to copy: ${LIB_FILES}")

  install(FILES 
    ${LIB_FILES_CORE} 
    ${LIB_FILES_GUI} 
    ${LIB_FILES_OPENGL} 
    ${LIB_FILES_OPENGLWIDGETS} 
    ${LIB_FILES_WIDGETS} 
    ${LIB_FILES_DBUS} 
    ${LIB_FILES_XCB}
    ${LIB_FILES_ICU} 
    DESTINATION lib)
  install(DIRECTORY "${QT_ROOT_DIR}/plugins/platforms" DESTINATION plugins)
  install(DIRECTORY "${QT_ROOT_DIR}/plugins/imageformats" DESTINATION plugins)
endif()
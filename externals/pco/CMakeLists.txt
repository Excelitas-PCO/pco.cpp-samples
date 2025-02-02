# Copy pco.cpp files to local externals/pco
# If you do not want an auto update, set AUTO_UPDATE_PCO_PACKAGE to false
if(${AUTO_UPDATE_PCO_PACKAGE})
  file(COPY
      ${PCO_PACKAGE_INSTALL_DIR}/pco.camera
      ${PCO_PACKAGE_INSTALL_DIR}/include
      ${PCO_PACKAGE_INSTALL_DIR}/lib
      DESTINATION
      ${CMAKE_CURRENT_SOURCE_DIR}
      )
  if(WIN32)
      file(COPY ${PCO_PACKAGE_INSTALL_DIR}/bin DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})
  endif()
endif()

add_library(pco_convert SHARED IMPORTED GLOBAL)
add_library(sc2_cam SHARED IMPORTED GLOBAL)
add_library(pco_recorder SHARED IMPORTED GLOBAL)
add_library(etc_xcite SHARED IMPORTED GLOBAL)
if(WIN32)
  set_property(TARGET pco_convert PROPERTY IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/pco_conv.lib")
  set_property(TARGET pco_convert PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/bin/pco_conv.dll")
  set_property(TARGET sc2_cam PROPERTY IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/sc2_cam.lib")
  set_property(TARGET sc2_cam PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/bin/sc2_cam.dll")
  set_property(TARGET pco_recorder PROPERTY IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/pco_recorder.lib")
  set_property(TARGET pco_recorder PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/bin/pco_recorder.dll")
  set_property(TARGET etc_xcite PROPERTY IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/etc_xcite.lib")
  set_property(TARGET etc_xcite PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/bin/etc_xcite.dll")

  file(GLOB SC2_LIB_FILES "${CMAKE_CURRENT_SOURCE_DIR}/bin/sc2_*.dll")
  file(GLOB ETC_LIB_FILES "${CMAKE_CURRENT_SOURCE_DIR}/bin/etc_*.dll")
  file(GLOB PCO_LIB_FILES "${CMAKE_CURRENT_SOURCE_DIR}/bin/pco_*.dll")

  file(COPY ${SC2_LIB_FILES} ${ETC_LIB_FILES} ${PCO_LIB_FILES} DESTINATION "${CMAKE_BINARY_DIR}")
  file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/bin/genicam" DESTINATION "${CMAKE_BINARY_DIR}")
  install(FILES ${SC2_LIB_FILES} ${ETC_LIB_FILES} ${PCO_LIB_FILES} DESTINATION bin)
  install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/genicam" DESTINATION bin)
else()
  set_property(TARGET pco_convert PROPERTY IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/libpco_convert.so")
  set_property(TARGET pco_convert PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/libpco_convert.so")
  set_property(TARGET sc2_cam PROPERTY IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/libpco_sc2cam.so")
  set_property(TARGET sc2_cam PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/libpco_sc2cam.so")
  set_property(TARGET pco_recorder PROPERTY IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/libpco_recorder.so")
  set_property(TARGET pco_recorder PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/libpco_recorder.so")
  set_property(TARGET etc_xcite PROPERTY IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/libetc_xcite.so")
  set_property(TARGET etc_xcite PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/libetc_xcite.so")
  file(GLOB PCO_LIB_FILES "${CMAKE_CURRENT_SOURCE_DIR}/lib/libpco*.so*")
  file(GLOB ETC_LIB_FILES "${CMAKE_CURRENT_SOURCE_DIR}/lib/libetc*.so*")
  file(COPY ${PCO_LIB_FILES} ${ETC_LIB_FILES} DESTINATION "${CMAKE_BINARY_DIR}")
  file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/lib/genicam" DESTINATION "${CMAKE_BINARY_DIR}")

  install(FILES ${PCO_LIB_FILES} ${ETC_LIB_FILES} DESTINATION lib)
  install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib/genicam" DESTINATION lib)
endif()
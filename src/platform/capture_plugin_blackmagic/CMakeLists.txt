# include opencv library
include_directories(${OpenCV_INCLUDE_DIRS})

# include alvar library
include_directories(${ALVAR_INCLUDE_DIRS})

# include Blackmagic DeckLink API
include_directories(${BLACKMAGIC_INCLUDE_DIRS})

# ensure plugin api is exported when building library
add_definitions(-DALVAR_Capture_Plugin_BlackMagic_BUILD)

# define plugin library source files to compile
set(ALVARCapturePluginBlackMagic_HEADERS
    CapturePluginBlackMagic.h
	cameradecklink.h
)
set(ALVARCapturePluginBlackMagic_SOURCES
    CapturePluginBlackMagic.cpp
     ${BLACKMAGIC_INCLUDE_DIRS}/DeckLinkAPIDispatch.cpp
	cameradecklink.cpp
)

# add source files to plugin library and set name and properties
add_library(ALVARCapturePluginBlackMagic MODULE
    ${ALVARCapturePluginBlackMagic_HEADERS}
    ${ALVARCapturePluginBlackMagic_SOURCES}
)
set_target_properties(ALVARCapturePluginBlackMagic PROPERTIES OUTPUT_NAME alvarcapturepluginBlackMagic${ALVAR_VERSION_NODOTS})
set_target_properties(ALVARCapturePluginBlackMagic PROPERTIES DEBUG_POSTFIX d)

# link plugin library target to required libraries
target_link_libraries(ALVARCapturePluginBlackMagic
    ALVAR
    ALVARPlatform
    ${OpenCV_LIBRARIES}
    #optimized ${BLACKMAGIC_LIBRARIES}
    #debug ${BLACKMAGIC_LIBRARIES_DEBUG}
)

# install headers
if(ALVAR_PACKAGE MATCHES sdk)
    install(FILES ${ALVARCapturePluginBlackMagic_HEADERS} DESTINATION include/platform/capture_plugin_BlackMagic)
endif(ALVAR_PACKAGE MATCHES sdk)

# install target
install(TARGETS ALVARCapturePluginBlackMagic DESTINATION bin/alvarplugins)

# hack to setup environment runtime path
if(NOT ${MSVC_IDE})
    list(APPEND ALVAR_RUNTIME
        ${CMAKE_CURRENT_BINARY_DIR}\;
    )
else(NOT ${MSVC_IDE})
    list(APPEND ALVAR_RUNTIME
        ${CMAKE_CURRENT_BINARY_DIR}/Debug\;
        ${CMAKE_CURRENT_BINARY_DIR}/Release\;
    )
endif(NOT ${MSVC_IDE})
set(ALVAR_RUNTIME ${ALVAR_RUNTIME} PARENT_SCOPE)

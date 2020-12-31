# Enable Qt MOC/UIC/RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC OFF)

# Executable
add_executable(librepcb_cli
    # Sources
    commandlineinterface.cpp
    main.cpp

    # Headers
    commandlineinterface.h
)
target_include_directories(librepcb_cli PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../libs")
target_link_libraries(librepcb_cli
    LibrePCB::LibraryManager
    LibrePCB::ProjectEditor
)
set_target_properties(librepcb_cli PROPERTIES OUTPUT_NAME librepcb-cli)

# Install target
install(TARGETS librepcb_cli
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Enable Qt MOC/UIC/RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Executable
add_executable(librepcb
    # Sources
    controlpanel/controlpanel.cpp
    firstrunwizard/firstrunwizard.cpp
    firstrunwizard/firstrunwizardpage_welcome.cpp
    firstrunwizard/firstrunwizardpage_workspacepath.cpp
    initializeworkspacewizard/initializeworkspacewizard.cpp
    initializeworkspacewizard/initializeworkspacewizard_chooseimportversion.cpp
    initializeworkspacewizard/initializeworkspacewizard_choosesettings.cpp
    initializeworkspacewizard/initializeworkspacewizard_finalizeimport.cpp
    initializeworkspacewizard/initializeworkspacewizardcontext.cpp
    markdown/markdownconverter.cpp
    projectlibraryupdater/projectlibraryupdater.cpp
    main.cpp

    # Headers
    controlpanel/controlpanel.h
    firstrunwizard/firstrunwizard.h
    firstrunwizard/firstrunwizardpage_welcome.h
    firstrunwizard/firstrunwizardpage_workspacepath.h
    initializeworkspacewizard/initializeworkspacewizard.h
    initializeworkspacewizard/initializeworkspacewizard_chooseimportversion.h
    initializeworkspacewizard/initializeworkspacewizard_choosesettings.h
    initializeworkspacewizard/initializeworkspacewizard_finalizeimport.h
    initializeworkspacewizard/initializeworkspacewizardcontext.h
    markdown/markdownconverter.h
    projectlibraryupdater/projectlibraryupdater.h

    # Forms
    controlpanel/controlpanel.ui
    firstrunwizard/firstrunwizard.ui
    firstrunwizard/firstrunwizardpage_welcome.ui
    firstrunwizard/firstrunwizardpage_workspacepath.ui
    initializeworkspacewizard/initializeworkspacewizard.ui
    initializeworkspacewizard/initializeworkspacewizard_chooseimportversion.ui
    initializeworkspacewizard/initializeworkspacewizard_choosesettings.ui
    initializeworkspacewizard/initializeworkspacewizard_finalizeimport.ui
    projectlibraryupdater/projectlibraryupdater.ui

    # Resources
    ../../img/images.qrc
)
target_include_directories(librepcb PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../libs")
target_link_libraries(librepcb
    LibrePCB::LibraryManager
    LibrePCB::LibraryEditor
    LibrePCB::ProjectEditor
    LibrePCB::Workspace
    LibrePCB::Project
    LibrePCB::Library
    LibrePCB::Common

    Optional::Optional
    TypeSafe::TypeSafe
    Polyclipping::Polyclipping

    Qt5::Gui
    Qt5::Widgets
)

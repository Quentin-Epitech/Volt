cmake_minimum_required(VERSION 3.10)
project(ProjetVoltaire)

# Options système
if(APPLE)
    set(MACOS TRUE)
    add_definitions(-DMACOS)
elseif(WIN32)
    set(WINDOWS TRUE)
    add_definitions(-DWINDOWS)
endif()

# Trouver les packages requis
find_package(CURL REQUIRED)
find_package(Tesseract REQUIRED)

# Inclure les répertoires d'en-têtes
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/ext
    ${CURL_INCLUDE_DIRS}
    ${TESSERACT_INCLUDE_DIRS}
)

# Fichiers source spécifiques à la plateforme
set(PLATFORM_SOURCES "")
if(MACOS)
    set(PLATFORM_SOURCES 
        src/base/platform/platform_mac.cpp
        src/base/screen/screen_mac.cpp
    )
    find_library(CORE_GRAPHICS CoreGraphics REQUIRED)
    find_library(APP_KIT AppKit REQUIRED)
    find_library(CORE_FOUNDATION CoreFoundation REQUIRED)
    find_library(IMAGE_IO ImageIO REQUIRED)
    set(PLATFORM_LIBS ${CORE_GRAPHICS} ${APP_KIT} ${CORE_FOUNDATION} ${IMAGE_IO})
elseif(WINDOWS)
    set(PLATFORM_SOURCES 
        src/base/platform/platform_win.cpp
        src/base/screen/screen_win.cpp
    )
    set(PLATFORM_LIBS "")
endif()

# Définir les fichiers source communs
file(GLOB_RECURSE COMMON_SOURCES 
    "src/base/main/*.cpp"
    "src/base/network/*.cpp"
    "src/base/parsing/*.cpp"
    "src/base/tesseract/*.cpp"
    "src/base/utils/*.cpp"
    "src/base/base.cpp"
)

# Créer l'exécutable
add_executable(ProjetVoltaire 
    ${COMMON_SOURCES}
    ${PLATFORM_SOURCES}
)

# Lier les bibliothèques
target_link_libraries(ProjetVoltaire 
    ${CURL_LIBRARIES}
    ${TESSERACT_LIBRARIES}
    ${PLATFORM_LIBS}
)
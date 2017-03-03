project(PlasmiumRunner)

cmake_minimum_required (VERSION 2.8.12 FATAL_ERROR)
set (QT_MIN_VERSION "5.2.0")
set (KF5_MIN_VERSION "5.15.0")

find_package(ECM 1.8.0 REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Widgets Svg Concurrent DBus)
find_package(KF5Runner ${KF5_MIN_VERSION} REQUIRED)
find_package(KF5KIO ${KF5_MIN_VERSION} REQUIRED)
find_package(KF5I18n ${KF5_MIN_VERSION} REQUIRED)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)
include(ECMInstallIcons)
include(ECMAddAppIcon)

set(plasmiumrunner_SRCS plasmiumrunner.cpp)

add_library(plasmium_runner MODULE ${plasmiumrunner_SRCS})
target_link_libraries(plasmium_runner KF5::Runner KF5::KIOCore KF5::I18n Qt5::DBus)

install(TARGETS plasmium_runner DESTINATION /usr/lib/qt/plugins/)
install(FILES plasmium_runner.desktop DESTINATION /usr/share/kservices5/)

cmake_minimum_required(VERSION 3.16)
project(lozim)

set(KF6_MIN_VERSION "6.0")
set(CMAKE_CXX_STANDARD 23)

find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)

find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS I18n Config Runner KIO Notifications)

kcoreaddons_add_plugin(lozim SOURCES src/lozim.cpp src/ZimArchive.cpp INSTALL_NAMESPACE "kf6/krunner")

target_compile_options(lozim PRIVATE -fexceptions) # TODO

target_link_libraries(lozim
        PRIVATE
        KF6::I18n
        KF6::ConfigCore
        KF6::Runner
        KF6::KIOWidgets
        KF6::Notifications
        zim
)
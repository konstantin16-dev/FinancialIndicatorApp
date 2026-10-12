cmake_minimum_required(VERSION 3.16)
project(FinancialIndicatorsApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql Network)

add_executable(FinancialIndicatorsApp
    main.cpp
    Models.h
    DatabaseManager.h
    AppController.h
    AuthDialog.h
    ProfileDialog.h
    MainWindow.h
)

target_link_libraries(FinancialIndicatorsApp PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
    Qt6::Network
)
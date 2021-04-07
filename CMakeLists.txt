cmake_minimum_required(VERSION 3.16)
project(win_autossh C)

set(CMAKE_C_STANDARD 11)

add_executable(win_autossh src/main.c res/version.rc src/version.h)

target_compile_options(win_autossh PRIVATE /Os)
target_link_options(win_autossh PRIVATE /VERSION:1.01)
target_link_options(win_autossh PRIVATE "/MANIFESTUAC:level='asInvoker' uiAccess='false'")
target_link_options(win_autossh PRIVATE /NXCOMPAT)
target_link_options(win_autossh PRIVATE /DYNAMICBASE)
target_link_options(win_autossh PRIVATE /RELEASE)
target_link_options(win_autossh PRIVATE /LARGEADDRESSAWARE)
target_link_options(win_autossh PRIVATE /OPT:REF)
target_link_options(win_autossh PRIVATE /OPT:ICF)

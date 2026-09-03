cmake_minimum_required(VERSION 4.4.2)

project(Editor C)

set(CMAKE_C_STANDART 23)
set(CMAKE_C_STANDART_REQURED ON)

add_compile_definitions(_FILE_OFFSET_BITS=64)

add_executable(
  editor
  src/main.c
  src/terminal/terminal.c
  src/widget/widget.c
  src/handlers/errorHandlers/errorHandlers.c
  src/handlers/signalHandlers/signalHandlers.c
  src/widgets/clock/clock.c
  src/widgets/bottombar/bottombar.c
  src/widgets/display/display.c
  src/debug/debug.c
  src/info.c
  src/widgets/topbar/topbar.c
  src/file/file.c
  src/widgets/editor/editor.c
  src/handlers/keyHandler/keyHandler.c
  src/widgets/mode/mode.c
)

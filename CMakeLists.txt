add_definitions(-std=c99 -D_BSD_SOURCE -Wall -Wextra)
add_library(ni-mk2 ni-mk2.c)

add_executable(test test.c)
target_link_libraries(test ni-mk2)

add_executable(alsa-kb alsa-kb.c)
target_link_libraries(alsa-kb ni-mk2 asound)

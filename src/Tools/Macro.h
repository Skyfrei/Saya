#ifndef MACRO_H
#define MACRO_H

#include <chrono>

#define MAP_SIZE 10
#define MAX_UNITS 100 // 40
#define MAX_STRUCTS 30
#define PEASANT_INDEX_IN_UNITS 50
#define HALL_INDEX_IN_STRCTS 5
#define BARRACK_INDEX_IN_STRUCTS 5
#define MAX_UNITS_TYPE 3

inline std::chrono::high_resolution_clock::time_point global_timer =
        std::chrono::high_resolution_clock::now();
inline std::chrono::duration<float, std::milli> attackCooldown = std::chrono::milliseconds(200);
inline std::chrono::duration<float, std::milli> hpCooldown = std::chrono::milliseconds(1000);
inline std::chrono::duration<float, std::milli> moveCooldown = std::chrono::milliseconds(100);

#endif

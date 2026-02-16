#ifndef MACRO_H
#define MACRO_H

#include <chrono>

#define MAP_SIZE 10
#define MAX_UNITS 30 // 40
#define MAX_STRUCTS 20
#define PEASANT_INDEX_IN_UNITS 10
#define HALL_INDEX_IN_STRCTS 5
#define BARRACK_INDEX_IN_STRUCTS 5
#define FARM_INDEX_IN_STRCTS 5 // this is missing from the index action i think gotta check
#define MAX_UNITS_TYPE 3
// ESTIMATED MAX VALUES FOR NORMALIZATION
// It is better to overestimate these than underestimate.
#define MAX_GOLD 5000.0f;       // Max expected gold
#define MAX_MINE_GOLD 10000
#define MAX_FOOD 100

inline std::chrono::high_resolution_clock::time_point global_timer =
        std::chrono::high_resolution_clock::now();
inline std::chrono::duration<float, std::milli> attackCooldown = std::chrono::milliseconds(0);
inline std::chrono::duration<float, std::milli> hpCooldown = std::chrono::milliseconds(100);
inline std::chrono::duration<float, std::milli> moveCooldown = std::chrono::milliseconds(100);

#endif

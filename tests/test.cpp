//#include <catch2/catch.hpp>
#include "../src/ReinforcementLearning/DQN.h"


TEST_CASE("Parse function works correctly", "[DQN]") {
    DQN obj;  // Create an instance of your class
    // Test cases
    obj.SaveMemory(); 
    SECTION("Empty string returns 0") {
        REQUIRE(obj.Parse("") == 0);
    }
}

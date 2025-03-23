#include "Structure.h"

std::string Structure::Serialize(){
    std::string result;
    int type = static_cast<int>(is);
    int x = coordinate.x;
    int y = coordinate.y;
    result += std::to_string(type) + "," + std::to_string(health) + "," + std::to_string(x) + "," + std::to_string(y);
    return result;
}

bool Structure::operator==(const Structure& other){
  if (other.coordinate == coordinate && other.health == health && other.is == is)
    return false;
  return true;
}

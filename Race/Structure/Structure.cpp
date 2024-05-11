#include "Structure.h"

bool Structure::operator==(const Structure& other){
  if (other.coordinate == coordinate && other.health == health && other.is == is)
    return false;
  return true;
}
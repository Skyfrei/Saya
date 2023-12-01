#pragma once
#include <vector>
#include "../Spells/Spell.h"
#include "../../Living.h"
#include "../../Map/Terrain.h"
#include <string>
#include <queue>
#include "../../Tools/Vec2.h"

#define n 15

using namespace Warcraft::Spells;
using Warcraft::Living;
using namespace Warcraft::Environment;

namespace Warcraft::Units
{
    enum Type
    {
        PEASANT,
        OTHER
    };

    enum Move
    {
        W,
        NW,
        N,
        NE,
        E,
        SE,
        S,
        SW,
        STAY
    };

    class Unit : public Living
    {
        public:
            Unit()
            {
                is = OTHER;
            }
        
        public:
            std::vector<Vec2> GetAdjacentEdges(int maxSize, Vec2& currentPlace)
            {
                std::vector<Vec2> adEdges;
                int y = currentPlace.y;
                int x = currentPlace.x;
                
                if (x < maxSize - 1 && x > 0)
                {
                    if (y < maxSize - 1 && y > 0)
                    {
                        adEdges.push_back(Vec2(x - 1, y - 1));
                        adEdges.push_back(Vec2(x - 1, y));
                        adEdges.push_back(Vec2(x - 1, y + 1));
                        adEdges.push_back(Vec2(x, y - 1));
                        adEdges.push_back(Vec2(x, y + 1));
                        adEdges.push_back(Vec2(x + 1, y - 1));
                        adEdges.push_back(Vec2(x + 1, y));
                        adEdges.push_back(Vec2(x + 1, y + 1));
                    }
                    else if (y == 0)
                    {
                        adEdges.push_back(Vec2(x - 1, y + 1));
                        adEdges.push_back(Vec2(x - 1, y));
                        adEdges.push_back(Vec2(x , y + 1));
                        adEdges.push_back(Vec2(x + 1, y));
                        adEdges.push_back(Vec2(x + 1, y + 1));
                    }
                    else
                    {
                         adEdges.push_back(Vec2(x - 1, y));
                        adEdges.push_back(Vec2(x - 1, y - 1));
                        adEdges.push_back(Vec2(x , y - 1));
                        adEdges.push_back(Vec2(x + 1, y - 1));
                        adEdges.push_back(Vec2(x + 1, y));
                    }
                }
                else if (x == 0)
                {
                    if ( y == 0)
                    {
                        adEdges.push_back(Vec2(x, y + 1));
                        adEdges.push_back(Vec2(x + 1, y));
                        adEdges.push_back(Vec2(x + 1, y + 1));
                    }
                    else if (y == maxSize - 1)
                    {
                        adEdges.push_back(Vec2(x, y - 1));
                        adEdges.push_back(Vec2(x + 1, y));
                        adEdges.push_back(Vec2(x + 1, y - 1));
                    }
                    else
                    {
                        adEdges.push_back(Vec2(x, y + 1));
                        adEdges.push_back(Vec2(x, y - 1));
                        adEdges.push_back(Vec2(x + 1, y));
                        adEdges.push_back(Vec2(x + 1, y + 1));
                        adEdges.push_back(Vec2(x + 1, y - 1));
                    }
                }
                else 
                {
                    if ( y == 0)
                    {
                        adEdges.push_back(Vec2(x - 1, y));
                        adEdges.push_back(Vec2(x - 1, y + 1));
                        adEdges.push_back(Vec2(x, y + 1));
                    }
                    else if (y == maxSize - 1)
                    {
                        adEdges.push_back(Vec2(x - 1, y));
                        adEdges.push_back(Vec2(x - 1, y - 1));
                        adEdges.push_back(Vec2(x, y - 1));
                    }
                    else
                    {
                        adEdges.push_back(Vec2(x, y + 1));
                        adEdges.push_back(Vec2(x, y - 1));
                        adEdges.push_back(Vec2(x - 1, y));
                        adEdges.push_back(Vec2(x - 1, y + 1));
                        adEdges.push_back(Vec2(x - 1, y - 1));
                    }
                }


            }
            void FindShortestPath(Vec2 terr)
            {

                std::queue<Vec2> queue;
                bool visited[n][n] = {false};

                
                queue.push(coordinate);
                visited[coordinate.x][coordinate.y] = true;

                while(!queue.empty())
                {
                    Vec2 v = queue.front();
                    queue.pop();

                    if (v.x == terr.coord.x && v.y == terr.coord.y)
                        Move(STAY);

                    for (auto adjacCoord : GetAdjacentEdges(n, v))
                    {
                        if (!visited[adjacCoord.x][adjacCoord.y])
                        {
                            visited[adjacCoord.x][adjacCoord.y] = true;
                            queue.push(adjacCoord);
                        }
                    }
                }
            }

            void Move(Move dir)
            {
                switch (dir)
                {
                    case W:
                        coordinate.x -= 1;
                        break;
                    case NW:
                        coordinate.x -= 1;
                        coordinate.y += 1;
                        break;
                    case N:
                        coordinate.y += 1;
                        break;
                    case NE:
                        coordinate.x += 1;
                        coordinate.y += 1;
                        break;
                    case E:
                        coordinate.x += 1;
                        break;
                    case SE:
                        coordinate.x += 1;
                        coordinate.y -= 1;
                        break;
                    case S:
                        coordinate.y -= 1;
                        break;
                    case SW:
                        coordinate.x -= 1;
                        coordinate.y -= 1;
                        break;
                    default:
                        break;
                }
            }
            void Attack(Living& un)
            {
                if (coordinate.x + 1 <= un.coordinate.x && coordinate.y + 1 <= un.coordinate.y)
                    un.health -= attack;
                else
                {
                    FindShortestPath(un.coordinate);
                }
            }

            void GoTo(Vec2& coord)
            {
                FindShortestPath(coord);
            }

            void RegenHealth()
            {
                if (health + healthRegen >= maxHealth)
                    return;
                health += healthRegen;
            }

        public:
        
            bool isInvisible = false;
            float attack;
            float attackCooldown;
            float attackRange;
            int movementSpeed = 1;
            float hpRegen = 0.25f;
            Type is;
    };
}

//
// Created by Klavio Tarka on 14.12.23.
//
#include "Unit.h"
namespace Warcraft::Units
{

    Unit::Unit(){
        is = OTHER;
        attackCooldown = 1;
    }
    void Unit::MoveDij(Vec2 terr){
        std::unordered_map<Vec2, Path> indices;

        std::vector<Vec2> q;
        for (auto& [key, value]: B) {
            std::cout << key << " " << value;
            Path indice;
            indice.dist = 5000;
            indices[key] = indice;

            q.push_back(key);
        }
        indices[coordinate] = 0;

        while(q.size() != 0) 
        {
            std::sort(q.begin(), q.end(), [&indices](const Vec2& a, const Vec2& b){
                return indices[a].dist < indices[b].dist;
            }
            
            Vec2 curr = q[0];
            q.erase(q.begin());
            for (auto neigh : map->graph.nodes[curr].neighbor){
                if (std::find(vec.begin(), vec.end(), neigh) != vec.end() ){
                    int alt = indices[q].dist + indices[neigh.location].dist
                    if (alt < indices[neigh].dist){
                        indices[neigh].dist = alt;
                        indices[neigh].prev = q;
                    }
                }
            }
        }
    }

    void Unit::Move(Vec2 terr){
        Vec2 difference;
        difference.x = coordinate.x - terr.x;
        difference.y = coordinate.y - terr.y;

        if (difference.x > 0 && difference.y > 0)
            ChangeCoordinate(NW);
        else if (difference.x > 0 && difference.y <0)
            ChangeCoordinate(NE);
        else if (difference.x < 0 && difference.y > 0)
            ChangeCoordinate(SW);
        else if (difference.x < 0 && difference.y < 0)
            ChangeCoordinate(SE);
        else if (difference.x == 0 && difference.y < 0)
            ChangeCoordinate(E);
        else if (difference.x == 0 && difference.y > 0)
            ChangeCoordinate(W);
        else if (difference.x > 0 && difference.y == 0)
            ChangeCoordinate(N);
        else if (difference.x < 0 && difference.y == 0)
            ChangeCoordinate(S);
    }

    bool Unit::WithinDistance(Vec2 terr)
    {
        Vec2 difference = FindDifference(terr);

        if (std::abs(difference.x) <= 1 && std::abs(difference.y) <= 1)
            return true;

        return false;
    }

    Vec2 Unit::FindDifference(Vec2 terr){
        Vec2 difference;
        difference.x = coordinate.x - terr.x;
        difference.y = coordinate.y - terr.y;

        return difference;
    }

    void Unit::Attack(Living& un){
        //if (attackTask)
       //attackTask.push(Task<Unit>());

        if (WithinDistance(un.coordinate))
        {
            if (GetAttackTime() == true)
                un.health -= attack;
        }
        else
            Move(un.coordinate);
    }

    bool Unit::GetAttackTime(){
        time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> diff = time - time1;
        time1 = std::chrono::high_resolution_clock::now();

        if (diff.count() >= attackCooldown)
            return true;

        return false;
    }

    void Unit::RegenHealth(){
        time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> diff = time - hpTime;
        hpTime = std::chrono::high_resolution_clock::now();

        if (health + hpRegen >= maxHealth)
            return;
        if (diff.count() >= attackCooldown)
            health += hpRegen;
    }

    void Unit::ChangeCoordinate(MoveType dir){

        switch (dir)
        {
            case W:
                coordinate.y -= 1;
                break;
            case NW:
                coordinate.x -= 1;
                coordinate.y -= 1;
                break;
            case N:
                coordinate.x -= 1;
                break;
            case NE:
                coordinate.x -= 1;
                coordinate.y += 1;
                break;
            case E:
                coordinate.y += 1;
                break;
            case SE:
                coordinate.x += 1;
                coordinate.y += 1;
                break;
            case S:
                coordinate.x += 1;
                break;
            case SW:
                coordinate.x += 1;
                coordinate.y -= 1;
                break;
            default:
                break;
        }
    }
}

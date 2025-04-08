#include "../Tools/Vec2.h"
#include "window.h"
#include <iostream>

int main(){
    Window win(Vec2(1000, 1000));
    int a;
    while(true){
        win.Render();
        std::cin>>a;
        if (a == 0)
            exit(0);

    }
}



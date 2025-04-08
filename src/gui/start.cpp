#include "../Tools/Vec2.h"
#include "window.h"

int main(){
    Window win(Vec2(600, 800));
    while(true){
        win.Render();
    }
}



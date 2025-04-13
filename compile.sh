if [ ! -d "Catch2" ]; then
   echo "SDL doesn't exist" 
   git clone https://github.com/catchorg/Catch2.git
fi

if [ ! -d "SDL" ]; then
    echo "Catch2 doesn't exist"
    git clone https://github.com/libsdl-org/SDL.git
    git clone https://github.com/libsdl-org/SDL_ttf.git
    mv SDL_ttf SDL/
fi

cd build/
ninja
mv Saya ..
cd ..
./build/tester

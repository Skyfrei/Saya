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

if [ ! -f "MonaspaceXenonFrozen-Regular.ttf" ]; then
    echo "MonaspaceXenonFrozen-Regular.ttf which is used for font is not in the dir"
    cp /usr/share/fonts/github/MonaspaceXenonFrozen-Regular.ttf .
fi

#find src/ -name "*.cpp" -o -name "*.h" | xargs clang-format -i
cd build/
ninja
mv Saya ..
cd ..
#./build/tester

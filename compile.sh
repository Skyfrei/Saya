if [ ! -d "SDL" ]; then
   echo "SDL doesn't exist" 
   git clone https://github.com/catchorg/Catch2.git
fi

if [ ! -d "Catch2" ]; then
    echo "Catch2 doesn't exist"
    git clone https://github.com/libsdl-org/SDL.git
fi

cd build/

ninja

mv Saya ..
cd ..
./build/tester

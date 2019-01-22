emcc main.cpp render/*.cpp -std=c++14 --bind -s WASM=1 -s USE_WEBGL2=1 -o bin/index.js
cp web/index.html bin
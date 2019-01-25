targetpath=bin
if [ ! -d $targetpath ];then 
	mkdir $targetpath
fi
emcc simulator/*.cpp core/*.cpp render/*.cpp -std=c++14 --bind -s WASM=1 -s USE_WEBGL2=1 -s ALLOW_MEMORY_GROWTH=1 -o $targetpath/index.js
cp web/index.html $targetpath
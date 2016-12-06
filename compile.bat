:: Generate Object Files
pushd src
for /f "delims=" %%f in ('dir /b /a-d-h-s') do (
	g++ -std=c++11 -Wall -c -I ../lib/winx64_lib/SFML-2.4.1/include -I ../lib/winx64_lib/glew-2.0.0/include -I ../lib/winx64_lib/glm "%%f" -o "../build/%%f.o"
)
popd

:: Generate executable
pushd build
for /f "delims=" %%f in ('dir /b /a-d-h-s') do (
	g++ "%%f" -o "../bin/Game.exe" -lGL -lGLEW -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network
)
popd
::$(CC) $^ -o build/*.o $(LIB)"; $(CC) $^ -o $(TARGET) $(LIB)
pause
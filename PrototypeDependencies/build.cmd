cd assimp
mkdir build
cd build
cmake -DBUILD_SHARED_LIBS=ON ..
cmake --build . --config Debug
cmake --build . --config Release
cd ../../

cd glfw
mkdir build
cd build
cmake ..
cmake --build . --config Debug
cmake --build . --config Release
cd ../../

cd jinja2cpp
mkdir build
cd build
cmake ../cmake/windows
cmake --build . --config Debug
cmake --build . --config Release
cd ../../

cd opencv
mkdir build
cd build
cmake -DBUILD_SHARED_LIBS=ON ..
cmake --build . --config Debug
cmake --build . --config Release
cd ../../

cd PhysX/physx/
call .\generate_projects.bat vc16win64
cd compiler/vc16win64
cmake --build . --config Debug
cmake --build . --config Release
cd ../../../../

cd sds
mkdir build
cd build
cmake -DBUILD_SHARED_LIBS=ON ..
cmake --build . --config Debug
cmake --build . --config Release
cd ../../
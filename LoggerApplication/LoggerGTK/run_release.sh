cd BuildFiles/Ninja-Build/Release/
ninja > /dev/null
status=$?
cd ../../..

if [ $status -eq 0 ]
    then
        ./BuildFiles/Ninja-Build/Release/LoggerApp
    else
        cd BuildFiles/Ninja-Build/Release
        ninja
        cd ../../..
fi
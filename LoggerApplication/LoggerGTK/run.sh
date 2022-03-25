cd BuildFiles/Ninja-Build/Debug/
ninja > /dev/null
status=$?
cd ../../..

if [ $status -eq 0 ]
    then
        ./BuildFiles/Ninja-Build/Debug/LoggerApp
    else
        cd BuildFiles/Ninja-Build/Debug
        ninja
        cd ../../..
fi
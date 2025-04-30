#!/bin/bash

# Get the number of CPUs on macOS
numCores=$(sysctl -n hw.ncpu)

echo "Working Directory: $PWD"
echo "Running Debug Build.. [make -j$numCores RELEASE=0]"

# Run the build command with the number of cores
make -j$numCores RELEASE=0

exitCode=$?

# Can comment this out once bug with log disappearing is fixed
if [ $exitCode != 0 ]
then
    echo "Error! [Exit code $exitCode]"
    read -n 1 -s -r -p "Press any key to continue..."
else
    echo "Success!"
fi

exit $exitCode

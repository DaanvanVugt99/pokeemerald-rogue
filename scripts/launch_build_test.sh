#!/bin/bash

echo "Working Directory: $PWD"

testToRunPrefix="Oblivious prevents Intimidate from lowering Attack"

# Get the number of CPUs on macOS
numCores=$(sysctl -n hw.ncpu)

# Run the make command with the correct options
echo "Running Test Build.. [make -j$numCores pokeemerald-test.elf TEST=1 RELEASE=0 TESTS=\"$testToRunPrefix\"]"
make -j$numCores pokeemerald-test.elf TEST=1 RELEASE=0 TESTS="$testToRunPrefix"

exitCode=$?

# Can comment this out once bug with log disappearing is fixed
if [ $exitCode != 0 ]; then
    echo "Error! [Exit code $exitCode]"
    read -n 1 -s -r -p "Press any key to continue..."
else
    echo "Success!"
    echo "Launching mgba with pokeemerald-test.elf..."
    mgba pokeemerald-test.elf
fi

exit $exitCode

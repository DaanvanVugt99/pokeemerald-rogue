numCores=$(sysctl -n hw.ncpu)

echo Working Directory: $PWD
echo Running Release Build.. [make -j$($numCores) -O RELEASE=1]
make -j$($numCores) -O RELEASE=1

exitCode=$?

# Can comment this out once bug with log disappearing is fixed
if [ $exitCode != 0 ]
then
    echo Error! [Exit code $exitCode]
    read -n 1 -s -r -p "Press any key to continue..."
else
    echo Success!
fi

exit $exitCode
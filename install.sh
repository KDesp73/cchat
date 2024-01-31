#!/bin/bash



if [ "$1" == "clean" ]; then
    rm -rf build
fi

build_folder="./build/"
executable_path=$(find "$build_folder" -maxdepth 1 -type f -executable | head -n 1)
as="cchat"

if [ -n "$executable_path" ]; then
    executable_name=$(basename "$executable_path")
    
    sudo cp "$executable_path" /usr/bin/$as
    
    if [ $? -eq 0 ]; then
		echo "Executable '$executable_name' installed successfully as '$as'."
    else
        echo "Error: Failed to copy the executable to /usr/bin/."
    fi
else
    make cchat
	if [ $? -eq 0 ]; then
        ./install.sh
	fi
fi

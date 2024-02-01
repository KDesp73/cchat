#!/bin/bash



if [ "$1" == "clean" ]; then
    rm -rf build
fi

build_folder="./build/"
executable_path=$(find "$build_folder" -maxdepth 1 -type f -executable | head -n 1)
as="cchat"

if [ -n "$executable_path" ]; then
    echo "Installing..."
    executable_name=$(basename "$executable_path")
    
    sudo cp "$executable_path" /usr/bin/$as
    
    if [ $? -eq 0 ]; then
		echo "Executable '$executable_name' installed successfully as '$as'."
    else
        echo "Error: Failed to copy the executable to /usr/bin/."
    fi
else
    echo "Building..."
    make cchat
	if [ $? -eq 0 ]; then
        ./install.sh
	fi
fi

echo "Installing man page..."

sudo install -g 0 -o 0 -m 0644 $as /usr/share/man/man8/$as.8
sudo gzip /usr/share/man/man8/$as.8

if [ $? -eq 0 ]; then
    echo "man page installed successfully"
else 
    echo "Failed to install man page"
fi


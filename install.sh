#!/bin/bash


if [ "$1" == "clean" ]; then
    rm -rf build
fi

exe="cchat"

if [ -f "$exe" ]; then
    echo "Installing..."
    
    sudo cp ./$exe /usr/bin/$exe
    
    if [ $? -eq 0 ]; then
		echo "Executable '$exe' installed successfully."
    else
        echo "Error: Failed to copy the executable to /usr/bin/."
        exit 1
    fi
else
    echo "Run \"make\" first"
    exit 1
fi

echo "Installing man page..."

sudo install -g 0 -o 0 -m 0644 $exe.man /usr/share/man/man8/$exe.8
sudo gzip /usr/share/man/man8/$exe.8

if [ $? -eq 0 ]; then
    echo "man page installed successfully"
else 
    echo "Failed to install man page"
fi


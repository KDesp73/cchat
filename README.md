# cchat

A simple chat service for the terminal written in C

## Install 

```console
git clone --depth=1 https://github.com/KDesp73/cchat
cd cchat
make
./install.sh
```

## Usage

> If you want to use this remotely you will have to forward the IP adddress and port of
your computer

### Start a server

```console
cchat serve --address 127.0.0.1 --port 1234
```

### Connect to the server as a client

```console
cchat connect --address 127.0.0.1 --port 1234
```

### Set a username

Run

```console
cd ~/.config/cchat
echo "[username]" > username
```

## Arguments

- `-a`, `--address`: Specify the ip address
- `-p`, `--port`: Specify the port
<!-- - `-u`, `--username`: Specify your username. Overrides the file username -->
- `-v`, `--version`: Print the program's version

## Internal commands

- `\help` or `\?`: Prints list of commands
- `\list`: Prints list of connected users
- `\clear`: Clears the terminal window
- `\whoami`: Prints users username
- `\whisper [username] [message]`: Sends private message to user with username `[username]`

> More to be implemented soon!

## License

[GNU](./LICENSE)

## Author

[KDesp73](https://github.com/KDesp73)



# cchat

A simple chat service for the terminal written in C

## Install 

```console
git clone --depth=1 https://github.com/KDesp73/cchat
./install.sh
```

## Usage

> If you want to use this remotely you will have to forward the IP adddress of
your computer

### Start a server

```console
cchat serve --address 127.0.0.1 --port 1234
```

### Connect to the server as a client

```console
cchat connect --address 127.0.0.1 --port 1234
```

## Limitations

- Currently supporting only one client
- Poor UI

## License

[GNU](./LICENSE)

## Author

[KDesp73](https://github.com/KDesp73)



#compdef cchat

_cchat() {
    local -a commands=("serve" "connect")

    _arguments \
        '1: :->command' \
        && return

    case $state in
        command)
            _describe 'command' commands
            ;;
    esac
}

_cchat "$@"

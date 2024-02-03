_cchat_completion() {
    local cur prev words
    COMPREPLY=()
    cur="${COMP_WORDS[COMP_CWORD]}"
    prev="${COMP_WORDS[COMP_CWORD-1]}"
    words=("serve" "connect")

    case "${prev}" in
        cchat)
            COMPREPLY=($(compgen -W "${words[*]}" -- "${cur}"))
            return 0
            ;;
    esac
}
complete -F _cchat_completion cchat

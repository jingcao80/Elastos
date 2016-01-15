while read I; do
    if [[ "${I%%=*}" != "HOME" && "${I%%=*}" != "_" && "${I%%=*}" != "DISPLAY" && "${I%%=*}" != "PATH" && "${I%%=*}" != "XDK_BUILD_ENV" && "${I%%=*}" != "TERM" && "${I%%=*}" != "SHELL" && "${I%%=*}" != "XDK_ROOT" ]]; then
        unset ${I%%=*} &>/dev/null || true
    fi 
done < <(env)

unset I

#!/usr/bin/env bash

check_lib () {
    printf 'Checking for %s\t\t\t[ PEND ]\r' "$1"
    if [[ "$(pkg-config --exists $1; echo $?)" == 0 ]]; then
        printf 'Checking for %s\t\t\t[ \x1b[32mPASS\x1b[0m ]\n' "$1"
    else
        printf 'Checking for %s\t\t\t[ \x1b[31mFAIL\x1b[0m ]\n' "$1"
        return 1
    fi
}

check_header () {
    printf 'Checking for %s\t[ PEND ]\r' "$1"
    if [[ -f "$1" ]]; then
        printf 'Checking for %s\t[ \x1b[32mPASS\x1b[0m ]\n' "$1"
    else
        printf 'Checking for %s\t[ \x1b[31mFAIL\x1b[0m ]\n' "$1"
        return 1
    fi
}

printf 'Checking for /usr/bin/clang\t\t[ PEND ]\r'
if [[ -f '/usr/bin/clang' ]]; then
    printf 'Checking for /usr/bin/clang\t\t[ \x1b[32mPASS\x1b[0m ]\n'
else
    printf 'Checking for /usr/bin/clang\t\t[ \x1b[31mFAIL\x1b[0m ]\n'
    return 1
fi

for i in "libcurl" "jansson"; do
    check_lib "$i"
done

for i in "/usr/include/"{"stdio","stdlib","string","getopt","errno","time","sys/"{"stat","types"},"stdint","math"}".h"; do
    check_header "$i"
done

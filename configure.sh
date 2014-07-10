#!/usr/bin/env bash

check_lib () {
    printf 'Checking for %s\t\t\t\t[ PEND ]\r' "$1"
    if [[ "$(pkg-config --exists $1; echo $?)" == 0 ]]; then
        printf 'Checking for %s\t\t\t\t[ PASS ]\n' "$1"
    else
        printf 'Checking for %s\t\t\t\t[ FAIL ]\n' "$1"
    fi
}

check_header () {
    printf 'Checking for %s\t\t[ PEND ]\r' "$1"
    if [[ -f "$1" ]]; then
        printf 'Checking for %s\t\t[ PASS ]\n' "$1"
    else
        printf 'Checking for %s\t\t[ FAIL ]\n' "$1"
        return 1
    fi
}

for i in "libcurl" "jansson"; do
    check_lib "$i"
done

for i in "/usr/include/"{"stdio","stdlib","string","getopt","errno","time","sys/"{"stat","types"},"stdint","math"}".h"; do
    check_header "$i"
done

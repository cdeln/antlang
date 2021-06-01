#!/bin/bash

set -eu

if [ $EUID -ne 0 ]
    then echo "Please run as root."
    exit 1
fi

packages=(
    doctest-dev
    libboost-all-dev
)

apt-get install -y ${packages[@]}

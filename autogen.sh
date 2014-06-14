#!/bin/sh -xue

autoreconf --force --install --verbose

./configure -C $@

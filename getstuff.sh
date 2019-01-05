#!/bin/sh
if test ! -d nghttp2 ; then
    git clone https://github.com/nghttp2/nghttp2
else
    echo Already have nghttp2.
fi

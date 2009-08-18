#! /usr/bin/env bash
$EXTRACTRC `find -name \*.ui -o -name \*.rc` >> rc.cpp || exit 11
$XGETTEXT `find -name \*.cpp -o -name \*.h` *.cpp *.h -o $podir/plasmate.pot
rm -f rc.cpp

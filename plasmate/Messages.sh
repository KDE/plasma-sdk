#! /usr/bin/env bash
$EXTRACTRC `find -name \*.ui -o -name \*.rc | grep -v '/test-kpart/'` >> rc.cpp || exit 11
$XGETTEXT `find -name \*.cpp -o -name \*.h | grep -v '/test/' | grep -v '/test-kpart/'` -o $podir/plasmate.pot
rm -f rc.cpp

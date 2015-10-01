#! /bin/sh
$XGETTEXT `find . \( -name "*.cpp" -o -name "*.qml" \) -a -not -path "./src/editorplugin/*"` -o $podir/cuttlefish.pot


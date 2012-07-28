install(FILES
  mainRunner.py
  mainDataEngine.py
  mainDataEngine.rb
  mainPlasmoid.js
  mainPlasmoid.py
  mainPlasmoid.qml
  mainPlasmoid.rb
  mainTabbox.qml
  mainKWinScript.js
  cmakelists
  cmakelistsWindowSwitcher
  cmakelistsKWinDecoration
  DESTINATION ${DATA_INSTALL_DIR}/plasmate/templates)

set(KWIN_NAME "kwin")
install(DIRECTORY qml DESTINATION ${DATA_INSTALL_DIR}/plasmate/templates)

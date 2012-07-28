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
  mainKWinEffect.js
  cmakelists
  cmakelistsWindowSwitcher
  cmakelistsKWinScript
  cmakelistsKWinEffect
  cmakelistsKWinDecoration
  DESTINATION ${DATA_INSTALL_DIR}/plasmate/templates)

set(KWIN_NAME "kwin")
install(DIRECTORY qml DESTINATION ${DATA_INSTALL_DIR}/plasmate/templates)

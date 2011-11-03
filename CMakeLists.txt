cmake_minimum_required (VERSION 2.8.0)

project (mpristester)

find_package (Qt4 4.5 REQUIRED QtCore QtGui QtDBus)
include (${QT_USE_FILE})
include_directories (${PROJECT_BINARY_DIR})

set (mpristester_SRCS
   main.cpp
   window.cpp 
   metadatamodel.cpp 
   mpris2/interfacetest.cpp 
   mpris2/rootinterfacetest.cpp 
   mpris2/roottestwidget.cpp 
   mpris2/playertestwidget.cpp
   mpris2/playerinterfacetest.cpp
   mpris2/testconsole.cpp
)
qt4_wrap_cpp (mpristester_SRCS
   window.h
   metadatamodel.h
   mpris2/interfacetest.h 
   mpris2/rootinterfacetest.h 
   mpris2/roottestwidget.h 
   mpris2/playerinterfacetest.h
   mpris2/playertestwidget.h
   mpris2/testconsole.h
)
qt4_wrap_ui (mpristester_SRCS
   ui/roottest.ui
   ui/playertest.ui
   ui/window.ui
)
add_executable (mpristester ${mpristester_SRCS})
target_link_libraries (mpristester ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDBUS_LIBRARY})

# vim:et:sw=3:sts=3

set(kimpanelplugin_SRCS
    kimpanel.cpp
    screen.cpp
    kimpanelplugin.cpp
    kimpanelagent.cpp
    kimpanelagent.h
)
QT5_ADD_DBUS_ADAPTOR(kimpanelplugin_SRCS
    org.kde.impanel.xml
    ./kimpanel/plugin/kimpanelagent.h
    PanelAgent)

add_library(kimpanelplugin SHARED ${kimpanelplugin_SRCS})
target_link_libraries(
    kimpanelplugin
    ${Qt5DBus_LIBRARIES}
    ${Qt5Quick_LIBRARIES}
    ${Qt5QuickWidgets_LIBRARIES}
    ${Qt5Widgets_LIBRARIES})

#ifndef FCITX_SUNPINYIN_BUS_H
#define FCITX_SUNPINYIN_BUS_H

#include <dbus/dbus.h>
#include <fcitx/instance.h>

class FcitxSunPinyinBus {
public:
    FcitxSunPinyinBus(struct _FcitxSunPinyinAddonInstance* sunpinyin);
    virtual ~FcitxSunPinyinBus();

    DBusHandlerResult DimpanelDBusEventHandler(DBusConnection *connection, DBusMessage *message);
    DBusHandlerResult DimpanelDBusFilter(DBusConnection *connection, DBusMessage *message);

    void DimExecMenu(DBusConnection* conn, char *props[], int n);
    void DimRegisterProperties(DBusConnection* conn, char *props[], int n);
    void DimUpdateProperty(DBusConnection* conn, char *prop);
    void DimShowAux(DBusConnection* conn, boolean toShow);
    void DimShowPreedit(DBusConnection* conn, boolean toShow);
    void DimShowLookupTable(DBusConnection* conn, boolean toShow);
    void DimUpdateLookupTable(DBusConnection* conn, char *labels[], int nLabel, char *texts[], int nText, boolean has_prev, boolean has_next);
    void DimUpdateLookupTableCursor(DBusConnection* conn, int cursor);
    void DimUpdatePreeditCaret(DBusConnection* conn, int position);
    void DimUpdatePreeditText(DBusConnection* conn, char *text);
    void DimUpdateAux(DBusConnection* conn, char *text);
    void DimUpdateSpotLocation(DBusConnection* conn, int x, int y);
    void DimSetSpotRect(DBusConnection* conn, int x, int y, int w, int h, boolean relative);
    void DimUpdateScreen(DBusConnection* conn, int id);
    void DimEnable(DBusConnection* conn, boolean toEnable);
    void DimpanelOwnerChanged(const char* serviceName, const char* oldName, const char* newName);

private:
    DBusConnection* m_privconn;
    DBusConnection* m_conn;
    _FcitxSunPinyinAddonInstance* m_sunpinyin;
    //add
    int iOffsetY;
    int iOffsetX;
    FcitxMessages* messageUp;
    FcitxMessages* messageDown;
    int iCursorPos;
    int lastUpdateY;
    int lastUpdateX;
    int version;
    int lastUpdateW;
    int lastUpdateH;
    int lastCursor;
    boolean hasSetLookupTable;
    boolean hasSetRelativeSpotRect;
};

#endif // FCITX_LIBPINYIN_BUS_H

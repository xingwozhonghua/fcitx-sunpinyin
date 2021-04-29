#include "bus.h"
#include "eim.h"
#include <fcitx/module/dbus/fcitx-dbus.h>
#include <fcitx-utils/utils.h>

const char *introspection_xml =
    "<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\"\n"
    "\"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\n"
    "<node name=\"" FCITX_SUNPINYIN_PATH "\">\n"
    "  <interface name=\"org.freedesktop.DBus.Introspectable\">\n"
    "    <method name=\"Introspect\">\n"
    "      <arg name=\"data\" direction=\"out\" type=\"s\"/>\n"
    "    </method>\n"
    "  </interface>\n"
    "  <interface name=\"" FCITX_SUNPINYIN_INTERFACE "\">\n"
    "    <signal name=\"ExecMenu\">"
    "      <arg name=\"prop\" direction=\"in\" type=\"as\"/>"
    "    </signal>"
    "    <signal name=\"RegisterProperties\">"
    "      <arg name=\"prop\" direction=\"in\" type=\"as\"/>"
    "    </signal>"
    "    <signal name=\"UpdateProperty\">"
    "      <arg name=\"prop\" direction=\"in\" type=\"s\"/>"
    "    </signal>"
    "    <signal name=\"ShowAux\">"
    "      <arg name=\"toshow\" direction=\"in\" type=\"b\"/>"
    "    </signal>"
    "    <signal name=\"ShowPreedit\">"
    "      <arg name=\"toshow\" direction=\"in\" type=\"b\"/>"
    "    </signal>"
    "    <signal name=\"ShowLookupTable\">"
    "      <arg name=\"toshow\" direction=\"in\" type=\"b\"/>"
    "    </signal>"
    "    <signal name=\"UpdateLookupTable\">"
    "      <arg name=\"label\" direction=\"in\" type=\"as\"/>"
    "      <arg name=\"text\" direction=\"in\" type=\"as\"/>"
    "    </signal>"
    "    <signal name=\"UpdateLookupTableCursor\">"
    "      <arg name=\"cursor\" direction=\"in\" type=\"i\"/>"
    "    </signal>"
    "    <signal name=\"UpdatePreeditCaret\">"
    "      <arg name=\"position\" direction=\"in\" type=\"i\"/>"
    "    </signal>"
    "    <signal name=\"UpdatePreeditText\">"
    "      <arg name=\"text\" direction=\"in\" type=\"s\"/>"
    "      <arg name=\"attr\" direction=\"in\" type=\"s\"/>"
    "    </signal>"
    "    <signal name=\"UpdateAux\">"
    "      <arg name=\"text\" direction=\"in\" type=\"s\"/>"
    "      <arg name=\"attr\" direction=\"in\" type=\"s\"/>"
    "    </signal>"
    "    <signal name=\"UpdateSpotLocation\">"
    "      <arg name=\"x\" direction=\"in\" type=\"i\"/>"
    "      <arg name=\"y\" direction=\"in\" type=\"i\"/>"
    "    </signal>"
    "    <signal name=\"UpdateScreen\">"
    "      <arg name=\"screen\" direction=\"in\" type=\"i\"/>"
    "    </signal>"
    "    <signal name=\"Enable\">"
    "      <arg name=\"toenable\" direction=\"in\" type=\"b\"/>"
    "    </signal>"
    "  </interface>"
    "</node>";

static DBusHandlerResult dBusEventHandler(DBusConnection* conn, DBusMessage* message, void* self) {
    FcitxSunPinyinBus* bus = (FcitxSunPinyinBus*) self;
    return bus->DimpanelDBusEventHandler(conn, message);
}

static DBusHandlerResult dBusFilter(DBusConnection* conn, DBusMessage* message, void* self) {
    FcitxSunPinyinBus* bus = (FcitxSunPinyinBus*) self;
    return bus->DimpanelDBusFilter(conn, message);
}

static DBusHandlerResult DimpanelDBusEventHandler(DBusConnection *connection, DBusMessage *message)
{
    if (dbus_message_is_method_call(message, DBUS_INTERFACE_INTROSPECTABLE, "Introspect")) {
        DBusMessage *reply = dbus_message_new_method_return(message);

        dbus_message_append_args(reply, DBUS_TYPE_STRING, &introspection_xml, DBUS_TYPE_INVALID);
        dbus_connection_send(connection, reply, NULL);
        dbus_message_unref(reply);
        return DBUS_HANDLER_RESULT_HANDLED;
    }
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

DBusHandlerResult FcitxSunPinyinBus::DimpanelDBusFilter(DBusConnection *connection, DBusMessage *message)
{
    FCITX_UNUSED(connection);
//    FcitxKimpanelUI* kimpanel = (FcitxKimpanelUI*) user_data;
    FcitxInstance* instance = m_sunpinyin->owner;
    FcitxInputState* input = FcitxInstanceGetInputState(m_sunpinyin->owner);
    int int0;
    const char* s0 = NULL;
    if (dbus_message_is_signal(message, "org.sunpinyin.impanel", "MovePreeditCaret")) {
        FcitxLog(DEBUG, "MovePreeditCaret");
        DBusError error;
        dbus_error_init(&error);
        dbus_message_get_args(message, &error, DBUS_TYPE_INT32, &int0 , DBUS_TYPE_INVALID);
        dbus_error_free(&error);
        return DBUS_HANDLER_RESULT_HANDLED;
    } else if (dbus_message_is_signal(message, "org.sunpinyin.impanel", "SelectCandidate")) {
        FcitxLog(DEBUG, "SelectCandidate: ");
        DBusError error;
        dbus_error_init(&error);
        if (dbus_message_get_args(message, &error, DBUS_TYPE_INT32, &int0 , DBUS_TYPE_INVALID)) {
            FcitxInstanceChooseCandidateByIndex(instance, int0);
        }
        dbus_error_free(&error);
        return DBUS_HANDLER_RESULT_HANDLED;
    } else if (dbus_message_is_signal(message, "org.sunpinyin.impanel", "LookupTablePageUp")) {
        FcitxLog(DEBUG, "LookupTablePageUp");
        if (FcitxCandidateWordPageCount(FcitxInputStateGetCandidateList(input)) != 0) {
            FcitxCandidateWordGoPrevPage(FcitxInputStateGetCandidateList(input));
            FcitxInstanceProcessInputReturnValue(instance, IRV_DISPLAY_CANDWORDS);
        }
        return DBUS_HANDLER_RESULT_HANDLED;
    } else if (dbus_message_is_signal(message, "org.sunpinyin.impanel", "LookupTablePageDown")) {
        FcitxLog(DEBUG, "LookupTablePageDown");
        if (FcitxCandidateWordPageCount(FcitxInputStateGetCandidateList(input)) != 0) {
            FcitxCandidateWordGoNextPage(FcitxInputStateGetCandidateList(input));
            FcitxInstanceProcessInputReturnValue(instance, IRV_DISPLAY_CANDWORDS);
        }
        return DBUS_HANDLER_RESULT_HANDLED;
    } else if (dbus_message_is_signal(message, "org.sunpinyin.impanel", "SelectTotalCandidate")) {
        FcitxLog(DEBUG, "SelectTotalCandidate: ");
        DBusError error;
        dbus_error_init(&error);
        if (dbus_message_get_args(message, &error, DBUS_TYPE_INT32, &int0, DBUS_TYPE_INVALID)) {
            FcitxInstanceChooseCandidateByTotalIndex(instance, int0);
        }
        dbus_error_free(&error);
        return DBUS_HANDLER_RESULT_HANDLED;
    }
    else if (dbus_message_is_signal(message, "org.sunpinyin.impanel", "TriggerProperty")) {
        FcitxLog(DEBUG, "TriggerProperty: ");
        DBusError error;
        dbus_error_init(&error);
        if (dbus_message_get_args(message, &error, DBUS_TYPE_STRING, &s0 , DBUS_TYPE_INVALID)) {
            if (strlen(s0) > strlen("/Fcitx/")) {
                s0 += strlen("/Fcitx/");
                if (strcmp("logo", s0) == 0) {
//                    char *prop[2];
//                    char *trans_str = ("Toggle Input Method");
//                    fcitx_utils_alloc_cat_str(prop[0], "/Fcitx/logo/toggle:",
//                                              trans_str, "::", trans_str);
//                    trans_str = ("Restart");
//                    fcitx_utils_alloc_cat_str(prop[1], "/Fcitx/logo/restart:",
//                                              trans_str, ":view-refresh:",
//                                              trans_str);
//                    DimExecMenu(connection, prop, 2);
//                    int i;
//                    for (i = 0;i < 2;i++) {
//                        free(prop[i]);
//                    }
                } else if (strncmp("logo/", s0, strlen("logo/")) == 0) {
                    s0 += strlen("logo/");
                    if (strcmp(s0, "toggle") == 0)
                        FcitxInstanceChangeIMState(instance, FcitxInstanceGetCurrentIC(instance));
                    else if (strcmp(s0, "restart") == 0) {
                        FcitxInstanceRestart(instance);
                    }
                } else if (strcmp("keyboard", s0) == 0) {
                    FcitxInstanceCloseIM(instance,
                                         FcitxInstanceGetCurrentIC(instance));
                } else if (strncmp("im/", s0, strlen("im/")) == 0) {
                    s0 += strlen("im/");
                    FcitxInstanceSwitchIMByName(instance, s0);
                } else if (strncmp("im", s0, strlen("im")) == 0) {
//                    UT_array* imes = FcitxInstanceGetIMEs(instance);
//                    FcitxIM* pim;
//                    int index = 0;
//                    size_t len = utarray_len(imes);
//                    char **prop = fcitx_utils_malloc0(len * sizeof(char*));
//                    for (pim = (FcitxIM *) utarray_front(imes);
//                         pim != NULL;
//                         pim = (FcitxIM *) utarray_next(imes, pim)) {

//                        SetIMMenu(pim, &prop[index]);
//                        index ++;
//                    }
//                    DimExecMenu(connection, prop, len);
//                    while (len --)
//                        free(prop[len]);
//                    free(prop);
                } else {
                    /* menu */
                    const char* pos;
                    if ((pos = strchr(s0, '/'))) {
                        char* statusName = strndup(s0, pos - s0);
                        FcitxUIMenu* menup = FcitxUIGetMenuByStatusName(instance, statusName);
                        free(statusName);
                        pos ++;
                        int index = 0;
                        sscanf(pos, "%d", &index);
                        if (menup)
                            menup->MenuAction(menup, index);
                    } else {
//                        FcitxUIMenu *menu = FcitxUIGetMenuByStatusName(instance, s0);
//                        if (menu) {
//                            menu->UpdateMenu(menu);

//                            unsigned int i = 0, index = 0;
//                            unsigned int len = utarray_len(&menu->shell);
//                            char *prop[len];
//                            for (i = 0; i < len; i++) {
//                                FcitxMenuItem *menu_item = GetMenuItem(menu, i);
//                                if (menu_item->type == MENUTYPE_SIMPLE) {
//                                    asprintf(&prop[index],
//                                             "/Fcitx/%s/%d:%s::%s", s0, index,
//                                             menu_item->tipstr,
//                                             menu_item->tipstr);
//                                    index++;
//                                }
//                            }
//                            DimExecMenu(connection, prop, index);
//                            while (index--)
//                                free(prop[index]);
//                        } else {
//                            FcitxUIUpdateStatus(instance, s0);
//                        }
                    }
                }
            }
        }
        dbus_error_free(&error);
        return DBUS_HANDLER_RESULT_HANDLED;
    } else if (dbus_message_is_signal(message, "org.sunpinyin.impanel", "PanelCreated")) {
//        FcitxLog(DEBUG, "PanelCreated");
//        FcitxUIResumeFromFallback(instance);
//        KimpanelReset();
//        KimpanelRegisterAllStatus();
        return DBUS_HANDLER_RESULT_HANDLED;
    } else if (dbus_message_is_signal(message, "org.sunpinyin.impanel2", "PanelCreated2")) {
//        FcitxLog(DEBUG, "PanelCreated2");
//        FcitxUIResumeFromFallback(instance);
//        version = 2;
//        KimpanelReset();
//        KimpanelRegisterAllStatus();
//        KimpanelIntrospect();
        return DBUS_HANDLER_RESULT_HANDLED;
    } else if (dbus_message_is_signal(message, "org.sunpinyin.impanel", "Exit")) {
        FcitxLog(DEBUG, "Exit");
        FcitxInstanceEnd(instance);
        return DBUS_HANDLER_RESULT_HANDLED;
    } else if (dbus_message_is_signal(message, "org.sunpinyin.impanel", "ReloadConfig")) {
        FcitxLog(DEBUG, "ReloadConfig");
        FcitxInstanceReloadConfig(instance);
        return DBUS_HANDLER_RESULT_HANDLED;
    } else if (dbus_message_is_signal(message, "org.sunpinyin.impanel", "Restart")) {
        FcitxLog(DEBUG, "Restart");
        FcitxInstanceRestart(instance);
        return DBUS_HANDLER_RESULT_HANDLED;
    } else if (dbus_message_is_signal(message, "org.sunpinyin.impanel", "Configure")) {
        FcitxLog(DEBUG, "Configure");
        fcitx_utils_launch_configure_tool();
        return DBUS_HANDLER_RESULT_HANDLED;
    }
    else if (dbus_message_is_signal(message, "org.sunpinyin.impanel", "LookupTablePageSkip")) {
        FcitxLog(DEBUG, "LookupTablePageSkip");
        DBusError error;
        dbus_error_init(&error);
        if (dbus_message_get_args(message, &error, DBUS_TYPE_INT32, &int0, DBUS_TYPE_INVALID)) {
            FcitxCandidateWordSetPage(FcitxInputStateGetCandidateList(input), int0);
        }
        dbus_error_free(&error);

        return DBUS_HANDLER_RESULT_HANDLED;
    }

    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

FcitxSunPinyinBus::FcitxSunPinyinBus(struct _FcitxSunPinyinAddonInstance* sunpinyin)
{
    FcitxInstance* instance = sunpinyin->owner;
    DBusConnection *conn = FcitxDBusGetConnection(instance);
    DBusConnection *privconn = FcitxDBusGetPrivConnection(instance);
    if (conn == NULL && privconn == NULL) {
        FcitxLog(ERROR, "DBus Not initialized");
    }

    m_sunpinyin = sunpinyin;
    m_conn = conn;
    m_privconn = privconn;

    iCursorPos = 0;
    version = 1;
    lastCursor = -2;

//    iOffsetY;
//    iOffsetX;
//    messageUp;
//    messageDown;
//    lastUpdateY;
//    lastUpdateX;
//    lastUpdateW;
//    lastUpdateH;
//    hasSetLookupTable;
//    hasSetRelativeSpotRect;


    DBusError err;
    dbus_error_init(&err);
    do {
        if (conn == NULL) {
            FcitxLog(ERROR, "DBus Not initialized");
            break;
        }

        // add a rule to receive signals from kimpanel
        dbus_bus_add_match(conn,
                           "type='signal',sender='org.sunpinyin.impanel',interface='org.sunpinyin.impanel'",
                           &err);
        dbus_connection_flush(conn);
        if (dbus_error_is_set(&err)) {
            FcitxLog(ERROR, "Match Error (%s)", err.message);
            break;
        }
        dbus_bus_add_match(conn,
                           "type='signal',sender='org.sunpinyin.impanel',interface='org.sunpinyin.impanel2'",
                           &err);
        dbus_connection_flush(conn);
        if (dbus_error_is_set(&err)) {
            FcitxLog(ERROR, "Match Error (%s)", err.message);
            break;
        }

//        int id = FcitxDBusWatchName(instance, "org.sunpinyin.impanel", sunpinyin,
//                                    DimpanelOwnerChanged, NULL, NULL);
//        if (id == 0) {
//            break;
//        }

        if (!dbus_connection_add_filter(conn, dBusFilter, this, NULL)) {
            FcitxLog(ERROR, "No memory");
            break;
        }

        DBusObjectPathVTable vtable = {NULL, &dBusEventHandler, NULL, NULL, NULL, NULL };

        dbus_connection_register_object_path(conn, FCITX_SUNPINYIN_PATH, &vtable, this);

        messageUp = FcitxMessagesNew();
        messageDown = FcitxMessagesNew();

        const char* kimpanelServiceName = "org.sunpinyin.impanel";
        DBusMessage* message = dbus_message_new_method_call(DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "NameHasOwner");
        dbus_message_append_args(message, DBUS_TYPE_STRING, &kimpanelServiceName, DBUS_TYPE_INVALID);

        DBusPendingCall *call = NULL;
        dbus_bool_t reply =
            dbus_connection_send_with_reply(conn, message,
                                            &call, DBUS_TIMEOUT_USE_DEFAULT);
//        if (reply == TRUE) {
//            dbus_pending_call_set_notify(call,
//                                         DimpanelServiceExistCallback,
//                                         sunpinyin,
//                                         NULL);
//            dbus_pending_call_unref(call);
//        }
        dbus_connection_flush(conn);
        dbus_message_unref(message);

//        KimpanelRegisterAllStatus();
        dbus_error_free(&err);
    } while (0);

    dbus_error_free(&err);
}

FcitxSunPinyinBus::~FcitxSunPinyinBus()
{
    if (m_conn) {
        dbus_connection_unregister_object_path(m_conn, FCITX_SUNPINYIN_PATH);
    }
    if (m_privconn) {
        dbus_connection_unregister_object_path(m_privconn, FCITX_SUNPINYIN_PATH);
    }
}

void FcitxSunPinyinBus::DimExecMenu(DBusConnection* conn, char *props[], int n)
{
    dbus_uint32_t serial = 0; // unique number to associate replies with requests
    DBusMessage* msg;
    DBusMessageIter args;
    int i;

    // create a signal and check for errors
    msg = dbus_message_new_signal(FCITX_SUNPINYIN_PATH, // object name of the signal
                                  FCITX_SUNPINYIN_INTERFACE, // interface name of the signal
                                  "ExecMenu"); // name of the signal
    if (NULL == msg) {
        FcitxLog(DEBUG, "Message Null");
        return;
    }

    for (i = 0; i < n; i++) {
        if (!fcitx_utf8_check_string(props[i]))
            return;
    }

    // append arguments onto signal
    dbus_message_iter_init_append(msg, &args);
    DBusMessageIter sub;
    dbus_message_iter_open_container(&args, DBUS_TYPE_ARRAY, "s", &sub);
    for (i = 0; i < n; i++) {
        if (!dbus_message_iter_append_basic(&sub, DBUS_TYPE_STRING, &props[i])) {
            FcitxLog(DEBUG, "Out Of Memory!");
        }
    }
    dbus_message_iter_close_container(&args, &sub);

    // send the message and flush the connection
    if (!dbus_connection_send(conn, msg, &serial)) {
        FcitxLog(DEBUG, "Out Of Memory!");
    }
    dbus_connection_flush(conn);

    // free the message
    dbus_message_unref(msg);
}

void FcitxSunPinyinBus::DimRegisterProperties(DBusConnection* conn, char *props[], int n)
{
    dbus_uint32_t serial = 0; // unique number to associate replies with requests
    DBusMessage* msg;
    DBusMessageIter args;
    int i;

    // create a signal and check for errors
    msg = dbus_message_new_signal(FCITX_SUNPINYIN_PATH, // object name of the signal
                                  FCITX_SUNPINYIN_INTERFACE, // interface name of the signal
                                  "RegisterProperties"); // name of the signal
    if (NULL == msg) {
        FcitxLog(DEBUG, "Message Null");
        return;
    }

    for (i = 0; i < n; i++) {
        if (!fcitx_utf8_check_string(props[i]))
            return;
    }

    // append arguments onto signal
    dbus_message_iter_init_append(msg, &args);
    DBusMessageIter sub;
    dbus_message_iter_open_container(&args, DBUS_TYPE_ARRAY, "s", &sub);
    for (i = 0; i < n; i++) {
        if (!dbus_message_iter_append_basic(&sub, DBUS_TYPE_STRING, &props[i])) {
            FcitxLog(DEBUG, "Out Of Memory!");
        }
    }
    dbus_message_iter_close_container(&args, &sub);

    // send the message and flush the connection
    if (!dbus_connection_send(conn, msg, &serial)) {
        FcitxLog(DEBUG, "Out Of Memory!");
    }

    // free the message
    dbus_message_unref(msg);
}

void FcitxSunPinyinBus::DimUpdateProperty(DBusConnection* conn, char *prop)
{
    dbus_uint32_t serial = 0; // unique number to associate replies with requests
    DBusMessage* msg;
    DBusMessageIter args;

    // create a signal and check for errors
    msg = dbus_message_new_signal(FCITX_SUNPINYIN_PATH, // object name of the signal
                                  FCITX_SUNPINYIN_INTERFACE, // interface name of the signal
                                  "UpdateProperty"); // name of the signal
    if (NULL == msg) {
        FcitxLog(DEBUG, "Message Null");
        return;
    }

    if (!fcitx_utf8_check_string(prop))
        return;

    // append arguments onto signal
    dbus_message_iter_init_append(msg, &args);
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &prop)) {
        FcitxLog(DEBUG, "Out Of Memory!");
    }

    // send the message and flush the connection
    if (!dbus_connection_send(conn, msg, &serial)) {
        FcitxLog(DEBUG, "Out Of Memory!");
    }

    // free the message
    dbus_message_unref(msg);
}

void FcitxSunPinyinBus::DimShowAux(DBusConnection* conn, boolean toShow)
{
    dbus_uint32_t serial = 0; // unique number to associate replies with requests
    DBusMessage* msg;
    DBusMessageIter args;

    // create a signal and check for errors
    msg = dbus_message_new_signal(FCITX_SUNPINYIN_PATH, // object name of the signal
                                  FCITX_SUNPINYIN_INTERFACE, // interface name of the signal
                                  "ShowAux"); // name of the signal
    if (NULL == msg) {
        FcitxLog(DEBUG, "Message Null");
        return;
    }

    // append arguments onto signal
    dbus_message_iter_init_append(msg, &args);
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_BOOLEAN, &toShow)) {
        FcitxLog(DEBUG, "Out Of Memory!");
    }

    // send the message and flush the connection
    if (!dbus_connection_send(conn, msg, &serial)) {
        FcitxLog(DEBUG, "Out Of Memory!");
    }

    // free the message
    dbus_message_unref(msg);
}

void FcitxSunPinyinBus::DimShowPreedit(DBusConnection* conn, boolean toShow)
{
    dbus_uint32_t serial = 0; // unique number to associate replies with requests
    DBusMessage* msg;
    DBusMessageIter args;

    // create a signal and check for errors
    msg = dbus_message_new_signal(FCITX_SUNPINYIN_PATH, // object name of the signal
                                  FCITX_SUNPINYIN_INTERFACE, // interface name of the signal
                                  "ShowPreedit"); // name of the signal
    if (NULL == msg) {
        FcitxLog(DEBUG, "Message Null");
        return;
    }

    // append arguments onto signal
    dbus_message_iter_init_append(msg, &args);
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_BOOLEAN, &toShow)) {
        FcitxLog(DEBUG, "Out Of Memory!");
    }

    // send the message and flush the connection
    if (!dbus_connection_send(conn, msg, &serial)) {
        FcitxLog(DEBUG, "Out Of Memory!");
    }

    // free the message
    dbus_message_unref(msg);
}

void FcitxSunPinyinBus::DimShowLookupTable(DBusConnection* conn, boolean toShow)
{
    dbus_uint32_t serial = 0; // unique number to associate replies with requests
    DBusMessage* msg;
    DBusMessageIter args;

    // create a signal and check for errors
    msg = dbus_message_new_signal(FCITX_SUNPINYIN_PATH, // object name of the signal
                                  FCITX_SUNPINYIN_INTERFACE, // interface name of the signal
                                  "ShowLookupTable"); // name of the signal
    if (NULL == msg) {
        FcitxLog(DEBUG, "Message Null");
        return;
    }

    // append arguments onto signal
    dbus_message_iter_init_append(msg, &args);
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_BOOLEAN, &toShow)) {
        FcitxLog(DEBUG, "Out Of Memory!");
    }

    // send the message and flush the connection
    if (!dbus_connection_send(conn, msg, &serial)) {
        FcitxLog(DEBUG, "Out Of Memory!");
    }

    // free the message
    dbus_message_unref(msg);
}

void FcitxSunPinyinBus::DimUpdateLookupTable(DBusConnection* conn, char *labels[], int nLabel, char *texts[], int nText, boolean has_prev, boolean has_next)
{
    int i;
    dbus_uint32_t serial = 0; // unique number to associate replies with requests
    DBusMessage* msg;
    DBusMessageIter args;

    // create a signal and check for errors
    msg = dbus_message_new_signal(FCITX_SUNPINYIN_PATH, // object name of the signal
                                  FCITX_SUNPINYIN_INTERFACE, // interface name of the signal
                                  "UpdateLookupTable"); // name of the signal
    if (NULL == msg) {
        FcitxLog(DEBUG, "Message Null");
        return;
    }
    for (i = 0; i < nLabel; i++) {
        if (!fcitx_utf8_check_string(labels[i]))
            return;
    }
    for (i = 0; i < nText; i++) {
        if (!fcitx_utf8_check_string(texts[i]))
            return;
    }
    DBusMessageIter subLabel;
    DBusMessageIter subText;
    DBusMessageIter subAttrs;
    // append arguments onto signal
    dbus_message_iter_init_append(msg, &args);
    dbus_message_iter_open_container(&args, DBUS_TYPE_ARRAY, "s", &subLabel);
    for (i = 0; i < nLabel; i++) {
        if (!dbus_message_iter_append_basic(&subLabel, DBUS_TYPE_STRING,
                                            &labels[i])) {
            FcitxLog(DEBUG, "Out Of Memory!");
        }
    }
    dbus_message_iter_close_container(&args, &subLabel);

    dbus_message_iter_open_container(&args, DBUS_TYPE_ARRAY, "s", &subText);
    for (i = 0; i < nText; i++) {
        if (!dbus_message_iter_append_basic(&subText, DBUS_TYPE_STRING,
                                            &texts[i])) {
            FcitxLog(DEBUG, "Out Of Memory!");
        }
    }
    dbus_message_iter_close_container(&args, &subText);

    char *attr = "";
    dbus_message_iter_open_container(&args, DBUS_TYPE_ARRAY, "s", &subAttrs);
    for (i = 0; i < nLabel; i++) {
        if (!dbus_message_iter_append_basic(&subAttrs, DBUS_TYPE_STRING, &attr)) {
            FcitxLog(DEBUG, "Out Of Memory!");
        }
    }
    dbus_message_iter_close_container(&args, &subAttrs);

    dbus_message_iter_append_basic(&args, DBUS_TYPE_BOOLEAN, &has_prev);
    dbus_message_iter_append_basic(&args, DBUS_TYPE_BOOLEAN, &has_next);

    // send the message and flush the connection
    if (!dbus_connection_send(conn, msg, &serial)) {
        FcitxLog(DEBUG, "Out Of Memory!");
    }

    // free the message
    dbus_message_unref(msg);
}

void FcitxSunPinyinBus::DimUpdateLookupTableCursor(DBusConnection* conn, int cursor)
{
    if (lastCursor != cursor)
        lastCursor = cursor;
    else
        return;
    dbus_uint32_t serial = 0; // unique number to associate replies with requests
    DBusMessage* msg;
    DBusMessageIter args;

    // create a signal and check for errors
    msg = dbus_message_new_signal(FCITX_SUNPINYIN_PATH, // object name of the signal
                                  FCITX_SUNPINYIN_INTERFACE, // interface name of the signal
                                  "UpdateLookupTableCursor"); // name of the signal
    if (NULL == msg) {
        FcitxLog(DEBUG, "Message Null");
        return;
    }

    // append arguments onto signal
    dbus_message_iter_init_append(msg, &args);
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &cursor)) {
        FcitxLog(DEBUG, "Out Of Memory!");
    }

    // send the message and flush the connection
    if (!dbus_connection_send(conn, msg, &serial)) {
        FcitxLog(DEBUG, "Out Of Memory!");
    }

    // free the message
    dbus_message_unref(msg);
}

void FcitxSunPinyinBus::DimUpdatePreeditCaret(DBusConnection* conn, int position)
{

    dbus_uint32_t serial = 0; // unique number to associate replies with requests
    DBusMessage* msg;
    DBusMessageIter args;

    // create a signal and check for errors
    msg = dbus_message_new_signal(FCITX_SUNPINYIN_PATH, // object name of the signal
                                  FCITX_SUNPINYIN_INTERFACE, // interface name of the signal
                                  "UpdatePreeditCaret"); // name of the signal
    if (NULL == msg) {
        FcitxLog(DEBUG, "Message Null");
        return;
    }

    // append arguments onto signal
    dbus_message_iter_init_append(msg, &args);
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &position)) {
        FcitxLog(DEBUG, "Out Of Memory!");
    }

    // send the message and flush the connection
    if (!dbus_connection_send(conn, msg, &serial)) {
        FcitxLog(DEBUG, "Out Of Memory!");
    }

    // free the message
    dbus_message_unref(msg);
}

void FcitxSunPinyinBus::DimUpdatePreeditText(DBusConnection* conn, char *text)
{
    dbus_uint32_t serial = 0; // unique number to associate replies with requests
    DBusMessage* msg;
    DBusMessageIter args;

    // create a signal and check for errors
    msg = dbus_message_new_signal(FCITX_SUNPINYIN_PATH, // object name of the signal
                                  FCITX_SUNPINYIN_INTERFACE, // interface name of the signal
                                  "UpdatePreeditText"); // name of the signal
    if (NULL == msg) {
        FcitxLog(DEBUG, "Message Null");
        return;
    }

    char *attr = "";
    // append arguments onto signal
    dbus_message_iter_init_append(msg, &args);
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &text)) {
        FcitxLog(DEBUG, "Out Of Memory!");
    }
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &attr)) {
        FcitxLog(DEBUG, "Out Of Memory!");
    }

    // send the message and flush the connection
    if (!dbus_connection_send(conn, msg, &serial)) {
        FcitxLog(DEBUG, "Out Of Memory!");
    }

    // free the message
    dbus_message_unref(msg);
}

void FcitxSunPinyinBus::DimUpdateAux(DBusConnection* conn, char *text)
{
    dbus_uint32_t serial = 0; // unique number to associate replies with requests
    DBusMessage* msg;
    DBusMessageIter args;

    // create a signal and check for errors
    msg = dbus_message_new_signal(FCITX_SUNPINYIN_PATH, // object name of the signal
                                  FCITX_SUNPINYIN_INTERFACE, // interface name of the signal
                                  "UpdateAux"); // name of the signal
    if (NULL == msg) {
        FcitxLog(DEBUG, "Message Null");
        return;
    }
    if (!fcitx_utf8_check_string(text))
        return;

    char *attr = "";

    // append arguments onto signal
    dbus_message_iter_init_append(msg, &args);
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &text)) {
        FcitxLog(DEBUG, "Out Of Memory!");
    }
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &attr)) {
        FcitxLog(DEBUG, "Out Of Memory!");
    }

    // send the message and flush the connection
    if (!dbus_connection_send(conn, msg, &serial)) {
        FcitxLog(DEBUG, "Out Of Memory!");
    }

    // free the message
    dbus_message_unref(msg);
}

void FcitxSunPinyinBus::DimUpdateSpotLocation(DBusConnection* conn, int x, int y)
{
    if (lastUpdateX == x && lastUpdateY == y)
        return;
    lastUpdateX = x;
    lastUpdateY = y;
    dbus_uint32_t serial = 0; // unique number to associate replies with requests
    DBusMessage* msg;
    DBusMessageIter args;

    // create a signal and check for errors
    msg = dbus_message_new_signal(FCITX_SUNPINYIN_PATH, // object name of the signal
                                  FCITX_SUNPINYIN_INTERFACE, // interface name of the signal
                                  "UpdateSpotLocation"); // name of the signal
    if (NULL == msg) {
        FcitxLog(DEBUG, "Message Null");
        return;
    }

    // append arguments onto signal
    dbus_message_iter_init_append(msg, &args);
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &x)) {
        FcitxLog(DEBUG, "Out Of Memory!");
    }
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &y)) {
        FcitxLog(DEBUG, "Out Of Memory!");
    }

    // send the message and flush the connection
    if (!dbus_connection_send(conn, msg, &serial)) {
        FcitxLog(DEBUG, "Out Of Memory!");
    }

    // free the message
    dbus_message_unref(msg);
}

void FcitxSunPinyinBus::DimUpdateScreen(DBusConnection* conn, int id)
{
    dbus_uint32_t serial = 0; // unique number to associate replies with requests
    DBusMessage* msg;
    DBusMessageIter args;

    // create a signal and check for errors
    msg = dbus_message_new_signal(FCITX_SUNPINYIN_PATH, // object name of the signal
                                  FCITX_SUNPINYIN_INTERFACE, // interface name of the signal
                                  "UpdateScreen"); // name of the signal
    if (NULL == msg) {
        FcitxLog(DEBUG, "Message Null");
        return;
    }

    // append arguments onto signal
    dbus_message_iter_init_append(msg, &args);
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &id)) {
        FcitxLog(DEBUG, "Out Of Memory!");
    }

    // send the message and flush the connection
    if (!dbus_connection_send(conn, msg, &serial)) {
        FcitxLog(DEBUG, "Out Of Memory!");
    }

    // free the message
    dbus_message_unref(msg);
}

void FcitxSunPinyinBus::DimEnable(DBusConnection* conn, boolean toEnable)
{
    dbus_uint32_t serial = 0; // unique number to associate replies with requests
    DBusMessage* msg;
    DBusMessageIter args;

    // create a signal and check for errors
    msg = dbus_message_new_signal(FCITX_SUNPINYIN_PATH, // object name of the signal
                                  FCITX_SUNPINYIN_INTERFACE, // interface name of the signal
                                  "Enable"); // name of the signal
    if (NULL == msg) {
        FcitxLog(DEBUG, "Message Null");
        return;
    }

    // append arguments onto signal
    dbus_message_iter_init_append(msg, &args);
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_BOOLEAN, &toEnable)) {
        FcitxLog(DEBUG, "Out Of Memory!");
    }

    // send the message and flush the connection
    if (!dbus_connection_send(conn, msg, &serial)) {
        FcitxLog(DEBUG, "Out Of Memory!");
    }

    // free the message
    dbus_message_unref(msg);
}

void FcitxSunPinyinBus::DimpanelOwnerChanged(const char* serviceName, const char* oldName, const char* newName) {
    /* old die and no new one */
    if (strcmp(serviceName, "org.sunpinyin.impanel") == 0) {
        if (strlen(oldName) > 0 && strlen(newName) == 0) {
//            FcitxUISwitchToFallback(m_sunpinyin->owner);
        }
    }
}

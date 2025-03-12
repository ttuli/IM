QT       += core gui network quickwidgets sql quick multimedia concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ChatState/addfriend.cpp \
    ChatState/chatdialog.cpp \
    ChatState/chatdialogmodel.cpp \
    ChatState/chatlist.cpp \
    ChatState/chatlistmodel.cpp \
    ChatState/chatstate.cpp \
    ChatState/customdialog.cpp \
    ChatState/emojiimageprovider.cpp \
    ChatState/emojimodel.cpp \
    ChatState/personalinfodialog.cpp \
    ChatState/searchmodel.cpp \
    ChatState/storeemoji.cpp \
    IM_login.pb.cc \
    PictureViewer/pictureview.cpp \
    PictureViewer/pictureviewmanager.cpp \
    customizequickwidget.cpp \
    framless_dialog.cpp \
    friendAgroup/friendapplymodel.cpp \
    friendAgroup/friendlist.cpp \
    friendAgroup/friendlistmodel.cpp \
    generalimageprovider.cpp \
    imagemanager.cpp \
    imageprovider.cpp \
    login.cpp \
    main.cpp \
    networkmanager.cpp \
    register_dialog.cpp \
    sqlitemanager.cpp \
    threadwork.cpp \
    windows_background.cpp

HEADERS += \
    ChatState/addfriend.h \
    ChatState/chatdialog.h \
    ChatState/chatdialogmodel.h \
    ChatState/chatlist.h \
    ChatState/chatlistmodel.h \
    ChatState/chatstate.h \
    ChatState/customdialog.h \
    ChatState/emojiimageprovider.h \
    ChatState/emojimodel.h \
    ChatState/personalinfodialog.h \
    ChatState/searchmodel.h \
    ChatState/storeemoji.h \
    IM_login.pb.h \
    PictureViewer/pictureview.h \
    PictureViewer/pictureviewmanager.h \
    customizequickwidget.h \
    framless_dialog.h \
    friendAgroup/friendapplymodel.h \
    friendAgroup/friendlist.h \
    friendAgroup/friendlistmodel.h \
    generalimageprovider.h \
    imagemanager.h \
    imageprovider.h \
    login.h \
    networkmanager.h \
    register_dialog.h \
    sqlitemanager.h \
    threadwork.h \
    windows_background.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += E:\Github\protobuf\protobuf-3.15.0\src
LIBS += -ldwmapi \
        -LE:\Github\protobuf\build -lprotobuf

RESOURCES += \
    qml.qrc \
    res.qrc

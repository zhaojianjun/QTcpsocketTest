#include "mainwindow.h"
#include <QApplication>

#include "Socket/sockethandler.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<QAbstractSocket::SocketError>("SocketError");
    qRegisterMetaType<QAbstractSocket::SocketState>("SocketState");

    MainWindow w;
    w.show();
    //strRoomId "9684" "131"(2019-07-01 10:58:15周一)
    //vCreatConf "131" "1" "老师审核" "1244" "Sra9j77Y3Y4k3SoNfVoLAnKA18pk0f4m9kmipHaWVHs=" "9684" "T131"(2019-07-01 10:58:36周一)


//    RoomInfo sRoomInfo;
//    sRoomInfo.strUid = "T131";
//    sRoomInfo.strRole = "1";
//    sRoomInfo.strName = "Teacher";
//    sRoomInfo.strHeadIcon = "";
//    sRoomInfo.strArgoId = "";
//    sRoomInfo.strToken = "Sra9j77Y3Y4k3SoNfVoLAnKA18pk0f4m9kmipHaWVHs=";

//    SocketHandler::getInstance()->vSetAddressInfo("192.168.40.161", "8080");
//    SocketHandler::getInstance()->vSetRoomInfo("9684", "131", sRoomInfo);

//    SocketHandler::getInstance()->start();

    return a.exec();
}

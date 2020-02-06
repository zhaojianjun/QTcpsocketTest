#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Socket/sockethandler.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(SocketHandler::getInstance(), &SocketHandler::sigLoginInfo, this, &MainWindow::socket_connect);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    RoomInfo sRoomInfo;
    sRoomInfo.strUid = "T131";
    sRoomInfo.strRole = "1";
    sRoomInfo.strName = "Teacher";
    sRoomInfo.strHeadIcon = "";
    sRoomInfo.strArgoId = "";
    sRoomInfo.strToken = "CjBY4kU2gmSgBSafh3fH8HKA18pk0f4m9kmipHaWVHs=";

    SocketHandler::getInstance()->vSetAddressInfo("192.168.40.161", "8080");
    SocketHandler::getInstance()->vSetRoomInfo("9684", "131", sRoomInfo);

    SocketHandler::getInstance()->start();
}

void MainWindow::socket_connect()
{
    qDebug()<<"hhhhhhhhhhh";
    SocketHandler::getInstance()->vCreatRoom();
}

void MainWindow::on_pushButton_2_clicked()
{
    //SocketHandler::getInstance()->vLoginServer();
}

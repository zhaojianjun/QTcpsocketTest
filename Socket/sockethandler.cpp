#include "sockethandler.h"

#include <QHostAddress>
#include <QDataStream>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QThread>
#include <QTime>

SocketHandler* SocketHandler::pSocketHandler = NULL;

SocketHandler *SocketHandler::getInstance()
{
    if(pSocketHandler == NULL)
    {
        pSocketHandler = new SocketHandler();
    }

    return pSocketHandler;
}

SocketHandler::SocketHandler(QObject *parent) : QThread(parent)
{

    m_pHeartBeatTask = new QTimer(this);
    if(m_pHeartBeatTask)
    {
        m_pHeartBeatTask->setInterval(10000);
    }

    m_pConnectTask = new QTimer(this);
    if(m_pConnectTask)
    {
        m_pConnectTask->setInterval(60000);
    }

    //connect(m_pTcpSocket, &QTcpSocket::connected, this, &SocketHandler::connected_success);
//    connect(m_pTcpSocket, &QTcpSocket::bytesWritten, this, &SocketHandler::bytes_written);
//    connect(m_pTcpSocket, &QTcpSocket::readyRead, this, &SocketHandler::read_message);
//    connect(m_pTcpSocket, &QTcpSocket::disconnected, this, &SocketHandler::disconnected_socket);
//    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this,SLOT(display_Error(QAbstractSocket::SocketError)));

    connect(m_pConnectTask,&QTimer::timeout,this, &SocketHandler::new_connect, Qt::DirectConnection);
}

void SocketHandler::vSetAddressInfo(QString strAddress, QString strPort)
{
    m_strAddress = strAddress;
    m_strPort = strPort;
}

void SocketHandler::vSetRoomInfo(QString strRoomId, QString strSendUid, RoomInfo sRoomInfo)
{
    m_strRoomId = strRoomId;
    m_strSendUid = "T" + strSendUid;
    //m_sRoomInfo = sRoomInfo;

    //memcpy(&m_sRoomInfo, &sRoomInfo, sizeof(sRoomInfo));
    m_sRoomInfo.strUid = sRoomInfo.strUid;
    m_sRoomInfo.strRole = sRoomInfo.strRole;
    m_sRoomInfo.strName = sRoomInfo.strName;
    m_sRoomInfo.strHeadIcon = sRoomInfo.strHeadIcon;
    m_sRoomInfo.strArgoId = sRoomInfo.strArgoId;
    m_sRoomInfo.strToken = sRoomInfo.strToken;

    qDebug()<<m_sRoomInfo.strArgoId<<m_sRoomInfo.strToken<<m_sRoomInfo.strUid;
}

void SocketHandler::vLoginServer()
{
    if(m_pTcpSocket)
    {
        m_pTcpSocket->abort();
        m_pTcpSocket->connectToHost(m_strAddress, m_strPort.toInt(), QIODevice::ReadWrite, QAbstractSocket::AnyIPProtocol);
        qDebug()<<"Start connectToHost";
    }
}

void SocketHandler::vLogoutServer()
{
    vStopHeartBeat();
    vStopAutoConnect();
    if(m_pTcpSocket)
    {
        m_pTcpSocket->disconnectFromHost();
    }

    m_bAutoLogin = true;
}

void SocketHandler::vCreatConf(QString strUid, QString strRole, QString strName, QString strHeadIcon, QString strArgoId, QString strToken)
{
    qDebug()<<"vCreatConf"<<strUid<<strRole<<strArgoId<<m_strRoomId<<m_strSendUid;

    QMap<int, QString> mapMes;
    mapMes.clear();
    mapMes.insert(1, m_sRoomInfo.strUid);
    mapMes.insert(2, m_sRoomInfo.strRole);
    mapMes.insert(3, m_sRoomInfo.strName);
    mapMes.insert(4, m_sRoomInfo.strHeadIcon);
    mapMes.insert(5, m_sRoomInfo.strArgoId);
    mapMes.insert(6, m_sRoomInfo.strToken);
    vCollectivePacking(SCREATECONF, rand(), m_strRoomId, m_strSendUid, "", mapMes);
}

void SocketHandler::vCreatRoom()
{
    qDebug()<<"vCreatRoom....";
    emit sigCreateRoom();
}

void SocketHandler::vLeaveConf()
{
    QMap<int, QString> mapMes;
    mapMes.clear();
    vCollectivePacking(SDELETECONF, rand(), m_strRoomId, m_strSendUid, "", mapMes);

    m_bTotleByteArray.clear();

    m_bAutoLogin = true;
}

void SocketHandler::getRoomMember()
{
    QMap<int, QString> mapMes;
    mapMes.clear();
    vCollectivePacking(SGETMEMBER, 0, m_strRoomId, m_strSendUid, "", mapMes);
}

void SocketHandler::sendMessage(int iMesType, QMap<int, QString> mapMes)
{
    vCollectivePacking(iMesType, 0, m_strRoomId, m_strSendUid, "", mapMes);
}

void SocketHandler::sendRoomMessage(const QString &strRoomId, const QString &strData, const QString &strReceiveUid)
{
    QMap<int, QString> mapMes;
    mapMes.insert(1, strData);
    vCollectivePacking(SCONFINFO, 0, m_strRoomId, m_strSendUid, strReceiveUid, mapMes);
}

void SocketHandler::vDeleteConf()
{
    QMap<int, QString> mapMes;
    mapMes.insert(1, "");
    vCollectivePacking(308, 0, m_strRoomId, m_strSendUid, "", mapMes);
}

void SocketHandler::getShareContent(QString strKey, QString strType)
{
    QMap<int, QString> mapMes;
    mapMes.insert(1, strKey);
    mapMes.insert(2, strType);
    vCollectivePacking(SGETSHARECONTENT, 0, m_strRoomId, m_strSendUid, "", mapMes);
}

void SocketHandler::creatShareContent(QString strKey, QString strValue, QString strType)
{
    QMap<int, QString> mapMes;
    mapMes.insert(1, strKey);
    mapMes.insert(2, strValue);
    mapMes.insert(3, strType);
    mapMes.insert(4, "upd");
    vCollectivePacking(SCREATSHARECONTENT, 0, m_strRoomId, m_strSendUid, "", mapMes);
}

void SocketHandler::updateShareContent(QString strKey, QString strValue, QString strType)
{
    QMap<int, QString> mapMes;
    mapMes.insert(1, strKey);
    mapMes.insert(2, strValue);
    mapMes.insert(3, strType);
    mapMes.insert(4, "upd");
    vCollectivePacking(SCREATSHARECONTENT, 0, m_strRoomId, m_strSendUid, "", mapMes);
}

void SocketHandler::deleteShareContent(QString strKey, QString strType)
{
    QMap<int, QString> mapMes;
    mapMes.insert(1, strKey);
    mapMes.insert(2, "");
    mapMes.insert(3, strType);
    mapMes.insert(4, "del");
    vCollectivePacking(SCREATSHARECONTENT, 0, m_strRoomId, m_strSendUid, "", mapMes);
}

void SocketHandler::removeShareContent(QString strKey, QString strType, QString strCount)
{
    QMap<int, QString> mapMes;
    mapMes.insert(1, strKey);
    mapMes.insert(2, "");
    mapMes.insert(3, strType);
    mapMes.insert(4, "remove.n.index");
    mapMes.insert(7, strCount);
    vCollectivePacking(SCREATSHARECONTENT, 0, m_strRoomId, m_strSendUid, "", mapMes);
}

void SocketHandler::vCollectivePacking(short iMesType, int iEventId, QString strRoomId, QString strSendUid, QString strReceiveUid, QMap<int, QString> mapMes)
{
    qDebug()<<strRoomId<<strSendUid<<mapMes.count();
    int iMapCount = mapMes.count();
    QByteArray blockData;
    QDataStream out(&blockData,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_5);
    out.setByteOrder(QDataStream::BigEndian);
    out<<(int) 0x04;
    out<<(short) iMesType;
    out<<(int) rand();
    out.writeBytes(strRoomId.toStdString().c_str(), qstrlen(strRoomId.toStdString().c_str()));
    out.writeBytes(strSendUid.toStdString().c_str(), qstrlen(strSendUid.toStdString().c_str()));
    out.writeBytes(strReceiveUid.toStdString().c_str(), qstrlen(strReceiveUid.toStdString().c_str()));
    out<<(int) 0;
    out<<(int) iMapCount;

    QMapIterator<int, QString> map(mapMes);
    while (map.hasNext())
    {
        map.next();
        int key = map.key();
        QString strValue = map.value();
        out<<(int) key;

        qDebug()<<strValue;
        out.writeBytes(strValue.toStdString().c_str(), qstrlen(strValue.toStdString().c_str()));
    }

    out.device()->seek(0);
    int len = blockData.size();
    out << len - 4;

    int iRet = m_pTcpSocket->write(blockData);
    if(iRet < 0)
    {

    }
    m_pTcpSocket->flush();
}

void SocketHandler::vBuffer()
{
    m_bTotleByteArray = m_bTotleByteArray + m_pTcpSocket->readAll();

    int iTotleSize = 0;

    while(true)
    {
        QDataStream in(m_bTotleByteArray);
        in.setVersion(QDataStream::Qt_5_5);
        in.setByteOrder(QDataStream::BigEndian);
        in >> iTotleSize;

        if(m_bTotleByteArray.size() - 4 >= iTotleSize)
        {
            vUnPack(in);
            m_bTotleByteArray = m_bTotleByteArray.remove(0, iTotleSize + 4);
            if(m_bTotleByteArray.size() < 4)
            {
                break;
            }
        }
        else if(m_bTotleByteArray.size() < iTotleSize)
        {
            break;
        }
    }
}

void SocketHandler::vStopHeartBeat()
{
    if (m_pHeartBeatTask->isActive())
    {
        m_pHeartBeatTask->stop();
    }
}

void SocketHandler::vStopAutoConnect()
{
    m_pConnectTask->stop();
}

void SocketHandler::vReloginServer()
{
    if(m_pTcpSocket && !m_bAutoLogin)
    {
        m_pTcpSocket->abort();
        //连接服务器
        QHostAddress host = QHostAddress(m_strAddress);
        m_pTcpSocket->connectToHost(host,m_strPort.toInt());
    }
}

void SocketHandler::vUnPack(QDataStream &in)
{
    short   iMesbType = 0;
    int     iEventId = 0;
    int     iRoomIdSize = 0;
    int     iSendUidSize;
    int     iReceiveUidSize;
    int     id;
    int     size;
    int     mesSize;
    int     key;

    in >> iMesbType;
    in >> iEventId;
    in >> iRoomIdSize;
    char *roomId = new char[iRoomIdSize+1]{};
    in.readRawData(roomId, iRoomIdSize);
    in >> iSendUidSize;
    char *sendUid = new char[iSendUidSize+1]{};
    in.readRawData(sendUid, iSendUidSize);
    in >> iReceiveUidSize;
    char *receiveUid = new char[iReceiveUidSize+1]{};
    in.readRawData(receiveUid, iReceiveUidSize);
    in >> id;
    in >> size;

    switch (iMesbType) {
    case RCREATECONFSUCCESS :          //创建房间成功消息
    {
        if(m_pHeartBeatTask)
        {
            m_pHeartBeatTask->start();
        }
        emit sigCreatConf();
    }
        break;
    case RMULTISPOTLOGIN:           //被迫下线消息
    {
        emit sigMultispotLogin();
    }
        break;
    case RDELETECONFSUCCESS:           //销毁房间成功消息
    {
        emit sigDeleteConf();
    }
        break;
    case RCONFINFO:           //房间内消息
    {
        in >> key;
        in >> mesSize;
        char *mesInfo = new char[mesSize+1]{};
        in.readRawData(mesInfo, mesSize);
        emit sigReceiveMessage(QString(sendUid), QString(mesInfo));
        delete [] mesInfo;
    }
        break;
    case RCONFMEMBERINFO:           //房间内用户列表信息
    {
        in >> key;
        in >> mesSize;
        char *mesInfo = new char[mesSize+1]{};
        in.readRawData(mesInfo, mesSize);
        if(key == 11)
        {
            QString strMes = QString(mesInfo);

            QJsonParseError json_error;
            QJsonDocument jsonDocument = QJsonDocument::fromJson(strMes.toUtf8(), &json_error);

            if(!jsonDocument.isNull())
            {
                QJsonObject rootObject = jsonDocument.object();

                if(rootObject.contains("id") && rootObject.contains("name"))
                {
                    QString strUid = rootObject.take("id").toString();
                    QString strName = rootObject.take("name").toString();
                    QString strAgoraId = rootObject.take("agoraId").toString();
                    QJsonArray jsonarray = rootObject.take("role").toArray();
                    QString strRole = jsonarray.at(0).toString();
                    emit sigUserJoin(strUid, strName, strAgoraId, strRole);
                }
            }

            delete [] mesInfo;
        }
        else if(key == 12)
        {
            emit sigUserOffline(QString(mesInfo));
        }
        else if(key == 1)
        {
            QString strMember = QString(mesInfo);
            emit sigRoomMember(strMember);
        }
    }
        break;
    case RSHARECONTENT:           //获取房间内的共享内容
    {
        in >> key;
        in >> mesSize;
        char *keyInfo = new char[mesSize+1]{};
        in.readRawData(keyInfo, mesSize);

        in >> key;
        in >> mesSize;
        char *mesType = new char[mesSize+1]{};
        in.readRawData(mesType, mesSize);

        in >> key;
        in >> mesSize;
        char *mesInfo = new char[mesSize+1]{};
        in.readRawData(mesInfo, mesSize);
        emit sigGetShareContent(keyInfo, mesType, mesInfo);

        delete [] keyInfo;
        delete [] mesType;
        delete [] mesInfo;
    }
        break;
    case RUPDATECONTENT:
    {
        in >> key;
        in >> mesSize;
        char *keyInfo = new char[mesSize+1]{};
        in.readRawData(keyInfo, mesSize);

        in >> key;
        in >> mesSize;
        char *mesInfo = new char[mesSize+1]{};
        in.readRawData(mesInfo, mesSize);

        in >> key;
        in >> mesSize;
        char *mesType = new char[mesSize+1]{};
        in.readRawData(mesType, mesSize);
        emit sigUpdateShareContent(sendUid, keyInfo, mesType, mesInfo);


        delete [] keyInfo;
        delete [] mesType;
        delete [] mesInfo;
    }
        break;
    case RSHAREHISTORY:           //获取共享历史

        break;
    case RERRORINFO:           //错误消息
    {
        m_iId = id;
        if(id == 10098)
        {
            emit sigMultispotLogin();
        }
    }
        break;
    default:
        break;
    }

    delete [] roomId;
    delete [] sendUid;
    delete [] receiveUid;
}

void SocketHandler::setESocketState(const SocketState &eSocketState)
{
    m_eSocketState = eSocketState;
}

void SocketHandler::setBAutoLogin(bool bAutoLogin)
{
    m_bAutoLogin = bAutoLogin;
}

void SocketHandler::run()
{
    m_pTcpSocket = new QTcpSocket();
    if(m_pTcpSocket)
    {
        m_pTcpSocket->setSocketDescriptor(ptr);
    }

    m_pHeartBeatTask = new QTimer();
    if(m_pHeartBeatTask)
    {
        m_pHeartBeatTask->setInterval(10000);
    }

    qDebug()<<"run..........";

    if(m_eSocketState == Logining)
    {
        vLoginServer();
    }
    else if(m_eSocketState == ReLogining)
    {
    }
    else if(m_eSocketState == Sending)
    {
    }
    else if(m_eSocketState == Receiveing)
    {
    }
    else if(m_eSocketState == Logouting)
    {
    }
    else if(m_eSocketState == LoginingSuccess)
    {
    }

    connect(m_pTcpSocket, &QTcpSocket::connected, this, &SocketHandler::connected_success);
    connect(m_pTcpSocket, &QTcpSocket::disconnected, this, &SocketHandler::disconnected_socket);
    connect(m_pTcpSocket, &QTcpSocket::bytesWritten, this, &SocketHandler::bytes_written);
    connect(m_pTcpSocket, &QTcpSocket::readyRead, this, &SocketHandler::read_message);
    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this,SLOT(display_error(QAbstractSocket::SocketError)));
    connect(m_pTcpSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),this, SLOT(connected_state(QAbstractSocket::SocketState)));
    connect(m_pHeartBeatTask, &QTimer::timeout, this, &SocketHandler::heart_beat);
    connect(this, &SocketHandler::sigCreateRoom, this, &SocketHandler::create_room);

    exec();

    qDebug()<<"KKKKKK";
}

void SocketHandler::connected_success()
{
    qDebug()<<"connected success";

    m_eSocketState = LoginingSuccess;

    emit sigLoginInfo(200);

    //vCreatConf("", "", "", "", "", "");
}

void SocketHandler::create_room()
{
    QMap<int, QString> mapMes;
    mapMes.clear();
    mapMes.insert(1, m_sRoomInfo.strUid);
    mapMes.insert(2, m_sRoomInfo.strRole);
    mapMes.insert(3, m_sRoomInfo.strName);
    mapMes.insert(4, m_sRoomInfo.strHeadIcon);
    mapMes.insert(5, m_sRoomInfo.strArgoId);
    mapMes.insert(6, m_sRoomInfo.strToken);

    vCollectivePacking(SCREATECONF, rand(), m_strRoomId, m_strSendUid, "", mapMes);
}

void SocketHandler::bytes_written()
{
    m_bSend = true;
}

void SocketHandler::read_message()
{
    qDebug()<<"jjjjj";
    vBuffer();
}

void SocketHandler::display_error(QAbstractSocket::SocketError error)
{
    qDebug()<<"display error:"<<error;

    switch (error) {
    case QAbstractSocket::NetworkError:

        break;
    case QAbstractSocket::RemoteHostClosedError:

        break;
    default:
        break;
    }
}

void SocketHandler::connected_state(QAbstractSocket::SocketState state)
{
    qDebug()<<"state:"<<state;
}

void SocketHandler::heart_beat()
{
    QMap <int, QString> mapMes;
    mapMes.clear();
    sendMessage(SHEARBEAT, mapMes);
}

void SocketHandler::disconnected_socket()
{
    qDebug()<<"disconnected socket";

    vStopHeartBeat();
    vStopAutoConnect();

    if(m_bAutoLogin == false)
    {
        vReloginServer();
        if(m_pConnectTask)
        {
            m_pConnectTask->start(10000);
        }
    }
}

void SocketHandler::new_connect()
{
    qDebug()<<"new connect";

    if(m_iId != 10098)
    {
        vReloginServer();
    }
    else
    {
        vStopHeartBeat();
        vStopAutoConnect();
    }
}



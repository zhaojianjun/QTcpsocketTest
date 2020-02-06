#ifndef INFOTYPEDEFINE_H
#define INFOTYPEDEFINE_H

#define         SCREATECONF                     101         //创建房间
#define         SHEARBEAT                       102         //发送心跳包
#define         SDELETECONF                     105         //销毁房间
#define         SCONFINFO                       302         //发消息到房间内
#define         SGETMEMBER                      304         //获取房间成员
#define         SGETSHARECONTENT                305         //获取共享内容
#define         SCREATSHARECONTENT              306         //创建\修改共享内容
#define         SDELETESHARECONTENT             307         //删除共享内容


#define         RCREATECONFSUCCESS              201         //创建房间成功消息
#define         RMULTISPOTLOGIN                 204         //被迫下线消息
#define         RDELETECONFSUCCESS              205         //销毁房间成功消息
#define         RCONFINFO                       402         //收到房间内消息
#define         RCONFMEMBERINFO                 404         //房间内用户进出消息
#define         RSHARECONTENT                   405         //获取房间内共享内容
#define         RUPDATECONTENT                  406         //共享对象修改
#define         RSHAREHISTORY                   415         //获取共享历史
#define         RERRORINFO                      501         //错误消息


#endif // INFOTYPEDEFINE_H

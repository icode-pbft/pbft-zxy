//
// Created by SDEE on 2019/7/13.
//

#ifndef PBFT_REQUEST_H
#define PBFT_REQUEST_H

#include <iostream>
#include <json/json.h>

using namespace std;


struct Request {

    int type; //消息类型
    int onode; //发起请求的节点
    int vnum; //视图编号
    long time; //时间戳
//    int client; //客户端
    size_t digest; //摘要
    string message; //信息
    int seriesNumber; //序列号

    int masterId;   //主节点id
    vector<int> selectedNodesList;  //已选列表


public:

    Request fromJson(const string &blockStr);

    Json::Value toJsonValue();
};

class Quest {


public:

    Request static toMessage(string &message, int vnum, int client, int masterId, vector<int> selectedNodesList);

    Request static toRequest(string &quest);

    //1. <REUEST,o,t,m> o->operation,t->timestamp,message->信息
    Request static toPrePrepare(Request &quest, int i, int no);

    //2. <<PRE-PREPARE,v,n,d>m> v->view,n->序列号,d->m的消息摘要,m->捎带给backup的消息
    Request static toPrepare(Request &quest, int i);

    //3. <PREPARE,v,n,d,i> v->view,n->序列号，d->m的消息摘要，i->节点i的编号
    Request static toCommit(Request &quest, int i);

    //4. <COMMIT,v,n,D(m),i> v->view,n->序列号,D(m)->m的消息摘要，i->节点i的编号
    Request static toReply(Request &quest, int i, string message);

    //5. <REPLY,v,t,c,i,r> v->view,t->timestamp,c->client,i->节点i的编号,r->结果
//    Request static toAnswer(Request &quest, int i);

};

#endif //PBFT_REQUEST_H

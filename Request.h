//
// Created by SDEE on 2019/7/13.
//

#ifndef PBFT_REQUEST_H
#define PBFT_REQUEST_H

#include <iostream>
#include <json/json.h>

using namespace std;


struct Request {

    int type; //��Ϣ����
    int onode; //��������Ľڵ�
    int vnum; //��ͼ���
    long time; //ʱ���
//    int client; //�ͻ���
    size_t digest; //ժҪ
    string message; //��Ϣ
    int seriesNumber; //���к�

    int masterId;   //���ڵ�id
    vector<int> selectedNodesList;  //��ѡ�б�


public:

    Request fromJson(const string &blockStr);

    Json::Value toJsonValue();
};

class Quest {


public:

    Request static toMessage(string &message, int vnum, int client, int masterId, vector<int> selectedNodesList);

    Request static toRequest(string &quest);

    //1. <REUEST,o,t,m> o->operation,t->timestamp,message->��Ϣ
    Request static toPrePrepare(Request &quest, int i, int no);

    //2. <<PRE-PREPARE,v,n,d>m> v->view,n->���к�,d->m����ϢժҪ,m->�Ӵ���backup����Ϣ
    Request static toPrepare(Request &quest, int i);

    //3. <PREPARE,v,n,d,i> v->view,n->���кţ�d->m����ϢժҪ��i->�ڵ�i�ı��
    Request static toCommit(Request &quest, int i);

    //4. <COMMIT,v,n,D(m),i> v->view,n->���к�,D(m)->m����ϢժҪ��i->�ڵ�i�ı��
    Request static toReply(Request &quest, int i, string message);

    //5. <REPLY,v,t,c,i,r> v->view,t->timestamp,c->client,i->�ڵ�i�ı��,r->���
//    Request static toAnswer(Request &quest, int i);

};

#endif //PBFT_REQUEST_H

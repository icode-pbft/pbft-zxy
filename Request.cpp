//
// Created by SDEE on 2019/7/13.
//

#include <iostream>
#include <vector>
#include <time.h>
#include <sstream>
#include "Request.h"

using namespace std;

//int Quest::seriesNumbers = -1;

Request Quest::toMessage(string &message, int vnum, int client, int masterId, vector<int> selectedNodesList) {

    //创建时间戳
    time_t t = time(nullptr);
    long timeTamp = t;

    hash<string> h;

    Request rRequest = {0, client, vnum, timeTamp, h(message), message, -1, masterId, selectedNodesList};

    return rRequest;
}


//1. X <REUEST,o,t,m> o->operation,t->timestamp,message->信息
Request Quest::toRequest(string &quest) {

//    int tempNode = 0;

    //创建时间戳
    time_t t = time(nullptr);
    long timeTamp = t;


    hash<string> h;
    //消息类型，发起请求的结点，视图编号，时间戳，摘要，信息，序列号
//    Request rRequest = {0, NULL, NULL, timeTamp, NULL, quest, NULL};
    Request rRequest = {0, -1, -1, timeTamp, h(quest), quest, -1};

    return rRequest;

}

//2. <PRE-PREPARE,v,n,d> v->view,n->序列号,d->m的消息摘要
Request Quest::toPrePrepare(Request &quest, int i, int seriesNumber) {


    //消息类型，发起请求的结点，视图编号，时间戳，摘要，信息，序列号
//    Request ppRequest = {1, i, viewNum, quest.time, "master", quest.message, seriesNumbers};
    Request ppRequest = {1, i, quest.vnum, quest.time, quest.digest, quest.message, seriesNumber};

    return ppRequest;

}

//3. <PREPARE,v,n,d,i> v->view,n->序列号，d->m的消息摘要，i->节点i的编号
Request Quest::toPrepare(Request &quest, int i) {

    Request pRequest = {2, i, quest.vnum, quest.time, quest.digest, quest.message, quest.seriesNumber};
    return pRequest;

}

//4. <COMMIT,v,n,D(m),i> v->view,n->序列号,D(m)->m的消息摘要，i->节点i的编号
Request Quest::toCommit(Request &quest, int i) {

    //消息类型，发起请求的结点，视图编号，时间戳，摘要，信息，序列号
    Request pRequest = {3, i, quest.vnum, quest.time, quest.digest, quest.message, quest.seriesNumber};
    return pRequest;
}

//5. <REPLY,v,t,c,i> v->view,t->timestamp,c->client,i->节点i的编号
Request Quest::toReply(Request &quest, int i, string message) {

//    //创建时间戳
//    time_t t = time(0);
//    long timeTamp = t;

    //消息类型，发起请求的结点，视图编号，时间戳，摘要，信息，序列号
//    Request pRequest = {4, i, viewNum, quest.time, "消息摘要->主节点id", quest.message, quest.seriesNumber};
    hash<string> h;
    Request pRequest = {4, i, quest.vnum, quest.time, h(message), message, quest.seriesNumber};

    return pRequest;

}

Request Request::fromJson(const string &blockStr) {
    Json::Value jsonItem;
    Json::CharReaderBuilder builder;
    Json::CharReader *reader(builder.newCharReader());
    JSONCPP_STRING errors;

    Request request;
    if (reader->parse(blockStr.data(), blockStr.data() + blockStr.size(), &jsonItem, &errors)) {
//        long time; //时间戳
        long tempTime;
        istringstream isTime(jsonItem["time"].asString());
        isTime >> tempTime;

//        size_t digest; //摘要
        size_t tempDigest;
        istringstream isDigest(jsonItem["digest"].asString());
        isDigest >> tempDigest;

//        vector<int> selectedNodesList;  //已选列表
        stringstream sTemp(jsonItem["selectedNodesList"].asString());
        int i;
        vector<int> tempList;
        while (sTemp >> i) {
            tempList.push_back(i);
        }

        request = {jsonItem["type"].asInt(), jsonItem["onode"].asInt(), jsonItem["vnum"].asInt(),
                   tempTime, tempDigest, jsonItem["message"].asString(),
                   jsonItem["seriesNumber"].asInt(), jsonItem["masterId"].asInt(), tempList};

    }

    return request;
}


Json::Value Request::toJsonValue() {
    Json::Value result;

    result["type"] = this->type;
    result["onode"] = this->onode;
    result["vnum"] = this->vnum;

    string tempTime = to_string(this->time);
    result["time"] = tempTime;
    string tempDigest = to_string(this->digest);
    result["digest"] = tempDigest;
    result["message"] = this->message;
    result["seriesNumber"] = this->seriesNumber;
    result["masterId"] = this->masterId;

    stringstream ss;
    for (size_t i = 0; i < this->selectedNodesList.size(); ++i) {
        if (i != 0) {
            ss << " ";
        }
        ss << this->selectedNodesList[i];
    }
    string tempSelectedNodesList = ss.str();
    result["selectedNodesList"] = tempSelectedNodesList;

    return result;
}

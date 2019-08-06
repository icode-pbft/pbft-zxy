//
// Created by SDEE on 2019/8/2.
//


#include "ClientController.h"
#include "Client.h"
#include <time.h>

using namespace std;
map<size_t, string> ClientController::quests;

//1.接收请求.
//2.接收反馈.
string ClientController::getConsensus(vector<int> cans, string quest) {

    map<size_t, string>::iterator iter;
    iter = quests.begin();
    while (iter != quests.end()) {
        if (quest == iter->second) {
            return "短时间内多次提案";
        }
        iter++;
    }


    //若是队列中没有找到，则进行操作.
    time_t t = time(nullptr);
    quests[t] = quest;

    Client client;

    if (client.isSixRound()) {
        client.selectRandomNode(cans);
        client.selectMasterNode();
    }

    Request request = Quest::toMessage(quest, client.getViewNum(), client.getClientId(), client.getMasterId(),
                                       client.getSelectedNodesList());
    request.time = t;
    // todo: NodeController 修改.
    sendRequest(request);
    getReply();

    //fixme:等待客户验证接口；
    if (1) {
        //todo:如果正确消息收到f+1
        return "达成共识";
    }
    return std::string();
}

void ClientController::sendRequest(Request request) {

}

string ClientController::getTestConsensus(vector<int> selectedCans, Request quest) {
    return std::string();
}

string ClientController::getErrorNodesMessage(vector<int> cans, Request quest) {
    return std::string();
}

void ClientController::getReply() {

}

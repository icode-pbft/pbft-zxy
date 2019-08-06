//
// Created by SDEE on 2019/8/2.
//

#ifndef DPBFT_CLIENTCONTROLLER_H
#define DPBFT_CLIENTCONTROLLER_H


#include "Request.h"

class ClientController {

//    static vector<string> quests;
    static map<size_t, string> quests;

    // 提供给用户的共识验证的接口
    string getConsensus(vector<int> cans, string quest);

    //内部测试节点
    string getTestConsensus(vector<int> selectedCans, Request quest);

    // 得到错误节点信息
    string getErrorNodesMessage(vector<int> cans, Request quest);


    //广播消息
    void sendRequest(Request request);

    //监听消息
    void getReply();


};


#endif //DPBFT_CLIENTCONTROLLER_H

//
// Created by HP on 2019/8/2.
//

#ifndef DPBFT_CLIENT_H
#define DPBFT_CLIENT_H

#include <iostream>
#include <vector>
#include "Request.h"

using namespace std;

class Client {
private:
    static int clientId;    //客户端id
    static int viewNum;  //视图编号.
    static int candidaSize;  //待选节点数.
    static int selectSize;   //选择节点数.
    static int masterId; //主节点Id.
    static int changeRound; //切换轮数.
    static int round;  //当前轮数.
    static vector<int> selectedNodesList;  //已选列表.

    static map<string, int> countReply;  //回复统计
public:

//    Client();

    //根据待选列表随机选出投票节点.
    void selectRandomNode(vector<int> cans);

    //判断是否需要切换节点.
    bool isSixRound();

    //选出主节点.
    void selectMasterNode();


    //getter&setter
    static int getViewNum();

    static void setViewNum(int viewNum);

    static int getCandidaSize();

    static void setCandidaSize(int candidaSize);

    static int getSelectSize();

    static void setSelectSize(int selectSize);

    static int getMasterId();

    static void setMasterId(int masterId);

    static int getChangeRound();

    static void setChangeRound(int changeRound);

    static int getRound();

    static void setRound(int round);

    static const vector<int> &getSelectedNodesList();

    static void setSelectedNodesList(const vector<int> &selectedNodesList);

    static int getClientId();

    static void setClientId(int clientId);

};


#endif //DPBFT_CLIENT_H

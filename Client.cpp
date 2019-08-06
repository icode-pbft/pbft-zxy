//
// Created by HP on 2019/8/2.
//

#include "Client.h"
#include "config/Config.h"
#include <set>
#include <time.h>


//一直接收请求，每接收一次请求，创建一个action线程

const char ConfigFile[] = "../Config/config.txt";

int Client::clientId = 0;
int Client::viewNum = 0;
int  Client::candidaSize = 0;  //待选节点数
int  Client::selectSize = 0;   //选择节点数
int  Client::masterId = 0; //主节点Id
int  Client::changeRound = 0; //切换轮数
int Client::round = 0;  // 当前轮数
vector<int> Client::selectedNodesList;  //已选列表
map<string, int>  Client::countReply;  //回复统计

void Client::selectRandomNode(vector<int> cans) {
    int candidaSize = cans.size();  //待选节点数
    int selectSize = getSelectSize();   //选择节点数

    set<int> select;
    vector<int> changeSelect;

    srand((unsigned) time(NULL));
    while (select.size() < selectSize) {
        int index = (rand() % candidaSize);
        select.insert(index);
    }

    set<int>::iterator iter = select.begin();
    while (iter != select.end()) {
        int index = *iter;
        changeSelect.push_back(cans[index]);
        cout << cans[index] << " ";
        ++iter;
    }

    setSelectedNodesList(changeSelect);
}

bool Client::isSixRound() {
    // todo: 1.发送请求需要先过滤（消息队列，共识就删除）
    //当前轮次
    int round = getRound();
    //轮次切换
    int changeRound = getChangeRound();

    if (round == 0) {
        //读取配置文件
        int changeRound;
        int selectSize;
        Config configSettings(ConfigFile);
        changeRound = configSettings.Read("changeRound", changeRound);
        selectSize = configSettings.Read("selectSize", selectSize);
        setChangeRound(changeRound);
        setSelectSize(selectSize);
        //增加视图
        setViewNum(getViewNum() + 1);
        setRound(getRound() + 1);
        return true;
    } else if (round == changeRound) {
        cout << "切换轮次" << endl;
        //读取配置文件
        int changeRound;
        int selectSize;
        Config configSettings(ConfigFile);
        changeRound = configSettings.Read("changeRound", changeRound);
        selectSize = configSettings.Read("selectSize", selectSize);
        setChangeRound(changeRound);
        setSelectSize(selectSize);
        //设置轮次为0
        setRound(1);
        return true;
    } else {
        setRound(getRound() + 1);
        return false;
    }
}

void Client::selectMasterNode() {
    int viewNum = getViewNum();
    int selectSize = getSelectSize();   //选择节点数
    int index = viewNum % selectSize;
    cout << "主节点index" << index << endl;
    setMasterId(getSelectedNodesList()[index]);
}


int Client::getViewNum() {
    return viewNum;
}

void Client::setViewNum(int viewNum) {
    Client::viewNum = viewNum;
}

int Client::getCandidaSize() {
    return candidaSize;
}

void Client::setCandidaSize(int candidaSize) {
    Client::candidaSize = candidaSize;
}

int Client::getSelectSize() {
    return selectSize;
}

void Client::setSelectSize(int selectSize) {
    Client::selectSize = selectSize;
}

int Client::getMasterId() {
    return masterId;
}

void Client::setMasterId(int masterId) {
    Client::masterId = masterId;
}

int Client::getChangeRound() {
    return changeRound;
}

void Client::setChangeRound(int changeRound) {
    Client::changeRound = changeRound;
}

int Client::getRound() {
    return round;
}

void Client::setRound(int round) {
    Client::round = round;
}

const vector<int> &Client::getSelectedNodesList() {
    return selectedNodesList;
}

void Client::setSelectedNodesList(const vector<int> &selectedNodesList) {
    Client::selectedNodesList = selectedNodesList;
}

int Client::getClientId() {
    return clientId;
}

void Client::setClientId(int clientId) {
    Client::clientId = clientId;
}

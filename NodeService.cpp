//
// Created by SDEE on 2019/8/1.
//

#include "NodeService.h"
#include <iostream>
#include <string>
#include "log/logger.h"
#include "config/Config.h"

#include <thread>

using namespace std;

//视图变更view= -1，
static const int REQ = 0; //获取数据
static const int PP = 1;    //预准备阶段
static const int PA = 2;    //准备阶段
static const int CM = 3;    //确认阶段
static const int REPLY = 4; //回复阶段


map<int, Request>::iterator iter;

vector<NodeService> NodeService::NodeServices;

hash<string> hashFromStr;

int checkPoint; //低水线
int Litre;      //水位

void NodeService::start(Request request) {

    bool flag = true;
    for (auto &NodeService : NodeServices) {
        if (NodeService.getVNum() == request.vnum) {
            // 2019年8月1日22点04分
            NodeService.action(request);
            flag = false;
            break;
        }
    }

    if (flag) {
        auto *nodeService = new NodeService(request.vnum, request.vnum);
        nodeService->action(request);
        NodeServices.push_back(*nodeService);
    }

    if (NodeServices.size() == 12) {
        int temp = NodeServices.size() / 2;
        NodeServices.erase(NodeServices.begin(), NodeServices.begin() + temp);
    }
}

void NodeService::action(Request request) {

    switch (request.type) {
        case REQ:
            onRequest(request);
            break;
        case PP:
            onPrePrepare(request);
            break;
        case PA:
            onPrepare(request);
            break;
        case CM:
            onCommit(request);
            break;
        default:
            break;
    }
}

void NodeService::onRequest(Request request) {

    //增加轮次，读取配置文件
    setRounds(getRounds() + 1);

    if (getRounds() == 0) {
        readParameterFromConfig();
    }

    int count = request.selectedNodesList.size();
    setState(false);

    //标识投票节点
    vector<NodeService> nodes;
    for (int i = 0; i < count; i++) {
        if (request.selectedNodesList[i] == getId()) {
            // 视图
            setVNum(request.vnum);

            //主节点id
            setMastNode(request.masterId);

            setState(true);

            //todo：节点ID又节点而定，暂时写死 2019年8月6日09点22分
            string temp = "标识阶段: 投票结点ID: " + to_string(1) + " , 主节点编号:" + to_string(getMastNode()) + " , 视图:" +
                          to_string(getVNum());

            if (PrePrepareCandidate.find(request.time) != PrePrepareCandidate.end()) {
                //写入日志
                writeLog(temp);
                //执行下一阶段
                action(PrePrepareCandidate[request.time]);
            }
        }
    }

    if (request.masterId == getId()) {
        //生成序列号
        // todo: getSeriesNumber()的值是未初始化的int值 -> 修改：只在主节点调用的方法
        int tempNo = getSeriesNumber();
        if (tempNo >= (checkPoint + Litre) || tempNo < checkPoint) {
            tempNo = checkPoint;
            setSeriesNumber(tempNo);
        } else {
            setSeriesNumber(getSeriesNumber() + 1);
        }

        PrePrepareList.push_back(seriesNumber);

        string temp = "pp阶段: 投票结点ID: " + to_string(getId()) + " , 主节点编号:" + to_string(getMastNode())
                      + " , 视图:" + to_string(getVNum()) + " , 序列号:" + to_string(getSeriesNumber())
                      + " , 摘要:" + to_string(request.digest) + " , 信息:" + request.message + " , 时间戳:"
                      + to_string(request.time);

        writeLog(temp);

        countPrepare[request.seriesNumber] = 0;
        countCommit[request.seriesNumber] = 0;

        Request ppRequest = Quest::toPrePrepare(request, getId(), getSeriesNumber());
        sendRequest(ppRequest);
    }
    informList.push_back(request.time);
}

//收到主节点发过来的pp消息
void NodeService::onPrePrepare(Request request) {

    //收到主节点发来的pp消息
    //先判断有没有收到通知消息，若未收到通知消息，则加入pp候选列表
    //若已经收到通知消息（消息转发），再检查p候选列表，若p消息存在，处理消息

    bool flag = true;
    for (int i = 0; i < informList.size(); ++i) {
        // 验证时间戳、摘要、视图编号
        if (informList[i] == request.time && hashFromStr(request.message) == request.digest &&
            request.vnum == getVNum()) {

            //从消息队列中删除
            informList.erase(informList.begin() + i);

            Request pRequest = Quest::toPrepare(request, getId());
            sendRequest(pRequest);
            flag = false;

            //遍历p消息候选列表，如果有则执行
            iter = PrepareCandidate.begin();
            if (iter != PrepareCandidate.end()) {
                action(PrepareCandidate[request.seriesNumber]);
            }
            break;
        }
    }

    if (flag) {
        PrePrepareCandidate[request.time] = request;
    }
}

void NodeService::onPrepare(Request request) {

    //收到p消息，先判断有没有收到pp消息，若未收到pp消息，则加入p候选列表
    //若已经收到pp消息（消息转发），再检查c候选列表，若c消息存在，处理消息

    bool flag = true;
    //判断pp消息列表是否存在对应消息
    for (int i = 0; i < PrePrepareList.size(); ++i) {
        if (PrePrepareList[i] == request.seriesNumber && getSeriesNumber() == request.seriesNumber
            && hashFromStr(request.message) == request.digest && request.vnum == getVNum()) {

            countPrepare[request.seriesNumber]++;
            PrepareList.push_back(request.seriesNumber);
            // 容忍数
            if (countPrepare[request.seriesNumber] == getSelectedNodesList().size() / 3 * 2) {
                //从消息队列中删除
                PrePrepareList.erase(PrePrepareList.begin() + i);

                string temp = "p阶段: 投票结点ID: " + to_string(getId()) + " , 主节点编号:" + to_string(getMastNode())
                              + " , 视图:" + to_string(getVNum()) + " , 序列号:" + to_string(getSeriesNumber())
                              + " , 摘要:" + to_string(request.digest) + " , 信息:" + request.message + " , 时间戳:" +
                              to_string(request.time);
                writeLog(temp);

                Request cRequest = Quest::toCommit(request, getId());
//                Request ppRequest = Quest::toPrePrepare(request, getId(), getSeriesNumber());
                sendRequest(cRequest);

            }
            flag = false;

            //遍历c消息候选列表，如果有则执行
            iter = CommitCandidate.begin();
            if (iter != CommitCandidate.end()) {
                action(CommitCandidate[request.seriesNumber]);
            }
            break;
        }
    }
    if (flag) {
        PrepareCandidate[request.seriesNumber] = request;
    }
}
void NodeService::onCommit(Request request) {
    bool flag = true;

    for (int i = 0; i < PrepareList.size(); ++i) {
        if (PrepareList[i] == request.seriesNumber
            && getSeriesNumber() == request.seriesNumber && hashFromStr(request.message) == request.digest &&
            request.vnum == getVNum()) {

            countCommit[request.seriesNumber]++;

            //容忍数
            if (countCommit[request.seriesNumber] == getSelectedNodesList().size() / 3 * 2) {

                //从消息队列中删除
                PrepareList.erase(PrepareList.begin() + i);

                string temp = "c阶段: 投票结点ID: " + to_string(getId()) + " , 主节点编号:" + to_string(getMastNode())
                              + " , 视图:" + to_string(getVNum()) + " , 序列号:" + to_string(getSeriesNumber())
                              + " , 摘要:" + to_string(request.digest) + " , 信息:" + request.message + " , 时间戳: " +
                              to_string(request.time);
                writeLog(temp);
                // todo:调用验证接口，得到验证结果
                Request rRequest = Quest::toReply(request, getId(), "验证结果");
                sendRequestToClient(rRequest);
            }
            flag = false;
            break;
        }
    }
    if (flag) {
        CommitCandidate[request.seriesNumber] = request;
    }
}
//
void NodeService::onReply(Request request) {

}


void NodeService::writeLog(string message) {
    initLogger("../log/info.txt", "../log/warn.txt", "../log/error.txt");
    LOG(INFO) << message;
}

void NodeService::readParameterFromConfig() {
    const char ConfigFile[] = "../config/config.txt";
    Config configSettings(ConfigFile);
    checkPoint = configSettings.Read("checkPoint", checkPoint);
    Litre = configSettings.Read("litre", Litre);
}

void NodeService::sendRequest(Request request) {
}

void NodeService::sendRequestToClient(Request request) {
}

bool NodeService::onCountinformation(Request request) {
    return false;
}


int NodeService::getOutage() const {
    return outage;
}

void NodeService::setOutage(int outage) {
    NodeService::outage = outage;
}

bool NodeService::isState() const {
    return isVote;
}

void NodeService::setState(bool state) {
    NodeService::isVote = state;
}

int NodeService::getId() const {
    return id;
}

void NodeService::setId(int id) {
    NodeService::id = id;
}

int NodeService::getMastNode() const {
    return mastNode;
}

void NodeService::setMastNode(int mastNode) {
    NodeService::mastNode = mastNode;
}

int NodeService::getVNum() const {
    return vNum;
}

void NodeService::setVNum(int vNum) {
    NodeService::vNum = vNum;
}

int NodeService::getSeriesNumber() const {
    return seriesNumber;
}

void NodeService::setSeriesNumber(int seriesNumber) {
    NodeService::seriesNumber = seriesNumber;
}

int NodeService::getRounds() const {
    return rounds;
}

void NodeService::setRounds(int rounds) {
    NodeService::rounds = rounds;
}

const vector<int> &NodeService::getSelectedNodesList() const {
    return selectedNodesList;
}

void NodeService::setSelectedNodesList(const vector<int> &selectedNodesList) {
    NodeService::selectedNodesList = selectedNodesList;
}

const map<int, int> &NodeService::getCountPrepare() const {
    return countPrepare;
}

void NodeService::setCountPrepare(const map<int, int> &countPrepare) {
    NodeService::countPrepare = countPrepare;
}

const map<int, int> &NodeService::getCountCommit() const {
    return countCommit;
}

void NodeService::setCountCommit(const map<int, int> &countCommit) {
    NodeService::countCommit = countCommit;
}

const vector<int> &NodeService::getPrePrepareList() const {
    return PrePrepareList;
}

void NodeService::setPrePrepareList(const vector<int> &prePrepareList) {
    PrePrepareList = prePrepareList;
}

const map<int, Request> &NodeService::getPrepareCandidate() const {
    return PrepareCandidate;
}

void NodeService::setPrepareCandidate(const map<int, Request> &prepareCandidate) {
    PrepareCandidate = prepareCandidate;
}

const vector<int> &NodeService::getPrepareList() const {
    return PrepareList;
}

void NodeService::setPrepareList(const vector<int> &prepareList) {
    PrepareList = prepareList;
}

const map<int, Request> &NodeService::getCommitCandidate() const {
    return CommitCandidate;
}

void NodeService::setCommitCandidate(const map<int, Request> &commitCandidate) {
    CommitCandidate = commitCandidate;
}

NodeService::NodeService(int id, int vNum) : id(id), vNum(vNum) {
    rounds = -1;
}








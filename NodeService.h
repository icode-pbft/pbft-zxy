//
// Created by SDEE on 2019/8/1.
//

#ifndef DPBFT_NODESERVICE_H
#define DPBFT_NODESERVICE_H

#include <iostream>
#include <vector>
#include <map>
#include "Request.h"

using namespace std;

class NodeService {
private:
    int outage; //标志是否宕机0、作恶-1、正常1
    bool isVote;  //节点状态,
    int id;  //节点Id
    // todo: mastNode无用
    int mastNode;  //主节点Id
    int vNum;  //视图编号
    int seriesNumber;  //序列号
    int rounds; // 轮次
    // todo: 记录其他节点的nodeId（无用），还要用到其size来算(3f+1)中的f，待优化
    vector<int> selectedNodesList;  //已选列表

    //map : <序列号，总数>
    map<int, int> countPrepare; // 收到准备阶段(P)消息计数
    map<int, int> countCommit;  // 收到确认阶段(C)消息计数

    // 通知消息，request消息
    vector<int> informList;
    // pp消息候选列表
    map<int, Request> PrePrepareCandidate;
    // pp消息队列
    vector<int> PrePrepareList;
    //p消息候选列表
    map<int, Request> PrepareCandidate;
    //p消息队列
    vector<int> PrepareList;
    //c消息候选列表
    map<int, Request> CommitCandidate;


    static vector<NodeService> NodeServices;


public:

    static void start(Request request);

    NodeService(int id, int vNum);

    // 根据消息类型执行相关处理
    void action(Request request);

    //得到通知消息
    void onRequest(Request request);

    //预准备
    void onPrePrepare(Request request);

    //准备
    void onPrepare(Request request);

    //确认
    void onCommit(Request request);

    //回复
    void onReply(Request request);

    //广播;
    void sendRequest(Request request);

    //给客户端发送消息
    void sendRequestToClient(Request request);

    // 验证信息
    // todo: 这里面调用客户给的验证结果接口
    bool onCountinformation(Request request);

    // 写日志
    void writeLog(string message);

    // 读取配置文件
    void readParameterFromConfig();

public:
    int getOutage() const;

    void setOutage(int outage);

    bool isState() const;

    void setState(bool state);

    int getId() const;

    void setId(int id);

    int getMastNode() const;

    void setMastNode(int mastNode);

    int getVNum() const;

    void setVNum(int vNum);

    int getSeriesNumber() const;

    void setSeriesNumber(int seriesNumber);

    int getRounds() const;

    void setRounds(int rounds);

    const vector<int> &getSelectedNodesList() const;

    void setSelectedNodesList(const vector<int> &selectedNodesList);

    const map<int, int> &getCountPrepare() const;

    void setCountPrepare(const map<int, int> &countPrepare);

    const map<int, int> &getCountCommit() const;

    void setCountCommit(const map<int, int> &countCommit);

    const vector<int> &getPrePrepareList() const;

    void setPrePrepareList(const vector<int> &prePrepareList);

    const map<int, Request> &getPrepareCandidate() const;

    void setPrepareCandidate(const map<int, Request> &prepareCandidate);

    const vector<int> &getPrepareList() const;

    void setPrepareList(const vector<int> &prepareList);

    const map<int, Request> &getCommitCandidate() const;

    void setCommitCandidate(const map<int, Request> &commitCandidate);
};


#endif //DPBFT_NODESERVICE_H

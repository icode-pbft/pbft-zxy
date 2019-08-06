#include <iostream>
#include <thread>
#include "log/logger.h"
#include <Windows.h>

using namespace std;

class student {
public:
    int id;
    string name;
};

//int main() {
//
//    int count = 0;
//    char ByteArray[20];
//    student s;
//    s.id = 12;
//    s.name = "specialist";
//    memcpy(ByteArray,&s.id,sizeof(s.id));
//
//    for (int i = 0; i <20 ; ++i) {
//        count<<ByteArray[i];
//    }
//    count += sizeof(s.id);
//    memcpy(ByteArray+count,s.name.c_str(),s.name.length());
//    count += s.name.length();
//
//    return 0;
//}
//



void test1() {
    LOG(WARNING) << "test1\n";
}

void test2() {
    LOG(INFO) << "test2\n";
}

int main() {
    thread th1(test1);
    thread th2(test2);
    thread th3(test2);
    thread th4(test2);
    th1.join();
    th2.join();
    th3.join();
    th4.join();
    return 0;
}

//
// Created by SDEE on 2019/7/31.
//
#include "logger.h"
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>

using namespace std;

std::ofstream Logger::m_error_log_file;
std::ofstream Logger::m_info_log_file;
std::ofstream Logger::m_warn_log_file;
std::mutex Logger::mtx;


void initLogger(const std::string &info_log_filename,
                const std::string &warn_log_filename,
                const std::string &error_log_filename) {

    Logger::m_info_log_file.open(info_log_filename.c_str(), ios::app);
    Logger::m_warn_log_file.open(warn_log_filename.c_str(), ios::app);
    Logger::m_error_log_file.open(error_log_filename.c_str(), ios::app);
}

std::ostream &Logger::getStream(log_rank_t log_rank) {
//    mtx.lock();
//    if(INFO==log_rank){
//        m_info_log_file:std::cout;
//    }else if(WARNING==log_rank){
//        m_warn_log_file:std::cerr;
//    } else{
//        m_error_log_file:std::cerr;
//    }
//


    return (INFO == log_rank) ?
           (m_info_log_file.is_open() ? m_info_log_file : std::cout ) :
           (WARNING == log_rank ?
            (m_warn_log_file.is_open() ? m_warn_log_file : std::cerr) :
            (m_error_log_file.is_open() ? m_error_log_file : std::cerr));
}



std::ostream &Logger::start(log_rank_t log_rank,
                            const int line,
                            const std::string &function) {

    time_t tm;
    time(&tm);
    char *t_string = NULL;
//    char time_string[128];
//    struct tmStruct *p;

    t_string = ctime(&tm);
    mtx.lock();
    return getStream(log_rank) << t_string
                               << "function (" << function << ")"
                               << "line " << line
                               << std::flush;

}

Logger::~Logger() {
    getStream(m_log_rank) << std::endl << std::flush;

    if (FATAL == m_log_rank) {
        m_info_log_file.close();
        m_info_log_file.close();
        m_info_log_file.close();
        abort();
    }
    mtx.unlock();
}
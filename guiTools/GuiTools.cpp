#include "Tools.h"
#include <iostream>
#include <QMutex>
#include <QMutexLocker>
#include <iostream>
#include <string>
#include <QString>
#include <QThreadPool>



void nullPointerHandling(const char *pFile, int line, const QString &m)
{
//    gen::loggerQuit();

    std::cout << "nullptr:" << m.toLocal8Bit().data() << "\n" << pFile << "\n" << "LINE:" << line << "\n";
    std::cout << "program will exit in 5 sec\n";

  //  gen::xSleep(5000);

    exit(1);

}


using namespace std;

namespace guiTools
{
    void exec_thread(std::function<void()> f)
    {
        QThreadPool * daPool = QThreadPool::globalInstance();
        NCHECK(daPool);
        daPool->start([f = std::move(f)]()
                      {
                          f();
                      });
    }


//    static QMutex mutexLog;
//
//    void log(const QString & s)
//    {
//        QMutexLocker locker(&mutexLog);
//
////        cout << s.toStdString();
//        cout << "\n";
//    }

}
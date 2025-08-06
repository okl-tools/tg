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

    enum class SESS_GUI
    {
        NOT_INITIALIZED,
        X11,
        WAYLAND
    };


    static SESS_GUI sessGui = SESS_GUI::NOT_INITIALIZED;

    bool detect_wayland()
    {
        if (sessGui != SESS_GUI::NOT_INITIALIZED)
        {
            return sessGui == SESS_GUI::WAYLAND;
        }

        const char* session = getenv("XDG_SESSION_TYPE");

        sessGui = session && QString(session).compare("wayland", Qt::CaseInsensitive) == 0 ? SESS_GUI::WAYLAND : SESS_GUI::X11;

        return sessGui == SESS_GUI::WAYLAND;

    }

}
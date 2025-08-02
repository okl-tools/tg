#include "Tools.h"
#include <QApplication>
#include <QThread>
#include <QTimer>
#include <iostream>


using namespace okl;

namespace guiTools
{

    XMainWindow::XMainWindow (QWidget * parent, Qt::WindowFlags flags) : QMainWindow(parent, flags)
    {
        QShortcut * shortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q), this);
        QObject::connect(shortcut, &QShortcut::activated, [] ()
        {
            pp("CTRL+Q pressed!");
            QCoreApplication::quit();
        });


    }



    void XMainWindow::exec_gui_method(std::function<void()> f, uint64_t msDelay )
    {
        QMetaObject::invokeMethod(this, [f = std::move(f), msDelay]() mutable
        {
            if (msDelay == 0)
            {
                f();
            }
            else
            {
                QTimer::singleShot(msDelay, [f = std::move(f)]
                {
                    f();
                });
            }
        }, Qt::QueuedConnection);
    }

}
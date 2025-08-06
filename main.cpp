#include <QApplication>
#include <QPushButton>
#include <QMainWindow>
#include <QThreadPool>
#include <QImage>
#include <QSplitter>
#include <QPlainTextEdit>
#include "guiTools/Tools.h"

#include <QClipboard>
#include <QImage>

#include <QObject>
#include <QLocalServer>
#include <QLocalSocket>
#include <QString>
#include <QMessageBox>

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include "WindowOCR.h"


using namespace guiTools;


struct SingleInstanceHelper : QObject
{
//Q_OBJECT

    QLocalServer * server = nullptr;
    QString serverName;

    SingleInstanceHelper (const QString & name, QObject * parent = nullptr) : QObject(parent), serverName(name)
    {
    }

    bool is_running ()
    {
        QLocalSocket socket;
        socket.connectToServer(serverName);

        if (socket.waitForConnected(100))
        {
            // already running
            socket.disconnectFromServer();
            return true;
        }

        QLocalServer::removeServer(serverName);

        server = new QLocalServer(this);
        if (!server->listen(serverName))
        {
            pp("main::is_running, Failed to listen on socket:$", server->errorString());
            return true; // sicherheitshalber "running" zurückgeben
        }

        return false;
    }


};


//void set_top_left()

int main (int argc, char * argv[])
{
//    if (api.Init(NULL, "eng") != 0)


    QApplication app(argc, argv);


    SingleInstanceHelper singleInstance("tgApp");

    if (singleInstance.is_running())
    {
        QMessageBox::warning(nullptr, "tg already running", "This application tg - tesseract gui\nis already running !");
        return 0;
    }


    WindowOCR winOCR;

    winOCR.ini();

    QString sFile;
    QImage img;
    if (argc == 2)
    {
        sFile = argv[1];

        QImage img;
        bool ok = img.load(sFile);

        if (!ok || img.isNull())
        {
            pp("XImageViewer::load_image, img not valid");
        }
        else
        {
            pp("XImageViewer::load_image, image '$' load, $x$", sFile, img.width(), img.height());
            winOCR.compoImage->set_image(img);
        }
    }

    if (!sFile.isEmpty())
    {
        pp("tg, sFile argument '$'", sFile);
    }

    QObject::connect(&app, &QCoreApplication::aboutToQuit, [&winOCR] ()
    {
        pp("main::main - quit");
        winOCR.api.End();
    });


    winOCR.resize(1200, 800);
    winOCR.restore_window_state();
    winOCR.show();

    return QApplication::exec();
}

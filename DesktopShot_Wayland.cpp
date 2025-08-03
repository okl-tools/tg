// DesktopShot.cpp

#include "DesktopShot.h"

#include <QTemporaryFile>
#include <QUrl>
#include <QFile>
#include <QProcess>
#include <QDateTime>
#include <QMessageBox>
#include <QTimer>


QImage DesktopShot::shotDesktopRectangle_Wayland ()
{
    pp("DesktopShot_Wayland::shotDesktopRectangle_Wayland");
    QString path = "/tmp/snap.png";
    QFile::remove(path);  // cleanup before

    int result = QProcess::execute("gnome-screenshot", {"-a", "-f", path});
    if (result != 0 || !QFile::exists(path))
    {
        QMessageBox::warning(nullptr, "Wayland", "Screenshot cancelled\n"
                                                 "or\n"
                                                 "'gnome-screenshot' is not installed\n"
                                                 "\n"
                                                 "install like this\n"
                                                 "\n"
                                                 "sudo apt-get install gnome-screenshot"
                                                 ""
                                                 );
        return QImage();
    }

    QImage img (path);

    QFile::remove(path);

    return img;
}
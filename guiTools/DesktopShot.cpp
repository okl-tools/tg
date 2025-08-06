// DesktopShot.cpp

#include "Tools.h"

#include <QImage>
#include <QMessageBox>
#include <QTimer>
#include <QScreen>
#include <QMouseEvent>

#include <QUrl>
#include <QEventLoop>
#include <QApplication>
#include <QGuiApplication>

#include <QTemporaryFile>
#include <QFile>
#include <QProcess>
#include <QDateTime>

namespace guiTools
{
    namespace internal
    {
        struct DesktopShot : public QWidget
        {

            DesktopShot (QWidget * parent = nullptr);

            QPoint startPoint;
            QRect selectedRect;
            bool selecting = false;
            QImage result;
            QEventLoop loop;

            void mousePressEvent (QMouseEvent * e) override;
            void mouseMoveEvent (QMouseEvent * e) override;
            void mouseReleaseEvent (QMouseEvent * e) override;
            void paintEvent (QPaintEvent *) override;
            void keyPressEvent (QKeyEvent * event);
        };


        DesktopShot::DesktopShot (QWidget * parent) : QWidget(parent)
        {
            // Virtual geometrie crossing all screens
            QRect virtualGeometry;

            const QList<QScreen *> screens = QGuiApplication::screens();
            for (QScreen * screen: screens)
            {
                virtualGeometry = virtualGeometry.united(screen->geometry());
            }

            setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint | Qt::Tool);

            setAttribute(Qt::WA_NoSystemBackground);
            setAttribute(Qt::WA_TranslucentBackground);
            setCursor(Qt::CrossCursor);
            setMouseTracking(true);

            setGeometry(virtualGeometry);

            show();

            // force to be in foreground
            raise();
            activateWindow();

            grabMouse();
        }


        void DesktopShot::mousePressEvent (QMouseEvent * e)
        {
            startPoint = e->pos();
            selecting = true;
            selectedRect = QRect(startPoint, startPoint);
        }

        void DesktopShot::mouseMoveEvent (QMouseEvent * e)
        {
            if (selecting)
            {
                selectedRect = QRect(startPoint, e->pos());
                update();
            }
        }


        void DesktopShot::mouseReleaseEvent (QMouseEvent * e)
        {
            selecting = false;
            releaseMouse();

            QRect rect = selectedRect.normalized();

            hide();  // hide overlay
            QApplication::processEvents();


            QTimer::singleShot(150, [this, rect]
            {
                // find correct QScreenwhich contains selectedRect
                QScreen * selectedScreen = nullptr;
                for (QScreen * screen: QGuiApplication::screens())
                {
                    if (screen->geometry().intersects(rect))
                    {
                        selectedScreen = screen;
                        break;
                    }
                }

                if (selectedScreen)
                {
                    // shot of screen
                    QPixmap fullScreen = selectedScreen->grabWindow(0);

                    // be attentive for correct offset - rect is global
                    QRect screenGeom = selectedScreen->geometry();
                    QRect localRect = rect.translated(-screenGeom.topLeft());

                    result = fullScreen.copy(localRect).toImage();
                }
                else
                {
                    pp("ERROR: no screen for rect $", rect);
                }

                loop.quit();
                close();
            });


        }

        void DesktopShot::keyPressEvent (QKeyEvent * event)
        {
            if (event->key() == Qt::Key_Escape)
            {
                selecting = false;
                releaseMouse();
                result = QImage();  // leeres Ergebnis bei Abbruch

                loop.quit();
                close();
            }
            else
            {
                QWidget::keyPressEvent(event);
            }
        }


        void DesktopShot::paintEvent (QPaintEvent *)
        {
            QPainter p (this);
            p.fillRect(rect(), QColor(0, 0, 0, 100)); // halbtransparent

            if (selecting)
            {
                p.setPen(QPen(Qt::red, 2));
                p.drawRect(selectedRect.normalized());
            }
        }
    }



    static QImage shotDesktopRectangle_X11 ()
    {
        pp("guiTools::shotDesktopRectangle_X11");

        internal::DesktopShot diaShot;
        diaShot.loop.exec();
        return diaShot.result;
    }

    static QImage shotDesktopRectangle_Wayland ()
    {
        pp("guiTools::shotDesktopRectangle_Wayland");

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
                                                     "");
            return QImage();
        }

        QImage img(path);

        QFile::remove(path);

        return img;
    }


    QImage shotDesktopRectangle ()
    {
        return detect_wayland() ?
            shotDesktopRectangle_Wayland():
            shotDesktopRectangle_X11();
    }

}
#include "WindowOCR.h"

#include <QClipboard>
#include <QImage>
#include <QMimeData>
#include <QSettings>
#include <QMessageBox>
#include <QTimer>


DesktopSelection::DesktopSelection (QWidget * parent) : QWidget(parent)
{
    // Virtual geometrie crossing all screens
    QRect virtualGeometry;

    const QList<QScreen*> screens = QGuiApplication::screens();
    for (QScreen* screen : screens)
    {
        virtualGeometry = virtualGeometry.united(screen->geometry());
    }

    setWindowFlags(Qt::FramelessWindowHint |
                   Qt::WindowStaysOnTopHint |
                   Qt::X11BypassWindowManagerHint |
                   Qt::Tool);

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

QImage DesktopSelection::run ()
{
    DesktopSelection* selector = new DesktopSelection();
    selector->loop.exec();
    return selector->result;
}

void DesktopSelection::mousePressEvent (QMouseEvent * e)
{
    startPoint = e->pos();
    selecting = true;
    selectedRect = QRect(startPoint, startPoint);
}

void DesktopSelection::mouseMoveEvent (QMouseEvent * e)
{
    if (selecting)
    {
        selectedRect = QRect(startPoint, e->pos());
        update();
    }
}


void DesktopSelection::mouseReleaseEvent(QMouseEvent* e)
{
    selecting = false;
    releaseMouse();

    QRect rect = selectedRect.normalized();

    hide();  // hide overlay
    QApplication::processEvents();


    QTimer::singleShot(150, [this, rect]
    {
        // find correct QScreenwhich contains selectedRect
        QScreen* selectedScreen = nullptr;
        for (QScreen* screen : QGuiApplication::screens())
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

void DesktopSelection::keyPressEvent(QKeyEvent* event)
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


void DesktopSelection::paintEvent (QPaintEvent *)
{
    QPainter p(this);
    p.fillRect(rect(), QColor(0, 0, 0, 100)); // halbtransparent

    if (selecting)
    {
        p.setPen(QPen(Qt::red, 2));
        p.drawRect(selectedRect.normalized());
    }
}



// DesktopShot.h

#pragma once




#include <QApplication>
#include <QPushButton>
#include <QMainWindow>
#include <QImage>

#include "guiTools/Tools.h"


using namespace guiTools;

struct DesktopShot : public QWidget
{
    DesktopShot(QWidget* parent = nullptr);

    static QImage shotDesktopRectangle();
    static QImage shotDesktopRectangle_Wayland_old();
    static QImage shotDesktopRectangle_Wayland();

protected:

    QPoint startPoint;
    QRect  selectedRect;
    bool   selecting = false;
    QImage result;
    QEventLoop loop;

    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void paintEvent(QPaintEvent*) override;
    void keyPressEvent(QKeyEvent* event);
};

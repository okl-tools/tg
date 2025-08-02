#include "Tools.h"
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QColor>

using namespace okl;

namespace guiTools
{
    void XGrid::draw(XPainter & painter)
    {
        if (!is_enabled())
            return;

        QColor col = QColor(222,222,222);
        qreal w = 1.0;

        for (qreal x=0; x < totalGridSize; x+= gridSize)
        {
            painter.drawXLine(x, -totalGridSize, x, totalGridSize, w, col);
            painter.drawXLine(-x, -totalGridSize, -x, totalGridSize, w, col);
        }

        for (qreal y=0; y < totalGridSize; y+= gridSize)
        {
            painter.drawXLine(-totalGridSize, y, totalGridSize, y, w, col);
            painter.drawXLine(-totalGridSize, -y, totalGridSize, -y, w, col);
        }

        painter.drawXLine(-totalGridSize, 0, totalGridSize, 0, 3, col);
        painter.drawXLine(0, -totalGridSize, 0, totalGridSize, 3,  col);

    }

}
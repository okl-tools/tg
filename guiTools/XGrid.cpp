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
    void XGrid::draw(XPainter & painter, qreal zoomFactor)
    {
        if (!is_enabled())
            return;

        QColor col = QColor(222,222,222);
        qreal w = 1.0 / zoomFactor;

        pp("XGrid::draw, zoomFactor:$, w:$", zoomFactor, w);

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

        painter.drawXLine(-totalGridSize, 0, totalGridSize, 0, w*2, col);
        painter.drawXLine(0, -totalGridSize, 0, totalGridSize, w*2,  col);

    }

}
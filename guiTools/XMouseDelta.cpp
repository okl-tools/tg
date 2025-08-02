#include "Tools.h"
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QFileInfo>

using namespace okl;

namespace guiTools
{
    void XMouseDelta::fromPoint(const QPoint & ptSource, DELTAS deltas)
    {
        if (!flagValid)
        {
            flagValid = true;
            ptStart = ptSource;
            return;
        }

        pt = ptSource;

        if (deltas == DELTA_ALL || deltas == DELTA_HORIZONTAL)
        {
            int tmp = dx;
            dx = (pt.x() - ptStart.x());
            dxLast = dx - tmp;
        }

        if (deltas == DELTA_ALL || deltas == DELTA_VERTICAL)
        {
            int tmp = dy;
            dy = (pt.y() - ptStart.y());
            dyLast = dy - tmp;
        }
    }

    bool XMouseDelta::hasData() const
    {
        return flagValid && (dx != 0 || dy != 0);
    }

    bool XMouseDelta::hasDragData() const
    {
        return flagValid && (dxLast != 0 || dyLast != 0);
    }

    bool XMouseDelta::hasDataX() const
    {
        return flagValid && (dx != 0);
    }

    bool XMouseDelta::hasDataY() const
    {
        return flagValid && (dy != 0);
    }

    void XMouseDelta::reset()
    {
        dx = 0;
        dy = 0;
        dxLast = 0;
        dyLast = 0;
        flagValid = false;
    }

}
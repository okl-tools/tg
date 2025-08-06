//#include "Tools.h"
#include "XImageViewer.h"


#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QWheelEvent>

using namespace okl;

namespace guiTools
{
    template<typename T>
    using XArray = std::vector<T>;     // template alias "XArray" because I think std::vector can be often mixed up with geometrical vectors

    XImageViewer::XImageViewer ()
    {
        setMouseTracking(true);
        setAcceptDrops(true);
    }


    void XImageViewer::paintEvent (QPaintEvent * event)
    {
        const int w = width();
        const int h = height();

        pp("XImageViewer::paintEvent, w:$, h:$", w, h);

        XPainter painter (this);
        painter.setRenderHint(QPainter::Antialiasing);

        QRect r(0, 0, w - 1, h - 1);
        painter.fillRect(r, Qt::white);

        painter.save(); // for LW * overlay

        painter.setTransform(matrixCartesian);


        QMutexLocker locker(&mutexImage);

        QImage img = get_image();

        if (!img.isNull())
        {
            QImage imgTransformed = img.mirrored(false, true);
            painter.drawImage(-imgTransformed.width() / 2, -imgTransformed.height() / 2, imgTransformed);
        }

        if (flagControlRect)
        {
            painter.drawXRect(rMark, 1.0, Qt::black);
            painter.fillXRect(rMark, QColor(255,0,0, 70));
        }

        painter.save();
        painter.restore();

        painter.save();
        draw_grid(painter, get_zoom());
        painter.restore();

        painter.restore();

    }

    void XImageViewer::draw_grid(XPainter & painter, qreal zoomFactor)
    {
        if (!is_grid_enabled())
            return;

        QColor col = QColor(222,222,222);
        qreal w = 1.0 / zoomFactor;

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

    void XImageViewer::dragEnterEvent (QDragEnterEvent * event)
    {
        pp("XImageViewer::dragEnterEvent");

        const QMimeData * md = event->mimeData();
        if (md == nullptr)
        {
            pp("XImageViewer::dragEnterEvent, no mime data");
            return;
        }

        auto listFormats = md->formats();
        pp(4, "Formats:$", listFormats.size());
        for (auto format: listFormats)
        {
            pp(8, "format:'$'", format);
        }

        event->acceptProposedAction();

        //event->
    }


    void XImageViewer::dropEvent (QDropEvent * event)
    {
        pp("XImageViewer::dropEvent");

        if (!event)
        {
            pp("XImageViewer::dropEvent, event == nullptr");
            return;
        }

        const QMimeData * md = event->mimeData();
        if (md == nullptr)
        {
            pp("XImageViewer::dropEvent, no mime data");
            return;
        }

        QObject * o = event->source();
        if (o)
        {
            pp("AbstractDropper::x_dropEvent, drag source:$", o);
            o->dumpObjectTree();
            pp("-");
        }


        QFileInfo fi;


        if (md->hasUrls())
        {
            pp("XImageViewer::dropEvent, has URL's");
            XArray<QString> ar_sFiles;
            XArray<QFileInfo> ar_QFiles;
            const QList<QUrl> urls = event->mimeData()->urls();

            if (!urls.isEmpty())
            {
                for (QUrl url: urls)
                {
                    QString s = url.toLocalFile().trimmed();
                    if (s.length() > 0)
                    {
                        fi = QFileInfo(s);
                        break;
                    }
                }
            }

            event->acceptProposedAction();

        }
        else if (md->hasText())
        {
            pp(4, "AbstractDropper::x_dropEvent, has Text");
            pp(4, "<<<$>>>", md->text());
        }

        if (fi.isFile())
        {
            pp("XImageViewer::dropEvent, fi: '$'", fi.path());
//            load_image(fi.path());
            load_image(fi.absoluteFilePath());
        }
    }

    bool XImageViewer::load_image (const QString & sPath)
    {
        QImage img;
        bool ok = img.load(sPath);

        if (!ok || img.isNull())
        {
            pp("XImageViewer::load_image, img not valid");
        }
        else
        {
            pp("XImageViewer::load_image, image '$' load, $x$", sPath, img.width(), img.height());
            set_image(img);
            delta.reset();
            update();

        }
        return false;
    }

    void XImageViewer::wheelEvent (QWheelEvent * event)
    {
        int delta = 0;

        if (!event->pixelDelta().isNull())
        {
            delta = event->pixelDelta().ry();
        }
        else
        {
            delta = event->angleDelta().ry();
        }

        if (delta != 0)
        {
            on_zoom(delta > 0);
        }
    }

    void XImageViewer::mousePressEvent (QMouseEvent * event)
    {
        pp("XImageViewer::mousePressEvent, at:$", event->pos());
        delta.reset();

        if (event->modifiers() & Qt::ControlModifier)
        {
            pp("XImageViewer::mousePressEvent - ctrl");
            flagControlRect = true;

            QPointF pt = current_point_from_cartesian();
            rMark.setTopLeft(pt);


            rCut.setTopLeft(point_from_image(event->position().toPoint()));
            rCut.setBottomRight(point_from_image(event->position().toPoint()));


            pp("rCut:$", rCut);
        }
        else
        {
            flagControlRect = false;
        }


    }

    void XImageViewer::mouseReleaseEvent (QMouseEvent * event)
    {
        rCut.setBottomRight(point_from_image(event->position().toPoint()));

        pp("rCut:$", rCut);

        delta.fromPoint(event->pos());

        vTranslate = vTranslate + QVector2D(delta.dx, delta.dy);

        delta.reset();

        update();
    }

    void XImageViewer::mouseMoveEvent (QMouseEvent * event)
    {
        if (event->buttons() == Qt::LeftButton)
        {
            delta.fromPoint(event->pos());

            QVector2D vTrans(delta.dxLast, -delta.dyLast);

            vTrans /= get_zoom();

            if (flagControlRect)
            {
                if (event->modifiers() & Qt::ControlModifier)
                {
                    QPointF pt = current_point_from_cartesian();
                    rMark.setBottomRight(pt);

                    rCut.setBottomRight(event->position().toPoint());
                    pp("XImageViewer::mouseMoveEvent, rCut:$", rCut);
                }
            }
            else
            {
                matrixCartesian.translate(vTrans);
            }
            update();
        }
    }

    double XImageViewer::get_zoom () const
    {
        return matrixCartesian.get_scale_x();
    }

    QVector2D XImageViewer::get_translation () const
    {
        return matrixCartesian.get_translation();
    }

    void XImageViewer::on_resize (QSize sz)
    {
        matrixCartesian.reset();

        double x = sz.width() / 2;
        double y = sz.height() / 2;
        matrixCartesian.translate(x, y);
        matrixCartesian.scale(1, -1);

    }

    void XImageViewer::resizeEvent (QResizeEvent * event)
    {
        QSize sz = event->size();
        on_resize(sz);
    }

    QPoint XImageViewer::getCurrentMousePos () const
    {
        return mapFromGlobal(QCursor::pos());
    }


    QPoint XImageViewer::mid_point_of_view () const
    {
        return QPoint(width() / 2, height() / 2);
    }

    QPointF XImageViewer::point_from_cartesian (QPointF pt) const
    {
        QTransform m = matrixCartesian.inverted();
        return m.map(pt);
    }

    QPointF XImageViewer::current_point_from_cartesian () const
    {
        return point_from_cartesian(getCurrentMousePos());
    }

    QSize XImageViewer::get_image_size () const
    {
        QImage img = get_image();
        if (img.isNull())
        {
            return QSize();
        }

        return img.size();
    }

    QPoint XImageViewer::point_from_image(QPoint pt) const
    {
        QImage img = get_image();

        if (img.isNull())
        {
            return { 0, 0 };
        }

        QTransform inv = matrixCartesian.inverted();
        QPointF ptScene = inv.map(QPointF(pt));
        QPointF ptImage = ptScene + QPointF(img.width() / 2.0, img.height() / 2.0);
        ptImage.setY(img.height() - ptImage.y());

        return ptImage.toPoint();
    }

    void XImageViewer::on_fit ()
    {
        pp("XImageViewer::on_fit, visible:$", isVisible());

        on_resize(size());

        const QSizeF szImg = get_image_size();
        const QSizeF sz = size();

        double zoom = sz.width() / szImg.width();
        if (zoom * szImg.height() > sz.height())
        {
            zoom = sz.height() / szImg.height();
        }

        matrixCartesian.scale(zoom);

        update();

    }

    QVector2D XImageViewer::vector_from_cartesian (QPointF pt) const
    {
        return QVector2D(point_from_cartesian(pt));
    }


    void XImageViewer::on_zoom (bool flagUp, bool flagCurrentMousePos)
    {
        pp("XImageViewer::on_zoom");

        const QPoint pt = flagCurrentMousePos ?
                          getCurrentMousePos() :
                          mid_point_of_view();

        const double zoom = flagUp ?
                            1.1 :
                            1.0 / 1.1;

        QVector2D vPos = vector_from_cartesian(pt);
        QVector2D vPos2 = vPos * zoom;
        QVector2D vOffset = vPos - vPos2;

        matrixCartesian.translate(vOffset);
        matrixCartesian.scale(zoom);


        update();

    }

    void XImageViewer::set_image (QImage img)
    {
        QMutexLocker locker(&mutexImage);
        image = img;
        delta.reset();
        flagControlRect = false;

        update();
    }

    QImage XImageViewer::get_image () const
    {
        //QMutexLocker locker(&mutexImage);
        return image;
    }


    QImage XImageViewer::get_selection ()
    {
        QMutexLocker locker(&mutexImage);

        if (flagControlRect && !image.isNull())
        {
            rCut = rCut.normalized();
            return image.copy(rCut);
        }

        return image.copy();  // fallback: ganzes Bild
    }

    void XImageViewer::set_grid_enabled (bool flagEnable)
    {
        flagEnabledGrid = flagEnable;
    }

    bool XImageViewer::is_grid_enabled () const
    {
        return flagEnabledGrid;
    }


}
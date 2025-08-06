#pragma once

#include <QMainWindow>
#include <QLayout>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QPainter>
#include <QString>
#include <QTextStream>
#include <QMutex>
#include <QSize>

#include <functional>
#include "Tools.h"

#include "pp.h"




namespace guiTools
{
    struct XImageViewer : public QWidget
    {
        XImageViewer ();

        bool load_image (const QString & sPath);

        void set_image(QImage img);
        void on_fit();
        QImage get_selection() ;

        bool flagControlRect = false;
        QRectF rMark; // karthesian
        QRect rCut; // image

        XMouseDelta delta;


        void set_grid_enabled(bool flagEnable);
        bool is_grid_enabled() const;

    protected:

        void paintEvent (QPaintEvent * event) override;

        QImage get_image() const;

        void dragEnterEvent (QDragEnterEvent * event) override;
        void dropEvent (QDropEvent * event) override;

        void wheelEvent (QWheelEvent * event) override;

        void mousePressEvent (QMouseEvent * event) override;
        void mouseReleaseEvent (QMouseEvent * event) override;
        void mouseMoveEvent (QMouseEvent * event) override;

        void draw_grid(XPainter & painter, qreal zoomFactor);


        void resizeEvent (QResizeEvent * event) override;
        void on_resize(QSize sz);
        void on_zoom(bool up, bool flagCurrentMousePos=true); // flagCurrentMousePos==false means middle of window

        QPoint getCurrentMousePos() const;
        QPoint mid_point_of_view() const;

        QPointF current_point_from_cartesian() const;
        QPointF point_from_cartesian(QPointF pt)  const;
        QVector2D vector_from_cartesian(QPointF pt)  const;

        QSize get_image_size() const;

        QPoint point_from_image(QPoint pt) const;

        double get_zoom() const;
        QVector2D get_translation() const;

        XMatrix matrixCartesian;

        QVector2D vTranslate;

        mutable QMutex mutexImage;
        QImage image;

        qreal gridSize = 100;
        qreal totalGridSize = 3000;
        bool flagEnabledGrid = true;

    };

}
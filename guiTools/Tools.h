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

#include "pp.h"


void nullPointerHandling (const char * pFile, int line, const QString & m);

// FAST FAIL null pointer checking
#define NCHECK(pointer) \
    if (pointer==nullptr) nullPointerHandling((__FILE__),(__LINE__),(#pointer " == NULL"))



using namespace okl; // for pp(...) - power print, small logging tool


namespace guiTools
{
    enum class GrowPolicy
    {
        growNot, growVert, growHorz, growAll
    };

    template<typename T> concept QWIDGET = std::derived_from<T, QWidget>;


    template<QWIDGET QWT>
    inline QWT * set_growable (QWT * qwt)
    {
        QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
        sp.setVerticalStretch(1);
        sp.setHorizontalStretch(1);

        qwt->setSizePolicy(sp);
        return qwt;
    }

    template<QWIDGET QWT>
    inline QWT * set_growable_horizontal (QWT * qwt)
    {
        QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sp.setHorizontalStretch(1);
        qwt->setSizePolicy(sp);
        return qwt;
    }

    template<QWIDGET QWT>
    inline QWT * set_growable_vertical (QWT * qwt)
    {
        QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
        sp.setVerticalStretch(1);
        qwt->setSizePolicy(sp);
        return qwt;
    }

    using MILLI  = uint64_t; // milliseconds


    void exec_thread(std::function<void()> f); // execute a function with an other thread of QThreadPool


    bool detect_wayland();

    QImage shotDesktopRectangle();
//    QImage shotDesktopRectangle_X11();
//    QImage shotDesktopRectangle_Wayland();

    struct XPanel : public QFrame
    {
        XPanel ();
        XPanel (Qt::Orientation orientation);

        void set_layout (Qt::Orientation orientation);

        XPanel * set_spacing (int space);
        XPanel * set_margins (int left = 0, int top = 0, int right = 0, int bottom = 0);


        template<QWIDGET WIDGET>
        inline WIDGET * add (WIDGET * w, GrowPolicy growPolicy = GrowPolicy::growNot)
        {
            NCHECK(w);
            if (lay == nullptr)
            {
                NCHECK(lay);
            }

            if (containsWidget(w))
            {
                return w;
            }
            lay->addWidget(w);

            if (growPolicy == GrowPolicy::growAll)
            {
                set_growable(w);
            }
            else if (growPolicy == GrowPolicy::growHorz)
            {
                set_growable_horizontal(w);
            }
            else if (growPolicy == GrowPolicy::growVert)
            {
                set_growable_vertical(w);
            }

            w->setParent(this);

            return w;
        }


        template<QWIDGET WIDGET>
        inline WIDGET * create (GrowPolicy growPolicy = GrowPolicy::growNot)
        {
            WIDGET * component = new WIDGET;
            NCHECK(component);

            add(component, growPolicy);
//
            return component;
        }

        bool containsWidget (QWidget * w) const;

        QLabel * createSpacer (GrowPolicy growPolicy = GrowPolicy::growAll);

        QPushButton * create_button(const QString & sTitle, const QString & sToolTip,std::function<void()> f);

    protected:
        QLayout * lay = nullptr;

    };


    template<typename T> concept XPANEL = std::derived_from<T, XPanel>;


    struct XMainWindow : public QMainWindow
    {
        template<XPANEL MAIN_PANEL>
        MAIN_PANEL * create_content_panel (Qt::Orientation orientation = Qt::Vertical)
        {
            MAIN_PANEL * panContent = new MAIN_PANEL(orientation);
            setCentralWidget(panContent);
            panMain = panContent;
            return panContent;
        }

        XMainWindow (QWidget * parent = 0, Qt::WindowFlags flags = Qt::Widget);

        void exec_gui_method(std::function<void()> f, uint64_t msDelay=0);

    protected:

        XPanel * panMain = nullptr; // Content => setCentralWidget(...)
    };


    struct XMatrix : public QTransform
    {
        using QTransform::QTransform;

        XMatrix & translate (const QVector2D & v);
        XMatrix & translate (const QPointF & v);
        XMatrix & translate (qreal dx, qreal dy);

        XMatrix & scale (qreal factor);
        XMatrix & scale (qreal factorX, qreal factorY);
        XMatrix & rotate (qreal degrees);

        double get_scale_x () const;
        double get_scale_y () const;

        QVector2D get_translation () const;
    };


    struct XPainter : public QPainter
    {
        XPainter (QPaintDevice *);

        void drawCircle (QPointF pt, qreal radius, QColor col = Qt::black);
        void drawXLine (QPointF pt1, QPointF pt2, qreal w = 1.0, QColor col = Qt::black, Qt::PenStyle style = Qt::SolidLine);
        void drawXLine (qreal x1, qreal y1, qreal x2, qreal y2, qreal w = 1.0, QColor col = Qt::black, Qt::PenStyle style = Qt::SolidLine);
        void drawXRect (const QRectF & r, qreal w = 1.0, QColor col = Qt::black, Qt::PenStyle style = Qt::SolidLine);
        void fillXRect (const QRectF & r, QColor col = Qt::green);
        void fillCircle (QPointF pt, qreal radius, QColor colBorder = Qt::black, qreal lineWidth = 1, QColor colFill = Qt::yellow);
        void setAntiAlias (bool flagOn = true);
    };


    struct XMouseDelta
    {
        enum DELTAS
        {
            DELTA_VERTICAL = 1, DELTA_HORIZONTAL = 2, DELTA_ALL = 4,
        };


        // complete delta from drag beginning (ptStart)
        int dx=0;
        int dy=0;

        // last delta from drag
        int dxLast=0;
        int dyLast=0;

        void fromPoint (const QPoint & ptSource, DELTAS deltas = DELTA_ALL);  // global mouse for example

        bool hasData () const;
        bool hasDragData () const;
        bool hasDataX () const;
        bool hasDataY () const;

        void reset ();


    private:
        QPoint pt;
        QPoint ptStart;  // start drag
        bool flagValid;
    };


}
#pragma once


#include <QApplication>
#include <QPushButton>
#include <QMainWindow>
#include <QThreadPool>
#include <QImage>
#include <QSplitter>
#include <QPlainTextEdit>
#include "guiTools/Tools.h"


#include <QApplication>
#include <QScreen>
#include <QPixmap>
#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QCursor>
#include <QEventLoop>
#include <QCheckBox>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>


using namespace guiTools;



struct WindowOCR : XMainWindow
{
    QSplitter * splitter = nullptr;
    XImageViewer * compoImage = nullptr;
    QPlainTextEdit * editor = nullptr;

    QPushButton * btExtract = nullptr;
    QCheckBox * cbGrid = nullptr;
    QComboBox * comboLang = nullptr;

    QLineEdit * edLang = nullptr;

    tesseract::TessBaseAPI api;


    void ini ();


    void save_window_state();
    void restore_window_state();

    inline static QString sCompany = "okl.de";
    inline static QString sProgram = "tg";

    QString sLangCurr="eng";

protected:
    void closeEvent (QCloseEvent * event) override;

    enum class EXT
    {
        REPLACE,
        APPEND
    };

    void on_extract (EXT ext);
    void on_paste();
    void on_about();
    void on_screen_shot();

    void on_clear_image ();
    void on_clear_text ();

    void setup_hello();
    void on_show_lang ();
    void on_sel_lang ();

    void select_lang(const QString & sLang);
};


class DesktopSelection : public QWidget
{
    QPoint startPoint;
    QRect  selectedRect;
    bool   selecting = false;
    QImage result;
    QEventLoop loop;

public:
    DesktopSelection(QWidget* parent = nullptr);


    static QImage run();

protected:
    void mousePressEvent(QMouseEvent* e) override;

    void mouseMoveEvent(QMouseEvent* e) override;

    void mouseReleaseEvent(QMouseEvent* e) override;

    void paintEvent(QPaintEvent*) override;

    void keyPressEvent(QKeyEvent* event);

};

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
#include "guiTools/XImageViewer.h"

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>


using namespace guiTools;



struct WindowOCR : XMainWindow
{
    QSplitter * splitter = nullptr;
    XImageViewer * compoImage = nullptr;
    QPlainTextEdit * editor = nullptr;

    QPushButton * btExtract = nullptr;
    QPushButton * btExtractAppend = nullptr;
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
    void on_sel_lang ();

    void select_lang(const QString & sLang);

    bool flagWelcomeText = true;
    void on_help ();
};


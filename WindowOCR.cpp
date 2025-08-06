#include "WindowOCR.h"
#include "DiaSelectLang.h"

#include <QClipboard>
#include <QImage>
#include <QMimeData>
#include <QSettings>
#include <QMessageBox>
#include <QDir>
#include <QShortcut>


void WindowOCR::on_extract (EXT ext)
{
    pp("WindowOCR::on_extract");

    if (compoImage->flagControlRect)
    {
        pp("main::on_extract, rect:$", compoImage->rCut);
    }

    if (flagWelcomeText)
    {
        flagWelcomeText = false;
        editor->setPlainText("");
    }


    QImage img = compoImage->get_selection();
    if (img.isNull())
    {
        pp("main::on_extract - img is null");
        return;
    }


    pp("sel:$", img);
//


    QString s = "/tmp/a.png";
    img.save("/tmp/a.png");


    exec_thread([this, s, ext] ()
                {
                    btExtract->setEnabled(false);
                    btExtractAppend->setEnabled(false);


                    api.SetImage(pixRead(s.toUtf8().constData()));
                    char * outText = api.GetUTF8Text();
                    QString sText;
                    if (outText)
                    {
                        sText = outText;
                        //pp("da text <<<$>>>", outText);
                        //std::cout << outText;
                        delete[] outText;

                    }
                    else
                    {
                        sText = "geht nicht";
                    }

                    exec_gui_method([sText, this, ext]
                                    {
                                        btExtract->setEnabled(true);
                                        btExtractAppend->setEnabled(true);
                                        if (ext == EXT::REPLACE)
                                        {
                                            editor->setPlainText(sText);
                                        }
                                        else
                                        {
                                            editor->moveCursor(QTextCursor::End);
                                            editor->insertPlainText(sText);
                                        }
                                    });

                });


}

void WindowOCR::on_paste ()
{
    pp("main::on_paste");

    const QClipboard * clipboard = QGuiApplication::clipboard();

    if (clipboard->mimeData()->hasImage())
    {
        QImage image = qvariant_cast<QImage>(clipboard->mimeData()->imageData());

        if (!image.isNull())
        {
            compoImage->set_image(image);
            compoImage->delta.reset();
            compoImage->update();
        }
        else
        {
            qDebug() << "Bild ist leer oder ungültig.";
        }
    }
    else
    {
        qDebug() << "Kein Bild in der Zwischenablage.";
    }

}

void WindowOCR::ini ()
{
    XPanel * pan = create_content_panel<XPanel>();

    splitter = pan->create<QSplitter>(GrowPolicy::growAll);
    splitter->setOrientation(Qt::Horizontal);

//        compoImage = pan->create<XImageViewer>(GrowPolicy::growAll);
    compoImage = new XImageViewer;

    editor = new QPlainTextEdit;
    editor->setPlainText("....press extract button");

    set_growable(compoImage);
    set_growable(editor);

    splitter->addWidget(compoImage);
    splitter->addWidget(editor);

    XPanel * panButtons = pan->create<XPanel>();
    panButtons->set_layout(Qt::Horizontal);
//    panButtons->set_spacing(0);
    panButtons->set_margins(0);

    cbGrid = panButtons->create<QCheckBox>();
    cbGrid->setText("Grid");
    cbGrid->setChecked(true);

//

    QObject::connect(cbGrid, &QCheckBox::stateChanged, [this] (int state)
    {
        compoImage->set_grid_enabled(state == Qt::Checked);
        compoImage->update();
    });

    panButtons->create_button("&Fit", "Resize image that it fits", [this]
    {
        compoImage->on_fit();
    });

    panButtons->create_button("&Paste", "Paste image from clipboard", [this]
    {
        on_paste();
    });
    panButtons->create_button("&Clear", "Clear image", [this]
    {
        on_clear_image();
    });

    panButtons->create_button("&Screenshot", "Start screenshot ...", [this]
    {
        on_screen_shot();
    });


    btExtract = panButtons->create_button("&Extract", "Extract text and replace", [this]
    {
        on_extract(EXT::REPLACE);
    });
    btExtractAppend = panButtons->create_button("Extract/Appen&d", "Extract text and append", [this]
    {
        on_extract(EXT::APPEND);
    });


    panButtons->createSpacer();


//    QLabel * lab = panButtons->create<QLabel>();
//    lab->setText("language");

    edLang = panButtons->create<QLineEdit>();
    edLang->setText("");
    edLang->setReadOnly(true);


    panButtons->create_button("Select &Language", "Select language", [this]
    {
        on_sel_lang();
    });

    panButtons->create_button("Clea&r", "Clears all text", [this]
    {
        on_clear_text();
    });

    panButtons->create_button("&About", "", [this]
    {
        on_about();
    });

    QPushButton * btHelp = panButtons->create_button("?", "", [this]
    {
        on_help();
    });

//    btHelp->setContentsMargins(0,0,0,0);
    btHelp->setMaximumWidth(30);

    setWindowTitle("tg - Tesseract GUI");

    QShortcut * shortcut = new QShortcut(QKeySequence(Qt::Key_F1), this);
    QObject::connect(shortcut, &QShortcut::activated, [this] ()
    {
        on_help();
    });

    setup_hello();

}


void WindowOCR::closeEvent (QCloseEvent * event)
{
    pp("WindowOCR::closeEvent");

    save_window_state();

    QWidget::closeEvent(event);

}

void WindowOCR::save_window_state ()
{
    QMainWindow * window = this;

    QSettings settings(sCompany, sProgram);

    settings.setValue("window/size", window->size());
    settings.setValue("window/pos", window->pos());


    settings.setValue("window/grid", cbGrid->isChecked());
    settings.setValue("window/lang", sLangCurr);

    if (splitter)
    {
        settings.setValue("window/splitter", splitter->saveState());
    }

    //if (cbGrid)
}

void WindowOCR::restore_window_state ()
{
    QMainWindow * window = this;

    QSettings settings(sCompany, sProgram);

    QSize size = settings.value("window/size", QSize(800, 600)).toSize();
    QPoint pos = settings.value("window/pos", QPoint(100, 100)).toPoint();

    bool flag = settings.value("window/grid", true).toBool();
    cbGrid->setChecked(flag);
    compoImage->set_grid_enabled(flag);

    sLangCurr = settings.value("window/lang", QString("eng")).toString();

    pp("WindowOCR::restore_window_state, lang:<<<$>>>", sLangCurr);

    edLang->setText(sLangCurr);

    select_lang(sLangCurr);

    if (splitter)
    {
        splitter->restoreState(settings.value("window/splitter").toByteArray());
    }


    window->resize(size);
    window->move(pos);

}

void WindowOCR::on_about ()
{
    QString s = "tg 2025 - v 0.205\n"
                "by Olaf Kliche (C) - Software Developer \n"
                "Inspired by Tanya Kliche - Professional Linguist\n";


    QMessageBox::information(this, "About", s);

}

void WindowOCR::on_screen_shot ()
{
    pp("WindowOCR::on_screen_shot");

    if (flagWelcomeText)
    {
        flagWelcomeText = false;
        editor->setPlainText("");
    }

    hide();
    QApplication::processEvents();

    QImage img = shotDesktopRectangle();

    if (img.isNull())
    {
        pp("WindowOCR::on_screen_shot - CANCEL");
    }
    else
    {
        compoImage->set_image(img);
    }

    show();

    // force to be in foreground
    raise();
    activateWindow();

    pp("WindowOCR::on_screen_shot, img:$", img);
}

void WindowOCR::on_clear_image ()
{
    compoImage->set_image(QImage());

}

void WindowOCR::on_clear_text ()
{
    editor->setPlainText("");
}



void WindowOCR::select_lang (const QString & sLang)
{
    if (sLang.isEmpty())
    {
        pp("WindowOCR::select_lang, could not initialize tesseract with EMPTY lang");
        return;
    }

    api.End();

    exec_gui_method([this, sLang]
                    {
                        if (api.Init(NULL, sLang.toUtf8().data()) != 0)
                        {
                            pp("WindowOCR::select_lang, could not initialize tesseract with lang <<<$>>>", sLang);
                            return;
                        }
                        sLangCurr = sLang;
                        edLang->setText(sLangCurr);
                    }, 500);


}

void WindowOCR::on_sel_lang ()
{
    pp("WindowOCR::on_sel_lang");

    QString s = DiaSelectLang::get_selected_lang(this, sLangCurr);

    if (!s.isEmpty())
    {
        select_lang(s);
    }

    pp("WindowOCR::on_sel_lang <<<$>>>", s);
}

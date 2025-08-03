#include "DiaSelectLang.h"
#include "guiTools/pp.h"

using namespace okl;

DiaSelectLang::DiaSelectLang (QWidget * parent, const QString & sLangCurr) : QDialog(parent)
{
    setWindowTitle("Choose Languages");
    setModal(true);
    resize(400, 500);

    listWidget = new QListWidget(this);
    listWidget->setSelectionMode(QAbstractItemView::NoSelection);

    QStringList langs = list_languages(autoDetectTessdataPath());
    langs.sort();

    QStringList parts = sLangCurr.split("+");

    for (const QString & lang: langs)
    {
        auto * item = new QListWidgetItem(lang, listWidget);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
//        item->setCheckState(Qt::Unchecked);
        item->setCheckState(parts.contains(lang) ? Qt::Checked : Qt::Unchecked);
    }

    auto * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto * layout = new QVBoxLayout(this);
    layout->addWidget(listWidget);
    layout->addWidget(buttonBox);
    setLayout(layout);
}

QString DiaSelectLang::get_selected_lang (QWidget * parent, const QString & sLangCurr)
{
    DiaSelectLang dlg(parent, sLangCurr);
    if (dlg.exec() == QDialog::Accepted)
    {
        QStringList selected;
        QString sEng;
        QString sDeu;
        for (int i = 0; i < dlg.listWidget->count(); ++i)
        {
            auto * item = dlg.listWidget->item(i);
            if (item->checkState() == Qt::Checked)
            {
                QString s = item->text();
                selected << item->text();
            }
        }
        return selected.join("+");
    }

    return {};
}

QString DiaSelectLang::autoDetectTessdataPath ()
{
    QString path = QString::fromLocal8Bit(qgetenv("TESSDATA_PREFIX"));

#ifdef Q_OS_WIN
    if (path.isEmpty())
            path = "C:/Program Files/Tesseract-OCR/tessdata";
#elif defined(Q_OS_UNIX)
    if (path.isEmpty())
    {
        path = "/usr/share/tesseract-ocr/4.00/tessdata";
        if (!QDir(path).exists())
        {
            path = "/usr/share/tesseract-ocr/5/tessdata";
        }
        if (!QDir(path).exists())
        {
            pp("DiaSelectLang::autoDetectTessdataPath, problems to detect 'tessdata' folder");
            pp(4, "expected /usr/share/tesseract-ocr/5/tessdata or /usr/share/tesseract-ocr/4.00/tessdata");
        }
    }
#endif

    return path;
}

QStringList DiaSelectLang::list_languages (const QString & tessdataDir)
{
    QDir dir(tessdataDir);

    if (!dir.exists())
    {
        pp("DiaSelectLang::list_languages, tessdata directory <<<$>>> not found:", tessdataDir);
        return {};
    }

    QStringList result;

    for (const QString & file: dir.entryList(QStringList("*.traineddata")))
    {
        QString lang = file.section('.', 0, 0); // vor ".traineddata"
        result << lang;
    }

    return result;
}

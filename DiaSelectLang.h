#pragma once

#include <QDialog>
#include <QListWidget>

struct DiaSelectLang : QDialog
{
    QListWidget* listWidget;

    DiaSelectLang(QWidget* parent, const QString & sLangCurr);

    static QString get_selected_lang(QWidget* parent, const QString & sLangCurr);

private:
    QString autoDetectTessdataPath();

    QStringList list_languages(const QString& tessdataDir);
};

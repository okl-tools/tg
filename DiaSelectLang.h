#pragma once

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QDir>
#include <QFile>
#include <QDebug>

struct DiaSelectLang : QDialog
{

    QListWidget* listWidget;

    DiaSelectLang(QWidget* parent, const QString & sLangCurr);

    static QString get_selected_lang(QWidget* parent, const QString & sLangCurr);

private:
    QString autoDetectTessdataPath();

    QStringList list_languages(const QString& tessdataDir);
};

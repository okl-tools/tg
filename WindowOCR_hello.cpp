#include "WindowOCR.h"
#include "DiaSelectLang.h"

#include <QClipboard>
#include <QImage>
#include <QMimeData>
#include <QSettings>
#include <QMessageBox>
#include <QDir>

static QString get_help_text()
{
    QString s = "Welcome to tg - Tesseract GUI\n"
                "\n"
                "tg is an OCR program\n"
                "tg – tesseract gui is based on the powerful Tesseract library.\n"
                "The program is open source and written in C++20 / Qt6.\n"
                "\n"
                "Usage in brief:\n"
                "The interface is split into two panels.\n"
                "The left side displays the image to be analyzed.\n"
                "The right side shows the extracted text.\n"
                "\n"
                "The left panel accepts images via clipboard or screenshot.\n"
                "You can also drag and drop an image from your file manager.\n"
                "\n"
                "To extract text from an image, press one of the Extract buttons.\n"
                "By default, the Tesseract library attempts to recognize all text in the image.\n"
                "\n"
                "If you want to limit recognition to a specific area, you can select a rectangle:\n"
                "Hold the Ctrl key, press the left mouse button, and drag a rectangle around the desired area.\n"
                "Once you release the mouse, the selected region will be used for text extraction.\n"
                "\n"
                "Image navigation:\n"
                "Use the mouse wheel to zoom, and drag the image with the left mouse button (no Ctrl key required).\n"
                "\n"
                "How to install language packages for tesseract:\n"
                "\n"
                "sudo apt install tesseract-ocr-eng  (English)\n"
                "sudo apt install tesseract-ocr-deu  (German)\n"
                "sudo apt install tesseract-ocr-rus  (Russian)\n"
                "sudo apt install tesseract-ocr-fra  (French)\n"
                "sudo apt install tesseract-ocr-chi-sim  (Chinese Simplified)\n"
                "sudo apt install tesseract-ocr-chi-tra  (Chinese Traditional)\n"
                "sudo apt install tesseract-ocr-jpn  (Japanese)\n"
                " \n"
                "See all language packages:\n"
                " https://github.com/tesseract-ocr/tessdoc/blob/main/Data-Files-in-different-versions.md\n"
                "\n";

    return s;
}


void WindowOCR::setup_hello ()
{
    editor->setPlainText(get_help_text());

}

void WindowOCR::on_help ()
{
    QMessageBox::information(this, "Help", get_help_text());

}

#include "Tools.h"
#include <iostream>

#include <QVBoxLayout>
#include <QHBoxLayout>



namespace guiTools
{
    XPanel::XPanel ()
    {

    }

    XPanel::XPanel(Qt::Orientation orientation)
    {
        if (orientation == Qt::Vertical)
        {
            lay = new QVBoxLayout();
        }
        else
        {
            lay = new QHBoxLayout();
        }

        setLayout(lay);

    }

    bool XPanel::containsWidget (QWidget * w) const
    {
        for (int i = 0; i < lay->count(); i++)
        {
            QLayoutItem * ql = lay->itemAt(i);
            if (ql)
            {
                QWidget * wTmp = ql->widget();
                if (wTmp)
                {
                    if (w == wTmp)
                    {
                        return true;
                    }
                }
            }
        }


        return false;
    }

    void XPanel::set_layout (Qt::Orientation orientation)
    {
        if (orientation == Qt::Vertical)
        {
            lay = new QVBoxLayout();
        }
        else
        {
            lay = new QHBoxLayout();
        }

        setLayout(lay);

    }

    XPanel * XPanel::set_spacing (int space)
    {
        if (lay)
        {
            lay->setSpacing(space);
        }
        return this;
    }

    XPanel * XPanel::set_margins(int left, int top, int right, int bottom)
    {
        if (lay)
        {
            lay->setContentsMargins(left, top, right, bottom);
        }

        return this;
    }

    QLabel * XPanel::createSpacer (GrowPolicy growPolicy)
    {
        QLabel * lab = create<QLabel>(growPolicy);

        return lab;
    }

    QPushButton * XPanel::create_button (const QString & sTitle, const QString & sToolTip, std::function<void ()> f)
    {

        QPushButton * bt = create<QPushButton>();
        bt->setText(sTitle);
        bt->setToolTip(sToolTip);

        QPushButton::connect(bt, &QPushButton::clicked, [this, f] ()
        {
            f();
        });

        return bt;
    }


}
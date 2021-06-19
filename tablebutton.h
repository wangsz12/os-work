#ifndef TABLEBUTTON_H
#define TABLEBUTTON_H

#include <QObject>
#include "qpushbutton.h"

class TableButton : public QPushButton
{
    Q_OBJECT
public:
    TableButton(QWidget*);

signals:
    void clicked(QPushButton*);
};

#endif // TABLEBUTTON_H

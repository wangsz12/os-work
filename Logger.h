#pragma once
#include "qstring.h"
#include "qobject.h"
using namespace std;

class Logger : public QObject
{
    Q_OBJECT

public:
    void log(QString);

signals:
    void logSignal(QString);
};

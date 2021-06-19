#include "Logger.h"
#include <iostream>
#include <chrono>
#include "qstring.h"
#include <string>
#include "qdatetime.h"
using namespace std;

void Logger::log(QString info) {
    QTime time = QTime::currentTime();
    QString out = "[" + time.toString("hh:mm:ss") + "] " + QString(info);
    emit logSignal(out);
}

#pragma once 
#include <QVector>
#include <QByteArray>
#include <QString>
QVector <QString> split(QString str, char delimiter);
QString LZ77(QByteArray input, int option);

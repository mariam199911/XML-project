#pragma once 
#include <QVector>
#include <QByteArray>
#include <QString>
struct Match_Pointer{
    qsizetype begin;
    qsizetype length;
    Match_Pointer(qsizetype begin,qsizetype length);
};
Match_Pointer _largest_match(QByteArray::iterator window, QByteArray::iterator look_ahead_buffer);
QString minify(QString file);
QByteArray compress(QString& file);
QString decompress(QByteArray& compressed_byte_array);

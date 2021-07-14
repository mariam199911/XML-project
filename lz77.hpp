#pragma once
#include <QVector>
#include <QByteArray>
#include <QString>
#include <QFile>
struct Match_Pointer{
    int begin;
    int length;
    Match_Pointer(int begin,int length);
};
Match_Pointer _largest_match(QByteArray::iterator window, QByteArray::iterator look_ahead_buffer);
QString minimize(QString file);
QByteArray compression(QString& file);
QString decompression(QByteArray& compressed_byte_array);

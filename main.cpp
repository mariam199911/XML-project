#include "mainwindow.h"
#include "lz77.hpp"
#include <QApplication>
#include <QByteArray>
#include <QDebug>
int main(int argc, char *argv[])
{
    QString s="<book category=\"cooking\"><title lang=\"en\">Everyday Italian</title><author>Giada De Laurentiis</author><year>2005</year><price>30.00</price></book><book category=\"children\"><title lang=\"en\">Harry Potter</title><author>J K. Rowling</author><year>2005</year><price>29.99</price></book><book category=\"web\"><title lang=\"en\">Learning XML</title><author>Erik T. Ray</author><year>2003</year><price>39.95</price></book></bookstore>";
    QString output=LZ77(s.toUtf8(),1);
    qDebug() << output;
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();

}

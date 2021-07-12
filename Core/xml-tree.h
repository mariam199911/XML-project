#ifndef XMLTREE_H
#define XMLTREE_H

#include "tag-block.h"
#include <QVector>
#include <QString>
#include <QMap>
#include <QStringList>
#include <QFile>
#include <QStack>
#include <QTextStream>
#include <QDebug>

class XMLTree
{
private:
    QString fileText;
    QFile *file;
    MainBlock *xmlRoot = nullptr;

    /*
     * Converting QFile into QString for analysis purposes.
     */
    void convertingXMLFileIntoText();

    void analyzeXMLFileText();

    /*
     * Dividing the XML file into tags (open tag, closed tag, text tag, info tag).
     * Each tag is stored in a separate string.
     */
    QVector<QString> * breakingFileTextIntoTags();

    /*
     * Building the XML tree from separate tags.
     */
    void buildingXMLTree(QVector<QString> * separateTag);

public:
    XMLTree(QString fileText);
    XMLTree(QFile *file);

    /*
     * Printing the XML tree structure after prettifying.
     */
    QString prettifyingXMLTreeFile(MainBlock *root, int &spacesNum, QString& outputFile);

    /*
     * Printing the XML tree structure after minfying.
     */
    QString minfyingXMLTreeFile(MainBlock *root, QString& outputFile);

    MainBlock* getXMLFileRoot();
    ~XMLTree();
};

#endif // XMLTREE_H

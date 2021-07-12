#ifndef XMLFILECONVERTER_H
#define XMLFILECONVERTER_H

#include <QString>
#include <QMap>
#include <QVector>
#include <QTextStream>
#include "xml-tree.h"

class XMLFileConverter
{
private:
    QString JSONFile;
    QString fileText;
    int indentCounter;

    void generateJSONFile(MainBlock* root);
    void generateJSONObject(MainBlock *currentBlock, bool &isLastBlock);

    /*
     * Dividing open tag into parts to indicate the tag name and the tag attributes.
     */
    QStringList breakingStartTagIntoParts(QString startTag);

    void generateIndentationForJSON();
    void printing();
public:
    XMLFileConverter(QString fileText);
};

#endif // XMLFILECONVERTER_H

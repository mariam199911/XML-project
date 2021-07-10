#ifndef TEXTBLOCK_H
#define TEXTBLOCK_H

#include "main-block.h"
#include <QVector>
#include <QString>
#include <QMap>

class TextBlock : public MainBlock
{
public:
    TextBlock(QString name);
    QVector<MainBlock *> * getInternalBlocks() override;
    QMap<QString, QString> * getTagAttributes() override;
    ~TextBlock() override;
};

#endif // TEXTBLOCK_H

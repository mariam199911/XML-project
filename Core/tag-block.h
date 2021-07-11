#ifndef TAGBLOCK_H
#define TAGBLOCK_H

#include "main-block.h"
#include <QVector>
#include <QMap>
#include <QMap>

class TagBlock : public MainBlock
{
private:
    //internalBlocks is a pointer to a QVector, and each element in the QVector is a pointer to a MainBlock object.
    QVector<MainBlock *> * internalBlocks;
    //QMap<QString, QString> * tagAttributes;
public:
    TagBlock(QString content, TagType tagType);
    ~TagBlock() override;

    QVector<MainBlock *> * getInternalBlocks() override;
    //QMap<QString, QString> * getTagAttributes() override;
};

#endif // TAGBLOCK_H

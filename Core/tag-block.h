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
    QVector<MainBlock*> *internalBlocks;
    QMap<QString, QString> *tagAttributes;
public:
    TagBlock(QString blockName, TagType tagType);
    virtual ~TagBlock();

    virtual QVector<MainBlock*> *getInternalBlocks();
    virtual QMap<QString, QString> *getTagAttributes();
};

#endif // TAGBLOCK_H

#include "tag-block.h"

TagBlock::TagBlock(QString name, TagType type)
    : MainBlock(name, type)
    , internalBlocks(new QVector<MainBlock*>())
    , tagAttributes(new QMap<QString, QString> ())
{}

TagBlock::~TagBlock()
{
    for(int i = 0; i < internalBlocks->size(); i++)
    {
        //Access each element in the internalBlock and delete it.
        delete (*internalBlocks)[i];
    }
    delete internalBlocks;
    delete tagAttributes;
}

QVector<MainBlock*> *TagBlock::getInternalBlocks()
{
    return internalBlocks;
}

QMap<QString, QString> *TagBlock::getTagAttributes()
{
    return tagAttributes;
}

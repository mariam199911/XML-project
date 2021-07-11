#include "tag-block.h"

TagBlock::TagBlock(QString content, TagType type)
    : MainBlock(content, type)
    , internalBlocks(new QVector<MainBlock*>())
    //, tagAttributes(new QMap<QString, QString> ())
{}

TagBlock::~TagBlock()
{
    for(int i = 0; i < internalBlocks->size(); i++)
    {
        //Access each element in the internalBlock and delete it.
        delete (*internalBlocks)[i];
    }
    delete internalBlocks;
    //delete tagAttributes;
}

QVector<MainBlock *> * TagBlock::getInternalBlocks()
{
    return internalBlocks;
}

/*QMap<QString, QString> * TagBlock::getTagAttributes()
{
    return tagAttributes;
}*/

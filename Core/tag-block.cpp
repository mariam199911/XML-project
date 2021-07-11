#include "tag-block.h"

TagBlock::TagBlock(QString content, TagType type)
    : MainBlock(content, type)
    , internalBlocks(new QVector<MainBlock*>())
{}

TagBlock::~TagBlock()
{
    for(int i = 0; i < internalBlocks->size(); i++)
    {
        //Access each element in the internalBlock and delete it.
        delete (*internalBlocks)[i];
    }
    delete internalBlocks;
}

QVector<MainBlock *> * TagBlock::getInternalBlocks()
{
    return internalBlocks;
}

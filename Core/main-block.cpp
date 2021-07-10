#include "main-block.h"

MainBlock::MainBlock(QString name, TagType type)
    : blockName(name)
    , tagType(type)
{}

MainBlock::~MainBlock()
{
}

QString MainBlock::getBlockName()
{
    return blockName;
}

void MainBlock::setBlockName(QString name)
{
    blockName = name;
}

TagType MainBlock::getBlockTagType()
{
    return tagType;
}

void MainBlock::setBlockTagType(TagType type)
{
    tagType = type;
}

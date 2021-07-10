#include "main-block.h"

MainBlock::MainBlock(QString blockName, TagType tagType)
    : blockName(blockName),
      tagType(tagType)
{}

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

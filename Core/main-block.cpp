#include "main-block.h"

MainBlock::MainBlock(QString content, TagType type)
    : blockContent(content)
    , tagType(type)
{}

MainBlock::~MainBlock()
{
}

QString MainBlock::getBlockContent()
{
    return blockContent;
}

void MainBlock::setBlockContent(QString content)
{
    blockContent = content;
}

TagType MainBlock::getBlockTagType()
{
    return tagType;
}

void MainBlock::setBlockTagType(TagType type)
{
    tagType = type;
}

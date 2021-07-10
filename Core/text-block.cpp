#include "text-block.h"

TextBlock::TextBlock(QString name)
    : MainBlock(name, TagType::TAG_CONTENT)
{}

TextBlock::~TextBlock()
{
}

QVector<MainBlock *> * TextBlock::getInternalBlocks()
{
    return nullptr;
}

QMap<QString, QString> * TextBlock::getTagAttributes()
{
    return nullptr;
}


#ifndef MAINBLOCK_H
#define MAINBLOCK_H

#include <QString>
#include <QMap>
#include <QVector>

typedef enum TagType
{
    OPEN_TAG,
    CLOSED_TAG,
    TAG_CONTENT,
    TAG_INFO
} TagType;

/*
 * This class is the base class for representing the data stored in the XML file.
 *
 * Illustration:
 *
 * 1. The prolog defines the XML version and the character encoding is a TagType of TAG_INFO.
 *    example: <?xml version="1.0" encoding="UTF-8"?>
 *
 * 2. <book category="cooking"> representing a MainBlock in which book is the blockName,
 *    and category="cooking" is an attribute of type QMap, the MainBlock is of TagType (OPEN_TAG).
 *
 * 3. The MainBlock can be either TagBlock or TextBlock, TagBlock for blocks of OPEN_TAG, CLOSED_TAG, INFO_TAG.
 *    and TextBlock for text inserted between OPEN_TAG and CLOSED_TAG, and it is of TagType (TAG_CONTENT).
 *
 * 4. <book category="cooking">
 *        <title lang="en">Everyday Italian</title>
 *    </book>
 *    a. <title lang="en"> represnting a MainBlock in which title is the blockName, and
 *       lang="en" is an attribute of type QMap, the MainBlock is of TagType (OPEN_TAG).
 *    b. Everyday Italian is a TextBlock, the text inserted between OPEN_TAG and CLOSED_TAG.
 *    c. </title> represnting a MainBlock in which title is the blockName, and its TagType is CLOSED_TAG.
 *    d. <book category="cooking">, This MainBlock contains an internal block stored in QVactor<MainBlock*>
 */

class MainBlock
{
protected:
    QString blockName;
    TagType tagType;
public:
    MainBlock(QString name, TagType type);
    QString getBlockName();
    void setBlockName(QString name);
    TagType getBlockTagType();
    void setBlockTagType(TagType type);
    virtual QVector<MainBlock*> *getInternalBlocks() = 0;
    virtual QMap<QString, QString> *getTagAttributes() = 0;
    virtual ~MainBlock();
};

#endif // MAINBLOCK_H

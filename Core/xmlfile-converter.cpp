#include "xmlfile-converter.h"
#include <QDebug>

XMLFileConverter::XMLFileConverter(QString fileText)
    : fileText(fileText)
{
    XMLTree* treeNode = new XMLTree(fileText);
    MainBlock* root = treeNode->getXMLFileRoot();

    qDebug("Entered");
    bool isLast1 = false;
    generateJSONObject(root, isLast1);
    //generateJSONFile(root);
    //printing();
}

void XMLFileConverter::generateJSONFile(MainBlock* root)
{
    qDebug("Generate started");
    //Creating a root node for JSON file.
    MainBlock* jsonRoot = root;
    JSONFile.append('{');
    JSONFile.append('\n');

    indentCounter = 1;

    bool isLast1 = false;
    bool isLast2 = true;

    //Getting the internal blocks of the root block and call generateJSONObject, which will be called recursively.
    QVector<MainBlock*> *internalBlocks = jsonRoot->getInternalBlocks();
    for(int i = 0; i < internalBlocks->size(); i++)
    {
        //TO DO, may be xmlRoot->getInternalBlocks()->size() - 2 (the previous block of the last one)
        if(i == internalBlocks->size() - 1)
        {
            generateJSONObject((*internalBlocks)[i], isLast2);
        }
        else {
            generateJSONObject((*internalBlocks)[i], isLast1);
        }
    }

    JSONFile.append('}');
    JSONFile.append('\n');
    qDebug("Generate finished");
}

void XMLFileConverter::generateJSONObject(MainBlock *currentBlock, bool &isLastBlock)
{
    qDebug("recursion");
    qDebug() << JSONFile;
    QVector<MainBlock*> *internalBlocks = currentBlock->getInternalBlocks();

    //In case the current block is closed tag or text tag, which has no children in it.
    //Indicates the end of a given block.
    if(internalBlocks->empty()) {
        return;
    }

    //Splitting the open tag into tag name, and tag attributes.
    QStringList startTagContents = breakingStartTagIntoParts(currentBlock->getBlockContent());
    QString startTagName = startTagContents[0];
    QMap<QString, QString> startTagAttributes;

    for (int i = 1; i < startTagContents.length(); i++)
    {
        if (i % 2 == 1)
        {
            startTagAttributes.insert(startTagContents[i], startTagContents[i+1]);
        }
    }

    generateIndentationForJSON();

    JSONFile.append('"');
    JSONFile.append(startTagName);
    JSONFile.append("\": ");

    //In case the current block is an open tag, and has only its text tag, and its closed tag.
    //No internal blocks in it.
    if(internalBlocks->size() == 2 ) {

        //In case there is no tag attributes, just insert the text tag into the JSON file.
        if(startTagAttributes.empty()) {
            JSONFile.append('"');
            JSONFile.append((*internalBlocks)[0]->getBlockContent());
            JSONFile.append('"');

            if(isLastBlock) {
                JSONFile.append("");
            }
            else {
                JSONFile.append(',');
            }

            JSONFile.append('\n');
        }
        //In case open tag has attributes.
        else {
            JSONFile.append('{');
            indentCounter++;

            //Inserting start tag attributes in the JSON file.
            auto mapIterator = startTagAttributes.constBegin();
            while(mapIterator != startTagAttributes.constEnd())
            {
                generateIndentationForJSON();

                JSONFile.append("\"@");
                JSONFile.append(mapIterator.key());
                JSONFile.append("\": \"");
                JSONFile.append(mapIterator.value());
                JSONFile.append("\",\n");
            }

            //Inserting the text tag in the JSON file.
            generateIndentationForJSON();

            JSONFile.append("\"#text\": \"");
            JSONFile.append((*internalBlocks)[0]->getBlockContent());
            JSONFile.append("\"\n");

            indentCounter--;
            generateIndentationForJSON();

            //Inseting a closed bracket that indicates the end of the start tag.
            JSONFile.append('}');
            if(isLastBlock) {
                JSONFile.append("");
            }
            else {
                JSONFile.append(',');
            }

            JSONFile.append('\n');
        }
        return;
    }
    JSONFile.append('{');
    JSONFile.append('\n');
    indentCounter++;

    //Inserting start tag attributes in the JSON file.
    auto mapIterator = startTagAttributes.constBegin();
    while(mapIterator != startTagAttributes.constEnd())
    {
        generateIndentationForJSON();
        JSONFile.append("\"@");
        JSONFile.append(mapIterator.key());
        JSONFile.append("\": \"");
        JSONFile.append(mapIterator.value());
        JSONFile.append("\",\n");
    }

    for(int i = 0; i < internalBlocks->size(); i++)
    {
        //TO DO, may be xmlRoot->getInternalBlocks()->size() - 2 (the previous block of the last one)
        if(i == internalBlocks->size() - 1)
        {
            generateJSONObject((*internalBlocks)[i], isLastBlock);
        }
        else {
            generateJSONObject((*internalBlocks)[i], isLastBlock);
        }
    }

    indentCounter--;
    generateIndentationForJSON();

    //Inseting a closed bracket that indicates the end of the current tag.
    JSONFile.append('}');
    if(isLastBlock) {
        JSONFile.append("");
    }
    else {
        JSONFile.append(',');
    }

    JSONFile.append('\n');
}

QStringList XMLFileConverter::breakingStartTagIntoParts(QString startTag)
{
    QStringList tagParts;
    QString currentPart = "";
    startTag = startTag.mid(1, startTag.length() - 1);
    bool isAttribute = false;
    QChar currentCharacter;
    for(int i = 0 ; i < startTag.length(); i++)
    {
        currentCharacter = startTag[i];

        // An empty character exists either after the tag name or between the attributes.
        if(currentCharacter == ' ') {
            // The empty character exists between the attributes.
            if(isAttribute) {
                currentPart += currentCharacter;
            }
            // Indicates the end of a tag name.
            else if(!currentPart.isEmpty()) {
                tagParts.push_back(currentPart);
                currentPart.clear();
            }
        }
        else if(currentCharacter == '"') {
            // Indicates the end of an attribute value.
            if(isAttribute) {
                tagParts.push_back(currentPart);
                currentPart.clear();
            }

            isAttribute = !isAttribute;
        }
        // Indicates the end of an attribute key.
        else if(currentCharacter == '=') {
            tagParts.push_back(currentPart);
            currentPart.clear();
        }
        // Indicates the end of the tag.
        else if(currentCharacter == '>' && !currentPart.isEmpty())
        {
            tagParts.push_back(currentCharacter);
        }
        // In case of an info tag, the '?' is only stored at the end of the info tag.
        else if(currentCharacter == '?' && currentPart.isEmpty())
        {
            continue;
        }
        else if(currentCharacter == '/' && !isAttribute)
        {
            tagParts.push_back("/");
        }
        else {
            currentPart += currentCharacter;
        }
    }
    return tagParts;
}

void XMLFileConverter::generateIndentationForJSON()
{
    for (int i = 0; i < indentCounter; i++)
    {
        JSONFile.append('\t');
    }
}

void XMLFileConverter::printing()
{
    qDebug() << JSONFile;
}


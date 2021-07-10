#include "xml-tree.h"

XMLTree::XMLTree(QString fileText)
    : fileText(fileText)
{
    analyzeXMLFileText();
}

XMLTree::~XMLTree()
{
    if(xmlRoot != nullptr)
    {
        delete xmlRoot;
    }
    delete file;
}

void XMLTree::analyzeXMLFileText()
{
    QVector<QString> *separateTag = breakingFileTextIntoTags();
    buildingXMLTree(separateTag);
}

QVector<QString> * XMLTree::breakingFileTextIntoTags()
{
    QVector<QString> *separateTags = new QVector<QString> ();
    QString currentTag = "";
    QString currentCharacter;
    bool isComment = false;
    bool isLetter = false;

    //TO DO: FIX (int i) an overflow may happen if number of characters exceeds MAX_INT.
    for(int i = 0; i < fileText.length(); i++)
    {
        currentCharacter = fileText[i];

        if(currentCharacter == '<')
        {
            if(fileText[i+1] == '!') {
                isComment = true;
            }
            else if (!isComment){
                //In case of open tag, the current tag is always an empty string.
                //In case of text tag, the current tag has a value so it is pushed in the vector of strings.
                if(!currentTag.isEmpty()) {
                    separateTags->push_back(currentTag);
                    currentTag.clear();
                }
                currentTag += currentCharacter;
                isLetter = true;
            }
        }
        //Indicates the tag end, therefore the current tag is pushed in the vector of strings.
        else if(currentCharacter == '>' && !isComment)
        {
            currentTag += currentCharacter;
            separateTags->push_back(currentTag);
            currentTag.clear();
            isLetter = false;
        }
        else if(isLetter || (currentCharacter != ' ' && currentCharacter != '\t' && currentCharacter != '\n'))
        {
            if(currentCharacter == '-' && isComment) {
                //Indicates the end of a comment
                if(fileText[i+1] == '-' && fileText[i+2] == '>') {
                    isComment = false;
                    i+=2;
                }
            }
            else if(!isComment) {
                currentTag += currentCharacter;
                isLetter = true;
            }
        }
    }
    return separateTags;
}

void XMLTree::buildingXMLTree(QVector<QString> *separateTag)
{
    xmlRoot = new TagBlock("XML-Root", TagType::OPEN_TAG);
    QStack<MainBlock* > stack;
    stack.push(xmlRoot);

    for(int i = 0; i < separateTag->length(); i++)
    {
        QString currentTag = (*separateTag)[i];

        //Indicates the end of a tag, which means that the whole tag ends with its internal blocks.
        if(currentTag[0] == '<' && currentTag[1] == '/')
        {
            /*
             * In case there is no text tag
             * example: <pointer></pointer>
             * Corner case:
             * <library>
             * <pointer></pointer>
             * </library>
             */
            if((*separateTag)[i-1][0] == '<' && (*separateTag)[i-1][1] != '/')
            {
                MainBlock* topElement = stack.top();
                QVector <MainBlock* > * internalBlocks = topElement->getInternalBlocks();
                internalBlocks->push_back(new TextBlock(""));
            }
            stack.pop();
        }
        //Indicates an open tag.
        else if(currentTag[0] == '<')
        {
            QStringList startTag = breakingStartTagIntoParts(currentTag);

            //Creating a new block for the open tag.
            MainBlock* newBlock = new TagBlock(startTag[0], TagType::OPEN_TAG);

            //Storing the tag attributes for the new created block.
            for(int i = 1; i < startTag.length(); i++)
            {
                if(i % 2 && startTag[i] != "/")
                {
                    QMap<QString, QString> *attributes = newBlock->getTagAttributes();
                    attributes->insert(startTag[i], startTag[i+1]);
                }
                else if(startTag[i] == "/")
                {
                    newBlock->setBlockTagType(TagType::CLOSED_TAG);
                }
            }

            //Push back the new created block into the internal blocks of the parent block.
            MainBlock* topElement = stack.top();
            QVector <MainBlock* > * internalBlocks = topElement->getInternalBlocks();
            internalBlocks->push_back(newBlock);

            //Pushing the new created block into the stack in case if the tag isn't info tag.
            //Because to insert the children of the new created block into its internal blocks.
            //In case of an info tag, it doesn't contain tag childen so no need to push it into the stack.
            if(startTag[0] != "xml" && startTag[0] != "xml-model" && newBlock->getBlockTagType() != TagType::CLOSED_TAG)
            {
                stack.push(newBlock);
            }
            else if(newBlock->getBlockTagType() != TagType::CLOSED_TAG)
            {
                newBlock->setBlockTagType(TagType::TAG_INFO);
            }
        }
        //In case of having a text tag.
        else {
            MainBlock* topElement = stack.top();
            QVector <MainBlock* > * internalBlocks = topElement->getInternalBlocks();
            internalBlocks->push_back(new TextBlock(currentTag));
        }
    }
}

QStringList XMLTree::breakingStartTagIntoParts(QString startTag)
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

MainBlock* XMLTree::getXMLFileRoot()
{
    return xmlRoot;
}

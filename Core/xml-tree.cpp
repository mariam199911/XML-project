#include "xml-tree.h"

XMLTree::XMLTree(QString fileText)
    : fileText(fileText)
    , JSONFile("")
    , indentCounter(0)

{
    analyzeXMLFileText();
}

XMLTree::XMLTree(QFile *file)
    : fileText("")
    , JSONFile("")
    , indentCounter(0)
    , file(file)
{
    convertingXMLFileIntoText();
    analyzeXMLFileText();
}

XMLTree::~XMLTree()
{
    if(xmlRoot != nullptr)
    {
        delete xmlRoot;
    }
}

void XMLTree::convertingXMLFileIntoText()
{
    file->open(QIODevice::ReadOnly);
    QTextStream fileInput(file);

    //Storing each line in the XML file in the QString private member.
    while(!fileInput.atEnd())
    {
        fileText.append(fileInput.readLine());
    }
    file->close();
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
    /*
     * TO DO:
     * In case of having Info Tags at the start of the XML file:
     *  ---> Info Tags has no children, so we need to create a seperate root for it.
     *  ---> we have 2 info tags, each will be a separate block.
     *  ---> Let xmlRoot starts from (*separateTag)[2]
     *  ---> Looping starts from i = 3.
     */

    xmlRoot = new TagBlock((*separateTag)[0], TagType::OPEN_TAG);
    QStack<MainBlock* > stack;
    stack.push(xmlRoot);

    for(int i = 1; i < separateTag->length(); i++)
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
               stack.top()->getInternalBlocks()->push_back(new TagBlock("", TagType::TAG_CONTENT));
            }
            stack.top()->getInternalBlocks()->push_back(new TagBlock(currentTag, TagType::CLOSED_TAG));
            stack.pop();
        }
        //Indicates an open tag.
        else if(currentTag[0] == '<')
        {
            MainBlock* newBlock = new TagBlock(currentTag, TagType::OPEN_TAG);

            //Push back the new created block into the internal blocks of the parent block.
            stack.top()->getInternalBlocks()->push_back(newBlock);

            //Pushing the new created block into the stack.
            stack.push(newBlock);
        }
        //In case of having a text tag.
        else {
            stack.top()->getInternalBlocks()->push_back(new TagBlock(currentTag, TagType::TAG_CONTENT));
        }
    }
}

QString XMLTree::convertXMLFileIntoJSONFile()
{
    generateJSONFile();
    return JSONFile;
}

void XMLTree::generateJSONFile()
{
    MainBlock* jsonRoot = xmlRoot;
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
}

void XMLTree::generateJSONObject(MainBlock *currentBlock, bool isLastBlock)
{
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
            indentCounter += 3;

            //Inserting start tag attributes in the JSON file.
            auto mapIterator = startTagAttributes.constBegin();
            while(mapIterator != startTagAttributes.constEnd())
            {
                JSONFile.append("\n");
                generateIndentationForJSON();
                JSONFile.append("\"@");
                JSONFile.append(mapIterator.key());
                JSONFile.append("\": \"");
                JSONFile.append(mapIterator.value());
                JSONFile.append("\",\n");
                mapIterator++;
            }

            //Inserting the text tag in the JSON file.
            generateIndentationForJSON();

            JSONFile.append("\"#text\": \"");
            JSONFile.append((*internalBlocks)[0]->getBlockContent());
            JSONFile.append("\"\n");

            indentCounter -= 3;
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
    indentCounter += 3;

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

        mapIterator++;
    }

    for(int i = 0; i < internalBlocks->size(); i++)
    {
        //TO DO, may be xmlRoot->getInternalBlocks()->size() - 2 (the previous block of the last one)
        if(i == internalBlocks->size() - 1)
        {
            generateJSONObject((*internalBlocks)[i], true);
        }
        else {
            generateJSONObject((*internalBlocks)[i], false);
        }
    }

    indentCounter -= 3;
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

QStringList XMLTree::breakingStartTagIntoParts(QString startTag)
{
    QStringList tagParts;
    QString currentPart = "";
    bool isAttribute = false;
    QChar currentCharacter;
    for(int i = 1 ; i < startTag.length(); i++)
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
            tagParts.push_back(currentPart);
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

void XMLTree::generateIndentationForJSON()
{
    for (int i = 0; i < indentCounter; i++)
    {
        JSONFile.append(' ');
    }
}


QString XMLTree::prettifyingXMLTreeFile(MainBlock *root, int &spacesNum, QString &outputFile)
{
    outputFile += root->getBlockContent().rightJustified(spacesNum + root->getBlockContent().length(), ' ');

    //Getting the children of the root node.
    QVector<MainBlock*> *internalBlocks = root->getInternalBlocks();

    if(root->getInternalBlocks()->size() == 2)
    {
        outputFile += (*root->getInternalBlocks())[0]->getBlockContent();
        outputFile += (*root->getInternalBlocks())[1]->getBlockContent();
        outputFile += "\n";
        return outputFile;
    }

    outputFile += "\n";

    if(root->getInternalBlocks()->empty()) {
        return outputFile;
    }

    spacesNum += 3;

    for(int i = 0; i < internalBlocks->size(); i++)
    {
        if(i == internalBlocks->size() - 1) {
            spacesNum -= 3;
        }
        MainBlock* blockChild = (*internalBlocks)[i];
        prettifyingXMLTreeFile(blockChild, spacesNum, outputFile);
    }
    return outputFile;
}

QString XMLTree::minfyingXMLTreeFile(MainBlock *root, QString &outputFile)
{
    outputFile += root->getBlockContent();

    if(root->getInternalBlocks()->empty()) {
        return outputFile;
    }

    //Getting the children of the root node.
    QVector<MainBlock*> *internalBlocks = root->getInternalBlocks();

    for(int i = 0; i < internalBlocks->size(); i++)
    {
        MainBlock* blockChild = (*internalBlocks)[i];
        minfyingXMLTreeFile(blockChild, outputFile);
    }
    return outputFile;
}

MainBlock* XMLTree::getXMLFileRoot()
{
    return xmlRoot;
}

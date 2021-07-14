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
    JSONFile.append('{');
    JSONFile.append('\n');
    indentCounter += 3;

    generateJSONObject(xmlRoot, false);

    indentCounter -= 3;
    JSONFile.append('}');
    JSONFile.append('\n');
    return JSONFile;
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

QString XMLTree::error_checking(QString &outputFile)
{
    QVector<QString> *tags = breakingFileTextIntoTags();
    QStack<QString> s;
    QStack<quint8> index;

    for (int i=0; i<tags->length(); i++)
    {
        QString tag = (*tags)[i];
        bool flag = 0;

        //opening tag case
        if (tag[0] == '<' && tag[1] != '/')
        {
            //check if stack is empty
            if (! s.isEmpty())
            {
                QString top = s.pop();
                quint8 topi = index.pop();
                
                /*check if there's text before the opening tag 
                then this text has a missing closing tag*/ 
                if (top[0] != '<')
                {
                   s.pop();
                   index.pop();
                   (*tags)[topi] = top + " ### error,close tag missing ###";
                }
                
                /*if there's another tag before the opening tag
                then just push the opening tag*/
                else
                {
                    s.push(top);
                    index.push(topi);
                }
            }
            
            //special tag: frame ==> don't push in stack as it has no closing tag
            if (tag.mid(1, 5) != "frame")
            {
                s.push(tag);
                index.push(i);
            }
        }

        //closing tag case
        else if (tag[0] == '<' && tag[1] == '/')
        {
            //check if stack is empty
            if (! s.isEmpty())
            {
                QString top = s.pop();
                quint8 topi = index.pop();
                
                /*if stack is not empty then check if the top of the stack is text 
                then pop it and get the new top which should be an opening tag*/
                if (top[0] != '<')
                {
                    if (! s.isEmpty())
                    {
                        top = s.pop();
                        topi = index.pop();
                    }
                    
                    //if there's nothing before this text then there's an opening tag missing
                    else
                        (*tags)[topi] = top + " ### error,open tag missing ###";
                }

                //compare the closing tag with the opening tag popped from the stack
                QString str1 = tag.mid(2, tag.length()-3);
                QString str2 = top.mid(1, tag.length()-3);
                
                /*if both tags don't match then check if the error is 
                mismatching tags or missing tags*/
                if (str1 != str2)
                {
                    QString temp = (*tags)[i-1];
                    
                    /*if the closing tag is preceeded by another closing tag
                    then there's an opening or closing tag missing*/
                    if (temp[0] == '<' && temp[1] == '/')
                    {
                        for (int j=i+1; j<tags->length(); j++)
                        {
                            temp = (*tags)[j];
                            
                            /*compare the opening tag in top of the stack with the remaining tags
                            in the file, if there's another matching opening tag following 
                            then the closing tag of this opening tag(in top of the stack) is missing*/
                            if (temp[0] == '<' && temp[1] != '/')
                            {
                                str1 = temp.mid(1, temp.length()-3);
                                str2 = top.mid(1, temp.length()-3);
                                if (str1 == str2)
                                {
                                    (*tags)[topi] = top + " ### error,close tag missing ###";
                                    flag = 1;
                                    i--;
                                    break;
                                }
                            }
                    
                            /*if there's another matching closing tag following 
                            then the opening tag of the current closing tag is missing*/
                            else if (temp[0] == '<' && temp[1] == '/')
                            {
                                str1 = temp.mid(2, temp.length()-3);
                                str2 = top.mid(1, temp.length()-3);
                                if (str1 == str2)
                                {
                                    (*tags)[i] = tag + " ### error,open tag missing ###";
                                    s.push(top);
                                    index.push(topi);
                                    flag = 1;
                                    break;
                                }
                            }
                        }
                        
                        /*if no matching tags exist at all
                        then the closing tag of this opening tag(in top of the stack) is missing*/
                        if (flag == 0)
                        {
                            (*tags)[topi] = top + " ### error,close tag missing ###";
                            i--;
                        }
                        else
                            flag = 0;
                    }

                    /*if the closing tag is preceeded by a text
                    then there's an opening tag missing or mismatching tags exist*/
                    else
                    {
                        temp = (*tags)[i-2];
                        if (temp[0] == '<' && temp[1] != '/')
                        {
                            temp = (*tags)[i+1];
                            
                            if (temp[0] == '<' && temp[1] == '/')
                            {
                                str1 = temp.mid(2, temp.length()-3);
                                str2 = top.mid(1, temp.length()-3);
                                
                                /*if the closing tag is followed by another closing tag
                                and it matches the opening tag in top of the stack
                                then the current closing tag has a missing opening tag*/
                                if (str1 == str2)
                                {
                                    (*tags)[i-1] = (*tags)[i-1] + " ### error,open tag missing ###";
                                    i++;
                                }
                                
                                /*if the closing tag is followed by another closing tag
                                and it doesn't match the opening tag in top of the stack
                                then there's mismatching tags*/
                                else
                                    (*tags)[i] = tag + " ### error,mismatching ###";
                            }
                            
                            /*if the closing tag is followed by an opening tag or text
                            then there's mismatching tags*/
                            else
                                (*tags)[i] = tag + " ### error,mismatching ###";
                        }
                    }
                }
            }
            
            //if stack is empty then the opening tag of the current closing tag is missing
            else
                (*tags)[i] = tag + " ### error,open tag missing ###";
        }

        //text case
        else
        {
            //check if stack is empty
            if (! s.isEmpty())
            {
                QString temp1 = (*tags)[i-1];
                QString temp2 = (*tags)[i+1];
                if (temp1[0] == '<' && temp1[1] != '/')
                {
                    if (temp2[0] == '<' && temp2[1] == '/')
                    {
                        s.push(tag);
                        index.push(i);
                    }
                }
                else
                {
                    /*if there's text not preceeded by opening tag but followed by closing tag
                    then its opening tag is missing*/
                    if (temp2[0] == '<' && temp2[1] == '/')
                    {
                        (*tags)[i] = tag + " ### error,open tag missing ###";
                        i++;
                    }
                }
            }
            /*if there's text and stack is empty (no opening tags)
            then the text's opening tag is missing*/
            else
                (*tags)[i] = tag + " ### error,open tag missing ###";
        }
    }

    //if tags are over and stack is not empty then there are missing closing tags
    while (! s.isEmpty())
        (*tags)[index.pop()] = s.pop() + " ### error,close tag missing ###";

    //organizing the output string and adding necessary spaces
    for (int i=0; i<tags->length(); i++)
    {
        if (i==0)
            outputFile += (*tags)[i];
        else
        {
            QString temp = (*tags)[i];
            if (temp[0] == '<' && temp[0] == '/')
            {
                outputFile = outputFile.left(outputFile.length()-1);
                outputFile += (*tags)[i];
            }
            else
            {
                outputFile += '     ';
                outputFile += (*tags)[i];
            }
        }
        outputFile += '\n';
    }

    return outputFile;
}

QString XMLTree::error_correction(QString &outputFile)
{
    QVector<QString> *tags = breakingFileTextIntoTags();
    QStack<QString> s;
    QStack<quint8> index;
    QVector<QString> *output = new QVector<QString>();

    for (int i=0; i<tags->length(); i++)
        output->push_back((*tags)[i]);

    for (int i=0; i<tags->length(); i++)
    {
        QString tag = (*tags)[i];
        bool flag = 0;

        //opening tag case
        if (tag[0] == '<' && tag[1] != '/')
        {
            if (! s.isEmpty())
            {
                QString top = s.pop();
                quint8 topi = index.pop();
                if (top[0] != '<')
                {
                   s.pop();
                   index.pop();
                   QString temp = (*tags)[topi-1];
                   quint8 length = temp.length()-1;
                   for (int k=0; k<temp.length(); k++)
                       if (temp[k] == ' ' && k != 1)
                           length = k;
                   
                   //adding the missing closing tag
                   (*output)[topi] = top + temp.mid(0, 1) + "/" + temp.mid(1, length-1) + ">";
                }
                else
                {
                    s.push(top);
                    index.push(topi);
                }
            }
            if (tag.mid(1, 5) != "frame")
            {
                s.push(tag);
                index.push(i);
            }
        }
        //closing tag case
        else if (tag[0] == '<' && tag[1] == '/')
        {
            if (! s.isEmpty())
            {
                QString top = s.pop();
                quint8 topi = index.pop();
                if (top[0] != '<')
                {
                    if (! s.isEmpty())
                    {
                        top = s.pop();
                        topi = index.pop();
                    }
                    else
                    {
                        //adding the missing opening tag
                        (*output)[topi] = tag.mid(0, 1) + tag.mid(2, tag.length()-2) + top;
                    }
                }
                QString str1 = tag.mid(2, tag.length()-3);
                QString str2 = top.mid(1, tag.length()-3);
                if (str1 != str2)
                {
                    QString temp = (*tags)[i-1];
                    
                    //open or close tag missing
                    if (temp[0] == '<' && temp[1] == '/')
                    {
                        for (int j=i+1; j<tags->length(); j++)
                        {
                            temp = (*tags)[j];
                            if (temp[0] == '<' && temp[1] != '/')
                            {
                                str1 = temp.mid(1, temp.length()-3);
                                str2 = top.mid(1, temp.length()-3);
                                if (str1 == str2)
                                {
                                    quint8 length = top.length()-1;
                                    for (int k=0; k<top.length(); k++)
                                        if (top[k] == ' ' && k != 1)
                                            length = k;
                                    
                                    //adding the missing closing tag
                                    (*output)[i] = top.mid(0, 1) + "/" + top.mid(1, length-1) + ">" +tag;
                                    flag = 1;
                                    i--;
                                    break;
                                }
                            }
                            else if (temp[0] == '<' && temp[1] == '/')
                            {
                                str1 = temp.mid(2, temp.length()-3);
                                str2 = top.mid(1, temp.length()-3);
                                if (str1 == str2)
                                {
                                    //adding the missing opening tag
                                    (*output)[topi] = top + tag.mid(0, 1) + tag.mid(2, tag.length()-2);
                                    s.push(top);
                                    index.push(topi);
                                    flag = 1;
                                    break;
                                }
                            }
                        }
                        if (flag == 0)
                        {
                            quint8 length = top.length()-1;
                            for (int k=0; k<top.length(); k++)
                                if (top[k] == ' ' && k != 1)
                                    length = k;

                            //adding the missing closing tag
                            (*output)[i] = top.mid(0, 1) + "/" + top.mid(1, length-1) + ">" + tag;
                            i--;
                        }
                        else
                            flag = 0;
                    }

                    //open tag missing or mismatching
                    else
                    {
                        temp = (*tags)[i-2];
                        if (temp[0] == '<' && temp[1] != '/')
                        {
                            temp = (*tags)[i+1];
                            if (temp[0] == '<' && temp[1] == '/')
                            {
                                str1 = temp.mid(2, temp.length()-3);
                                str2 = top.mid(1, temp.length()-3);
                                if (str1 == str2)
                                {
                                    //adding the missing opening tag
                                    (*output)[topi] = top + tag.mid(0, 1) + tag.mid(2, tag.length()-2);
                                    i++;
                                }
                                else
                                {
                                    quint8 length = top.length()-1;
                                    for (int k=0; k<top.length(); k++)
                                        if (top[k] == ' ' && k != 1)
                                            length = k;
                                    //making the tags match
                                    (*output)[i] = top.mid(0, 1) + "/" + top.mid(1, length-1) + ">";
                                }
                            }
                            else
                            {
                                quint8 length = top.length()-1;
                                for (int k=0; k<top.length(); k++)
                                    if (top[k] == ' ' && k != 1)
                                        length = k;
                                //making the tags match
                                (*output)[i] = top.mid(0, 1) + "/" + top.mid(1, length-1) + ">";
                            }
                        }
                    }
                }
            }
            else
            {
                //adding the missing opening tag
                (*output)[0] = tag.mid(0, 1) + tag.mid(2, tag.length()-2);
            }
        }
        //text case
        else
        {
            if (! s.isEmpty())
            {
                QString temp1 = (*tags)[i-1];
                QString temp2 = (*tags)[i+1];
                if (temp1[0] == '<' && temp1[1] != '/')
                {
                    if (temp2[0] == '<' && temp2[1] == '/')
                    {
                        s.push(tag);
                        index.push(i);
                    }
                    else
                    {
                        QString temp = (*tags)[i-1];
                        quint8 length = temp.length()-1;
                            for (int k=0; k<temp.length(); k++)
                                if (temp[k] == ' ' && k != 1)
                                    length = k;
                        
                        //adding the missing closing tag
                        (*output)[i] = tag + temp.mid(0, 1) + '/' + temp.mid(1, length-1) + ">";
                        s.pop();
                        index.pop();
                    }
                }
                else
                {
                    if (temp2[0] == '<' && temp2[1] == '/')
                    {
                        QString temp = (*tags)[i+1];
                        
                        //adding the missing opening tag
                        (*output)[i] = temp.mid(0, 1) + temp.mid(2, temp.length()-2) + tag;
                        i++;
                    }
                }
            }
            else
            {
                QString temp = (*tags)[i+1];

                //adding the missing opening tag
                (*output)[i] = temp.mid(0, 1) + temp.mid(2, temp.length()-2) + tag;
            }
        }
    }

    //adding the missing closing tag in the end of the file
    while (! s.isEmpty())
    {
        QString temp = s.pop();
        quint8 length = temp.length()-1;
            for (int k=0; k<temp.length(); k++)
                if (temp[k] == ' ' && k != 1)
                    length = k;
        output->push_back(temp.mid(0, 1) + "/" + temp.mid(1, length-1) + ">");
    }

     //constructing the output string
    for (int i=0; i<output->length(); i++)
    {
        if (i==0)
            outputFile += (*output)[i];
        else
        {
            QString temp = (*output)[i];
            if (temp[0] == '<' && temp[0] == '/')
            {
                outputFile = outputFile.left(outputFile.length()-1);
                outputFile += (*output)[i];
            }
            else
            {
                outputFile += '     ';
                outputFile += (*output)[i];
            }
        }
        outputFile += '\n';
    }

    return outputFile;
}
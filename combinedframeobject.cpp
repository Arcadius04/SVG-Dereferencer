#include "combinedframeobject.h"

CombinedFrameObject::CombinedFrameObject() {}

QString CombinedFrameObject::getCurrentDirectory(){
    return this->currentDirectory;
}
QString CombinedFrameObject::setCurrentDirectory(QString currentDirectory){
    this->currentDirectory = currentDirectory;
    return this->currentDirectory;
}

void CombinedFrameObject::outputSvg(QString filePath){
    QFile file(filePath);
    if(!file.exists()){
        //qDebug() << "File at '" << filePath << "' does not exist!";
        return;
    }
    if(!file.open(QFile::ReadOnly)){
        //qDebug() << "File at '" << filePath << "' could not be read!";
        return;
    }
    //qDebug() << filePath << ": " << file.readAll();
    file.close();
    return;
}

void CombinedFrameObject::setDefinitionsDocument(QDomDocument* definitions){
    this->definitions = definitions;
}
QDomDocument* CombinedFrameObject::getDefinitionsDecoument(){
    return definitions;
}

void CombinedFrameObject::processSvg(QString filePath){
    firstElement = true;
    QFile file(filePath);
    if(!file.exists()){
        //qDebug() << "File at '" << filePath << "' does not exist!";
        return;
    }
    if(!file.open(QFile::ReadOnly)){
        //qDebug() << "File at '" << filePath << "' could not be read!";
        return;
    }

    QDomDocument doc;
    doc.setContent(file.readAll());
    file.close();

    QDomElement docRoot = doc.documentElement();

    QDomDocument* finalized = new QDomDocument;
    finalized->setContent("<g></g>");
    QDomElement parentElement = finalized->documentElement();

    /*// SET THE ROOT TAG (<SVG></SVG>) ATTRIBUTES
    QDomNamedNodeMap docRootAttr = docRoot.attributes();
    for(int i = 0; i < docRootAttr.count(); i++){
        QDomNode attr = docRootAttr.item(i);
        if(attr.nodeName().toLower() == "xmlns"){//attr.nodeName().toLower() == "height" || attr.nodeName().toLower() == "width" ||
            parentElement.setAttribute(attr.nodeName(),attr.nodeValue());
        }
    }*/
    parentElement.setAttribute("height","100.0px");
    parentElement.setAttribute("width","100.0px");
    parentElement.setAttribute("id","frame-" + QString::number(frameID)+"-"+QString::number(currentSpriteIndex));

    if(docRoot.hasChildNodes()){
        QDomElement childElement = docRoot.firstChildElement();
        recursiveSvg(finalized, parentElement, childElement);
    }
    docs.insert(QString::number(currentSpriteIndex), finalized);
    return;
}

void CombinedFrameObject::dereferenceSvg(QString filePath){
    int currentMax = maxSprites;
    processSvg(filePath);
    if(currentMax != maxSprites){
        for(int i = currentSpriteIndex; i <= maxSprites; i++){
            currentSpriteIndex++;
            processSvg(filePath);
        }
    }
}

void CombinedFrameObject::recursiveSvg(QDomDocument* ownerDoc, QDomElement& parentElement, QDomElement& element){
    int reservedId = -1;
    while(!element.isNull()){
        QDomElement nextSibling = element.nextSiblingElement();
        if(element.tagName() == "clipPath"){
            QDomElement newElement = ownerDoc->createElement("clipPath");
            QDomNamedNodeMap docElementAttr = element.attributes();
            for(int i = 0; i < docElementAttr.count(); i++){
                QDomNode attr = docElementAttr.item(i);
                if(attr.nodeName().contains("id",Qt::CaseInsensitive)){
                    reservedId = clipIdIndex;
                    newElement.setAttribute("id","clip-" + QString::number(reservedId));
                    clipIdIndex++;
                } else if(!attr.nodeName().contains("ffdec",Qt::CaseInsensitive) && !attr.nodeName().contains("xlink",Qt::CaseInsensitive)){
                    newElement.setAttribute(attr.nodeName(),attr.nodeValue());
                }
            }
            parentElement.appendChild(newElement);
            if (element.hasChildNodes()) {
                QDomElement child = element.firstChildElement();
                recursiveSvg(ownerDoc, newElement, child);
            }
        }
        if(element.tagName().toLower() == "g"){
            QDomElement newElement = ownerDoc->createElement("g");
            QDomNamedNodeMap docElementAttr = element.attributes();
            for(int i = 0; i < docElementAttr.count(); i++){
                QDomNode attr = docElementAttr.item(i);
                if(attr.nodeName().contains("transform",Qt::CaseInsensitive) && firstElement){
                    QStringList matrix = attr.nodeValue().replace("matrix(","").replace(")","").split(", ");
                    matrix[4] = QString::number(matrix[4].toDouble()+50);
                    matrix[5] = QString::number(matrix[5].toDouble()+50);
                    newElement.setAttribute("transform","matrix("+matrix[0]+", "+matrix[1]+", "+matrix[2]+", "+matrix[3]+", "+matrix[4]+", "+matrix[5]+")");
                    firstElement = false;
                } else if(attr.nodeName().contains("clip-path",Qt::CaseInsensitive)){
                    newElement.setAttribute("clip-path","clip-" + QString::number(reservedId));
                } else if(!attr.nodeName().contains("ffdec",Qt::CaseInsensitive) && !attr.nodeName().contains("xlink",Qt::CaseInsensitive)){
                    newElement.setAttribute(attr.nodeName(),attr.nodeValue());
                }
            }
            parentElement.appendChild(newElement);
            if (element.hasChildNodes()) {
                QDomElement child = element.firstChildElement();
                recursiveSvg(ownerDoc, newElement, child);
            }
        }
        if(element.tagName().toLower() == "use"){
            QDomElement newElement = ownerDoc->createElement("use");
            TYPE type = NONE;
            int ID = -1;
            QDomNamedNodeMap docElementAttr = element.attributes();
            for(int i = 0; i <= docElementAttr.count(); i++){
                QDomNode attr = docElementAttr.item(i);
                //qDebug() << attr.nodeName() << attr.nodeValue();
                if(!attr.nodeName().isEmpty() && !attr.nodeValue().isEmpty() && !attr.nodeName().contains("ffdec",Qt::CaseInsensitive) && !attr.nodeName().contains("xlink",Qt::CaseInsensitive) && !attr.nodeName().endsWith("id",Qt::CaseInsensitive)){
                    newElement.setAttribute(attr.nodeName(),attr.nodeValue());
                } else if(attr.nodeName().toLower() == "xlink:href" && attr.nodeValue().toLower().contains("shape")){
                    type = SHAPE;
                } else if(attr.nodeName().toLower() == "xlink:href" && attr.nodeValue().toLower().contains("sprite")){
                    type = SPRITE;
                } else if(attr.nodeName().toLower() == "ffdec:characterid"){
                    ID = attr.nodeValue().toInt();
                }
            }
            if(type == SHAPE || type == SPRITE){
                if(ID != -1){
                    if(type == SHAPE){
                        newElement.setAttribute("href","#shape-"+QString::number(ID));
                    }else if(type == SPRITE){
                        newElement.setAttribute("href","#sprite-"+QString::number(ID)+"-"+QString::number(currentSpriteIndex));
                    }
                }
                QString matrix = newElement.attribute("transform");
                //newElement.removeAttribute("transform");
                /*
                QString width = newElement.attribute("width");
                newElement.removeAttribute("width");
                QString height = newElement.attribute("height");
                newElement.removeAttribute("height");*/
                parentElement.appendChild(newElement.cloneNode(true));
                newElement.removeAttribute("transform");
                //newElement.setAttribute("width",width);
                //newElement.setAttribute("height",height);
                newElement.removeAttribute("href");
                if(ID != -1){
                    if(type == SHAPE){
                        newElement.setAttribute("id","shape-"+QString::number(ID));
                    }else if(type == SPRITE){
                        newElement.setAttribute("id","sprite-"+QString::number(ID)+"-"+QString::number(currentSpriteIndex));
                    }
                }
                QString path = currentDirectory;
                if(type == SHAPE){
                    path += "/shapes";
                    QString string = QString::number(ID)+".svg";
                    QFile file(path+"/"+string);
                    if(file.exists()){
                        if(file.open(QFile::ReadOnly)){
                            QDomDocument doc;
                            doc.setContent(file.readAll());
                            file.close();
                            QDomElement docRoot = doc.documentElement();
                            if (docRoot.hasChildNodes()) {
                                QDomElement child = docRoot.firstChildElement();
                                QDomElement e = definitions->documentElement();
                                bool alreadyInDefinition = false;
                                if(addedToDefinition.contains(newElement.attribute("id"))){
                                    alreadyInDefinition = true;
                                }
                                if(alreadyInDefinition == false){
                                    addedToDefinition.append(newElement.attribute("id"));
                                    newElement.setTagName("g");
                                    e.appendChild(newElement);
                                    recursiveSvg(definitions, newElement, child);
                                }
                            }
                        }else{
                            qDebug() << "File at '" << path << "' could not be read!";
                        }
                    }else{
                        qDebug() << "File at '" << path << "' does not exist!";
                    }
                } else if(type == SPRITE){
                    path += "/sprites/DefineSprite_" + QString::number(ID);
                    QDir dir(path);
                    if(dir.exists()){
                        bool found = false;
                        QCollator col;
                        col.setNumericMode(true);
                        QStringList list = dir.entryList(QDir::Files);
                        if(list.count() > maxSprites){
                            maxSprites = list.count();
                        }
                        int list_size = list.count();
                        std::sort(list.begin(),list.end(),col);
                        QString target = QString::number(currentSpriteIndex)+".svg";
                        for(int i = list.count()-1; i >= 0; i--){
                            QString string = list.at(i);
                            if(found){
                                break;
                            }
                            if(string == target || ((i+1) < currentSpriteIndex)){
                                QFile file(path+"/"+string);
                                if(file.open(QFile::ReadOnly)){
                                    QDomDocument doc;
                                    doc.setContent(file.readAll());
                                    file.close();
                                    QDomElement docRoot = doc.documentElement();
                                    if (docRoot.hasChildNodes()) {
                                        QDomElement child = docRoot.firstChildElement();
                                        QDomElement e = definitions->documentElement();
                                        QDomNodeList nodeList = e.elementsByTagName("g");
                                        bool alreadyInDefinition = false;
                                        if(addedToDefinition.contains(newElement.attribute("id"))){
                                            alreadyInDefinition = true;
                                        }
                                        if(alreadyInDefinition == false){
                                            addedToDefinition.append(newElement.attribute("id"));
                                            newElement.setTagName("g");
                                            e.appendChild(newElement);
                                            recursiveSvg(definitions, newElement, child);
                                        }
                                    }
                                    found = true;
                                }else{
                                    qDebug() << "File at '" << path << "' could not be read!";
                                }
                            }
                        }
                        if(!found) {
                            qDebug() << "Sorry, didn't match " << path << ", for ID " << ID;
                        }
                    }else{
                        qDebug() << "File at '" << path << "' does not exist!";
                    }
                }
            }
        }
        if(element.tagName().toLower() == "path"){
            parentElement.appendChild(element);
        }
        element = nextSibling;
    }
}

QMap<QString,QDomDocument*>& CombinedFrameObject::getMap(){
    return this->docs;
}

int CombinedFrameObject::getID(){
    return this->frameID;
}

int CombinedFrameObject::setID(int frameID){
    this->frameID = frameID;
    return this->frameID;
}

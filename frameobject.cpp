#include "frameobject.h"

FrameObject::FrameObject() {}

QString FrameObject::getCurrentDirectory(){
    return this->currentDirectory;
}
QString FrameObject::setCurrentDirectory(QString currentDirectory){
    this->currentDirectory = currentDirectory;
    return this->currentDirectory;
}

void FrameObject::outputSvg(QString filePath){
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

void FrameObject::saveFile(QString directoryOutput){
    QDir dir(directoryOutput);
    if(!dir.exists()){
        //qDebug() << "Directory does not exists at '" << directoryOutput << "', creating!";
        QDir().mkpath(directoryOutput);
        dir.setPath(directoryOutput);
        //qDebug() << "Successfully created directory at '" << directoryOutput << "'!";
    }
    for(auto i = docs.begin(); i != docs.end(); i++){
        QStringList list = i.key().split("/");
        dir.mkdir(list[0]);

        QFile file(directoryOutput + "/" + i.key());
        if(file.open(QFile::ReadWrite)){
            //qDebug() << "Writing file at '" + directoryOutput + "/" + i.key() + "'!";
            file.write(i.value()->toByteArray());
            file.close();
            qDebug() << "Successfully wrote and closed file at '" + directoryOutput + "/" + i.key() + "'!";
        }else{
            qDebug() << "Error opening file, Error code: " + file.errorString();
        }
    }

}

void FrameObject::dereferenceSvg(QString filePath){
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
    finalized->setContent("<svg></svg>");
    QDomElement parentElement = finalized->documentElement();

    // SET THE ROOT TAG (<SVG></SVG>) ATTRIBUTES
    QDomNamedNodeMap docRootAttr = docRoot.attributes();
    for(int i = 0; i < docRootAttr.count(); i++){
        QDomNode attr = docRootAttr.item(i);
        if(attr.nodeName().toLower() == "height" || attr.nodeName().toLower() == "width" || attr.nodeName().toLower() == "xmlns"){
            parentElement.setAttribute(attr.nodeName(),attr.nodeValue());
        }
    }

    if(docRoot.hasChildNodes()){
        QDomElement childElement = docRoot.firstChildElement();
        recrusiveSvg(finalized, parentElement, childElement);
    }
    return;
}

void FrameObject::recrusiveSvg(QDomDocument* ownerDoc, QDomElement& parentElement, QDomElement& element){
    while(!element.isNull()){
        if(element.tagName().toLower() == "g"){
            QDomElement newElement = ownerDoc->createElement("g");
            QDomNamedNodeMap docElementAttr = element.attributes();
            for(int i = 0; i < docElementAttr.count(); i++){
                QDomNode attr = docElementAttr.item(i);
                if(attr.nodeName().toLower() == "height" || attr.nodeName().toLower() == "width" || attr.nodeName().toLower() == "transform"){
                    newElement.setAttribute(attr.nodeName(),attr.nodeValue());
                }
            }
            parentElement.appendChild(newElement);
            if(element.childNodes().count() > 0){
                for(int i = 0; i < element.childNodes().count(); i++){
                    QDomElement childElement = element.childNodes().at(i).toElement();
                    recrusiveSvg(ownerDoc,newElement, childElement);
                }
            }
        }
        if(element.tagName().toLower() == "use"){
            QDomElement newElement = ownerDoc->createElement("g");
            TYPE type = NONE;
            int ID = -1;
            QDomNamedNodeMap docElementAttr = element.attributes();
            for(int i = 0; i < docElementAttr.count(); i++){
                QDomNode attr = docElementAttr.item(i);
                if(attr.nodeName().toLower() == "height" || attr.nodeName().toLower() == "width" || attr.nodeName().toLower() == "transform"){
                    newElement.setAttribute(attr.nodeName(),attr.nodeValue());
                } else if(attr.nodeName().toLower() == "xlink:href" && attr.nodeValue().toLower().contains("shape")){
                    type = SHAPE;
                } else if(attr.nodeName().toLower() == "xlink:href" && attr.nodeValue().toLower().contains("sprite")){
                    type = SPRITE;
                } else if(attr.nodeName().toLower() == "ffdec:characterid"){
                    ID = attr.nodeValue().toInt();
                }
            }
            if(ID != -1){
                if(type == SHAPE){
                    newElement.setAttribute("id","shape-"+QString::number(ID));
                }else if(type == SPRITE){
                    newElement.setAttribute("id","sprite-"+QString::number(ID));
                }
            }
            QString path = currentDirectory;
            if(type == SHAPE){
                parentElement.appendChild(newElement);
                path += "/shapes";
                QString string = QString::number(ID)+".svg";
                QFile file(path+"/"+string);
                if(file.exists()){
                    if(file.open(QFile::ReadOnly)){
                        QDomDocument doc;
                        doc.setContent(file.readAll());
                        file.close();
                        QDomElement docRoot = doc.documentElement();
                        if(docRoot.hasChildNodes()){
                            QDomElement docElement = docRoot.firstChildElement();
                            recrusiveSvg(ownerDoc,newElement,docElement);
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
                    std::sort(list.begin(),list.end(),col);
                    for(QString string : list){
                        QFile file(path+"/"+string);
                        if(file.open(QFile::ReadOnly)){
                            QDomDocument doc;
                            doc.setContent(file.readAll());
                            file.close();
                            QDomElement docRoot = doc.documentElement();
                            if(docRoot.hasChildNodes()){
                                QDomElement docElement = docRoot.firstChildElement();

                                QDomDocument* duplicateDoc = new QDomDocument();
                                QDomElement duplicateDocElement = duplicateDoc->importNode(ownerDoc->documentElement(),true).toElement();
                                duplicateDoc->appendChild(duplicateDocElement);

                                docs.insert(QString::number(frameID)+"/"+string,duplicateDoc);
                                QDomElement duplicateNewElem = duplicateDoc->importNode(newElement,true).toElement();
                                duplicateDocElement.appendChild(duplicateNewElem);

                                recrusiveSvg(duplicateDoc,duplicateNewElem,docElement);
                            }
                            found = true;
                        }else{
                            qDebug() << "File at '" << path << "' could not be read!";
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
        if(element.tagName().toLower() == "path"){
            parentElement.appendChild(element.cloneNode(true));
        }
        element = element.nextSiblingElement();
    }
}

QMap<QString,QDomDocument*>& FrameObject::getMap(){
    return this->docs;
}

int FrameObject::getID(){
    return this->frameID;
}

int FrameObject::setID(int frameID){
    this->frameID = frameID;
    return this->frameID;
}

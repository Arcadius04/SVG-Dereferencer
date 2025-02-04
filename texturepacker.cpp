#include "texturepacker.h"

TexturePacker::TexturePacker() {}


QString TexturePacker::setDirectory(QString dir){
    this->directory = dir;
    return this->directory;
}

QString TexturePacker::getDirectory(){
    return this->directory;
}

QString TexturePacker::setOutputDirectory(QString dir){
    this->outputDirectory = dir;
    return this->outputDirectory;
}

QString TexturePacker::getOutputDirectory(){
    return this->outputDirectory;
}


QString TexturePacker::setOutputFileName(QString name){
    this->outputFileName = name;
    return this->outputFileName;
}
QString TexturePacker::getOutputFileName(){
    return this->outputFileName;
}

void TexturePacker::dereferenceSVG(){
    if(directory.isEmpty()){
        qDebug() << "The directory path to be worked on is empty!";
        return;
    }
    frames.clear();
    int id = 1;

    QDir dir(directory + "/frames");
    if(!dir.exists()){
        qDebug() << "Directory does not exist!";
        return;
    }

    qDebug() << "Found directory!";

    QStringList list = dir.entryList(QDir::Files);
    QCollator col;
    col.setNumericMode(true);
    std::sort(list.begin(),list.end(),col);

    definitions.setContent("<defs></defs>");

    for(QString string : list){
        CombinedFrameObject frame;
        frame.setCurrentDirectory(directory);
        frame.setDefinitionsDocument(&definitions);
        frame.setID(id);
        frame.dereferenceSvg(dir.absolutePath()+"/"+string);
        frames.append(frame);
        id++;
    }
}

void TexturePacker::writeData(){
    svgDocByteArray.clear();
    QJsonArray framesArr;
    int width = 0;
    int height = 0;

    int r = 0;
    int c = 0;

    QByteArray tempArray;

    bool firstRow = true;

    for(CombinedFrameObject frame : frames){
        QJsonArray frameArr;
        QCollator col;
        col.setNumericMode(true);
        QList<QString> name = frame.getMap().keys();
        std::sort(name.begin(),name.end(),[&col](const QString &a, const QString &b) {
            return col.compare(a, b) < 0;
        });
        for(QString n : name){
            QDomDocument* d = frame.getMap().value(n);
            QJsonObject obj;
            obj["xmin"] = QString::number(c*spriteWidth);
            obj["xmax"] = QString::number((c*spriteWidth)+spriteWidth);
            obj["ymin"] = QString::number(r*spriteHeight);
            obj["ymax"] = QString::number((r*spriteHeight)+spriteHeight);
            QByteArray use = "<use id=\"" + d->documentElement().attribute("id").replace("g-","").toLocal8Bit() + "\" href=\"#" + d->documentElement().attribute("id").toLocal8Bit() + "\" width=\"100px\" height=\"100px\" x=\""+QByteArray().setNum(10)+"\" y=\""+ QByteArray().setNum(10) +"\"/>";
            //tempArray.append(use);
            tempArray.append(d->toByteArray(0));
            if(c >= (colMax-1)){
                firstRow = false;
                c = 0;
                r++;
                height += 100;
            }else{
                c++;
                if(firstRow){
                    width += 100;
                }
            }
            frameArr.append(QJsonObject{{n,QJsonValue(obj)}});
        }
        framesArr.append(QJsonObject{{QString::number(frame.getID()),QJsonValue(frameArr)}});
    }

    jsonDoc.setArray(framesArr);
    //QByteArray openingTag = "<svg width=\"" + QByteArray().setNum(width) + "px\" height=\"" + QByteArray().setNum(height) + "px\" xmlns=\"http://www.w3.org/2000/svg\">";
    //svgDocByteArray.append(openingTag);
    QByteArray openingTag = "<svg width=\"100px\" height=\"100px\" xmlns=\"http://www.w3.org/2000/svg\">";
    svgDocByteArray.append(openingTag);
    svgDocByteArray.append(tempArray);
    svgDocByteArray.append(definitions.toByteArray(0));
    QByteArray endTag = "</svg>";
    svgDocByteArray.append(endTag);
}

void TexturePacker::saveJsonFile(){
    if(jsonDoc.isNull()){
        qDebug() << "The JSON document you are trying to save is null!";
        return;
    }
    if(jsonDoc.isEmpty()){
        qDebug() << "The JSON document you are trying to save is empty!";
        return;
    }
    if(outputFileName.isEmpty()){
        qDebug() << "You must choose a file name for the output!";
        return;
    }
    QDir dir(outputDirectory);
    if(!dir.exists()){
        dir.mkpath(outputDirectory);
        qDebug() << "Output directory was not found, creating it!";
    }
    dir.mkpath(outputDirectory);
    QFile jsonFile(outputDirectory + "/" + outputFileName + ".json");
    if(jsonFile.open(QFile::WriteOnly)){
        qint64 len = jsonFile.write(jsonDoc.toJson());
        jsonFile.close();
        qDebug() << "Successfully wrote " << len << " bytes and closed file at '" + outputDirectory + "/" + outputFileName + ".json" + "'!";
    }else{
        qDebug() << "Error opening file, Error code: " + jsonFile.errorString();
    }
}

void TexturePacker::saveOutputSVG(){
    if(svgDocByteArray.isNull()){
        qDebug() << "The SVG file you are trying to save is null!";
        return;
    }
    if(svgDocByteArray.isEmpty()){
        qDebug() << "The SVG file you are trying to save is empty!";
        return;
    }
    if(outputFileName.isEmpty()){
        qDebug() << "You must choose a file name for the output!";
        return;
    }
    QDir dir(outputDirectory);
    if(!dir.exists()){
        dir.mkpath(outputDirectory);
        qDebug() << "Output directory was not found, creating it!";
    }
    QFile svgFile(outputDirectory + "/" + outputFileName + ".svg");
    if(svgFile.open(QFile::WriteOnly)){
        qint64 len = svgFile.write(svgDocByteArray,svgDocByteArray.size());
        svgFile.close();
        qDebug() << "Successfully wrote " << len << " bytes and closed file at '" + outputDirectory + "/" + outputFileName + ".svg" + "'!";
        return;
    }else{
        qDebug() << "Error opening file, Error code: " + svgFile.errorString();
        return;
    }
}

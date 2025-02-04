#include <QCoreApplication>

#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include "frameobject.h"
#include <QDomElement>
#include "texturepacker.h"
#include "combinedframeobject.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    /*QDir dir1("C:/Users/mfn45/OneDrive/Desktop/Clothings/1-499/sprite_export");
    if(!dir1.exists()){
        qDebug() << "Directory does not exist!";
        return -1;
    }
    qDebug() << "Found directory!";
    QStringList itemsFolders = dir1.entryList(QDir::Dirs);
    std::sort(itemsFolders.begin(),itemsFolders.end(),col);
    qDebug() << "Files found: " << itemsFolders.count();

    for(QString itemFolder : itemsFolders){
        if(itemFolder == "." || itemFolder == "..") continue;
        qDebug() << itemFolder;
        int id = 1;
        QDir dir2(dir1.absolutePath()+"/"+itemFolder+"/frames");
        QStringList list = dir2.entryList(QDir::Files);
        std::sort(list.begin(),list.end(),col);
        for(QString string : list){
            qDebug() << itemFolder << string;
            FrameObject frame;
            frame.setCurrentDirectory(dir1.absolutePath()+"/"+itemFolder+"");
            frame.setID(id);
            frame.dereferenceSvg(dir1.absolutePath()+"/"+itemFolder+"/frames/"+string);
            //frame.outputSvg(dir.absolutePath() + "/" + string);
            QString outputFile = itemFolder;
            outputFile = outputFile.replace("ClothingSprites","");
            frame.saveFile(dir1.absolutePath() + "/output/" + outputFile);
            for(auto i : frame.getMap().values()){
                delete i;
            }
            id++;
        }
    }*/

    QString directory = "C:/Users/mfn45/OneDrive/Desktop/penguin_old_export/";//C:/Users/mfn45/OneDrive/Desktop/Clothings/1-499/sprite_export

    TexturePacker tp;
    tp.setDirectory(directory);// + "112");
    tp.setOutputDirectory(directory + "/output");//"112/output");
    tp.setOutputFileName("penguin");
    tp.dereferenceSVG();
    tp.writeData();
    tp.saveOutputSVG();
    tp.saveJsonFile();


    //qDebug() << definitions->toString();

    /*QDir dir(currentDirectory + "/frames");
    if(!dir.exists()){
        qDebug() << "Directory does not exist!";
        return -1;
    }
    qDebug() << "Directory found!";

    col.setNumericMode(true);
    QStringList list = dir.entryList(QDir::Files);
    std::sort(list.begin(),list.end(),col);

    QList<FrameObject> frames;
    int id = 1;
    for(QString string : list){
        FrameObject frame;
        frame.setCurrentDirectory(currentDirectory);
        frame.setID(id);
        frame.dereferenceSvg(dir.absolutePath() + "/" + string);
        //frame.outputSvg(dir.absolutePath() + "/" + string);
        frame.saveFile(currentDirectory + "/output");
        frames.append(frame);
        id++;
    }

    FrameObject obj = frames.at(0);
    QList<QString> keys = obj.getMap().keys();
    std::sort(keys.begin(),keys.end(),col);
    for(int i = 0; i < keys.size(); i++){
        qDebug() << i << " ----> " << keys.at(i) << " ----> "<< obj.getMap().value(keys.at(i))->toString();
    }
    qDebug() << obj.getMap().size();*/
    return a.exec();
}

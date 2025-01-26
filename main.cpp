#include <QCoreApplication>

#include <QDir>
#include "frameobject.h"

const QString currentDirectory = "C:/Users/mfn45/OneDrive/Desktop/SVG-Combining-Test";

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QDir dir(currentDirectory + "/frames");
    if(!dir.exists()){
        qDebug() << "Directory does not exist!";
        return -1;
    }
    qDebug() << "Directory found!";

    QCollator col;
    col.setNumericMode(true);
    QStringList list = dir.entryList(QDir::Files);
    std::sort(list.begin(),list.end(),col);

    QList<FrameObject> frames;
    int id = 1;
    for(QString string : list){
        if(id == 17){
        FrameObject frame;
        frame.setCurrentDirectory(currentDirectory);
        frame.setID(id);
        frame.dereferenceSvg(dir.absolutePath() + "/" + string);
        frames.append(frame);
        }
        id++;
    }

    FrameObject obj = frames.at(0);
    QList<QString> keys = obj.getMap().keys();
    std::sort(keys.begin(),keys.end(),col);
    for(int i = 0; i < keys.size(); i++){
        qDebug() << i << " ----> " << keys.at(i) << " ----> "<< obj.getMap().value(keys.at(i))->toString();
    }
    qDebug() << obj.getMap().size();

    return a.exec();
}

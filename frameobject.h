#ifndef FRAMEOBJECT_H
#define FRAMEOBJECT_H

#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QCollator>
#include <QMap>

class FrameObject
{
public:
    FrameObject();
    int getID();
    int setID(int frameID);
    QString getCurrentDirectory();
    QString setCurrentDirectory(QString currentDirectory);
    QMap<QString,QDomDocument*>& getMap();

    void recursiveSvg(QDomDocument* ownerDoc, QDomElement& parentElement, QDomElement& element);
    void outputSvg(QString filePath);
    void dereferenceSvg(QString filePath);
    void processSvg(QString filepath);
    void saveFile(QString filePath);

private:
    QMap<QString, QDomDocument*> docs;

    QString currentDirectory;

    int currentSpriteIndex = 1;
    int maxSprites = 1;

    enum TYPE {
        NONE,
        SHAPE,
        SPRITE
    };
    int frameID = -1;
};

#endif // FRAMEOBJECT_H

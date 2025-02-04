#ifndef COMBINEDFRAMEOBJECT_H
#define COMBINEDFRAMEOBJECT_H

#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QCollator>
#include <QMap>

class CombinedFrameObject
{
public:
    CombinedFrameObject();
    int getID();
    int setID(int frameID);
    QString getCurrentDirectory();
    QString setCurrentDirectory(QString currentDirectory);
    QMap<QString,QDomDocument*>& getMap();

    void recursiveSvg(QDomDocument* ownerDoc, QDomElement& parentElement, QDomElement& element);
    void outputSvg(QString filePath);
    void dereferenceSvg(QString filePath);
    void processSvg(QString filepath);

    void setDefinitionsDocument(QDomDocument* definitions);
    QDomDocument* getDefinitionsDecoument();

private:
    QMap<QString, QDomDocument*> docs;
    QList<QString> addedToDefinition;

    QDomDocument* definitions = nullptr;

    QString currentDirectory;

    bool firstElement = true;
    int currentSpriteIndex = 1;
    int maxSprites = 1;

    int clipIdIndex = 0;

    enum TYPE {
        NONE,
        SHAPE,
        SPRITE
    };
    int frameID = -1;
};

#endif // COMBINEDFRAMEOBJECT_H

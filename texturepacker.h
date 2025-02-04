#ifndef TEXTUREPACKER_H
#define TEXTUREPACKER_H

#include <QString>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include "combinedframeobject.h"

class TexturePacker
{
public:
    TexturePacker();
    void setMaximumRow(int rowMax);
    void setMaximumColumn(int colMax);

    QString setOutputFileName(QString name);
    QString getOutputFileName();
    QString setDirectory(QString dir);
    QString getDirectory();
    QString setOutputDirectory(QString dir);
    QString getOutputDirectory();

    void dereferenceSVG();
    void writeData();

    void saveJsonFile();
    void saveOutputSVG();

private:
    QJsonDocument jsonDoc;
    QByteArray svgDocByteArray;
    QDomDocument definitions;

    QString directory;
    QString outputDirectory;

    QString outputFileName;

    QList<CombinedFrameObject> frames;

    int rowMax = -1;
    int colMax = 20;

    int spriteWidth = 100;
    int spriteHeight = 100;
};

#endif // TEXTUREPACKER_H

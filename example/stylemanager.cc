#include "stylemanager.h"

#include <QApplication>
#include <QFile>
#include <QTextCodec>

StyleManager::StyleManager()
    : style_name_("dark")
{}

StyleManager* StyleManager::Instance()
{
    static StyleManager obj;
    return &obj;
}

bool StyleManager::LoadStyleSheet()
{
    QString filename = QString(":/resource/qss/stylesheet_%1.css").arg(style_name_);

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    qApp->setStyleSheet(file.readAll());
    file.close();

    return true;
}

#include "moc_stylemanager.cpp"

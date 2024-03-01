#ifndef STYLEMANAGER_H
#define STYLEMANAGER_H

#include <QColor>
#include <QWidget>

class StyleManager
{
public:
    static StyleManager* Instance();
    bool LoadStyleSheet();

private:
    Q_DISABLE_COPY(StyleManager)
    StyleManager();

private:
    QString style_name_;
};

#endif // STYLEMANAGER_H

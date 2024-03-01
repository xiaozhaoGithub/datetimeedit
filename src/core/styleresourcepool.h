#ifndef STYLERESOURCEPOOL_H_
#define STYLERESOURCEPOOL_H_

#include <QColor>
#include <QMap>
#include <QSettings>
#include <QWidget>

#define GET_COLOR(text) StyleResourcePool::Instance()->GetColor(text)
#define GET_TXT(text) StyleResourcePool::Instance()->GetText(text)

class StyleResourcePool
{
public:
    static StyleResourcePool* Instance();

    QColor GetColor(const QString& text);

    bool LoadStyleText();
    QString GetText(const QString& key);

private:
    Q_DISABLE_COPY(StyleResourcePool)
    StyleResourcePool();

private:
    QMap<QString, QColor> color_map_;
    std::unique_ptr<QSettings> txt_settings_;
};

#endif // STYLERESOURCEPOOL_H_

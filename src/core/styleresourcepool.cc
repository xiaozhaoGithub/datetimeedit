#include "styleresourcepool.h"

#include <QApplication>
#include <QFile>
#include <QTextCodec>

StyleResourcePool::StyleResourcePool()
{
    color_map_["normal_text_font"] = QColor(255, 255, 255, 255 * 0.8);
    color_map_["main_color"] = QColor(49, 152, 255);
    color_map_["normal_text_highlight_on_dark_font"] = QColor(255, 255, 255, 255 * 0.9);
    color_map_["normal_table_item_range"] = QColor(35, 68, 120);
    color_map_["normal_white"] = QColor(255, 255, 255);
    color_map_["normal_pop_border"] = QColor(0, 0, 0);

    LoadStyleText();
}

StyleResourcePool* StyleResourcePool::Instance()
{
    static StyleResourcePool obj;
    return &obj;
}

bool StyleResourcePool::LoadStyleText()
{
    QString filename = ":/resource/language/language.ini";

    txt_settings_ = std::make_unique<QSettings>(filename, QSettings::IniFormat);
    txt_settings_->setIniCodec(QTextCodec::codecForName("UTF8"));

    return true;
}

QString StyleResourcePool::GetText(const QString& key)
{
    if (key.isEmpty() || !txt_settings_)
        return QString();

    return txt_settings_->value("text/" + key).toString();
}

QColor StyleResourcePool::GetColor(const QString& text)
{
    if (!color_map_.contains(text))
        return QColor();

    return color_map_[text];
}

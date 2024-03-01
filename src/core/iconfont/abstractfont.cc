#include "abstractfont.h"
#include <QFontDatabase>

AbstractFont::AbstractFont(const QString& name)
{
    font_id_ = QFontDatabase::addApplicationFont(":/resource/iconfont/" + name + QString(".ttf"));
    icon_font_ = QFont(QFontDatabase::applicationFontFamilies(font_id_).at(0));
}

AbstractFont::~AbstractFont()
{
    QFontDatabase::removeApplicationFont(font_id_);
}

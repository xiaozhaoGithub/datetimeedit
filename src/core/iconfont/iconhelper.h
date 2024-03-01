#ifndef ICONHELPER_H
#define ICONHELPER_H

#include <QPixmap>

#include "abstractfont.h"

class IconManager : public QObject
{
    Q_OBJECT
public:
    static IconManager* Instance();
    QFont getFont() const { return icon_font_->icon_font(); }

private:
    IconManager();
    ~IconManager();

    IconManager(const IconManager& obj) = delete;
    IconManager& operator=(const IconManager& obj) = delete;

private:
    AbstractFont* icon_font_;
};

namespace IconHelper {

template <typename WidgetType>
void setIcon(WidgetType* widget, int icon_index)
{
    QFont system_font = IconManager::Instance()->getFont();
    system_font.setPixelSize(widget->font().pixelSize());
    widget->setFont(system_font);
    widget->setText(QChar(icon_index)); // icon_index对应字符
}

template <typename WidgetType>
void setIconSize(WidgetType* widget, int icon_pix_size)
{
    QFont font = widget->font();
    if (font.pixelSize() != icon_pix_size) {
        font.setPixelSize(icon_pix_size);
        widget->setFont(font);
    }
}

} // namespace IconHelper

#endif // ICONHELPER_H

#ifndef ICONWIDGET_H
#define ICONWIDGET_H

#include "defaultfont.h"
#include "iconhelper.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QResizeEvent>

#define ICON_METHOD                                                                \
    Q_PROPERTY(int icon_index READ icon WRITE setIcon)                             \
public:                                                                            \
    virtual void setIcon(int index)                                                \
    {                                                                              \
        if (index == icon_index_)                                                  \
            return;                                                                \
        icon_index_ = index;                                                       \
        IconHelper::setIcon(this, index);                                          \
        emit iconIndexChanged(index);                                              \
    }                                                                              \
    virtual void setIconSize(int pixel_size)                                       \
    {                                                                              \
        pixel_size_ = pixel_size;                                                  \
        IconHelper::setIconSize(this, pixel_size_);                                \
    }                                                                              \
    int icon() { return icon_index_; }                                             \
    int iconSize() { return pixel_size_ ? pixel_size_ : qMin(width(), height()); } \
                                                                                   \
protected:                                                                         \
    virtual void resizeEvent(QResizeEvent* event) override { adjustIconSize(); }   \
    virtual void adjustIconSize() { IconHelper::setIconSize(this, iconSize()); }   \
    int pixel_size_ = 0;                                                           \
    int icon_index_ = -1;

class IconButton : public QPushButton
{
    Q_OBJECT
    ICON_METHOD
public:
    explicit IconButton(int icon_index, QWidget* parent = nullptr);

signals:
    void iconIndexChanged(int index);

protected:
    virtual void paintEvent(QPaintEvent*) override;
    virtual void changeEvent(QEvent* event) override;
};

#endif // ICONWIDGET_H

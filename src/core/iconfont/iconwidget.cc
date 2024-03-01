#include "iconwidget.h"

#include <QBoxLayout>
#include <QCoreApplication>
#include <QStyleOptionButton>
#include <QStylePainter>

IconButton::IconButton(int icon_index, QWidget* parent)
    : QPushButton(parent)
{
#ifdef __MAC__
    setAttribute(Qt::WA_LayoutUsesWidgetRect);
#endif
    setIcon(icon_index);
}

void IconButton::paintEvent(QPaintEvent*)
{
    QStylePainter p(this);
    QStyleOptionButton option;
    initStyleOption(&option);
    option.features &= ~QStyleOptionButton::HasMenu;
    p.drawControl(QStyle::CE_PushButton, option);
}

void IconButton::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::StyleChange) {
        adjustIconSize();
    }
    QPushButton::changeEvent(event);
}

#include "moc_iconwidget.cpp"

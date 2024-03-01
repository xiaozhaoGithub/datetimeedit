#ifndef DISPLAY_WIDGET_H_
#define DISPLAY_WIDGET_H_

#include <QWidget>

class DisplayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DisplayWidget(QWidget* parent = nullptr);
    ~DisplayWidget();
};

#endif

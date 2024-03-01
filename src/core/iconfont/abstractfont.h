#ifndef ABSTRACTFONT_H_
#define ABSTRACTFONT_H_

#include <QFont>
#include <QString>

class AbstractFont
{
public:
    explicit AbstractFont(const QString& name);
    virtual ~AbstractFont();

    QFont icon_font() const { return icon_font_; }
    QString icon_lib_name() const { return icon_lib_name_; }

private:
    int font_id_;
    QFont icon_font_;
    QString icon_lib_name_;
};

#endif // ABSTRACTFONT_H_

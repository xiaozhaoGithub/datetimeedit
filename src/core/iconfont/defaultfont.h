#ifndef DEFAULTFONT_H_
#define DEFAULTFONT_H_

#include "abstractfont.h"

class DefaultFont : public AbstractFont
{
public:
    enum IconIndex
    {
        ICON_ZUOYOUJIANTOU1 = 0xe503,
        ICON_YOU = 0xe504,
        ICON_RILI = 0xe62a,
    };

    explicit DefaultFont();
};

#endif // DEFAULTFONT_H_

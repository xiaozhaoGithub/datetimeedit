#include "iconhelper.h"

#include <QLabel>
#include <QReadWriteLock>

#include "defaultfont.h"

IconManager* IconManager::Instance()
{
    static IconManager obj;
    return &obj;
}

IconManager::IconManager()
    : icon_font_(new DefaultFont())
{}

IconManager::~IconManager()
{
    if (icon_font_) {
        delete icon_font_;
        icon_font_ = nullptr;
    }
}

#include "moc_iconhelper.cpp"

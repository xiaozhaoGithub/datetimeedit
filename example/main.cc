#include <QApplication>
#include <QTextCodec>

#include "display_widget.h"
#include "stylemanager.h"

int main(int argc, char* argv[])
{
    std::unique_ptr<QApplication> a;
    a.reset(new QApplication(argc, argv));

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    if (!StyleManager::Instance()->LoadStyleSheet())
        return -1;

    auto font = a->font();
    font.setPixelSize(13);
    a->setFont(font);

    DisplayWidget w;
    w.show();

    return a->exec();
}

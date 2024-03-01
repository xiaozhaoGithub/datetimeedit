#include "display_widget.h"

#include "datetimeedit.h"

DisplayWidget::DisplayWidget(QWidget* parent)
    : QWidget(parent)
{
    setObjectName("widget_like_container");
    setFixedHeight(300);

    auto edit1 = new DateTimeEdit(this, DateTimeEdit::kDateTimeRange);
    auto edit2 = new DateTimeEdit(this, DateTimeEdit::kDateRange);
    auto edit3 = new DateTimeEdit(this, DateTimeEdit::kDateTime);
    auto edit4 = new DateTimeEdit(this, DateTimeEdit::kDate);

    auto calendar_widget = new CalendarWidget(this);

    QList<QDate> dates;
    auto date = QDate::currentDate();
    dates.push_back(date.addDays(-9));
    dates.push_back(date.addDays(-3));
    dates.push_back(QDate::currentDate());
    calendar_widget->setSpecialDate(dates);

    auto left_layout = new QVBoxLayout;
    left_layout->addWidget(edit1);
    left_layout->addWidget(edit2);
    left_layout->addWidget(edit3);
    left_layout->addWidget(edit4);
    left_layout->setAlignment(Qt::AlignCenter);

    auto right_layout = new QVBoxLayout;
    right_layout->addWidget(calendar_widget);
    right_layout->setAlignment(Qt::AlignCenter);

    auto main_layout = new QHBoxLayout(this);
    main_layout->setSpacing(24);
    main_layout->addLayout(left_layout);
    main_layout->addLayout(right_layout);
}

DisplayWidget::~DisplayWidget() {}

#include "moc_display_widget.cpp"

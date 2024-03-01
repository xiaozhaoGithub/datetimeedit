#include "datetimepicker.h"

#include <QKeyEvent>
#include <QPainter>

#include "styleresourcepool.h"

DateTimePicker::DateTimePicker(QWidget* parent, int year, int month)
    : QWidget(parent)
{
    calendar_widget_ = new CalendarWidget(this, year, month);
    calendar_widget_->setObjectName("calendar_widget");
    calendar_widget_->addSelectedDate(QDate::currentDate());

    time_edit_ = new QTimeEdit(this);
    time_edit_->setAttribute(Qt::WA_MacShowFocusRect, false);
    time_edit_->setObjectName("calendar_time_edit");
    time_edit_->setDisplayFormat("hh:mm:ss");
    time_edit_->setMinimumTime(QTime(0, 0, 0));
    time_edit_->setMaximumTime(QTime(23, 59, 59));
    time_edit_->setTime(QTime(0, 0, 0));
    time_edit_->setFixedSize(196, 32);

    connect(time_edit_, &QTimeEdit::userTimeChanged, this, &DateTimePicker::timeChanged);
    connect(calendar_widget_, &CalendarWidget::calendarSelectionChanged, this, &DateTimePicker::calendarSelectionChanged);

    calendar_widget_->addWidgetBottom(time_edit_);

    auto main_layout = new QVBoxLayout(this);
    main_layout->setMargin(0);
    main_layout->addWidget(calendar_widget_);
}

void DateTimePicker::setDateTime(const QDateTime& date_time)
{
    calendar_widget_->addSelectedDate(date_time.date());
    time_edit_->setTime(date_time.time());
}

QDateTime DateTimePicker::dateTime()
{
    return QDateTime(calendar_widget_->selected_date().at(0), time_edit_->time());
}

void DateTimePicker::calendarSelectionChanged()
{
    if (calendar_widget_->selected_date().isEmpty())
        return;

    QDate tmp_date = calendar_widget_->selected_date().first();
    calendar_widget_->setYearMonth(tmp_date.year(), tmp_date.month());

    emit dateChanged(tmp_date);
}

DatePicker::DatePicker(QWidget* parent, int year, int month)
    : DateTimePicker(parent, year, month)
{
    time_edit_->hide();
}

DateTimeRangePicker::DateTimeRangePicker(QWidget* parent, int year, int month)
    : QWidget(parent)
{
    setAttribute(Qt::WA_Hover, true);

    start_calendar_ = new CalendarWidget(this);
    start_calendar_->setObjectName("calendar_widget");
    start_calendar_->setTitle(GET_TXT("IDCS_STATR"));
    start_calendar_->setSelMode(CalendarTable::kRangeSelection);
    start_calendar_->setMaxDate(start_calendar_->max_date());
    start_calendar_->setYearMonth(year, month);

    end_calendar_ = new CalendarWidget(this);
    end_calendar_->setObjectName("calendar_widget");
    end_calendar_->setTitle(GET_TXT("IDCS_END"));
    end_calendar_->setSelMode(CalendarTable::kRangeSelection);
    end_calendar_->setMinDate(end_calendar_->min_date());
    end_calendar_->setYearMonth(year, month);

    auto createEdit = [this]() {
        auto edit = new QTimeEdit(this);
        edit->setFixedHeight(32);
        edit->setAttribute(Qt::WA_MacShowFocusRect, false);
        edit->setObjectName("calendar_time_edit");
        edit->setDisplayFormat("hh:mm:ss");
        edit->setMinimumTime(QTime(0, 0, 0));
        edit->setMaximumTime(QTime(23, 59, 59));
        return edit;
    };

    start_time_edit_ = createEdit();
    start_time_edit_->setTime(QTime(0, 0, 0));

    end_time_edit_ = createEdit();
    end_time_edit_->setTime(QTime(23, 59, 59));

    QDate date(year, month, QDate::currentDate().day());
    date_range_pair_.first = date;
    date_range_pair_.second = date;

    refreshCalendarSelection();

    // 信号槽连接
    connectSelectionAdded();
    connect(start_calendar_, &CalendarWidget::calendarSelectionChanged, this, &DateTimeRangePicker::dateRangeChanged);
    connect(end_calendar_, &CalendarWidget::calendarSelectionChanged, this, &DateTimeRangePicker::dateRangeChanged);
    connect(start_calendar_, &CalendarWidget::dateDoubleClicked, this, &DateTimeRangePicker::dateDoubleClicked);
    connect(end_calendar_, &CalendarWidget::dateDoubleClicked, this, &DateTimeRangePicker::dateDoubleClicked);
    connect(start_time_edit_, &QTimeEdit::timeChanged, this, &DateTimeRangePicker::timeChanged);
    connect(end_time_edit_, &QTimeEdit::timeChanged, this, &DateTimeRangePicker::timeChanged);

    // 布局
    start_calendar_->addWidgetBottom(start_time_edit_);
    end_calendar_->addWidgetBottom(end_time_edit_);

    auto main_layout = new QHBoxLayout(this);
    main_layout->setMargin(0);
    main_layout->setSpacing(0);
    main_layout->addWidget(start_calendar_);
    main_layout->addWidget(end_calendar_);

    start_time_edit_->installEventFilter(this);
    end_time_edit_->installEventFilter(this);
}

void DateTimeRangePicker::setStartDateTime(const QDateTime& start)
{
    start_calendar_->addSelectedDate(start.date());
    start_time_edit_->setTime(start.time());
}

void DateTimeRangePicker::setEndDateTime(const QDateTime& end)
{
    end_calendar_->addSelectedDate(end.date());
    end_time_edit_->setTime(end.time());
}

void DateTimeRangePicker::setDateTimeRange(const QDateTime& start, const QDateTime& end)
{
    if (start > end)
        return;

    start_calendar_->addSelectedDate(start.date());
    end_calendar_->addSelectedDate(end.date());

    start_time_edit_->setTime(start.time());
    end_time_edit_->setTime(end.time());
}

void DateTimeRangePicker::timeRange(QTime& start_time, QTime& end_time)
{
    start_time = start_time_edit_->time();
    end_time = end_time_edit_->time();
}

QPair<QTime, QTime> DateTimeRangePicker::timeRange()
{
    return qMakePair(start_time_edit_->time(), end_time_edit_->time());
}

void DateTimeRangePicker::dateRange(QDate& start_date, QDate& end_date)
{
    start_date = date_range_pair_.first;
    end_date = date_range_pair_.second;
}

QPair<QDate, QDate> DateTimeRangePicker::dateRange()
{
    return date_range_pair_;
}

void DateTimeRangePicker::calendarSelectionAdded(const QDate& date)
{
    disconnectSelectionAdded();

    bool is_start_triggered = (sender() == start_calendar_);
    QDate& dateRef = is_start_triggered ? date_range_pair_.first : date_range_pair_.second;
    dateRef = date;

    // The rules.
    if (date_range_pair_.first > date_range_pair_.second) {
        if (is_start_triggered) {
            date_range_pair_.second = date_range_pair_.first;
        } else {
            date_range_pair_.first = date_range_pair_.second;
        }
    }

    if ((date_range_pair_.first == date_range_pair_.second) && (start_time_edit_->time() > end_time_edit_->time())) {
        auto time = is_start_triggered ? QTime(0, 0, 0) : QTime(23, 59, 59);
        auto time_edit = is_start_triggered ? start_time_edit_ : end_time_edit_;
        showError(time_edit, time);
    }

    start_calendar_->setYearMonth(date_range_pair_.first.year(), date_range_pair_.first.month());
    end_calendar_->setYearMonth(date_range_pair_.second.year(), date_range_pair_.second.month());

    refreshCalendarSelection();

    connectSelectionAdded();
}

void DateTimeRangePicker::calendarRefresh(int year, int month)
{
    disconnect(start_calendar_, &CalendarWidget::calendarRefresh, this, &DateTimeRangePicker::calendarRefresh);
    disconnect(end_calendar_, &CalendarWidget::calendarRefresh, this, &DateTimeRangePicker::calendarRefresh);

    if (sender() == start_calendar_) {
        if (QDate(end_calendar_->year(), end_calendar_->month(), 1) <= QDate(year, month, 1)) {
            // end_calendar_->setYearMonth(year, month + 1);
        }
    } else {
        if (QDate(start_calendar_->year(), start_calendar_->month(), 1) >= QDate(year, month, 1)) {
            // start_calendar_->setYearMonth(year, month - 1);
        }
    }

    connect(start_calendar_, &CalendarWidget::calendarRefresh, this, &DateTimeRangePicker::calendarRefresh);
    connect(end_calendar_, &CalendarWidget::calendarRefresh, this, &DateTimeRangePicker::calendarRefresh);
}

void DateTimeRangePicker::dateDoubleClicked(const QDate& date)
{
    disconnectSelectionAdded();

    QPair<QDate, QDate>(date, date).swap(date_range_pair_);

    start_calendar_->setYearMonth(date.year(), date.month());
    end_calendar_->setYearMonth(date.year(), date.month());

    if (start_time_edit_->time() > end_time_edit_->time()) {
        start_time_edit_->setTime(QTime(0, 0, 0));
        end_time_edit_->setTime(QTime(23, 59, 59));
    }

    refreshCalendarSelection();

    connectSelectionAdded();
}

void DateTimeRangePicker::timeChanged()
{
    if ((date_range_pair_.first == date_range_pair_.second) && (start_time_edit_->time() > end_time_edit_->time())) {
        auto time_edit = qobject_cast<QTimeEdit*>(sender());
        showError(time_edit, time_edit == start_time_edit_ ? QTime(0, 0, 0) : QTime(23, 59, 59));
    }

    emit timeRangeChanged();
}

void DateTimeRangePicker::showError(QTimeEdit* const time_edit, const QTime& time)
{
    // set red border.
    auto setBorderColor = [this](QWidget* const widget, int styleId) {
        widget->setProperty("style", styleId);
        widget->style()->unpolish(widget);
        widget->style()->polish(widget);
    };

    setBorderColor(time_edit, 1);

    QTimer::singleShot(1000, this, [time_edit, setBorderColor, time]() {
        time_edit->setTime(time);
        setBorderColor(time_edit, 0);
    });
}

void DateTimeRangePicker::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);

    painter.save();

    painter.setPen(QPen(GET_COLOR("normal_pop_border"), 1));
    painter.drawRect(rect().adjusted(1, 1, -1, -1));
    painter.drawLine(width() / 2, 1, width() / 2, height() - 1);

    painter.restore();
}

void DateTimeRangePicker::refreshCalendarSelection()
{
    start_calendar_->clearSelectedDate();
    end_calendar_->clearSelectedDate();

    auto addSelectedDate = [this](const QDate& date, CalendarWidget* calendarWidget) {
        if (date >= calendarWidget->min_date() && date <= calendarWidget->max_date()) {
            calendarWidget->addSelectedDate(date);
        }
    };

    addSelectedDate(date_range_pair_.first, start_calendar_);
    addSelectedDate(date_range_pair_.second, end_calendar_);
}

void DateTimeRangePicker::connectSelectionAdded()
{
    connect(start_calendar_, &CalendarWidget::calendarSelectionAdded, this, &DateTimeRangePicker::calendarSelectionAdded);
    connect(end_calendar_, &CalendarWidget::calendarSelectionAdded, this, &DateTimeRangePicker::calendarSelectionAdded);
}

void DateTimeRangePicker::disconnectSelectionAdded()
{
    disconnect(start_calendar_, &CalendarWidget::calendarSelectionAdded, this,
               &DateTimeRangePicker::calendarSelectionAdded);
    disconnect(end_calendar_, &CalendarWidget::calendarSelectionAdded, this, &DateTimeRangePicker::calendarSelectionAdded);
}

DateRangePicker::DateRangePicker(QWidget* parent, int year, int month)
    : DateTimeRangePicker(parent, year, month)
{
    disconnectSelectionAdded();

    date_range_pair_.first = QDate(year, month, 1);
    date_range_pair_.second = QDate(year, month, QDate::currentDate().day());

    start_time_edit_->hide();
    end_time_edit_->hide();

    refreshCalendarSelection();

    connectSelectionAdded();
}

SimpleDateTimeEdit::SimpleDateTimeEdit(QWidget* parent)
    : QDateTimeEdit(parent)
{
    // 移除mac下focus frame
    setAttribute(Qt::WA_MacShowFocusRect, false);

    connect(lineEdit(), &QLineEdit::cursorPositionChanged, this, &SimpleDateTimeEdit::cursorPositionChanged);
}

void SimpleDateTimeEdit::keyPressEvent(QKeyEvent* ev)
{
    switch (ev->key()) {
    case Qt::Key_Left: {
        if (lineEdit()->cursorPosition() == 0) {
            emit corsorLeft();
        }
        break;
    }
    case Qt::Key_Right: {
        if (lineEdit()->cursorPosition() == lineEdit()->text().length()) {
            emit corsorRight();
        }
        break;
    }
    default:
        break;
    }

    QDateTimeEdit::keyPressEvent(ev);
}

#include "moc_datetimepicker.cpp"

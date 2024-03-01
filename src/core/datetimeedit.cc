#include "datetimeedit.h"

#include <QApplication>
#include <QDesktopWidget>

#include "styleresourcepool.h"
#include "iconfont/iconwidget.h"

DateTimeEdit::DateTimeEdit(QWidget* parent, DateTimeEdit::DateEditType type, int year, int month)
    : QFrame(parent)
    , type_(type)
{
    setObjectName("date_time_edit");

    setFixedHeight(32);
    setMinimumWidth(240);

    auto createEdit = [this]() {
        auto edit = new SimpleDateTimeEdit(this);
        edit->setMinimumHeight(32);
        edit->setButtonSymbols(QAbstractSpinBox::NoButtons);
        edit->setObjectName("simple_date_time_edit");
        return edit;
    };
    start_edit_ = createEdit();
    end_edit_ = createEdit();

    auto split_label = new QLabel(" ~ ", this);

    scroll_area_ = new QScrollArea(this);
    scroll_area_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    scroll_area_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll_area_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll_area_->setObjectName("no_border_scroll_area");

    auto edit_widget = new QWidget(this);

    auto calendar_btn = new IconButton(DefaultFont::ICON_RILI, this);
    calendar_btn->setFixedSize(16, 16);
    calendar_btn->setToolTip(GET_TXT("IDCS_CALENDAR"));

    popup_widget_ = new QFrame(this);
    popup_widget_->setWindowFlags(Qt::Popup | Qt::NoDropShadowWindowHint);
    popup_widget_->hide();

    switch (type) {
    case DateTimeEdit::kDateTime: {
        start_edit_->setMinimumWidth(140);
        popup_widget_->setFixedSize(215, 261);
        split_label->hide();
        end_edit_->hide();
        break;
    }
    case DateTimeEdit::kDate: {
        popup_widget_->setFixedSize(215, 230);
        split_label->hide();
        end_edit_->hide();
        break;
    }
    case DateTimeEdit::kDateTimeRange: {
        start_edit_->setMinimumWidth(140);
        end_edit_->setMinimumWidth(140);
        popup_widget_->setFixedSize(426, 286);
        break;
    }
    case DateTimeEdit::kDateRange: {
        start_edit_->setMinimumWidth(73);
        end_edit_->setMinimumWidth(73);
        popup_widget_->setFixedSize(426, 254);
        break;
    }
    default:
        break;
    }

    stacked_layout_ = new QStackedLayout(popup_widget_);
    stacked_layout_->setMargin(0);

    date_time_picker_ = new DateTimePicker(this, year, month);
    date_picker_ = new DatePicker(this, year, month);
    date_time_range_picker_ = new DateTimeRangePicker(this, year, month);
    date_range_picker_ = new DateRangePicker(this, year, month);

    stacked_layout_->insertWidget(kDateTime, date_time_picker_);
    stacked_layout_->insertWidget(kDate, date_picker_);
    stacked_layout_->insertWidget(kDateTimeRange, date_time_range_picker_);
    stacked_layout_->insertWidget(kDateRange, date_range_picker_);
    stacked_layout_->setCurrentIndex(type);

    date_time_picker_->installEventFilter(this);
    date_picker_->installEventFilter(this);
    date_time_range_picker_->installEventFilter(this);
    date_range_picker_->installEventFilter(this);

    auto edit_layout = new QHBoxLayout(edit_widget);
    edit_layout->setMargin(0);
    edit_layout->addWidget(start_edit_);
    edit_layout->addWidget(split_label);
    edit_layout->addWidget(end_edit_);
    edit_layout->addStretch();
    scroll_area_->setWidget(edit_widget);

    auto main_layout = new QHBoxLayout(this);
    main_layout->setContentsMargins(9, 0, 9, 0);
    main_layout->addWidget(scroll_area_);
    main_layout->addStretch();
    main_layout->addSpacing(8);
    main_layout->addWidget(calendar_btn);

    initDateFormat(type, year, month);

    // 信号槽连接
    connect(start_edit_, &SimpleDateTimeEdit::cursorPositionChanged, this, &DateTimeEdit::cursorPositionChanged);
    connect(end_edit_, &SimpleDateTimeEdit::cursorPositionChanged, this, &DateTimeEdit::cursorPositionChanged);
    connect(start_edit_, &SimpleDateTimeEdit::corsorRight, this, &DateTimeEdit::cursorJump);
    connect(end_edit_, &SimpleDateTimeEdit::corsorLeft, this, &DateTimeEdit::cursorJump);
    connect(start_edit_, &SimpleDateTimeEdit::dateTimeChanged, this, &DateTimeEdit::dateTimeChanged);
    connect(end_edit_, &SimpleDateTimeEdit::dateTimeChanged, this, &DateTimeEdit::dateTimeChanged);

    connect(calendar_btn, &QPushButton::clicked, this, &DateTimeEdit::editBtnClicked);

    connect(date_time_picker_, &DateTimePicker::timeChanged, this, &DateTimeEdit::timeChanged);
    connect(date_time_picker_, &DateTimePicker::dateChanged, this, &DateTimeEdit::dateChanged);
    connect(date_picker_, &DateTimePicker::dateChanged, this, &DateTimeEdit::dateChanged);

    connect(date_time_range_picker_, &DateTimeRangePicker::timeRangeChanged, this, &DateTimeEdit::timeRangeChanged);
    connect(date_time_range_picker_, &DateTimeRangePicker::dateRangeChanged, this, &DateTimeEdit::dateRangeChanged);
    connect(date_range_picker_, &DateTimeRangePicker::dateRangeChanged, this, &DateTimeEdit::dateRangeChanged);
}

void DateTimeEdit::setDate(const QDate& date)
{
    if (type_ == kDateTime)
        date_time_picker_->setDateTime(QDateTime(date));
    else if (type_ == kDate)
        date_picker_->setDateTime(QDateTime(date));
}

void DateTimeEdit::setDateTime(const QDateTime& date_time)
{
    if (type_ == kDateTime)
        date_time_picker_->setDateTime(date_time);
    else if (type_ == kDate)
        date_picker_->setDateTime(date_time);
}

void DateTimeEdit::setStartDateTime(const QDateTime& start)
{
    if (stacked_layout_->currentIndex() == kDateTimeRange)
        date_time_range_picker_->setStartDateTime(start);
    else if (stacked_layout_->currentIndex() == kDateRange)
        date_range_picker_->setStartDateTime(start);
}

void DateTimeEdit::setEndDateTime(const QDateTime& end)
{
    if (stacked_layout_->currentIndex() == kDateTimeRange)
        date_time_range_picker_->setEndDateTime(end);
    else if (stacked_layout_->currentIndex() == kDateRange)
        date_range_picker_->setEndDateTime(end);
}

void DateTimeEdit::setDateTimeRange(const QDateTime& start, const QDateTime& end)
{
    if (stacked_layout_->currentIndex() == kDateTimeRange)
        date_time_range_picker_->setDateTimeRange(start, end);
    else if (stacked_layout_->currentIndex() == kDateRange)
        date_range_picker_->setDateTimeRange(start, end);
}

void DateTimeEdit::resetDateTime()
{
    auto start = QDateTime(QDate::currentDate(), QTime(0, 0, 0));
    auto end = QDateTime(QDate::currentDate(), QTime(23, 59, 59));

    if (stacked_layout_->currentIndex() == kDateTimeRange)
        date_time_range_picker_->setDateTimeRange(start, end);
    else if (stacked_layout_->currentIndex() == kDateRange)
        date_range_picker_->setDateTimeRange(start, end);
}

QDate DateTimeEdit::date()
{
    if (stacked_layout_->currentIndex() == kDateTime)
        return date_time_picker_->date();
    else if (stacked_layout_->currentIndex() == kDate)
        return date_picker_->date();

    return QDate();
}

QList<QDate> DateTimeEdit::selected_date()
{
    if (stacked_layout_->currentIndex() == kDateTime)
        return date_time_picker_->selected_date();
    else if (stacked_layout_->currentIndex() == kDate)
        return date_picker_->selected_date();

    return QList<QDate>();
}

QTime DateTimeEdit::time()
{
    auto picker = stacked_layout_->currentIndex() == kDateTime ? date_time_picker_ : date_picker_;
    return picker->time();
}

QDateTime DateTimeEdit::start_date_time()
{
    return QDateTime::fromString(real_text_.split(" ~ ").at(0), date_format_);
}

QDateTime DateTimeEdit::end_date_time()
{
    return QDateTime::fromString(real_text_.split(" ~ ").at(1), date_format_);
}

bool DateTimeEdit::isValidDateTimeRange()
{
    auto range = real_text_.split(" ~ ");
    return QDateTime::fromString(range.at(0), date_format_) <= QDateTime::fromString(range.at(1), date_format_);
}

bool DateTimeEdit::isDifferentYearOrMonth()
{
    auto range = real_text_.split(" ~ ");
    auto start_date = QDateTime::fromString(range.at(0), date_format_).date();
    auto end_date = QDateTime::fromString(range.at(1), date_format_).date();

    return start_date.year() != end_date.year() || start_date.month() != end_date.month();
}

void DateTimeEdit::resizeEvent(QResizeEvent* event)
{
    setToolTip(real_text_);
}

bool DateTimeEdit::eventFilter(QObject* watched, QEvent* event)
{
    if (auto const picker = qobject_cast<DateTimeRangePicker*>(watched)) {
        if (event->type() == QEvent::Show) {
            auto range = real_text_.split(" ~ ");
            auto start_date_time = QDateTime::fromString(range.at(0), date_format_);
            auto end_date_time = QDateTime::fromString(range.at(1), date_format_);
            picker->setDateTimeRange(start_date_time, end_date_time);
        }
    }

    if (auto const picker = qobject_cast<DateTimePicker*>(watched)) {
        if (event->type() == QEvent::Show) {
            auto date_time = QDateTime::fromString(real_text_, date_format_);
            picker->setDateTime(date_time);
        }
    }

    return false;
}

void DateTimeEdit::cursorPositionChanged(int old_pos, int new_pos)
{
    auto simple_edit = qobject_cast<SimpleDateTimeEdit*>(sender());
    if (!simple_edit)
        return;

    if (new_pos > old_pos) {
        setCursorPosInVisivle(simple_edit, new_pos > 0 ? new_pos - 1 : 0);
    } else {
        setCursorPosInVisivle(simple_edit, new_pos);
    }
}

void DateTimeEdit::cursorJump()
{
    auto simple_edit = qobject_cast<SimpleDateTimeEdit*>(sender());
    if (!simple_edit)
        return;

    if (simple_edit == start_edit_) {
        end_edit_->setFocus(Qt::MouseFocusReason);
        setCursorPosInVisivle(end_edit_, 0);
    } else {
        start_edit_->setFocus(Qt::MouseFocusReason);
        start_edit_->innerLineEdit()->setCursorPosition(start_edit_->innerLineEdit()->text().length());
    }
}

void DateTimeEdit::dateTimeChanged(const QDateTime& date_time)
{
    auto simple_edit = qobject_cast<SimpleDateTimeEdit*>(sender());
    if (!simple_edit)
        return;

    if (type_ == kDateTime || type_ == kDate) {
        real_text_ = date_time.toString(date_format_);
        setDateTime(date_time);
    } else {
        if (simple_edit == start_edit_) {
            auto end_text = real_text_.split(" ~ ").at(1);

            real_text_ = date_time.toString(date_format_) + " ~ " + end_text;
            setStartDateTime(date_time);
        } else {
            auto startText = real_text_.split(" ~ ").at(0);
            real_text_ = startText + " ~ " + date_time.toString(date_format_);

            setEndDateTime(date_time);
        }
    }
}

void DateTimeEdit::editBtnClicked()
{
    if (popup_widget_->isHidden()) {
        QPoint pos = mapToGlobal(QPoint(0, 0));
        pos.setY(pos.y() + height() + 8);

        //屏幕越界判断
        QRect screenRect = QApplication::desktop()->screenGeometry();
        if (pos.x() + popup_widget_->width() > (screenRect.x() + screenRect.width()))
            pos.setX(screenRect.width() - popup_widget_->width());

        if (pos.y() + popup_widget_->height() > screenRect.height())
            pos.setY(mapToGlobal(QPoint(0, 0)).y() - popup_widget_->height() - 8);

        popup_widget_->move(pos);
        popup_widget_->show();
    } else {
        popup_widget_->hide();
    }
}

void DateTimeEdit::timeChanged(const QTime& time)
{
    auto widget = qobject_cast<DateTimePicker*>(sender());
    if (!widget)
        return;

    auto date_time = QDateTime::fromString(real_text_, date_format_);
    date_time.setTime(time);

    real_text_ = date_time.toString(date_format_);

    start_edit_->setDateTime(date_time);
    setToolTip(real_text_);
}

void DateTimeEdit::dateChanged(const QDate& date)
{
    auto widget = qobject_cast<DateTimePicker*>(sender());
    if (!widget)
        return;

    auto date_time = QDateTime::fromString(real_text_, date_format_);
    date_time.setDate(date);

    real_text_ = date_time.toString(date_format_);

    start_edit_->setDateTime(date_time);
    setToolTip(real_text_);
}

void DateTimeEdit::timeRangeChanged()
{
    auto widget = qobject_cast<DateTimeRangePicker*>(sender());
    if (!widget)
        return;

    auto range = real_text_.split(" ~ ");
    auto start = QDateTime::fromString(range.at(0), date_format_);
    auto end = QDateTime::fromString(range.at(1), date_format_);
    start.setTime(widget->timeRange().first);
    end.setTime(widget->timeRange().second);

    real_text_ = start.toString(date_format_) + " ~ " + end.toString(date_format_);

    start_edit_->setDateTime(start_date_time());
    end_edit_->setDateTime(end_date_time());
    setToolTip(real_text_);

    emit startDateTimeChanged(start);
    emit endDateTimeChanged(end);
}
void DateTimeEdit::dateRangeChanged()
{
    auto widget = qobject_cast<DateTimeRangePicker*>(sender());
    if (!widget)
        return;

    auto range = real_text_.split(" ~ ");
    auto start = QDateTime::fromString(range.at(0), date_format_);
    auto end = QDateTime::fromString(range.at(1), date_format_);
    start.setDate(widget->dateRange().first);
    end.setDate(widget->dateRange().second);

    real_text_ = start.toString(date_format_) + " ~ " + end.toString(date_format_);

    start_edit_->setDateTime(start_date_time());
    end_edit_->setDateTime(end_date_time());

    setToolTip(real_text_);

    emit startDateTimeChanged(start);
    emit endDateTimeChanged(end);
}

void DateTimeEdit::initDateFormat(DateEditType type, int year, int month)
{
    // QString date_fm；
    // QString date_time_fm;

    auto current_day = QDateTime::currentDateTime().date().day();
    auto init_date_time = QDateTime(QDate(year, month, current_day), QTime());

    switch (type) {
    case DateTimeEdit::kDateTime: {
        date_format_ = "yyyy-MM-dd hh:mm:ss";
        QString date_time = init_date_time.toString(date_format_);
#ifdef APP_PLATFORM
        date_format_ = date_timeFm;
        date_time = init_date_time.toString(date_format_);
#endif
        real_text_ = date_time;
        break;
    }
    case DateTimeEdit::kDate: {
        date_format_ = "yyyy-MM-dd";
        QString date = init_date_time.toString("yyyy-MM-dd");
#ifdef APP_PLATFORM
        date_format_ = dateFm;
        date = init_date_time.toString(dateFm);
#endif
        real_text_ = date;
        break;
    }
    case DateTimeEdit::kDateTimeRange: {
        date_format_ = "yyyy-MM-dd hh:mm:ss";
        QString start_date_time = init_date_time.toString(date_format_);
        QString end_date_time = init_date_time.addDays(1).addMSecs(-1).toString(date_format_);
#ifdef APP_PLATFORM
        date_format_ = date_timeFm;
        start_date_time = init_date_time.toString(date_format_);
        end_date_time = init_date_time.addDays(1).addSecs(-1).toString(date_format_);
#endif
        real_text_ = start_date_time + " ~ " + end_date_time;
        break;
    }
    case DateTimeEdit::kDateRange: {
        auto first_day_in_month = QDateTime(QDate(year, month, 1), QTime());
        date_format_ = "yyyy-MM-dd";
        QString start_date = first_day_in_month.toString(date_format_);
        QString end_date = init_date_time.toString(date_format_);
#ifdef APP_PLATFORM
        date_format_ = dateFm;
        start_date = first_day_in_month.toString(date_format_);
        end_date = init_date_time.toString(date_format_);
#endif
        real_text_ = start_date + " ~ " + end_date;
        break;
    }
    default:
        break;
    }

    start_edit_->setDisplayFormat(date_format_);
    end_edit_->setDisplayFormat(date_format_);

    if (real_text_.contains("~")) {
        start_edit_->setDateTime(start_date_time());
        end_edit_->setDateTime(end_date_time());
    } else {
        start_edit_->setDateTime(QDateTime(date(), time()));
    }

    setToolTip(real_text_);
}

void DateTimeEdit::setCursorPosInVisivle(SimpleDateTimeEdit* simple_edit, int index)
{
    if (!simple_edit || !simple_edit->innerLineEdit() || index < 0)
        return;

    auto line_edit = simple_edit->innerLineEdit();
    QFontMetrics fm(line_edit->fontMetrics());

    // 当前光标字符被遮挡的宽度
    auto relativeX = [=](int pos) {
        int inner_padding = 3;
        auto cursor_pos = fm.horizontalAdvance(line_edit->text().left(pos)) + inner_padding;
        auto parent_cursor_pos = simple_edit->mapToParent(QPoint(cursor_pos, 0));
        auto relative_pos = parent_cursor_pos + scroll_area_->widget()->mapToParent(QPoint(0, 0));
        return relative_pos.x();
    };

    int current_value = scroll_area_->horizontalScrollBar()->value();
    int char_width = index - 1 >= 0 ? fm.horizontalAdvance(line_edit->text().at(index - 1)) : 0;

    // 先取文本左侧的位置
    int relative_x = relativeX(index) - char_width;
    if (relative_x < 0) {
        // 文本左侧位于左侧不可见的位置
        scroll_area_->horizontalScrollBar()->setValue(current_value + relative_x);
        return;
    }

    char_width = index + 1 < line_edit->text().size() ? fm.horizontalAdvance(line_edit->text().at(index + 1)) : 0;

    relative_x = relativeX(index + 1) + char_width;
    if (relative_x > scroll_area_->width()) {
        // 文本右侧位于右侧不可见的位置
        scroll_area_->horizontalScrollBar()->setValue(current_value + relative_x - scroll_area_->width());
        return;
    }
}

#include "moc_datetimeedit.cpp"

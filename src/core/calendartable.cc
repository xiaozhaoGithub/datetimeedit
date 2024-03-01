#include "calendartable.h"

#include "flowlayout.h"
#include "styleresourcepool.h"

constexpr int kDaysInWeek = 7;
constexpr int kPointRadius = 1;
constexpr int kCalendarRows = 6;

DrawBaseDelegate::DrawBaseDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{}

void DrawBaseDelegate::drawBackgroud(QPainter* painter, const QRect& rect, const QColor& color) const
{
    painter->save();

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);
    painter->setBrush(color);

    painter->drawEllipse(rect.x() + 1, rect.y() + 1, rect.width() - 2, rect.height() - 2);

    painter->restore();
}

void DrawBaseDelegate::drawBorder(QPainter* painter, const QRect& rect, const QColor& color) const
{
    painter->save();

    painter->setPen(color);
    painter->drawRect(rect.x(), rect.y(), rect.width() - 1, rect.height() - 1);

    painter->restore();
}

void DrawBaseDelegate::drawText(QPainter* painter, const QRect& rect, const QColor& color, const QString& text,
                                const Qt::AlignmentFlag align) const
{
    painter->save();

    painter->setPen(color);
    painter->drawText(rect, align, text);

    painter->restore();
}

void DrawBaseDelegate::drawPoint(QPainter* painter, const QRect& rect, const QColor& color) const
{
    painter->save();

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(color);
    painter->setBrush(color);

    painter->drawEllipse(rect);

    painter->restore();
}

CalendarDelegate::CalendarDelegate(QAbstractItemView* parent)
    : DrawBaseDelegate(parent)
    , parent_view_(parent)
{
    parent_view_->viewport()->installEventFilter(this);
    parent_view_->viewport()->setAttribute(Qt::WA_Hover);
}

void CalendarDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QVariant alpha_data = index.data(CalendarTable::kAlpha);
    double color_alpha = alpha_data.canConvert<double>() ? alpha_data.toDouble() : 1.0;
    if (!(QStyle::State_Enabled & option.state)) {
        color_alpha *= 0.5;
    }

    QColor tmp_color;

    bool draw_bg = colorByRole(index, CalendarTable::kBgColor, color_alpha, tmp_color);
    if (draw_bg) {
        drawBackgroud(painter, option.rect, tmp_color);
    }

    if (colorByRole(index, CalendarTable::kBorderColor, color_alpha, tmp_color)) {
        drawBorder(painter, option.rect, tmp_color);
    }

    if (colorByRole(index, CalendarTable::kHoverBorderColor, color_alpha, tmp_color)) {
        if (!draw_bg && index == hover_index_ && (QStyle::State_Enabled & option.state)) {
            drawBackgroud(painter, option.rect, tmp_color);
        }
    }

    if (colorByRole(index, CalendarTable::kFontColor, color_alpha, tmp_color)) {
        drawText(painter, option.rect, tmp_color, index.data().toString());
    }

    if (colorByRole(index, CalendarTable::kPointColor, color_alpha, tmp_color)) {
        QRect tmp_rect(option.rect.x() + option.rect.width() / 2 - kPointRadius,
                       option.rect.y() + option.rect.height() * 6 / 7 - kPointRadius, 2 * kPointRadius, 2 * kPointRadius);
        drawPoint(painter, tmp_rect, tmp_color);
    }

    // QStyledItemDelegate::paint(painter, option, index);
}

bool CalendarDelegate::eventFilter(QObject* obj, QEvent* ev)
{
    if (obj == parent_view_->viewport()) {
        if (ev->type() == QEvent::HoverMove) {
            QPoint pos = parent_view_->viewport()->mapFromGlobal(QCursor::pos());
            hover_index_ = parent_view_->indexAt(pos);
        } else if (ev->type() == QEvent::Leave) {
            hover_index_ = QModelIndex();
        }
    }

    return DrawBaseDelegate::eventFilter(obj, ev);
}

bool CalendarDelegate::colorByRole(const QModelIndex& model_index, int role, double alpha, QColor& tmp_color) const
{
    QVariant var = model_index.data(role);
    if (var.canConvert<QColor>()) {
        tmp_color = var.value<QColor>();
        if (alpha) {
            tmp_color.setAlphaF(alpha);
        }
        return true;
    }
    return false;
}

CalendarTable::CalendarTable(QWidget* parent, int year, int month)
    : QTableWidget(kCalendarRows, kDaysInWeek, parent)
    , year_(year)
    , month_(month)
    , first_day_on_week_(7)
    , max_date_(QDate(year + 100, 1, 1).addDays(-1))
    , min_date_(QDate(year - 100, 1, 1))
    , is_double_clicked_(false)
    , select_mode_(kSingleSelection)
{
    setMinimumSize(180, 180);
    setObjectName("calendar_table");
    horizontalHeader()->setObjectName("calendar_table_header");

    setSelectionMode(QAbstractItemView::NoSelection);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    horizontalHeader()->setFixedHeight(24);

    // Stretch, 而不出现滚轮
    horizontalHeader()->setMinimumSectionSize(24);
    verticalHeader()->setMinimumSectionSize(24);

    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    verticalHeader()->hide();
    setShowGrid(false);

    installEventFilter(this);

    setItemDelegate(new CalendarDelegate(this));

    date_timer_ = new QTimer(this);
    connect(date_timer_, &QTimer::timeout, this, &CalendarTable::refreshTodayDate);

    startRefreshTimer();

    refreshCalendar();
    refreshCalendarHeader();

    setContentsMargins(0, 0, 0, 0);

    connect(this, &QTableWidget::itemClicked, this, &CalendarTable::itemClicked);
    connect(this, &QTableWidget::itemDoubleClicked, this, &CalendarTable::itemDoubleClicked);
}

bool CalendarTable::setYearMonth(int year, int month)
{
    year += (month - 1 + 12) / 12 - 1;
    month = (month - 1 + 12) % 12 + 1;

    if (year > max_date_.year() || year < min_date_.year()) {
        return false;
    } else if (QDate(year, month, 1) > QDate(max_date_.year(), max_date_.month(), 1)) {
        month = max_date_.month();
    } else if (QDate(year, month, 1) < QDate(min_date_.year(), min_date_.month(), 1)) {
        month = min_date_.month();
    }

    if (year_ == year && month_ == month)
        return false;

    year_ = year;
    month_ = month;

    refreshCalendar();
    return true;
}

void CalendarTable::setFristDayOnWeek(int first_day_on_week)
{
    first_day_on_week_ = first_day_on_week;
    refreshCalendarHeader();
}

void CalendarTable::setSelMode(CalendarTable::CalendarSelectionMode select_mode)
{
    select_mode_ = select_mode;
    clearSelectedDate();
}

void CalendarTable::setSpecialDate(const QList<QDate>& dates)
{
    special_dates_ = dates;
    refreshSpecialDate();
}

void CalendarTable::addSelectedDate(const QDate& date)
{
    if (appendSelection(date))
        refreshSelection();
}

void CalendarTable::deleteSelectedDate(const QDate& date)
{
    selected_dates_.removeAll(date);
    emit selectionChanged();
    refreshSelection();
}

void CalendarTable::clearSelectedDate()
{
    selected_dates_.clear();
    emit selectionChanged();
    refreshSelection();
}

void CalendarTable::setMaxDate(const QDate& date)
{
    if (max_date_ == date)
        return;

    max_date_ = date;

    if (QDate(year_, month_, max_date_.day()) > max_date_) {
        setYearMonth(max_date_.year(), max_date_.month());
    }

    emit maxDateChanged(date);
}

void CalendarTable::setMinDate(const QDate& date)
{
    if (min_date_ == date)
        return;

    min_date_ = date;

    if (QDate(year_, month_, min_date_.day()) < min_date_) {
        setYearMonth(min_date_.year(), min_date_.month());
    }

    emit minDateChanged(date);
}

void CalendarTable::setDateRange(const QDate& start_date, const QDate& end_date)
{
    selected_dates_.clear();
    selected_dates_.push_back(start_date);
    selected_dates_.push_back(end_date);
}

bool CalendarTable::eventFilter(QObject* obj, QEvent* ev)
{
    if (obj == this) {
        if (ev->type() == QEvent::Wheel && isEnabled()) {
            auto wheel_event = static_cast<QWheelEvent*>(ev);
            if (wheel_event->delta() > 0) { // 当滚轮远离使用者时
                setYearMonth(year_, month_ - 1);
            } else {
                setYearMonth(year_, month_ + 1);
            }
            ev->accept();
            return true;
        }

        if (ev->type() == QEvent::Show) {
            refreshSelection();
        }
    }
    return QWidget::eventFilter(obj, ev);
}

void CalendarTable::itemClicked(QTableWidgetItem* item)
{
    if (!item->data(kDate).canConvert<QDate>())
        return;

    // doubleClickedEvent: clicked -> doubleClicked -> clicked
    if (!is_double_clicked_) {
        cliked_date_ = item->data(kDate).value<QDate>();
        // use QApplication::doubleClickInterval(). The delay is too high, affecting the processing of the click event.
        QTimer::singleShot(120, this, &CalendarTable::clicked);
    } else {
        is_double_clicked_ = false;
    }
}

void CalendarTable::itemDoubleClicked(QTableWidgetItem* item)
{
    if (!item->data(kDate).canConvert<QDate>())
        return;

    is_double_clicked_ = true;

    QDate date = item->data(kDate).value<QDate>();
    if (date > max_date_ || date < min_date_)
        return;

    emit dateDoubleClicked(date);
}

void CalendarTable::clicked()
{
    if (is_double_clicked_)
        return;

    QDate tmp_date = cliked_date_;

    switch (select_mode_) {
    case kNoSelection:
    case kSingleSelection:
    case kMultiSelection: {
        if (selected_dates_.contains(tmp_date)) {
            if (!removeSelection(tmp_date))
                return;
        } else {
            if (!appendSelection(tmp_date))
                return;
        }

        if (tmp_date.month() != month_) {
            setYearMonth(tmp_date.year(), tmp_date.month());
        } else {
            refreshSelection();
        }

        break;
    }
    case kRangeSelection: {
        if (!appendSelection(tmp_date))
            return;

        refreshSelection();
        break;
    }
    default:
        break;
    }
}

void CalendarTable::refreshCalendar()
{
    // Note: 计算日历第一天, 表格项(0, 0)
    QDate cur_date;
    cur_date.setDate(year_, month_, 1);
    int fill_days = cur_date.dayOfWeek() - first_day_on_week_;
    if (fill_days > 0) {
        cur_date = cur_date.addDays(-fill_days); //开始日期
    } else {
        cur_date = cur_date.addDays(-fill_days - 7);
    }

    // 日历填充
    int max_item_count = columnCount() * rowCount();
    for (int i = 0; i < max_item_count; ++i) {
        auto item = new QTableWidgetItem;
        item->setSizeHint(QSize(24, 24));
        item->setData(Qt::DisplayRole, cur_date.day());
        item->setData(kDate, cur_date);
        item->setTextAlignment(Qt::AlignCenter);
        item->setData(kFontColor, GET_COLOR("normal_text_font"));
        item->setData(kHoverBorderColor, GET_COLOR("normal_table_item_range"));

        // 非本月字体显示灰色
        if (cur_date.month() != month_) {
            item->setData(kAlpha, 0.2);
        }

        setItem(i / columnCount(), i % columnCount(), item);
        cur_date = cur_date.addDays(1);
    }

    refreshSelection();

    emit refresh(year_, month_);
}

void CalendarTable::refreshCalendarHeader()
{
    QStringList str_list = GET_TXT("IDCS_CALENDAR_WEEK").split(",");
    if (str_list.size() != kDaysInWeek) {
        return;
    }

    QStringList tmp_list;
    for (int i = 0; i < columnCount(); i++) {
        tmp_list << str_list.at((i + first_day_on_week_ - 1) % 7);
    }
    setHorizontalHeaderLabels(tmp_list);
}

void CalendarTable::refreshSelection()
{
    for (int row = 0; row < rowCount(); row++) {
        for (int col = 0; col < columnCount(); col++) {
            auto item = this->item(row, col);
            QDate date = item->data(kDate).value<QDate>();

            if (selected_dates_.contains(date)) {
                QColor bg_color = GET_COLOR("main_color");
                QString point_color_key =
                    (date.month() == month_) ? "normal_text_highlight_on_dark_font" : "normal_text_font";

                item->setData(kBgColor, bg_color);
                item->setData(kFontColor, GET_COLOR(point_color_key));
            } else {
                QVariant bg_color;
                if (select_mode_ != kRangeSelection) {
                    bg_color = special_dates_.contains(date) ? GET_COLOR("normal_table_item_range") : QVariant();
                }

                item->setData(kBgColor, bg_color);
                item->setData(kFontColor, GET_COLOR("normal_text_font"));
            }

            // Set current date style.
            if (date == QDate::currentDate()) {
                QColor point_color = GET_COLOR("main_color");
                if (item->data(kBgColor).canConvert<QColor>()) {
                    auto bg_color = item->data(kBgColor).value<QColor>();
                    if (bg_color == GET_COLOR("main_color") || bg_color == GET_COLOR("normal_table_item_range")) {
                        point_color = GET_COLOR("normal_white");
                    }
                }
                item->setData(kPointColor, point_color);
            } else {
                item->setData(kPointColor, QVariant());
            }
        }
    }

    update();
}

void CalendarTable::refreshSpecialDate()
{
    for (int row = 0; row < rowCount(); row++) {
        for (int col = 0; col < columnCount(); col++) {
            auto item = this->item(row, col);
            QDate date = item->data(kDate).toDate();

            if (!selected_dates_.isEmpty() && date == selected_dates_.at(0)) {
                item->setData(kBgColor, GET_COLOR("main_color"));
            } else if (special_dates_.contains(date)) {
                item->setData(kBgColor, GET_COLOR("normal_table_item_range"));
            } else {
                item->setData(kBgColor, QVariant());
            }
        }
    }

    update();
}

void CalendarTable::refreshTodayDate()
{
    startRefreshTimer();
    refreshCalendar();
}

void CalendarTable::startRefreshTimer()
{
    // 滞后3s
    auto current_time = QTime::currentTime();
    int duration = 24 * 3600 - (current_time.hour() * 3600 + current_time.minute() * 60 + current_time.second()) + 3;
    date_timer_->start(duration * 1000);
}

bool CalendarTable::appendSelection(const QDate& date)
{
    if (selected_dates_.contains(date) && select_mode_ != kRangeSelection)
        return false;

    if (date > max_date_ || date < min_date_)
        return false;

    switch (select_mode_) {
    case kSingleSelection:
        selected_dates_.clear();
        selected_dates_.append(date);
        break;
    case kMultiSelection:
        selected_dates_.append(date);
        break;
    case kRangeSelection: {
        selected_dates_.clear();
        selected_dates_.append(date);
        break;
    }
    default:
        return false;
    }

    emit selectionAdded(date);
    emit selectionChanged();
    return true;
}

bool CalendarTable::removeSelection(const QDate& date)
{
    if (!selected_dates_.contains(date))
        return false;

    // 单选则保证一定有一个选中
    if (select_mode_ == kSingleSelection && selected_dates_.count() <= 1) {
        return false;
    }

    selected_dates_.removeAll(date);
    emit selectionDeleted(date);

    return true;
}

void CalendarButtonGruop::setSelection(int id)
{
    bool is_find = false;

    for (int i = 0; i < button_group_.size(); ++i) {
        auto button = button_group_.at(i);
        if (button->property("id").toInt() == id) {
            button->setChecked(true);
            is_find = true;
        }
    }

    if (!is_find) {
        int first_show_in_current_year_page = QDate::currentDate().year() - 6;
        int show_year = first_show_in_current_year_page;

        if (id >= first_show_in_current_year_page) {
            int page = (id - first_show_in_current_year_page) / 12;
            show_year = first_show_in_current_year_page + page * 12;
        } else {
            int page = (first_show_in_current_year_page - id) / 12 + 1;
            show_year = first_show_in_current_year_page - page * 12;
        }

        for (auto button : button_group_) {
            button->setText(QString::number(show_year));
            button->setProperty("id", show_year);

            if (button->property("id").toInt() == id)
                button->setChecked(true);

            signal_mapper_->setMapping(button, show_year++);
        }
    }
}

YearButtonGroup::YearButtonGroup(int year, QWidget* parent)
    : CalendarButtonGruop(parent)
{
    setMinimumWidth(212);

    auto flow_layout = new FlowLayout(-1, 4, 20);
    signal_mapper_ = new QSignalMapper(this);

    for (int i = year - 6; i < year + 6; ++i) {
        auto button = new QPushButton(QString::number(i), this);
        button->setObjectName("calendar_btn_year_month");
        // button->setFixedSize(42, 20);
        button->setMinimumSize(42, 20);
        button->setCheckable(true);
        button->setAutoExclusive(true);
        button->setProperty("id", i);
        button_group_.push_back(button);
        connect(button, SIGNAL(clicked()), signal_mapper_, SLOT(map()));

        signal_mapper_->setMapping(button, i);
        flow_layout->addWidget(button);
    }
    connect(signal_mapper_, SIGNAL(mapped(int)), this, SIGNAL(yearClicked(int)));

    setLayout(flow_layout);
}

void YearButtonGroup::toPreYearPage(int current_year)
{
    int first_show_year = button_group_.at(0)->property("id").toInt();
    int show_year = first_show_year - 12;

    refreshYearPage(show_year, current_year);
}

void YearButtonGroup::toNextYearPage(int current_year)
{
    int first_show_year = button_group_.at(0)->property("id").toInt();
    int show_year = first_show_year + 12;

    refreshYearPage(show_year, current_year);
}

void YearButtonGroup::refreshYearPage(int show_year, int current_year)
{
    for (auto button : button_group_) {
        button->setText(QString::number(show_year));
        button->setProperty("id", show_year);
        if (button->property("id").toInt() == current_year) {
            button->setAutoExclusive(false);
            button->setChecked(true);
            button->setAutoExclusive(true);
        } else {
            button->setAutoExclusive(false);
            button->setChecked(false);
            button->setAutoExclusive(true);
        }

        signal_mapper_->setMapping(button, show_year++);
    }
}

MonthButtonGroup::MonthButtonGroup(QWidget* parent)
    : CalendarButtonGruop(parent)
{
    setMinimumWidth(212);

    QStringList month_list;
    for (int i = 1; i <= 12; ++i) {
        month_list << GET_TXT(QString("IDCS_MONTH_%1").arg(i));
    }

    auto flow_layout = new FlowLayout(-1, 4, 20);
    signal_mapper_ = new QSignalMapper(this);

    for (int i = 0; i < 12; ++i) {
        auto button = new QPushButton(month_list.at(i), this);
        button->setObjectName("calendar_btn_year_month");
        // button->setFixedSize(42, 20);
        button->setMinimumSize(42, 20);
        button->setCheckable(true);
        button->setAutoExclusive(true);
        button->setProperty("id", i + 1);
        button_group_.push_back(button);
        connect(button, SIGNAL(clicked()), signal_mapper_, SLOT(map()));

        signal_mapper_->setMapping(button, i + 1);
        flow_layout->addWidget(button);
    }
    connect(signal_mapper_, SIGNAL(mapped(int)), this, SIGNAL(monthClicked(int)));

    setLayout(flow_layout);
}

#include "moc_calendartable.cpp"

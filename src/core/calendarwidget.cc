#include "calendarwidget.h"

#include "flowlayout.h"
#include "iconfont/iconwidget.h"
#include "styleresourcepool.h"

constexpr int kDaysInWeek = 7;
constexpr int kPointRadius = 1;
constexpr int kCalendarRows = 6;

CalendarWidget::CalendarWidget(QWidget* parent, int year, int month)
    : QWidget(parent)
{
    setAttribute(Qt::WA_StyledBackground);

    calendar_ = new CalendarTable(this, year, month);
    title_ = new QLabel("title", this);
    title_->setFixedHeight(20);
    title_->setObjectName("normal_itetitle_");
    title_->hide();

    auto month_key = QString("IDCS_MONTH_%1").arg(QString::number(calendar_->month()));
    month_btn_ = new QPushButton(GET_TXT(month_key), this);
    month_btn_->setProperty("month", calendar_->month());
    month_btn_->setCursor(Qt::PointingHandCursor);
    month_btn_->setFixedSize(48, 24);
    month_btn_->setObjectName("calendar_btn");

    year_btn_ = new QPushButton(QString::number(calendar_->year()), this);
    year_btn_->setCursor(Qt::PointingHandCursor);
    year_btn_->setFixedSize(48, 24);
    year_btn_->setObjectName("calendar_btn");

    pre_btn_ = new IconButton(DefaultFont::ICON_ZUOYOUJIANTOU1, this);
    pre_btn_->setFixedSize(16, 16);

    next_btn_ = new IconButton(DefaultFont::ICON_YOU, this);
    next_btn_->setFixedSize(16, 16);

    month_button_group_ = new MonthButtonGroup(this);
    year_button_group_ = new YearButtonGroup(year, this);

    // 信号槽关联
    connect(calendar_, &CalendarTable::refresh, this, &CalendarWidget::refresh);
    connect(calendar_, &CalendarTable::refresh, this, &CalendarWidget::refresh);
    connect(calendar_, &CalendarTable::maxDateChanged, this, &CalendarWidget::calendarMaxDateChanged);
    connect(calendar_, &CalendarTable::minDateChanged, this, &CalendarWidget::calendarMinDateChanged);
    connect(calendar_, &CalendarTable::selectionChanged, this, &CalendarWidget::calendarSelectionChanged);
    connect(calendar_, &CalendarTable::selectionAdded, this, &CalendarWidget::calendarSelectionAdded);
    connect(calendar_, &CalendarTable::selectionDeleted, this, &CalendarWidget::calendarSelectionDeleted);
    connect(calendar_, &CalendarTable::dateDoubleClicked, this, &CalendarWidget::dateDoubleClicked);
    connect(pre_btn_, &QPushButton::clicked, this, &CalendarWidget::pre);
    connect(next_btn_, &QPushButton::clicked, this, &CalendarWidget::next);
    connect(month_btn_, &QPushButton::clicked, this, &CalendarWidget::viewChanged);
    connect(year_btn_, &QPushButton::clicked, this, &CalendarWidget::viewChanged);
    connect(month_button_group_, &MonthButtonGroup::monthClicked, this, &CalendarWidget::monthClicked);
    connect(year_button_group_, &YearButtonGroup::yearClicked, this, &CalendarWidget::yearClicked);

    // 布局
    auto title_layout = new QHBoxLayout;
    title_layout->addWidget(title_);
    title_layout->addStretch();

    auto btn_layout = new QHBoxLayout;
    btn_layout->addStretch();
    btn_layout->addWidget(pre_btn_);
    btn_layout->addSpacing(12);
    btn_layout->addWidget(month_btn_);
    btn_layout->addSpacing(4);
    btn_layout->addWidget(year_btn_);
    btn_layout->addSpacing(12);
    btn_layout->addWidget(next_btn_);
    btn_layout->addStretch();

    auto calendar_table_widget = new QWidget(this);
    auto calendar_layout = new QHBoxLayout(calendar_table_widget);
    calendar_layout->setSpacing(0);
    calendar_layout->setContentsMargins(8, 0, 8, 0);
    calendar_layout->addWidget(calendar_);

    view_stacked_widget_ = new QStackedWidget(this);
    view_stacked_widget_->setContentsMargins(0, 0, 0, 0);
    view_stacked_widget_->insertWidget(kViewDate, calendar_table_widget);
    view_stacked_widget_->insertWidget(kViewMonth, month_button_group_);
    view_stacked_widget_->insertWidget(kViewYear, year_button_group_);

    main_layout_ = new QVBoxLayout(this);
    main_layout_->setMargin(0);
    main_layout_->setContentsMargins(8, 8, 8, 8);
    main_layout_->setSpacing(4);
    main_layout_->addLayout(title_layout);
    main_layout_->addLayout(btn_layout);
    main_layout_->addWidget(view_stacked_widget_);
    main_layout_->addStretch();

    initAnimation();

    refreshNextEnable();
    refreshPreEnable();
}

void CalendarWidget::setTitle(const QString& title)
{
    if (title_->isHidden())
        title_->show();
    title_->setText(title);
}

void CalendarWidget::refresh(int year, int month)
{
    month_btn_->setText(GET_TXT(QString("IDCS_MONTH_%1").arg(QString::number(month))));
    month_btn_->setProperty("month", month);
    year_btn_->setText(QString::number(year));

    refreshNextEnable();
    refreshPreEnable();
}

void CalendarWidget::calendarMaxDateChanged(const QDate& /*date*/)
{
    refreshNextEnable();
}

void CalendarWidget::calendarMinDateChanged(const QDate& /*date*/)
{
    refreshPreEnable();
}

void CalendarWidget::viewChanged()
{
    pre_btn_->setEnabled(true);
    next_btn_->setEnabled(true);

    if (sender() == month_btn_) {
        if (view_stacked_widget_->currentIndex() == kViewMonth) {
            view_stacked_widget_->setCurrentIndex(kViewDate);
        } else {
            month_button_group_->setSelection(month_btn_->property("month").toInt());
            view_stacked_widget_->setCurrentIndex(kViewMonth);
            pre_btn_->setEnabled(false);
            next_btn_->setEnabled(false);
        }
    } else {
        if (view_stacked_widget_->currentIndex() == kViewYear) {
            view_stacked_widget_->setCurrentIndex(kViewDate);
        } else {
            year_button_group_->setSelection(year_btn_->text().toInt());
            view_stacked_widget_->setCurrentIndex(kViewYear);
        }
    }

    int x = view_stacked_widget_->x();
    int y = view_stacked_widget_->y();
    int w = view_stacked_widget_->rect().width();
    int h = view_stacked_widget_->rect().height();
    geometry_animation_->setStartValue(QRect(x + w / 2, y + h / 2, 0, 0));
    geometry_animation_->setEndValue(QRect(x, y, w, h));

    animation_group_->start();
}

void CalendarWidget::pre()
{
    if (view_stacked_widget_->currentIndex() == kViewYear) {
        year_button_group_->toPreYearPage(year_btn_->text().toInt());
    } else {
        calendar_->setYearMonth(calendar_->year(), calendar_->month() - 1);
    }
}

void CalendarWidget::next()
{
    if (view_stacked_widget_->currentIndex() == kViewYear) {
        year_button_group_->toNextYearPage(year_btn_->text().toInt());
    } else {
        calendar_->setYearMonth(calendar_->year(), calendar_->month() + 1);
    }
}

void CalendarWidget::monthClicked(int month)
{
    calendar_->setYearMonth(calendar_->year(), month);
    view_stacked_widget_->setCurrentIndex(kViewDate);
    pre_btn_->setEnabled(true);
    next_btn_->setEnabled(true);
}

void CalendarWidget::yearClicked(int year)
{
    calendar_->setYearMonth(year, calendar_->month());
    view_stacked_widget_->setCurrentIndex(kViewDate);
    pre_btn_->setEnabled(true);
    next_btn_->setEnabled(true);
}

void CalendarWidget::initAnimation()
{
    auto graphics_opacity_effect = new QGraphicsOpacityEffect(this);
    graphics_opacity_effect->setOpacity(1.0);
    view_stacked_widget_->setGraphicsEffect(graphics_opacity_effect);

    auto opacity_animation = new QPropertyAnimation(graphics_opacity_effect, "opacity", this);
    opacity_animation->setStartValue(0.0);
    opacity_animation->setEndValue(1.0);
    opacity_animation->setDuration(300);
    opacity_animation->setEasingCurve(QEasingCurve::Linear);

    geometry_animation_ = new QPropertyAnimation(view_stacked_widget_, "geometry");
    geometry_animation_->setDuration(300);
    geometry_animation_->setEasingCurve(QEasingCurve::Linear);

    animation_group_ = new QParallelAnimationGroup(this);
    animation_group_->addAnimation(opacity_animation);
    animation_group_->addAnimation(geometry_animation_);
}

void CalendarWidget::refreshNextEnable()
{
    if (view_stacked_widget_->currentIndex() == kViewMonth)
        return;

    next_btn_->setEnabled(QDate(calendar_->year(), calendar_->month(), 1)
                          < QDate(calendar_->max_date().year(), calendar_->max_date().month(), 1));
}

void CalendarWidget::refreshPreEnable()
{
    if (view_stacked_widget_->currentIndex() == kViewMonth)
        return;

    pre_btn_->setEnabled(QDate(calendar_->year(), calendar_->month(), calendar_->min_date().day())
                         > QDate(calendar_->min_date().year(), calendar_->min_date().month(), 1));
}

#include "moc_calendarwidget.cpp"

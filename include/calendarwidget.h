#ifndef CALANDARWIDGET_H_
#define CALANDARWIDGET_H_

#include <QHeaderView>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>

#include "calendartable.h"
#include "datetimeedit_global.h"

/**
 * @brief 日历控件
 * 欢迎加入Qt技术交流群: 884099523, QQ: 1085080350
 *
 * @author xiaozhaoGithub
 * @date 2024-03-02
 */
class DATETIMEEDIT_EXPORT CalendarWidget : public QWidget
{
    Q_OBJECT
public:
    CalendarWidget(QWidget* parent, int year = QDate::currentDate().year(), int month = QDate::currentDate().month());

    void addWidgetBottom(QWidget* widget) { main_layout_->addWidget(widget); }
    void addLayoutBottom(QLayout* layout) { main_layout_->addLayout(layout); }
    void addSelectedDate(const QDate& date) { calendar_->addSelectedDate(date); }
    void setSpecialDate(const QList<QDate>& dates) { calendar_->setSpecialDate(dates); }
    void deleteSelectedDate(const QDate& date) { calendar_->deleteSelectedDate(date); }
    void clearSelectedDate() { calendar_->clearSelectedDate(); }

    bool setYearMonth(int year, int month) { return calendar_->setYearMonth(year, month); }
    void setSelMode(CalendarTable::CalendarSelectionMode select_mode) { calendar_->setSelMode(select_mode); }
    void setMaxDate(const QDate& date) { calendar_->setMaxDate(date); }
    void setMinDate(const QDate& date) { calendar_->setMinDate(date); }
    void setDateRange(const QDate& start_date, const QDate& end_date) { calendar_->setDateRange(start_date, end_date); }
    void setTitle(const QString& title);

    QDate max_date() { return calendar_->max_date(); }
    QDate min_date() { return calendar_->min_date(); }
    QList<QDate> selected_date() { return calendar_->selected_date(); }

    int year() { return calendar_->year(); }
    int month() { return calendar_->month(); }

signals:
    void calendarSelectionChanged();
    void calendarSelectionAdded(const QDate& date);
    void calendarSelectionDeleted(const QDate& date);
    void calendarRefresh(int year, int month);
    void dateDoubleClicked(const QDate& date);

private slots:
    void refresh(int year, int month);
    void calendarMaxDateChanged(const QDate& date);
    void calendarMinDateChanged(const QDate& date);
    void viewChanged();
    void pre();
    void next();
    void monthClicked(int month);
    void yearClicked(int year);

private:
    enum CalendarViewType
    {
        kViewDate,
        kViewMonth,
        kViewYear
    };

    void initAnimation();

    void refreshNextEnable();
    void refreshPreEnable();

private:
    QLabel* title_;
    QPushButton* month_btn_;
    QPushButton* year_btn_;
    class IconButton* pre_btn_;
    class IconButton* next_btn_;

    CalendarTable* calendar_;
    MonthButtonGroup* month_button_group_;
    YearButtonGroup* year_button_group_;

    QStackedWidget* view_stacked_widget_;
    QVBoxLayout* main_layout_;

    QPropertyAnimation* geometry_animation_;
    QParallelAnimationGroup* animation_group_;
};

#endif // DateTimeEdit_H_

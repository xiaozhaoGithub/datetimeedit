#ifndef DATETIMEPICKER_H_
#define DATETIMEPICKER_H_

#include <QLineEdit>
#include <QTimeEdit>

#include "calendarwidget.h"

class DateTimePicker : public QWidget
{
    Q_OBJECT
public:
    DateTimePicker(QWidget* parent, int year = QDate::currentDate().year(), int month = QDate::currentDate().month());

public:
    void setDateTime(const QDateTime& date_time);
    QDateTime dateTime();

    QDate date() { return calendar_widget_->selected_date().at(0); };
    QList<QDate> selected_date() { return calendar_widget_->selected_date(); };
    QTime time() { return time_edit_->time(); }

signals:
    void timeChanged(const QTime& time);
    void dateChanged(const QDate& date_time);

private slots:
    void calendarSelectionChanged();

protected:
    CalendarWidget* calendar_widget_;
    QTimeEdit* time_edit_;
};

class DatePicker : public DateTimePicker
{
    Q_OBJECT
public:
    DatePicker(QWidget* parent, int year = QDate::currentDate().year(), int month = QDate::currentDate().month());
};

class DateTimeRangePicker : public QWidget
{
    Q_OBJECT
public:
    DateTimeRangePicker(QWidget* parent, int year = QDate::currentDate().year(), int month = QDate::currentDate().month());

    void setStartDateTime(const QDateTime& start);
    void setEndDateTime(const QDateTime& end);

    void setDateTimeRange(const QDateTime& start, const QDateTime& end);
    void timeRange(QTime& start_time, QTime& end_time);
    QPair<QTime, QTime> timeRange();

    void dateRange(QDate& start_date, QDate& end_date);
    QPair<QDate, QDate> dateRange();

signals:
    void timeRangeChanged();
    void dateRangeChanged();

protected:
    void paintEvent(QPaintEvent* event) override;

    void refreshCalendarSelection();
    void connectSelectionAdded();
    void disconnectSelectionAdded();

private slots:
    void calendarSelectionAdded(const QDate& date);
    void calendarRefresh(int year, int month);
    void dateDoubleClicked(const QDate& date);
    void timeChanged();

private:
    void showError(QTimeEdit* const time_edit, const QTime& time);

protected:
    CalendarWidget* start_calendar_;
    CalendarWidget* end_calendar_;
    QTimeEdit* start_time_edit_;
    QTimeEdit* end_time_edit_;

    QPair<QDate, QDate> date_range_pair_;
    // QList<QDate> selected_dates_;// 保证只有两个日期
};

class DateRangePicker : public DateTimeRangePicker
{
    Q_OBJECT
public:
    DateRangePicker(QWidget* parent, int year = QDate::currentDate().year(), int month = QDate::currentDate().month());
};

class SimpleDateTimeEdit : public QDateTimeEdit
{
    Q_OBJECT
public:
    SimpleDateTimeEdit(QWidget* parent);
    QLineEdit* innerLineEdit() { return lineEdit(); };

signals:
    void cursorPositionChanged(int old_pos, int new_pos);
    void corsorRight();
    void corsorLeft();

protected:
    void keyPressEvent(QKeyEvent* ev) override;
};

#endif //

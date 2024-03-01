#ifndef DATETIMEEDIT_H_
#define DATETIMEEDIT_H_

#include <QDateTimeEdit>
#include <QScrollArea>
#include <QScrollBar>

#include "calendarwidget.h"
#include "datetimepicker.h"

/**
 * @brief 日期编辑控件
 * 欢迎加入Qt技术交流群: 884099523, QQ: 1085080350
 *
 * @author xiaozhaoGithub
 * @date 2024-03-02
 */
class DATETIMEEDIT_EXPORT DateTimeEdit : public QFrame
{
    Q_OBJECT
public:
    enum DateEditType
    {
        kDateTime,
        kDate,
        kDateTimeRange,
        kDateRange,
    };

    DateTimeEdit(QWidget* parent, DateEditType type = kDateTime, int year = QDate::currentDate().year(),
                 int month = QDate::currentDate().month());

    void setDate(const QDate& date);
    void setDateTime(const QDateTime& date_time);
    void setStartDateTime(const QDateTime& start);
    void setEndDateTime(const QDateTime& end);
    void setDateTimeRange(const QDateTime& start, const QDateTime& end);
    void resetDateTime();

    QDate date();
    QList<QDate> selected_date();
    QTime time();
    QDateTime start_date_time();
    QDateTime end_date_time();

    bool isValidDateTimeRange();
    bool isDifferentYearOrMonth();

signals:
    void startDateTimeChanged(const QDateTime& date_time);
    void endDateTimeChanged(const QDateTime& dateTim);

protected:
    void resizeEvent(QResizeEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void cursorPositionChanged(int old_pos, int new_pos);
    void cursorJump();
    void dateTimeChanged(const QDateTime& date_time);
    void editBtnClicked();
    void timeChanged(const QTime& time);
    void dateChanged(const QDate& date);
    void timeRangeChanged();
    void dateRangeChanged();

private:
    void initDateFormat(DateEditType type, int year, int month);
    void setCursorPosInVisivle(SimpleDateTimeEdit* simple_edit, int index);

private:
    SimpleDateTimeEdit* start_edit_;
    SimpleDateTimeEdit* end_edit_;
    QScrollArea* scroll_area_;

    QFrame* popup_widget_;
    QStackedLayout* stacked_layout_;

    DateTimePicker* date_time_picker_;
    DatePicker* date_picker_;
    DateTimeRangePicker* date_time_range_picker_;
    DateRangePicker* date_range_picker_;

    QString date_format_;
    QString real_text_;

    DateEditType type_;
};

#endif // DateTimeEdit_H_

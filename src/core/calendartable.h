#ifndef CALENDARTABLE_H_
#define CALENDARTABLE_H_

#include <QDate>
#include <QEvent>
#include <QLabel>
#include <QPushButton>
#include <QSignalMapper>
#include <QStackedLayout>
#include <QStackedWidget>
#include <QStyledItemDelegate>
#include <QTableWidget>
#include <QTimer>

class DrawBaseDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    DrawBaseDelegate(QObject* parent);

protected:
    void drawBackgroud(QPainter* painter, const QRect& rect, const QColor& color) const;
    void drawBorder(QPainter* painter, const QRect& rect, const QColor& color) const;
    void drawText(QPainter* painter, const QRect& rect, const QColor& color, const QString& text,
                  const Qt::AlignmentFlag align = Qt::AlignCenter) const;
    void drawPoint(QPainter* painter, const QRect& rect, const QColor& color) const;
};

class CalendarDelegate : public DrawBaseDelegate
{
    Q_OBJECT
public:
    CalendarDelegate(QAbstractItemView* parent);

protected:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    bool eventFilter(QObject* obj, QEvent* ev) override;

private:
    bool colorByRole(const QModelIndex& model_index, int role, double alpha, QColor& tmp_color) const;

private:
    QAbstractItemView* parent_view_;
    QModelIndex hover_index_;
};

class CalendarTable : public QTableWidget
{
    Q_OBJECT
public:
    CalendarTable(QWidget* parent, int year = QDate::currentDate().year(), int month = QDate::currentDate().month());

    enum CalendarRoleType
    {
        kDate = Qt::UserRole,
        kBgColor,
        kBorderColor,
        kHoverBorderColor,
        kFontColor,
        kPointColor,
        kAlpha, // 整体不透明度
    };

    enum CalendarSelectionMode
    {
        kNoSelection,
        kSingleSelection,
        kMultiSelection,
        kRangeSelection
    };

public:
    bool setYearMonth(int year, int month);
    void setFristDayOnWeek(int first_day_on_week);
    void setSelMode(CalendarSelectionMode select_mode);
    void setSpecialDate(const QList<QDate>& dates);
    void addSelectedDate(const QDate& date);
    void deleteSelectedDate(const QDate& date);
    void clearSelectedDate();
    void setMaxDate(const QDate& date);
    void setMinDate(const QDate& date);
    void setDateRange(const QDate& start_date, const QDate& end_date);

    int year() { return year_; }
    int month() { return month_; }
    int first_day_on_week() { return first_day_on_week_; }
    CalendarSelectionMode select_mode() { return select_mode_; }
    QList<QDate> selected_date() { return selected_dates_; }
    QDate max_date() { return max_date_; }
    QDate min_date() { return min_date_; }

signals:
    void refresh(int year, int month);
    void selectionChanged();
    void selectionAdded(const QDate& date);
    void selectionDeleted(const QDate& date);
    void maxDateChanged(const QDate& date);
    void minDateChanged(const QDate& date);
    void dateDoubleClicked(const QDate& date);

protected:
    bool eventFilter(QObject* obj, QEvent* ev) override;

private slots:
    void itemClicked(QTableWidgetItem* item);
    void itemDoubleClicked(QTableWidgetItem* item);
    void clicked();

private:
    void refreshCalendar();
    void refreshCalendarHeader();
    void refreshSelection();
    void refreshSpecialDate();
    void refreshTodayDate();
    void startRefreshTimer();

    bool appendSelection(const QDate& date);
    bool removeSelection(const QDate& date);

private:
    int year_;
    int month_;
    int first_day_on_week_;
    QDate max_date_;
    QDate min_date_;
    QDate cliked_date_;
    bool is_double_clicked_;

    QTimer* date_timer_;
    QList<QDate> selected_dates_;
    QList<QDate> special_dates_;
    CalendarSelectionMode select_mode_;
};

class CalendarButtonGruop : public QWidget
{
    Q_OBJECT
public:
    explicit CalendarButtonGruop(QWidget* parent = nullptr)
        : QWidget(parent){};
    void setSelection(int id);

protected:
    QList<QPushButton*> button_group_;
    QSignalMapper* signal_mapper_;
};

class YearButtonGroup : public CalendarButtonGruop
{
    Q_OBJECT
public:
    explicit YearButtonGroup(int year, QWidget* parent = nullptr);

    void toPreYearPage(int current_year);
    void toNextYearPage(int current_year);

signals:
    void yearClicked(int year);

private:
    void refreshYearPage(int show_year, int current_year);
};

class MonthButtonGroup : public CalendarButtonGruop
{
    Q_OBJECT
public:
    explicit MonthButtonGroup(QWidget* parent = nullptr);

signals:
    void monthClicked(int month);
};

#endif // CALENDARTABLE_H_

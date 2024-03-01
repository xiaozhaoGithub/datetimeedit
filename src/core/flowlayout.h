#ifndef FLOWLAYOUT_H_
#define FLOWLAYOUT_H_

#include <QObject>

QT_BEGIN_NAMESPACE
#include <QLayout>
#include <QScopedPointer>
#include <QtWidgets>
QT_END_NAMESPACE

class QRect;
class QStyle;
class FlowLayoutPrivate;

/**
 * @brief ��ʽ���֣���������Ӧ���ڱ仯
 */
class FlowLayout : public QLayout
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(FlowLayout)

public:
    explicit FlowLayout(QWidget* parent, int margin = -1, int hSpacing = -1, int vSpacing = -1);
    explicit FlowLayout(int margin = -1, int hSpacing = -1, int vSpacing = -1);
    ~FlowLayout();
    void setCenterEnable(bool enable);
    void addItem(QLayoutItem* item) override;
    int count() const override;
    QLayoutItem* itemAt(int index) const override;
    QLayoutItem* takeAt(int index) override;
    int horizontalSpacing();
    int verticalSpacing();
    Qt::Orientations expandingDirections() const override;
    bool hasHeightForWidth() const override;
    int heightForWidth(int) const override;
    QSize minimumSize() const override;
    void setGeometry(const QRect& rect) override;
    QSize sizeHint() const override;

Q_SIGNALS:
    // ���������˶���Ļ���������ֵ
    void layoutMarginChange(int);

private:
    QList<QLayoutItem*> m_itemList;
    QScopedPointer<FlowLayoutPrivate> d_ptr;
};

#endif // FLOWLAYOUT_H

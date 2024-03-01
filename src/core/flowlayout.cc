#include "flowlayout.h"

#include <QRect>
#include <QStyle>

class FlowLayoutPrivate
{
public:
    Q_DECLARE_PUBLIC(FlowLayout)
    FlowLayoutPrivate(int hSpacing, int vSpacing);
    int doLayout(const QRect& rect, bool testOnly) const;
    int smartSpacing(QStyle::PixelMetric pm) const;

    bool m_center;
    int m_hSpace;
    int m_vSpace;
    mutable int m_leftMargin;
    FlowLayout* q_ptr;
};

FlowLayoutPrivate::FlowLayoutPrivate(int hSpacing, int vSpacing)
    : m_center(false)
    , m_hSpace(hSpacing)
    , m_vSpace(vSpacing)
    , m_leftMargin(0)
    , q_ptr(nullptr)
{}

int FlowLayoutPrivate::doLayout(const QRect& rect, bool testOnly) const
{
    if (!q_ptr) {
        return -1;
    }

    int left, top, right, bottom;
    q_ptr->getContentsMargins(&left, &top, &right, &bottom);

    if (m_center) {
        static QSize m_itemSize;
        //设置两边margin对齐
        if (q_ptr && !q_ptr->m_itemList.isEmpty()) {
            if (q_ptr->m_itemList.at(0)->sizeHint().width() > 0) {
                m_itemSize = q_ptr->m_itemList.at(0)->sizeHint();
                left = ((rect.width() - 30) % q_ptr->m_itemList.at(0)->sizeHint().width()) / 2;
                m_leftMargin = left;
            }
        } else {
            left = ((rect.width() - 30) % m_itemSize.width()) / 2;
            m_leftMargin = left;
        }
    }

    QRect effectiveRect = rect.adjusted(+left, +top, -right, -bottom);
    int x = effectiveRect.x();
    int y = effectiveRect.y();
    int lineHeight = 0;

    QLayoutItem* item;
    foreach (item, q_ptr->m_itemList) {
        QWidget* wid = item->widget();
        int spaceX = q_ptr->horizontalSpacing();
        if (spaceX == -1)
            spaceX = wid->style()->layoutSpacing(QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Horizontal);
        int spaceY = q_ptr->verticalSpacing();
        if (spaceY == -1)
            spaceY = wid->style()->layoutSpacing(QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Vertical);

        // 控件隐藏时不考虑间距
        if (item->sizeHint() == QSize(0, 0)) {
            spaceX = 0;
            spaceY = 0;
        }

        int nextX = x + item->sizeHint().width() + spaceX;
        if (nextX - spaceX > effectiveRect.right() && lineHeight > 0) {
            x = effectiveRect.x();
            y = y + lineHeight + spaceY;
            nextX = x + item->sizeHint().width() + spaceX;
            lineHeight = 0;
        }

        if (!testOnly)
            item->setGeometry(QRect(QPoint(x, y), item->sizeHint()));

        x = nextX;

        lineHeight = qMax(lineHeight, item->sizeHint().height());
    }
    return y + lineHeight - rect.y() + bottom;
}

int FlowLayoutPrivate::smartSpacing(QStyle::PixelMetric pm) const
{
    QObject* parent = q_ptr->parent();

    if (!parent) {
        return -1;
    } else if (parent->isWidgetType()) {
        QWidget* pw = static_cast<QWidget*>(parent);
        return pw->style()->pixelMetric(pm, 0, pw);
    } else {
        return static_cast<QLayout*>(parent)->spacing();
    }
}

FlowLayout::FlowLayout(QWidget* parent, int margin, int hSpacing, int vSpacing)
    : QLayout(parent)
    , d_ptr(new FlowLayoutPrivate(hSpacing, vSpacing))
{
    //	Q_D(FlowLayout);
    m_itemList.clear();
    d_ptr->q_ptr = this;
    setContentsMargins(margin, margin, margin, margin);
}

FlowLayout::FlowLayout(int margin, int hSpacing, int vSpacing)
    : d_ptr(new FlowLayoutPrivate(hSpacing, vSpacing))
{
    Q_D(FlowLayout);
    m_itemList.clear();
    d->q_ptr = this;
    setContentsMargins(margin, margin, margin, margin);
}

FlowLayout::~FlowLayout()
{
    QLayoutItem* item;


    while ((item = takeAt(0)))
        delete item;
}

void FlowLayout::setCenterEnable(bool enable)
{
    Q_D(FlowLayout);
    d->m_center = enable;
}

void FlowLayout::addItem(QLayoutItem* item)
{
    Q_D(FlowLayout);
    if (!item->widget()) {
        return;
    }

    if (d->m_center) {
        //-1为收藏列表，末端插入，其余按照初始化顺序排列
        const int collect = item->widget()->property("collectIndex").toInt();

        if (collect == -1) {
            m_itemList.push_front(item);
        } else {
            m_itemList.append(item);
            //按照插入顺序进行排序重新渲染

            std::sort(m_itemList.begin(), m_itemList.end(), [](QLayoutItem* left, QLayoutItem* right) {
                int lIndex = left->widget()->property("index").toInt();
                int rIndex = right->widget()->property("index").toInt();
                return lIndex < rIndex;
            });
        }
    } else {
        m_itemList.append(item);
    }
}

int FlowLayout::horizontalSpacing()
{
    Q_D(FlowLayout);

    if (d->m_hSpace >= 0) {
        return d->m_hSpace;
    } else {
        return d->smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
    }
}

int FlowLayout::verticalSpacing()
{
    Q_D(FlowLayout);

    if (d->m_vSpace >= 0) {
        return d->m_vSpace;
    } else {
        return d->smartSpacing(QStyle::PM_LayoutVerticalSpacing);
    }
}

int FlowLayout::count() const
{
    return m_itemList.size();
}

QLayoutItem* FlowLayout::itemAt(int index) const
{
    return m_itemList.value(index);
}

QLayoutItem* FlowLayout::takeAt(int index)
{
    if (index >= 0 && index < m_itemList.size())
        return m_itemList.takeAt(index);
    else
        return 0;
}

Qt::Orientations FlowLayout::expandingDirections() const
{
    return Qt::Orientations();
}

bool FlowLayout::hasHeightForWidth() const
{
    return true;
}

int FlowLayout::heightForWidth(int width) const
{
    int height = d_ptr->doLayout(QRect(0, 0, width, 0), true);
    return height;
}

void FlowLayout::setGeometry(const QRect& rect)
{
    Q_D(FlowLayout);
    QLayout::setGeometry(rect);
    d->doLayout(rect, false);
    emit layoutMarginChange(d->m_leftMargin);
}

QSize FlowLayout::sizeHint() const
{
    return minimumSize();
}

QSize FlowLayout::minimumSize() const
{
    QSize size;
    QLayoutItem* item;

    foreach (item, m_itemList)
        size = size.expandedTo(item->minimumSize());

    size += QSize(2 * margin(), 2 * margin());
    return size;
}

#include "moc_flowlayout.cpp"

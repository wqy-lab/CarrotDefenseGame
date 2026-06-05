#include "tutorialarrow.h"
#include <QPainter>
#include <QtMath>

TutorialArrow::TutorialArrow(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

void TutorialArrow::setTarget(QPoint from, QPoint to, ArrowDirection dir)
{
    m_from = from;
    m_to = to;
    m_direction = dir;
    update();
}

void TutorialArrow::setPixmap(const QPixmap& pm)
{
    m_pixmap = pm;
    update();
}

void TutorialArrow::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    if (m_from.isNull() || m_to.isNull()) return;

    QPainter p(this);

    // Use pixmap if available
    if (!m_pixmap.isNull()) {
        int hw = m_pixmap.width() / 2;
        int hh = m_pixmap.height() / 2;
        p.save();
        p.translate(m_to);
        switch (m_direction) {
        case ArrowDirection::Up:    break;
        case ArrowDirection::Down:  p.rotate(180); break;
        case ArrowDirection::Left:  p.rotate(-90); break;
        case ArrowDirection::Right: p.rotate(90);  break;
        }
        p.drawPixmap(-hw, -hh, m_pixmap);
        p.restore();
        return;
    }

    // Fallback: draw a triangle arrow with QPainter
    p.setRenderHint(QPainter::Antialiasing);

    // Draw shaft line
    p.setPen(QPen(QColor(255, 200, 50), 3));
    p.drawLine(m_from, m_to);

    // Draw arrowhead triangle at m_to
    QPointF tip(m_to);
    const double headSize = 12.0;
    QPointF base;

    double angle = 0.0;
    switch (m_direction) {
    case ArrowDirection::Up:    angle = -M_PI_2; break;
    case ArrowDirection::Down:  angle =  M_PI_2; break;
    case ArrowDirection::Left:  angle =  M_PI;   break;
    case ArrowDirection::Right: angle =  0.0;    break;
    }
    base = tip - QPointF(std::cos(angle) * headSize, std::sin(angle) * headSize);
    QPointF left = base + QPointF(std::cos(angle + 2.5) * headSize * 0.6,
                                   std::sin(angle + 2.5) * headSize * 0.6);
    QPointF right = base + QPointF(std::cos(angle - 2.5) * headSize * 0.6,
                                    std::sin(angle - 2.5) * headSize * 0.6);

    p.setPen(Qt::NoPen);
    p.setBrush(QColor(255, 200, 50));
    QPolygonF head;
    head << tip << left << right;
    p.drawPolygon(head);
}

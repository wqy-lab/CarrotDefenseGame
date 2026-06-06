#include "tutorialarrow.h"
#include <QPainter>
#include <QMouseEvent>
#include <QtMath>

TutorialArrow::TutorialArrow(QWidget* parent)
    : QWidget(parent)
{
}

void TutorialArrow::mousePressEvent(QMouseEvent* event)
{
    emit clicked();
    event->accept();
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

void TutorialArrow::setMessage(const QString& msg)
{
    m_message = msg;
    update();
}

void TutorialArrow::setHighlightRect(const QRectF& rect)
{
    m_highlightRect = rect;
    m_showHighlight = true;

    QRegion mask(QRect(QPoint(0, 0), parentWidget()->size()));
    mask = mask.subtracted(QRegion(rect.toRect()));
    setMask(mask);

    update();
}

void TutorialArrow::clearHighlight()
{
    m_showHighlight = false;
    m_message.clear();
    clearMask();
    update();
}

void TutorialArrow::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter p(this);

    // ── Overlay highlight mask (for interactive steps) ──
    if (m_showHighlight && m_highlightRect.isValid()) {
        // Full-screen semi-transparent overlay
        p.fillRect(rect(), QColor(0, 0, 0, 140));

        // Cut hole at target cell so mouse events pass through
        p.setCompositionMode(QPainter::CompositionMode_Clear);
        p.fillRect(m_highlightRect, Qt::transparent);
        p.setCompositionMode(QPainter::CompositionMode_SourceOver);

        // Golden dashed border around target cell
        QPen pen(QColor(255, 200, 50), 3, Qt::DashLine);
        p.setPen(pen);
        p.setBrush(Qt::NoBrush);
        p.drawRect(m_highlightRect.adjusted(1, 1, -1, -1));
    }

    // ── Message text at bottom ──
    if (!m_message.isEmpty()) {
        QFont font("Microsoft YaHei", 14);
        p.setFont(font);
        p.setPen(QColor(255, 255, 255));
        QRectF textRect(rect().x() + 20, rect().bottom() - 170,
                        rect().width() - 40, 150);
        p.drawText(textRect, Qt::AlignCenter | Qt::TextWordWrap, m_message);
    }

    // ── Arrow shaft + head (skip if no target) ──
    if (m_from.isNull() || m_to.isNull()) return;

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

    // Fallback QPainter arrow
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(QColor(255, 200, 50), 3));
    p.drawLine(m_from, m_to);

    QPointF tip(m_to);
    const double headSize = 12.0;
    double angle = 0.0;
    switch (m_direction) {
    case ArrowDirection::Up:    angle = -M_PI_2; break;
    case ArrowDirection::Down:  angle =  M_PI_2; break;
    case ArrowDirection::Left:  angle =  M_PI;   break;
    case ArrowDirection::Right: angle =  0.0;    break;
    }
    QPointF base = tip - QPointF(std::cos(angle) * headSize,
                                  std::sin(angle) * headSize);
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

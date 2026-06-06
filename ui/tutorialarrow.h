#ifndef TUTORIALARROW_H
#define TUTORIALARROW_H

#include <QWidget>
#include <QMouseEvent>
#include <QPoint>
#include <QPixmap>
#include <QRectF>
#include <QString>

enum class ArrowDirection { Up, Down, Left, Right };

class TutorialArrow : public QWidget {
    Q_OBJECT

public:
    explicit TutorialArrow(QWidget* parent = nullptr);

    void setTarget(QPoint from, QPoint to, ArrowDirection dir);
    void setPixmap(const QPixmap& pm);
    void setMessage(const QString& msg);
    void setHighlightRect(const QRectF& rect);
    void clearHighlight();

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    QPoint m_from;
    QPoint m_to;
    ArrowDirection m_direction = ArrowDirection::Down;
    QPixmap m_pixmap;
    QString m_message;
    QRectF m_highlightRect;
    bool m_showHighlight = false;
};

#endif // TUTORIALARROW_H

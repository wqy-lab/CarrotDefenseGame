#ifndef TUTORIALARROW_H
#define TUTORIALARROW_H

#include <QWidget>
#include <QPoint>
#include <QPixmap>

enum class ArrowDirection { Up, Down, Left, Right };

class TutorialArrow : public QWidget {
    Q_OBJECT

public:
    explicit TutorialArrow(QWidget* parent = nullptr);

    void setTarget(QPoint from, QPoint to, ArrowDirection dir);
    void setPixmap(const QPixmap& pm);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QPoint m_from;
    QPoint m_to;
    ArrowDirection m_direction = ArrowDirection::Down;
    QPixmap m_pixmap;
};

#endif // TUTORIALARROW_H

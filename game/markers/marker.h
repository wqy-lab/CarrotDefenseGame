#ifndef MARKER_H
#define MARKER_H

#include <QString>
#include <memory>

class Enemy;

class Marker {
public:
    virtual ~Marker() = default;

    virtual void update(double dt, Enemy* enemy) = 0;
    virtual bool isActive() const = 0;
    virtual std::unique_ptr<Marker> clone() const = 0;
    virtual QString type() const = 0;
    virtual double priority() const = 0;  // 强度比较，越大越优先

protected:
    Marker() = default;
    Marker(const Marker&) = default;
};

#endif
#ifndef MARKER_H
#define MARKER_H

#include <QString>
#include <memory>

class Enemy;

class Marker {
public:
    virtual ~Marker() = default;

    // 驱动内部状态（duration递减等）
    virtual void update(double dt) = 0;
    virtual bool isActive() const = 0;
    virtual std::unique_ptr<Marker> clone() const = 0;
    virtual QString type() const = 0;
    virtual double priority() const = 0;  // 强度比较，越大越优先

    // 拉取效果查询（由Enemy每帧调用）
    virtual double speedFactor() const = 0;   // 0~1，0=完全停止
    virtual double poisonDps() const = 0;      // 毒伤，每秒伤害

    // 堆叠机制
    virtual int stackCount() const = 0;        // 当前层数
    virtual int stackThreshold() const = 0;    // 触发阈值（0=不堆叠）
    virtual void apply(int stacks) = 0;         // 被调用以应用层数
    virtual std::unique_ptr<Marker> createTriggered(int stacks) const = 0;  // 触发后创建新 Marker

protected:
    Marker() = default;
    Marker(const Marker&) = default;
};

#endif
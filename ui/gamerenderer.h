#ifndef GAMERENDERER_H
#define GAMERENDERER_H

#include <QWidget>
#include <QPainter>
#include "../game/towers/tower.h"
#include "../game/enemies/enemy.h"
#include "../game/bullets/bullet.h"
#include "../game/obstacles/obstacle.h"
#include "../game/cellentities.h"

class GameController;
class SpatialGrid;
class TowerManager;
class TowerPanel;

class GameRenderer : public QWidget {
    Q_OBJECT

public:
    explicit GameRenderer(QWidget* parent = nullptr);
    ~GameRenderer() = default;

    void setGameController(GameController* gc) { m_gameController = gc; }
    void setSpatialGrid(SpatialGrid* sg) { m_spatialGrid = sg; }
    void setTowerManager(TowerManager* tm) { m_towerManager = tm; }
    void setTowerPanel(TowerPanel* tp) { m_towerPanel = tp; }

    void render();

public slots:
    void setSelectedCell(int gx, int gy);
    void clearSelectedCell();
    void showTowerPreview(TowerType type, int gx, int gy);
    void hideTowerPreview();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void drawGrid(QPainter& p);
    void drawPath(QPainter& p);
    void drawTowers(QPainter& p);
    void drawEnemies(QPainter& p);
    void drawProjectiles(QPainter& p);
    void drawObstacles(QPainter& p);
    void drawHoverPreview(QPainter& p);
    void drawPriorityTarget(QPainter& p);
    void drawSelectedCell(QPainter& p);

    GameController* m_gameController = nullptr;
    SpatialGrid* m_spatialGrid = nullptr;
    TowerManager* m_towerManager = nullptr;
    TowerPanel* m_towerPanel = nullptr;

    int m_selectedGridX = -1;
    int m_selectedGridY = -1;

    TowerType m_previewType = TowerType::Arrow;
    int m_previewGridX = -1;
    int m_previewGridY = -1;
};

#endif
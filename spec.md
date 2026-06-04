# CarrotDefenseGame — 交互规范 (Interaction Specification)

> **维护规则**：当代码结构、信号/槽连接、组件职责发生变更时，必须同步更新本文档。

---

## 1. 架构分层与依赖方向

```
ui/  ── #include ──>  game/  ── 读取 JSON ──>  config/
```

- **UI 层** (`ui/`)：持有 `game/` 对象的指针，通过 `#include` 使用游戏类型。
- **游戏逻辑层** (`game/`)：通过 `DataManager` 单例读取 `config/` 下的 JSON 配置。
- **配置层** (`config/`)：纯数据 JSON 文件，被 `DataManager` 加载到内存。

**原则**：`game/` 永远不 `#include` `ui/` 下的任何头文件。

---

## 2. 组件所有权 & 生命周期

### 2.1 MainWindow（顶层窗口）

| 职责 | 详情 |
|------|------|
| 所有页面 | `QStackedWidget` 持有 3 个页面：`MainMenuWidget`(idx 0)、`LevelSelectWidget`(idx 1)、`GameScene`(idx 2) |
| 页面创建 | 构造函数中 `new` 三个页面，通过 `addWidget()` 加入 StackedWidget |
| 页面销毁 | StackedWidget 随 MainWindow 销毁，子页面由 Qt parent 机制自动释放 |
| 信号转发 | 监听 `GameScene::statsChanged`、`GameScene::gameEnded`，转发到自身 slot |
| 生命周期 | `main.cpp` 栈上创建，`app.exec()` 期间存活 |

### 2.2 GameScene（游戏页面）

| 职责 | 详情 |
|------|------|
| 子系统创建 | 构造时直接 `new` 全部 8 个子系统（见 2.3） |
| 生命周期 | 随 MainWindow 销毁而销毁 |
| 输入分发 | `mouseMoveEvent` → `InputHandler::handleMouseMove`，`mousePressEvent` → `InputHandler::handleMousePress`，`keyPressEvent` → `InputHandler::handleKeyPress` |
| 绘制委托 | `paintEvent` → `GameRenderer::render()` → `GameRenderer::update()` |
| 游戏时钟 | 持有 `QTimer*`，16ms 间隔触发 `gameLoop()` |

### 2.3 GameScene 持有的子系统

| 子系统 | 类型 | 说明 |
|--------|------|------|
| `m_spatialGrid` | `SpatialGrid*` | 地图网格，非 QObject |
| `m_gameController` | `GameController*` | 游戏逻辑核心，QObject |
| `m_towerManager` | `TowerManager*` | 炮塔管理，QObject |
| `m_gameRenderer` | `GameRenderer*` | 绘制逻辑，QWidget |
| `m_inputHandler` | `InputHandler*` | 输入处理，**非 QObject** |
| `m_panelController` | `PanelController*` | 面板协调，QObject |
| `m_towerPanel` | `TowerPanel*` | 塔详情面板，QWidget |
| `m_selectionPopup` | `TowerSelectionPopup*` | 塔选择弹窗，QWidget |

**注意**：`InputHandler` 当前不是 QObject，无法使用信号/槽，仅通过直接方法调用与各子系统交互。

---

## 3. 信号 / 槽合同

### 3.1 游戏状态信号

| 发送者 | 信号 | 接收者 | 连接位置 |
|--------|------|--------|----------|
| `GameController` | `statsChanged()` | `GameScene` | `gamescene.cpp:50` |
| `GameScene` | `statsChanged()` | `MainWindow` | `mainwindow.cpp:63` |
| `GameController` | `gameEnded(bool, int)` | `GameScene` | `gamescene.cpp:51` |
| `GameScene` | `gameEnded(bool, int)` | `MainWindow` | `mainwindow.cpp:65` |
| `TowerManager` | `towersChanged()` | `GameRenderer::update()` | `gamescene.cpp:52` |

### 3.2 面板信号

| 发送者 | 信号 | 接收者 | 连接位置 |
|--------|------|--------|----------|
| `TowerPanel` | `upgradeClicked()` | `PanelController::onUpgradeClicked()` | `gamescene.cpp:75` |
| `TowerPanel` | `sellClicked()` | `PanelController::onSellClicked()` | `gamescene.cpp:76` |
| `PanelController` | `hideTowerPanelRequested()` | `GameScene::onPanelControllerHideTowerPanel()` | `gamescene.cpp:77` |
| `TowerSelectionPopup` | `towerSelected(TowerType)` | `PanelController::onTowerSelectedFromPopup()` | `gamescene.cpp:80` |
| `TowerSelectionPopup` | `cancelled()` | `PanelController::hideTowerSelectionPopup()` | `gamescene.cpp:81` |

### 3.3 菜单信号

| 发送者 | 信号 | 接收者 | 连接位置 |
|--------|------|--------|----------|
| `MainMenuWidget` | `startGameClicked()` | `MainWindow::onStartGame()` | `mainwindow.cpp:51` |
| `MainMenuWidget` | `levelSelectClicked()` | `MainWindow` (lambda → 切换到页面1) | `mainwindow.cpp:53` |
| `LevelSelectWidget` | `levelSelected(int, QString)` | `MainWindow::onLevelSelected()` | `mainwindow.cpp:57` |
| `LevelSelectWidget` | `backClicked()` | `MainWindow` (lambda → 切换到页面0) | `mainwindow.cpp:59` |
| `GameResultWidget` | `retryClicked()` | `MainWindow::onRetry()` | `mainwindow.cpp:141` |
| `GameResultWidget` | `nextLevelClicked()` | `MainWindow::onNextLevel()` | `mainwindow.cpp:143` |
| `GameResultWidget` | `menuClicked()` | `MainWindow::onMenu()` | `mainwindow.cpp:145` |

---

## 4. 输入处理协议

### 4.1 事件入口

`GameScene` 重载以下 Qt 事件，全部委托给 `InputHandler`：

```cpp
mouseMoveEvent  → InputHandler::handleMouseMove
mousePressEvent → InputHandler::handleMousePress
keyPressEvent   → InputHandler::handleKeyPress
resizeEvent     → InputHandler::handleResize
```

### 4.2 MouseMove 行为

1. 将像素坐标转网格坐标 → `m_towerManager->setHoverGrid(x, y)`
2. 若 `m_towerManager->isPlacingTower()` 且该格可放置 → `m_towerManager->setShowRange(true)`
3. 否则 → `m_towerManager->setShowRange(false)`
4. 触发 `m_gameRenderer->update()` 重绘

### 4.3 MousePress 行为（优先级从高到低）

1. **前置检查**：游戏未运行 / 暂停 / 已结束 → 直接返回
2. **左键点击塔**：选中有塔的格子 → `m_panelController->showTowerPanel(tower)` → 返回
3. **左键点击敌人**：格子中有敌人 → `m_gameController->setPriorityEnemy(enemy)` + `hideTowerPanel()` → 返回
4. **左键点击障碍物**：格子中有障碍物 → `m_gameController->setPriorityObstacle(obs)` + `hideTowerPanel()` → 返回
5. **左键点击空地**：非路径、非障碍物、无塔的格子 → `m_panelController->showTowerSelectionPopup(gx, gy, globalPos)` → 返回
6. **其他**：`m_gameController->clearPriorityTarget()` + `hideTowerPanel()`

### 4.4 KeyPress 行为

- `Esc` → `m_panelController->hideTowerPanel()`

### 4.5 Resize 行为

1. 根据窗口宽高和网格行列数重新计算 `cellSize`
2. 最小 cellSize = 20px
3. 计算 offset 使网格居中
4. 调用 `m_spatialGrid->setCellSize()` / `setOffset()`
5. 触发 `m_gameRenderer->update()`

---

## 5. 炮塔放置交互协议

### 5.1 时序

```
Player 点击空地
  → InputHandler::handleMousePress
    → PanelController::showTowerSelectionPopup(gx, gy, globalPos)
      → TowerSelectionPopup 显示在点击位置附近，偏移 (+10, +10)
Player 点击塔按钮
  → TowerSelectionPopup::towerSelected(TowerType)
    → PanelController::onTowerSelectedFromPopup(type)
      → TowerManager::placeTowerAt(gx, gy, type)
        → 检查 canPlaceTower(gx, gy)
        → createTower(type, ...) 工厂创建
        → 检查金币是否足够
        → spendGold(cost)
        → push 到 m_towers
        → emit towersChanged() → GameRenderer::update()
      → PanelController::hideTowerSelectionPopup()
```

### 5.2 放置前置条件

```cpp
TowerManager::canPlaceTower(gx, gy):
  isValidGridPos(gx, gy)     // 坐标在有效范围内
  && !isPathCell(gx, gy)      // 非怪物路径格
  && !isObstacleCell(gx, gy)  // 非障碍物格
  && getTowerAt(gx, gy) == nullptr  // 该格上没有塔
```

### 5.3 TowerSelectionPopup 显示规则

- 使用 `Qt::Popup | Qt::FramelessWindowHint` 窗口标志
- 固定尺寸 260×200
- 2×3 网格布局，6 种塔类型按钮
- 每个按钮根据当前金币余额启用/禁用（金币不足时灰掉）
- 失焦或外部点击时自动隐藏，并 emit `cancelled()`

---

## 6. 炮塔面板交互协议

### 6.1 时序

```
Player 点击已有塔
  → InputHandler::handleMousePress
    → PanelController::showTowerPanel(tower)
      → TowerPanel::setTower(tower)  // 更新数据
      → 计算面板显示位置（塔上方，超出屏幕则显示在下方）
      → TowerPanel::show()

Player 点击 Upgrade
  → TowerPanel::onUpgradeClicked → emit upgradeClicked()
    → PanelController::onUpgradeClicked()
      → 检查金币 >= upgradeCost
      → spendGold(cost)
      → tower->upgrade()
      → TowerPanel::updateInfo()
      → emit GameController::statsChanged()

Player 点击 Sell
  → TowerPanel::onSellClicked → emit sellClicked()
    → PanelController::onSellClicked()
      → addGold(sellValue)
      → TowerManager::removeTower(tower)
      → hideTowerPanel()
      → emit GameController::statsChanged()

Player 按 Esc / 点击其他地方
  → PanelController::hideTowerPanel()
    → TowerPanel::hide()
    → m_currentTower = nullptr
    → emit hideTowerPanelRequested()
      → GameScene::onPanelControllerHideTowerPanel()
        → m_towerManager->setSelectedTowerPtr(nullptr)
        → update()  // 重绘去高亮
```

### 6.2 TowerPanel 显示规则

- 使用 `Qt::Popup | Qt::FramelessWindowHint`
- 固定尺寸 180×140
- 显示在塔所在格的上方 `cellSize * 0.8` 偏移处
- 若顶部空间不足则翻转到下方
- `showEvent` 时自动调用 `updateInfo()`
- `FocusOut` 事件时自动隐藏

---

## 7. 游戏主循环协议

### 7.1 时钟

- `QTimer` 间隔 16ms（≈60 FPS）
- `gameLoop()` 在三种情况下跳过更新：
  - `m_gameController->isRunning() == false`
  - `m_gameController->isPaused() == true`
  - `m_gameController->isGameOver() == true`

### 7.2 每帧执行顺序

1. `GameController::update(dt, towers)` — dt 上限钳位 0.1s
   1. `WaveManager::update(dt)` — 波次计时
   2. `spawnEnemy()` — 出怪（`WaveManager::shouldSpawn()`）
   3. 遍历 `m_enemies` → `enemy.update(dt)` — 移动
   4. 遍历 `m_enemies` → 检查 `reachedEnd()` → `reduceLives()` — 扣血
   5. 遍历 `towers` → `tower.update(dt, m_enemies)` — 设置优先目标 + 攻击更新
   6. 遍历 `towers` → 处理 `RemoteTower` 的 `pendingAttack` → `BulletFactory` 创建子弹
   7. 遍历 `towers` → 处理 `MeleeTower` 的 `pendingEffect` → 范围内敌人结算（含衰减、减速、中毒）
   8. `SpatialGrid::syncEntityGrid()` — 同步每格实体列表
   9. 遍历子弹 → `bullet.update(dt, enemies, cell)` — 移动 + 碰撞检测
   10. 遍历障碍物 → `obstacle.update(dt)` — 销毁动画等
   11. 子弹命中处理 → `handleProjectileHit()` — 结算奖励
   12. 清理死亡敌人 + 失效子弹
   13. 检查波次完成 → 发放奖励 or 胜利判定
   14. `checkGameEnd()` — 生命值 ≤ 0 → `emit gameEnded(false, levelId)`
   15. `emit statsChanged()`
2. `GameRenderer::update()` — 触发 `paintEvent` 重绘

### 7.3 启动与重置

| 操作 | 调用链 |
|------|--------|
| 启动游戏 | `GameScene::startGame()` → `GameController::startGame()` → `WaveManager::nextWave()` → `QTimer::start(16)` |
| 暂停 | `GameScene::pauseGame()` → `GameController::pauseGame()` |
| 恢复 | `GameScene::resumeGame()` → `GameController::resumeGame()` |
| 重置 | `GameScene::resetGame()` → `QTimer::stop()` → `GameController::resetGame()` → `m_towers.clear()` → `hide TowerPanel` → `update()` |

---

## 8. 页面导航协议

### 8.1 页面结构

```
MainWindow
  └── QStackedWidget (ui->centralwidget)
        ├── [0] MainMenuWidget   — 主菜单
        ├── [1] LevelSelectWidget — 选关
        └── [2] GameScene         — 游戏画面
```

### 8.2 导航路径

```
App启动 → 主菜单(idx 0)
  │
  ├─ "Start Game" → loadAndStartLevel(level1) → 游戏页(idx 2)
  ├─ "Level Select" → 选关页(idx 1)
  │     ├─ 点击关卡 → loadAndStartLevel(selected) → 游戏页(idx 2)
  │     └─ "Back" → 主菜单(idx 0)
  │
  └─ 游戏结束(overlay) → GameResultWidget
        ├─ "Retry" → 重新载入当前关
        ├─ "Next Level" → 载入下一关
        └─ "Menu" → 主菜单(idx 0)
```

### 8.3 页面显示/隐藏规则

- 显示菜单页：`toolBar->hide()` + `statusbar->hide()`
- 显示游戏页：`toolBar->show()` + `statusbar->show()` + `updateStatusBar()` + `updateTowerButtons()`
- `GameResultWidget` overlay 动态 `new` 在 MainWindow 上，`deleteLater` 回收

---

## 9. 数据流协议

### 9.1 DataManager（单例）

```
main() 启动时:
  1. DataManager::instance().loadShared("config/shared.json")
     → 填充 m_towerStats, m_enemyStats, m_obstacleStats
  2. DataManager::instance().loadLevel("config/levels/level2.json")
     → 填充 m_mapData, m_waves, m_obstacles, m_initialGold/Lives
  3. DataManager::instance().loadLevelsIndex("config/levels.json")
     → 填充 m_levels（关卡列表）

运行时读取:
  - TowerStats    → Tower 构造（TowerFactory） + MainWindow 价格显示
  - EnemyStats    → Enemy 构造（EnemyFactory）
  - MapData       → SpatialGrid::initMap()
  - WaveEntry     → WaveManager 波次
  - ObstacleEntry → GameController::resetGame() 中创建障碍物
  - LevelEntry    → LevelSelectWidget 关卡列表
```

### 9.2 数据更新方向

```
玩家操作 → TowerManager / GameController 数据变更
           → emit statsChanged() / towersChanged()
             → MainWindow::onStatsChanged()
               → updateStatusBar() / updateTowerButtons()
```

---

## 10. 渲染协议

### 10.1 渲染层叠顺序

`GameRenderer::paintEvent` 按以下顺序绘制（由底到顶）：

1. 背景填充 `QColor(34, 40, 34)`
2. **drawGrid** — 网格底色 + 路径格 + 塔格 + 障碍物格 + 网格线 + 起点(S)/终点(E)标记
3. **drawPath** — 半透明宽线连接 waypoints
4. **drawObstacles** — 调用 `obstacle->draw()`
5. **drawTowers** — 绘制塔方块 + 字母标签（仅 Arrow(A)/Cannon(C)/Ice(I) 有标签）
6. **drawEnemies** — 调用每个活跃敌人的 `draw()`
7. **drawProjectiles** — 调用每个活跃子弹的 `draw()`
8. **drawHoverPreview** — 放置预览（范围圈 + 半透明方块），条件：`isPlacingTower() && showRange()`
9. **游戏结束遮罩** — 半透明黑色 `QColor(0,0,0,150)`

### 10.2 触发重绘的时机

- `TowerManager::towersChanged()` → `GameRenderer::update()`（信号连接）
- `InputHandler::handleMouseMove()` → `GameRenderer::update()`（直接调用）
- `GameScene::gameLoop()` → `GameRenderer::update()`（直接调用）
- `GameScene::resetGame()` / `onPanelControllerHideTowerPanel()` → `update()`（自身触发 `paintEvent`）

---

## 11. 当前已知问题 & 待重构项

| # | 问题 | 影响 |
|---|------|------|
| 1 | `InputHandler` 不是 QObject，依赖直接方法调用 | 无法使用信号/槽解耦 |
| 2 | `MainWindow` 中 6 个塔按钮已初始化但始终 disabled | 死代码，约 40 行 |
| 3 | 样式字符串 (QSS) 散落在 `mainwindow.cpp`、`towerpanel.cpp`、`towerselectionpopup.cpp`、`main.cpp` | 修改样式需改多处 |
| 4 | `GameScene` 构造函数直接 `new` 8 个子系统 | 高耦合，难以测试 |
| 5 | `PanelController` 仅做转发，无实质业务逻辑 | 增加跳转层数 |
| 6 | `GameScene` 和 `GameRenderer` 同为 QWidget，存在绘制职责重叠 | `GameScene::paintEvent` 委托 `GameRenderer::render()`，后者再调用 `update()` |
| 7 | 魔法数字散布各处（宽高、偏移、颜色、阈值） | 维护性差 |
| 8 | `drawTowers()` 中仅对 Arrow/Cannon/Ice 绘制字母标签 | Poison/Lightning/Sun 无标签，显示不一致 |
| 9 | `mainwindow.cpp:257-266` 处的工具栏塔按钮禁用代码与 TowerSelectionPopup 重复逻辑 | 功能已被替代但未删除 |
| 10 | `CMakeLists.txt` 中源文件手动列举 | 新增文件需手动更新 CMake |

---

## 12. 网格坐标系统

### 12.1 设计原则

所有游戏内部实体（Enemy、Obstacle、Tower、Bullet）的位置和移动全部使用**网格坐标**存储，渲染时通过 `cellSize` 和 `offset` 换算为像素坐标。

**目的**：消除窗口 resize 时的同步需求——渲染时自动使用当前的 cellSize/offset 计算，无需更新任何实体的内部状态。

### 12.2 单位约定

| 类型 | 单位 | 说明 |
|------|------|------|
| 位置 `m_gridPos` / `m_gridX/Y` | 网格单位 (grid) | 1 grid = 1 cell |
| 速度 `speed()` | grids/sec | 网格单位/秒 |
| 射程/范围 `range` | grids | 网格单位 |
| 溅射半径 `splashRadius` | grids | 网格单位 |
| 攻击半径 `m_attackRadius` | grids | 网格单位（MeleeTower） |
| 链式闪电距离 `chainDist` | grids | 硬编码约3 grids |
| 碰撞半径 `hitRadius` | grids | `radius / cellSize * factor` |
| 绘制半径 `radius()` | **像素** (pixels) | JSON中定义的是像素，用于绘制 |

### 12.3 坐标转换

**网格 → 像素**：
```cpp
QPointF gridToPixel(int gx, int gy) {
    return QPointF(offsetX + gx * cellSize + cellSize / 2.0,
                   offsetY + gy * cellSize + cellSize / 2.0);
}
```

**像素 → 网格**：
```cpp
QPoint pixelToGrid(const QPointF& pos) {
    return QPoint(static_cast<int>((pos.x() - offsetX) / cellSize),
                  static_cast<int>((pos.y() - offsetY) / cellSize));
}
```

### 12.4 各类实体的网格化实现

#### Enemy
- `m_gridPos` : QPointF — 网格坐标（小数表示格子内精确位置）
- `m_path` : std::vector<QPointF> — 网格坐标数组（waypoints）
- `speed()` : 从 JSON 加载时 `speed / 48.0` 转为 grids/sec
- `pos(cellSize, offsetX, offsetY)` — 返回像素坐标用于绘制
- `draw()` — 使用 `pos()` 转为像素后绘制

#### Obstacle
- 仅存储 `m_gridX/Y/W/H`（网格坐标），无像素坐标成员
- `draw(cellSize, offsetX, offsetY)` — 计算像素位置绘制
- 不再需要 `setPosition()` / `pos()` 方法

#### Tower
- 仅存储 `m_gridX/Y`（网格坐标），无 `m_center` 像素坐标
- `centerPos(cellSize, offsetX, offsetY)` — 返回像素坐标
- `rangePx()` — 返回 `m_stats.range * m_cellSize`（像素，用于渲染预览圈）

#### Bullet
- `m_pos` / `m_targetPos` / `m_startPos` — 网格坐标（QPointF）
- 移动：`moveDist = (m_speed / m_cellSize) * dt` — 在网格空间移动
- 碰撞检测：使用 `e->gridPos()` 直接在网格空间计算距离
- `draw()` — 转为像素坐标绘制

### 12.5 DataManager 加载时的单位转换

```cpp
// 敌人速度：pixels/sec → grids/sec
s.speed = obj["speed"].toDouble() / 48.0;

// 塔射程/溅射：直接使用网格单位（JSON 中已是 grids）
s.range = obj["range"].toDouble();
s.splashRadius = obj["splashRadius"].toDouble();
```

### 12.6 RemoteTower 距离计算（网格空间）

```cpp
double RemoteTower::distTo(const Enemy& e) const {
    QPointF gp = e.gridPos();
    double dx = gp.x() - m_gridX;
    double dy = gp.y() - m_gridY;
    return dx*dx + dy*dy;  // 平方距离，避免开方
}

// 射程检测：直接用 m_stats.range（grids）的平方比较
double r2 = m_stats.range * m_stats.range;
```

### 12.7 MeleeTower 效果范围（GameController 中结算）

```cpp
// effect.center 是网格坐标 QPointF(m_gridX, m_gridY)
// effect.radius 是网格单位（如 2.0 grids）
double centerX = offsetX + effect.center.x() * cellSize + cellSize / 2.0;
double centerY = offsetY + effect.center.y() * cellSize + cellSize / 2.0;
double effectRadiusPx = effect.radius * cellSize;
```

### 12.8 Bullet 碰撞检测

```cpp
// hitRadius：敌人像素半径 / cellSize * factor（约4.0）
double hitRadius = static_cast<double>(e->radius()) / m_cellSize * 4.0;
// 距离比较直接在网格空间进行
QPointF d = e->gridPos() - m_pos;
double d2 = d.x()*d.x() + d.y()*d.y();
if (d2 <= hitRadius * hitRadius) { /* hit */ }
```

### 12.9 渲染时的坐标转换

**GameRenderer** 中各 draw 调用：
```cpp
drawEnemies:  e->draw(p, cellSize, offsetX, offsetY)
drawProjectiles: pj->draw(p, cellSize, offsetX, offsetY)
drawObstacles: obs->draw(&p, cellSize, offsetX, offsetY)
drawTowers:   t->centerPos(cellSize, offsetX, offsetY)
```

**drawPath** 需将 waypoints 从网格转像素后绘制连接线。

### 12.10 Resize 行为

resize 时**无需同步任何实体**：
```cpp
void InputHandler::handleResize(int width, int height) {
    // 仅更新 cellSize 和 offset
    m_spatialGrid->setCellSize(cellSize);
    m_spatialGrid->setOffset(offsetX, offsetY);
    // 渲染器在下一次 paintEvent 时自动使用新的 cellSize/offset
}
```

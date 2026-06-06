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
| 信号转发 | 监听 `GameScene::statsChanged`、`GameScene::gameEnded`、`GameScene::exitToLevelSelectRequested` |
| 游戏结果 | 动态 `new GameResultWidget` overlay，`cleanupOverlay()` 确保旧实例 `deleteLater` 回收 |
| 生命周期 | `main.cpp` 栈上创建，`app.exec()` 期间存活 |

### 2.2 GameScene（游戏页面）

| 职责 | 详情 |
|------|------|
| 子系统创建 | 构造时直接 `new` 全部 9 个子系统（见 2.3） |
| 生命周期 | 随 MainWindow 销毁而销毁 |
| 输入分发 | `mouseMoveEvent` → `InputHandler::handleMouseMove`，`mousePressEvent` → `InputHandler::handleMousePress`，`keyPressEvent` → `InputHandler::handleKeyPress` |
| 绘制委托 | `paintEvent` → `GameRenderer::render()` → `GameRenderer::update()` |
| 游戏时钟 | 持有 `QTimer*`，16ms 间隔触发 `gameLoop()` |
| HUD / Overlay 管理 | 持有 `GameHUD`（顶部状态栏）和 `GameOverlay`（暂停遮罩），均由 GameScene 管理生命周期和信号连接 |

### 2.3 GameScene 持有的子系统

| 子系统 | 类型 | 说明 |
|--------|------|------|
| `m_spatialGrid` | `SpatialGrid*` | 地图网格，非 QObject |
| `m_gameController` | `GameController*` | 游戏逻辑核心，QObject |
| `m_towerManager` | `TowerManager*` | 炮塔管理，QObject |
| `m_gameRenderer` | `GameRenderer*` | 绘制逻辑，QWidget |
| `m_inputHandler` | `InputHandler*` | 输入处理，非 QObject |
| `m_panelController` | `PanelController*` | 面板协调，QObject |
| `m_towerPanel` | `TowerPanel*` | 塔详情面板，QWidget |
| `m_selectionPopup` | `TowerSelectionPopup*` | 塔选择弹窗，QWidget |
| `m_gameHUD` | `GameHUD*` | 游戏内状态栏，QWidget，零 game/ 依赖 |
| `m_overlay` | `GameOverlay*` | 暂停/确认/消息遮罩，QWidget |

### 2.4 GameHUD（MVC Dumb View）

| 约束 | 详情 |
|------|------|
| 依赖方向 | 不 `#include` 任何 `game/` 头文件 |
| 数据接口 | `setGold(int)` / `setLives(int)` / `setWave(int,int)` / `setEnemies(int)` / `setPausedState(bool)` |
| 用户事件 | `pauseClicked()` 信号，不携带状态 |
| 布局 | 顶部半透明黑条，QPushButton + 4 组图标占位 + QLabel |
| Resize | `GameScene::resizeEvent` 中 `setGeometry(0, 0, width(), height)` 自适应宽度 |

---

## 3. 信号 / 槽合同

### 3.1 游戏状态信号

| 发送者 | 信号 | 接收者 | 连接位置 |
|--------|------|--------|----------|
| `GameController` | `statsChanged()` | `GameScene::onStatsChanged()` | `gamescene.cpp` 构造函数 |
| `GameScene` | `statsChanged()` | `MainWindow::onStatsChanged()` | `mainwindow.cpp` 构造函数 |
| `GameController` | `gameEnded(bool, int)` | `GameScene`（转发） | `gamescene.cpp` 构造函数 |
| `GameScene` | `gameEnded(bool, int)` | `MainWindow::onGameEnded()` | `mainwindow.cpp` 构造函数 |
| `TowerManager` | `towersChanged()` | `GameRenderer::update()` | `gamescene.cpp` 构造函数 |
| `GameScene` | `exitToLevelSelectRequested()` | `MainWindow::onExitToLevelSelect()` | `mainwindow.cpp` 构造函数 |

### 3.2 HUD 信号

| 发送者 | 信号 | 接收者 | 连接位置 |
|--------|------|--------|----------|
| `GameHUD` | `pauseClicked()` | `GameScene`（lambda） | `gamescene.cpp` 构造函数 |

### 3.3 面板信号

| 发送者 | 信号 | 接收者 | 连接位置 |
|--------|------|--------|----------|
| `TowerPanel` | `upgradeClicked()` | `PanelController::onUpgradeClicked()` | `gamescene.cpp` |
| `TowerPanel` | `sellClicked()` | `PanelController::onSellClicked()` | `gamescene.cpp` |
| `PanelController` | `hideTowerPanelRequested()` | `GameScene::onPanelControllerHideTowerPanel()` | `gamescene.cpp` |
| `TowerSelectionPopup` | `towerSelected(TowerType)` | `PanelController::onTowerSelectedFromPopup()` | `gamescene.cpp` |
| `TowerSelectionPopup` | `cancelled()` | `PanelController::hideTowerSelectionPopup()` | `gamescene.cpp` |

### 3.4 暂停遮罩信号

| 发送者 | 信号 | 接收者 | 连接位置 |
|--------|------|--------|----------|
| `GameOverlay` | `continueClicked()` | `GameScene::onOverlayContinue()` | `gamescene.cpp` |
| `GameOverlay` | `exitToLevelSelectConfirmed()` | `GameScene::onOverlayExitConfirmed()` | `gamescene.cpp` |

### 3.5 菜单信号

| 发送者 | 信号 | 接收者 | 连接位置 |
|--------|------|--------|----------|
| `MainMenuWidget` | `startGameClicked()` | `MainWindow::onStartGame()` | `mainwindow.cpp` |
| `MainMenuWidget` | `levelSelectClicked()` | `MainWindow` (lambda → 切换到页面1) | `mainwindow.cpp` |
| `LevelSelectWidget` | `levelSelected(int, QString)` | `MainWindow::onLevelSelected()` | `mainwindow.cpp` |
| `LevelSelectWidget` | `backClicked()` | `MainWindow` (lambda → 切换到页面0) | `mainwindow.cpp` |
| `GameResultWidget` | `retryClicked()` | `MainWindow::onRetry()` | `mainwindow.cpp` |
| `GameResultWidget` | `nextLevelClicked()` | `MainWindow::onNextLevel()` | `mainwindow.cpp` |
| `GameResultWidget` | `menuClicked()` | `MainWindow::onMenu()` | `mainwindow.cpp` |

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
5. `m_gameRenderer->setGeometry(rect())`
6. `m_overlay->setGeometry(rect())`
7. `m_gameHUD->setGeometry(0, 0, width(), height())`

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
      → TowerPanel::setTower(tower)
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
        → update()
```

### 6.2 TowerPanel 显示规则

- 使用 `Qt::Popup | Qt::FramelessWindowHint`
- 固定尺寸 180×140
- 显示在塔所在格的上方 `cellSize * 0.8` 偏移处
- 若顶部空间不足则翻转到下方
- `showEvent` 时自动调用 `updateInfo()`
- `FocusOut` 事件时自动隐藏

---

## 7. 暂停与 HUD 交互协议

### 7.1 暂停流程

```
Player 点击 HUD Pause 按钮
  → GameHUD::pauseClicked()
    → GameScene lambda: isPaused() ? hidePauseOverlay() : showPauseOverlay()
      → showPauseOverlay():
          → pauseGame() → m_gameHUD->setPausedState(true) → 按钮变 "▶ Resume"
          → m_overlay->showPauseMenu()
          → emit overlayVisibilityChanged(true)

Player 点击遮罩 "继续游戏"
  → GameOverlay::continueClicked()
    → GameScene::onOverlayContinue()
      → hidePauseOverlay()
          → m_overlay->hideOverlay()
          → m_gameHUD->setPausedState(false) → 按钮变 "⏸ Pause"
          → emit overlayVisibilityChanged(false)
      → resumeGame()
```

### 7.2 GameHUD 显示规则

- 使用 `.ui` 文件，QWidget 子类 + `WA_StyledBackground`
- 高度 ~36px，撑满 GameScene 宽度
- 半透明黑底 `rgba(0, 0, 0, x)`，透明度可在 Qt Designer 的 styleSheet 中调整
- 左侧 Pause 按钮 + stretch spacer + 右侧 4 组状态（图标占位 + 数值标签）
- 每组状态前有 16×16 的 QLabel 图标占位（`iconGold`/`iconLives`/`iconWave`/`iconEnemies`），`scaledContents=true`，后续可设置 QPixmap

---

## 8. 游戏主循环协议

### 8.1 时钟

- `QTimer` 间隔 16ms（≈60 FPS）
- 实际帧间隔 `dt` 由 `QElapsedTimer::restart()` 测量，上限钳位 0.1s 防止掉帧螺旋
- `gameLoop()` 在三种情况下跳过更新：
  - `m_gameController->isRunning() == false`
  - `m_gameController->isPaused() == true`
  - `m_gameController->isGameOver() == true`

### 8.2 每帧执行顺序

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

### 8.3 启动与重置

| 操作 | 调用链 |
|------|--------|
| 启动游戏 | `GameScene::startGame()` → `GameController::startGame()` → `WaveManager::nextWave()` → `m_gameHUD->setPausedState(false)` → `QTimer::start(16)` |
| 暂停 | `GameScene::pauseGame()` → `GameController::pauseGame()` → `m_gameHUD->setPausedState(true)` |
| 恢复 | `GameScene::resumeGame()` → `GameController::resumeGame()` → `m_gameHUD->setPausedState(false)` |
| 重置 | `GameScene::resetGame()` → `QTimer::stop()` → `GameController::resetGame()` → `WaveManager::reset()`（含 `buildWaves()`） → `m_towers.clear()` → `hide TowerPanel` → `update()` |

### 8.4 波次与出怪间隔

- 波次数据由 JSON 的 `waves` 数组定义，每个 `WaveEntry` 包含 `type`、`count`、`interval`
- `m_spawnQueue` 存储 `std::pair<EnemyType, double>`（类型 + 生成间隔）
- `popSpawnType()` 使用队列中存储的 `interval` 而非硬编码值
- 波次首怪延迟固定 1.5s

---

## 9. 页面导航协议

### 9.1 页面结构

```
MainWindow
  └── QStackedWidget (ui->centralwidget)
        ├── [0] MainMenuWidget   — 主菜单
        ├── [1] LevelSelectWidget — 选关
        └── [2] GameScene         — 游戏画面
```

### 9.2 导航路径

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
        ├─ "Next Level" → 按 levels 列表顺序查找下一关并载入
        └─ "Menu" → 主菜单(idx 0)
```

### 9.3 解锁逻辑

通关后解锁下一关，按 `levels` 列表顺序（非 `id + 1`）：

```cpp
for (size_t i = 0; i < levels.size(); ++i) {
    if (levels[i].id == levelId && i + 1 < levels.size()) {
        m_levelSelect->unlockLevel(levels[i + 1].id);
        break;
    }
}
```

Debug 构建下跳过此逻辑，所有关卡默认解锁。

---

## 10. 数据流协议

### 10.1 DataManager（单例）

```
main() 启动时:
  1. DataManager::instance().loadShared("config/shared.json")
     → 清空旧数据后填充 m_towerStats, m_enemyStats, m_obstacleStats
  2. DataManager::instance().loadLevelsIndex("config/levels.json")
     → 填充 m_levels（关卡列表）
  3. DataManager::instance().loadLevel(levels[0].file)
     → 填充 m_mapData, m_waves, m_obstacles, m_initialGold/Lives
     → 从关卡索引取第一个关卡的路径

运行时读取:
  - getTowerStats(type, level)  → Tower 构造 + TowerSelectionPopup 价格显示
  - getTowerMarkers(type, level) → Tower 构造时创建 Marker 模板
  - EnemyStats    → Enemy 构造（EnemyFactory）
  - MapData       → SpatialGrid::initMap()
  - WaveEntry     → WaveManager 波次
  - ObstacleEntry → GameController::resetGame() 中创建障碍物
  - LevelEntry    → LevelSelectWidget 关卡列表
```

### 10.2 HUD 数据流（MVC）

```
GameController [Model]
    │ statsChanged()
    ▼
GameScene::onStatsChanged() [Controller]
    │ gold / lives / wave / enemies 从 Controller 拉取
    ▼
GameHUD [Dumb View]
    │ setGold(int) / setLives(int) / setWave(int,int) / setEnemies(int)
    ▼
QLabel 更新
```

GameHUD 只接收基本类型参数，不持有 `GameController*` 引用。

### 10.3 数据更新方向

```
玩家操作 → TowerManager / GameController 数据变更
           → emit statsChanged()
             → GameScene::onStatsChanged()
               → GameHUD 更新
             → MainWindow::onStatsChanged()
```

---

## 11. 渲染协议

### 11.1 渲染层叠顺序

`GameRenderer::paintEvent` 按以下顺序绘制（由底到顶）：

1. 背景填充 `QColor(34, 40, 34)`
2. **drawGrid** — 网格底色 + 路径格 + 塔格 + 障碍物格 + 网格线 + 起点(S)/终点(E)标记
3. **drawPath** — 半透明宽线连接 waypoints
4. **drawObstacles** — 调用 `obstacle->draw()`
5. **drawTowers** — 绘制塔方块 + 字母标签
6. **drawEnemies** — 调用每个活跃敌人的 `draw()`
7. **drawProjectiles** — 调用每个活跃子弹的 `draw()`
8. **drawHoverPreview** — 放置预览（范围圈 + 半透明方块），条件：`isPlacingTower() && showRange()`
9. **游戏结束遮罩** — 半透明黑色 `QColor(0,0,0,150)`

GameHUD 和 GameOverlay 作为独立 Widget 覆盖在 GameRenderer 上方，由 `raise()` 保证 Z 序。

### 11.2 触发重绘的时机

- `TowerManager::towersChanged()` → `GameRenderer::update()`（信号连接）
- `InputHandler::handleMouseMove()` → `GameRenderer::update()`（直接调用）
- `GameScene::gameLoop()` → `GameRenderer::update()`（直接调用）
- `GameScene::resetGame()` / `onPanelControllerHideTowerPanel()` → `update()`（自身触发 `paintEvent`）

---

## 12. 当前已知问题 & 待重构项

| # | 问题 | 影响 |
|---|------|------|
| 1 | `InputHandler` 不是 QObject，依赖直接方法调用 | 无法使用信号/槽解耦 |
| 2 | 样式字符串 (QSS) 散落在 `mainwindow.cpp`、`towerpanel.cpp`、`towerselectionpopup.cpp`、`main.cpp` | 修改样式需改多处 |
| 3 | `GameScene` 构造函数直接 `new` 9 个子系统 | 高耦合，难以测试 |
| 4 | `PanelController` 仅做转发，无实质业务逻辑 | 增加跳转层数 |
| 5 | `GameScene` 和 `GameRenderer` 同为 QWidget，存在绘制职责重叠 | 冗余委托 |
| 6 | 魔法数字散布各处（宽高、偏移、颜色、阈值） | 维护性差 |
| 7 | `drawTowers()` 仅对 Arrow/Cannon/Ice 绘制字母标签 | Poison/Lightning/Sun 无标签，后续改为贴图 |
| 8 | `CMakeLists.txt` 中源文件手动列举 | 新增文件需手动更新 CMake |

---

## 13. Marker 多态系统

### 13.1 设计目标

将效果（减速、中毒）从子弹/塔的属性迁移到独立的 Marker 类，实现效果的可堆叠、可扩展。

### 13.2 Marker 基类

**文件**: `game/markers/marker.h`

```cpp
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
```

### 13.3 具体 Marker 类

| 类 | 文件 | update 行为 | priority 公式 |
|---|---|---|---|
| `SlowMarker` | `game/markers/slowmarker.h/cpp` | 每帧 `applySlow(factor, duration)` | `1.0 / m_factor`（factor 越小越强，priority 越大） |
| `PoisonMarker` | `game/markers/poisonmarker.h/cpp` | 每帧 `applyPoison(dps, duration)` | `m_dps`（dps 越大越强） |

### 13.4 Enemy 的 marker 管理

```cpp
struct MarkerSlot {
    std::unique_ptr<Marker> active;                      // 当前激活的 marker
    std::vector<std::unique_ptr<Marker>> pending;        // 等待队列，按 priority 降序
};
std::map<QString, MarkerSlot> m_markers;  // key: type()，如 "slow"、"poison"
```

**堆叠逻辑**:
- `active` 有值时比较 priority
- new marker 更强（priority 更大）→ active 入 pending，new 成为 active
- new marker 更弱 → new 入 pending（保持 priority 降序）
- active 过期时从 pending 队列 promote 下一个

### 13.5 Tower 持有 markers

Tower 持有 `std::array<std::vector<std::unique_ptr<Marker>>, 4> m_markerTemplates`（index 1-3 对应等级），存储各级别的 Marker 模板。攻击时通过 `cloneMarkers()` 克隆当前等级的 markers。

```cpp
// Tower 基类方法
std::vector<std::unique_ptr<Marker>> cloneMarkers() const {
    std::vector<std::unique_ptr<Marker>> result;
    for (const auto& m : markers()) {
        result.push_back(m->clone());
    }
    return result;
}
const std::vector<std::unique_ptr<Marker>>& markers() const {
    return m_markerTemplates[m_level];
}
```

### 13.6 Bullet 携带 markers

Bullet 基类持有 `std::vector<std::unique_ptr<Marker>> m_markerTemplates`，通过 `setMarkers()` 设置。`onHit` 时 clone 并添加到敌人。

| 子弹类 | 携带的 Marker | 来源 |
|--------|--------------|------|
| `ArrowBullet` | `SlowMarker` + `PoisonMarker` | Tower 的 markers() |
| `IceBullet` | `SlowMarker` | Tower 的 markers() |
| `PoisonBullet` | `PoisonMarker` | Tower 的 markers() |
| `CannonBullet` | 无 marker | 溅射逻辑在 onHit 自己处理 |
| `LightningBullet` | 无 marker | 链弹逻辑在 onHit 自己处理 |

```cpp
// Bullet::onHit
for (auto& marker : m_markerTemplates) {
    enemy->addMarker(marker->clone());
}
```

**BulletFactory 传递 markers**:
```cpp
auto b = createBullet(btype, start, direction, damage, splashRadius, color, std::move(markers));
b->setMarkers(std::move(markers));
```

**穿透机制**：Bullet 基类通过 `m_penetrationLeft` 计数和 `m_hitEnemies` / `m_hitObstacles` 列表实现穿透。命中敌人或障碍物时，如果 `m_penetrationLeft > 0` 则只扣减计数并继续飞行，否则 deactivate。`m_hitEnemies` / `m_hitObstacles` 追踪已击中的目标，避免同一帧内重复击中同一目标。

**多发机制**：Bullet 构造函数接收 `direction` 归一化方向向量而非目标点。GameController 使用旋转矩阵计算分散方向：
```cpp
QPointF bulletDir = centerDir * cos(angle) + perpDir * sin(angle);
```

### 13.7 JSON 编码方案

shared.json 中每种塔包含 `levels[]` 和 `markers[]` 两个数组，每个数组 3 个元素分别对应 level 1/2/3。

```json
{
    "type": "Ice",
    "levels": [
        { "cost": 60, "damage": 12.0, "range": 2.5, "attackSpeed": 1.0, "color": "#64B4FF", "penetration": 0, "shotCount": 1, "spreadAngle": 0, "waveCount": 1, "waveDelay": 0 },
        { "cost": 80, "damage": 16.0, "range": 2.6, "attackSpeed": 1.1, "color": "#64B4FF", "penetration": 0, "shotCount": 1, "spreadAngle": 0, "waveCount": 1, "waveDelay": 0 },
        { "cost": 100, "damage": 21.0, "range": 2.8, "attackSpeed": 1.2, "color": "#64B4FF", "penetration": 0, "shotCount": 1, "spreadAngle": 0, "waveCount": 1, "waveDelay": 0 }
    ],
    "markers": [
        { "type": "slow", "factor": 0.5, "duration": 2.0 },
        { "type": "slow", "factor": 0.4, "duration": 2.5 },
        { "type": "slow", "factor": 0.3, "duration": 3.0 }
    ]
}
```

**示例：Arrow 满级多发配置**：
```json
{ "cost": 75, "damage": 34.0, "range": 16.5, "attackSpeed": 0.73, "color": "#8BC34A", "penetration": 3, "shotCount": 3, "spreadAngle": 30, "waveCount": 2, "waveDelay": 0.3 }
```

**levels[] 字段**（纯攻击属性）：

| 字段 | 类型 | 说明 |
|------|------|------|
| `cost` | int | 建造成本 |
| `damage` | double | 基础伤害 |
| `range` | double | 射程（grids） |
| `attackSpeed` | double | 攻击间隔（秒） |
| `color` | string | 渲染颜色 |
| `penetration` | int | 穿透敌人数（0表示不穿透） |
| `shotCount` | int | 每波子弹数（奇数，如3,5,7） |
| `spreadAngle` | int | 分散角度总范围（度） |
| `waveCount` | int | 连续发几波 |
| `waveDelay` | double | 每波之间延迟（秒） |

**markers[] 字段**（效果属性，通过 Marker 执行）：

| 字段 | 类型 | 说明 |
|------|------|------|
| `type` | string | `"slow"` 或 `"poison"` |
| `factor` | double | slowFactor 或 poisonDps |
| `duration` | double | 持续时间（秒） |

**设计原则**：TowerStats 只保留纯攻击属性，效果属性由 Marker 管理，实现关注点分离。

### 13.8 网格坐标系统

### 13.9 设计原则

所有游戏内部实体（Enemy、Obstacle、Tower、Bullet）的位置和移动全部使用**网格坐标**存储，渲染时通过 `cellSize` 和 `offset` 换算为像素坐标。

**目的**：消除窗口 resize 时的同步需求——渲染时自动使用当前的 cellSize/offset 计算，无需更新任何实体的内部状态。

### 13.10 单位约定

| 类型 | 单位 | 说明 |
|------|------|------|
| 位置 `m_gridPos` / `m_gridX/Y` | 网格单位 (grid) | 1 grid = 1 cell |
| 速度 `speed()` | grids/sec | 网格单位/秒 |
| 射程/范围 `range` | grids | 网格单位 |
| 溅射半径 `splashRadius` | grids | 网格单位 |
| 攻击半径 `m_attackRadius` | grids | 网格单位（MeleeTower） |
| 链式闪电距离 `chainDist` | grids | 硬编码约3 grids |
| 碰撞半径 `hitRadius` | grids | 约0.5 grids |
| 绘制半径 `radius()` | **像素** (pixels) | JSON中定义的是像素，用于绘制 |

### 14.1 格子中心点约定

**格子 N 的中心点**位于 `(N+0.5, N+0.5)`
**格子 N 的有效范围**为 `[N, N+1)`（左闭右开）

```
格子 0  中心: (0.5, 0.5)  范围: [0, 1)
格子 1  中心: (1.5, 1.5)  范围: [1, 2)
格子 2  中心: (2.5, 2.5)  范围: [2, 3)
...
```

### 14.4 坐标转换

**网格 → 像素**（绘制）：
```cpp
QPointF gridToPixel(int gx, int gy) {
    return QPointF(offsetX + (gx + 0.5) * cellSize,
                   offsetY + (gy + 0.5) * cellSize);
}
```

**像素 → 网格**（查格子）：
```cpp
int gx = static_cast<int>(std::floor((pos.x() - offsetX) / cellSize));
int gy = static_cast<int>(std::floor((pos.y() - offsetY) / cellSize));
```

### 14.5 各类实体的网格化实现

#### Enemy
- `m_gridPos` : QPointF — 网格坐标，初始值为 `(path[0].x() + 0.5, path[0].y() + 0.5)`
- `m_path` : std::vector<QPointF> — 存储整数坐标，移动时加 0.5 转换为中心点
- `speed()` : 从 JSON 加载时 `speed / 48.0` 转为 grids/sec
- `pos(cellSize, offsetX, offsetY)` — 返回像素坐标 `offset + m_gridPos * cellSize`
- `draw()` — 使用 `pos()` 转为像素后绘制

#### Obstacle
- `m_gridX/Y/W/H` — 整数网格坐标
- `draw(cellSize, offsetX, offsetY)` — 中心 `(gridX + gridW/2, gridY + gridH/2) * cellSize`
- 碰撞范围：`[gridX, gridX+gridW)` × `[gridY, gridY+gridH)`（左闭右开）

#### Tower
- `m_gridX/Y` — 整数网格坐标
- `centerPos(cellSize, offsetX, offsetY)` — 返回 `(gridX + 0.5, gridY + 0.5) * cellSize`
- `rangePx()` — 返回 `range() * m_cellSize`（像素，用于渲染预览圈）
- `range()` — 返回 `m_baseStats[m_level].range`（grids）
- `damage()`, `attackSpeed()`, `color()` 等 — 通过 level 直接索引 `m_baseStats`

#### Bullet
- `m_pos` — 网格坐标（中心点），初始化为塔的中心 `(gridX + 0.5, gridY + 0.5)`
- 移动：`m_pos += m_direction * m_speed * dt`（固定方向，不追踪目标）
- 碰撞检测：直接在网格空间计算 `[gridX, gridX+gridW)` × `[gridY, gridY+gridH)`
- `draw()` — 转为像素坐标 `offset + m_pos * cellSize`

### 14.6 DataManager 加载时的单位转换

```cpp
// 敌人速度：pixels/sec → grids/sec
s.speed = obj["speed"].toDouble() / 48.0;

// 塔射程/溅射：直接使用网格单位（JSON 中已是 grids）
s.range = obj["range"].toDouble();
s.splashRadius = obj["splashRadius"].toDouble();
```

### 14.7 RemoteTower 距离计算（网格空间）

```cpp
double RemoteTower::distTo(const Enemy& e) const {
    QPointF gp = e.gridPos();
    double dx = gp.x() - (m_gridX + 0.5);
    double dy = gp.y() - (m_gridY + 0.5);
    return dx*dx + dy*dy;  // 平方距离，避免开方
}

// 射程检测：直接用 range()（grids）的平方比较
double r2 = range() * range();
```

### 14.8 MeleeTower 效果范围（GameController 中结算）

```cpp
// effect.center 是网格坐标 QPointF(m_gridX + 0.5, m_gridY + 0.5)
// effect.radius 是网格单位（如 2.0 grids）
double centerX = offsetX + effect.center.x() * cellSize;
double centerY = offsetY + effect.center.y() * cellSize;
double effectRadiusPx = effect.radius * cellSize;
```

### 14.9 Bullet 碰撞检测

```cpp
// 障碍物碰撞：使用左闭右开区间
if (m_pos.x() >= obs->gridX() && m_pos.x() < obs->gridX() + obs->gridWidth() &&
    m_pos.y() >= obs->gridY() && m_pos.y() < obs->gridY() + obs->gridHeight()) {
    // 碰撞
}

// 敌人碰撞：hitRadius 约 0.5 grids
QPointF d = e->gridPos() - m_pos;
double d2 = d.x()*d.x() + d.y()*d.y();
if (d2 <= 0.5 * 0.5) { /* hit */ }
```

### 14.10 Resize 行为

resize 时**无需同步任何实体**：
```cpp
void InputHandler::handleResize(int width, int height) {
    // 仅更新 cellSize 和 offset
    m_spatialGrid->setCellSize(cellSize);
    m_spatialGrid->setOffset(offsetX, offsetY);
    // 渲染器在下一帧 paintEvent 时自动使用新的 cellSize/offset
}
```

### 14.11 优先级系统

优先级目标互斥：设置敌方优先级自动清除障碍物优先级，反之亦然。
```cpp
void setPriorityEnemy(Enemy* e) { m_priorityEnemy = e; m_priorityObstacle = nullptr; }
void setPriorityObstacle(Obstacle* o) { m_priorityObstacle = o; m_priorityEnemy = nullptr; }
```

---

## 15. 新手教程系统

### 15.1 概述

教程以独立的 `level0` 关卡形式存在，玩家从主菜单点"开始游戏"直接进入。教程采用步骤状态机，通过事件过滤器和遮罩箭头引导玩家完成 7 个步骤。

### 15.2 组件

| 组件 | 类型 | 职责 |
|------|------|------|
| `TutorialController` | QObject | 步骤状态机，管理步骤切换、暂停/恢复、eventFilter |
| `TutorialArrow` | QWidget | 半透明遮罩 + 挖洞高亮 + 箭头绘制 + 文字提示 |

### 15.3 步骤推进机制

| 步骤类型 | 完成条件 | 检测方式 |
|----------|---------|---------|
| `ClickContinue` | 玩家点击屏幕任意处 | Arrow 发射 `clicked()` 信号 → Controller 推进 |
| `PlaceTower` | 指定格子成功放置塔 | 监听 `TowerManager::towersChanged` 信号检测 |
| `ClickCell` | 玩家点击指定格子 | eventFilter 检测坐标 + QTimer 延迟推进 |
| `TowerUpgraded` | 指定格子的塔升级到 ≥2 级 | 监听 `GameController::statsChanged` 信号检测 |
| `EnemyKilled` | 任意敌人死亡 | GameController 回调通知 |

### 15.4 遮罩系统

TutorialArrow 通过 `setMask()` 实现交互步骤的点击过滤：

```
mask = 整屏区域 - 目标格子区域
  → 目标格内：Qt 认为 Arrow 不存在于此点 → 点击穿透至底层 InputHandler
  → 目标格外：Qt 认为 Arrow 存在于此点 → Arrow::mousePressEvent 吞掉事件
```

Arrow 同时覆写 `mousePressEvent`，对落入 mask 的点击调用 `event->accept()` 阻止事件冒泡到父控件。

### 15.5 游戏暂停（timeScale）

教程通过 `GameController::setTimeScale(0)` 冻结游戏时间，而非使用 `pauseGame()`。这使得 `isPaused()` 保持 false，InputHandler 不拦截输入，玩家在教程引导下仍可正常操作地图。

### 15.6 与现有系统的集成点

| 集成点 | 方式 |
|--------|------|
| `GameScene::startGame()` | 检测 `levelId == 0` 时创建 `TutorialController`，不启动 gameTimer |
| `GameController::setOnEnemyKilled()` | 注册 lambda 回调，敌人死亡时通知教程 |
| eventFilter | 安装于 GameScene，处理 ClickCell 点击检测和 resize |
| Arrow::clicked 信号 | Controller 直接连接，处理 ClickContinue 步骤 |
| `GameScene::resumeClock()` | 步骤 4 时启动 gameTimer 和 QElapsedTimer |
| `TowerManager::towersChanged` | 检测塔放置 |
| `GameController::statsChanged` | 检测塔升级 |

### 15.7 教程结束

`m_currentStep >= 7` 时调用清理：`removeEventFilter`、隐藏 Arrow、`setTimeScale(1.0)` 恢复游戏。Controller 自身随 GameScene 销毁（Qt parent 机制）。

### 15.8 文件

| 文件 | 说明 |
|------|------|
| `ui/tutorialcontroller.h/cpp` | 步骤状态机 |
| `ui/tutorialarrow.h/cpp` | 遮罩 + 箭头 |
| `config/levels/level0.json` | 教程关数据 |
| `config/levels.json` | 含 `"id": 0, "name": "新手教程"` 条目 |
| `game/gamecontroller.h/cpp` | 新增 `m_timeScale` 和 `setOnEnemyKilled` |
| `ui/gamescene.cpp` | `startGame()` 中创建 TutorialController |
```

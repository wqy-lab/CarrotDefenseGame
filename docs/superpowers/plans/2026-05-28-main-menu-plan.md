# Main Menu & Level Select — Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans
>
> **Spec:** `docs/superpowers/specs/2026-05-28-main-menu-design.md`
>
> **Branch:** `sck_test`
>
> **Waves:** 2 (data layer → UI layer)

---

## File Responsibility Map

| File | Responsibility |
|------|---------------|
| `config/levels.json` | Level index: id, name, file path |
| `game/config/datamanager.h/.cpp` | Add `LevelEntry`, `loadLevelsIndex()`, `levels()` |
| `ui/mainmenuwidget.ui/.h/.cpp` | Title + 3 buttons, emits signals on click |
| `ui/levelselectwidget.ui/.h/.cpp` | QListWidget (IconMode), dynamic addItem loop, emits levelSelected |
| `ui/gameresultwidget.ui/.h/.cpp` | Overlay with result text + retry/next/menu buttons |
| `ui/mainwindow.ui/.h/.cpp` | QStackedWidget wiring, page navigation, overlay lifecycle |
| `ui/gamescene.h` | Signal signature: `gameEnded(bool won, int levelId)` |
| `main.cpp` | Add `loadLevelsIndex()` call |
| `CMakeLists.txt` | Add 6 new source files |

---

## Wave 1: Data Layer (30 min)

### Task 1.1: Create `config/levels.json`
- **Action:** Write `config/levels.json` with 2 levels
- **Content:** `[{ "id": 1, "name": "蛇形关", "file": "levels/level1.json" }, ...]`
- **QA:** File is valid JSON

### Task 1.2: Extend DataManager header
- **Files:** `game/config/datamanager.h`
- **Action:** Add `struct LevelEntry { int id; QString name; QString file; };`, `bool loadLevelsIndex(const QString& path);`, `const std::vector<LevelEntry>& levels() const;`, plus `std::vector<LevelEntry> m_levels;` member
- **QA:** No new includes needed beyond existing

### Task 1.3: Implement `loadLevelsIndex()` in DataManager
- **Files:** `game/config/datamanager.cpp`
- **Action:** Parse the `levels` JSON array, populate `m_levels`
- **QA:** Read `config/levels.json`, verify `m_levels.size() == 2`

### Task 1.4: Call `loadLevelsIndex()` in main.cpp
- **Files:** `main.cpp`
- **Action:** Add `DataManager::instance().loadLevelsIndex("config/levels.json");` after existing load calls, with error check
- **QA:** Program starts without load error

### Task 1.5: Build & verify data layer
- **Action:** Build, fix any compilation errors
- **QA:** Clean build, no errors

---

## Wave 2: UI Layer (2 hours)

### Task 2.1: Create `mainmenuwidget.ui`
- **Files:** `ui/mainmenuwidget.ui`
- **Action:** Write XML: QWidget with QVBoxLayout containing QLabel ("保卫萝卜") + 3 QPushButton ("开始游戏", "选择关卡", "退出")
- **QA:** UIC compiles without errors

### Task 2.2: Create `mainmenuwidget.h/.cpp`
- **Files:** `ui/mainmenuwidget.h`, `ui/mainmenuwidget.cpp`
- **Action:** `class MainMenuWidget : public QWidget { Q_OBJECT }` with 3 signals: `startGameClicked()`, `levelSelectClicked()`, `exitClicked()`. Constructor loads `.ui`, connects buttons to signals.
- **QA:** Compiles, signals emit on button click

### Task 2.3: Create `levelselectwidget.ui`
- **Files:** `ui/levelselectwidget.ui`
- **Action:** Write XML: QWidget with QVBoxLayout. Top row: QLabel ("选择关卡") + QPushButton ("返回"). Main area: empty QListWidget (IconMode, Wrapping, LeftToRight).
- **QA:** UIC compiles without errors

### Task 2.4: Create `levelselectwidget.h/.cpp`
- **Files:** `ui/levelselectwidget.h`, `ui/levelselectwidget.cpp`
- **Action:** `class LevelSelectWidget : public QWidget { Q_OBJECT }`. Constructor: loop `DataManager::instance().levels()`, `addItem()` to QListWidget. Each item stores `id` and `file` via `setData(Qt::UserRole, ...)`. Connect `itemClicked` → emit `levelSelected(int id, QString file)`. Lock handling: `#ifdef QT_DEBUG` all enabled, else disable items after first.
- **QA:** Shows 2 items, clicking emits correct id+file

### Task 2.5: Create `gameresultwidget.ui`
- **Files:** `ui/gameresultwidget.ui`
- **Action:** Write XML: QWidget with semi-transparent dark background. QVBoxLayout centered: QLabel (result text) + QHBoxLayout with 3 QPushButton ("重试", "下一关", "返回主菜单").
- **QA:** UIC compiles without errors

### Task 2.6: Create `gameresultwidget.h/.cpp`
- **Files:** `ui/gameresultwidget.h`, `ui/gameresultwidget.cpp`
- **Action:** `class GameResultWidget : public QWidget { Q_OBJECT }`. Constructor takes `bool won, int levelId`. Sets result text. Hides "下一关" if not won. 3 signals: `retryClicked()`, `nextLevelClicked()`, `menuClicked()`. Connects buttons.
- **QA:** Widget appears with correct text, only 3 buttons on victory

### Task 2.7: Modify `gamescene.h` signal
- **Files:** `ui/gamescene.h`
- **Action:** Change `gameEnded(bool won)` → `gameEnded(bool won, int levelId)`. Add `m_currentLevelId` member. Set it when level is loaded.
- **QA:** No other changes to .cpp needed beyond emit call

### Task 2.8: Modify `mainwindow.ui` for QStackedWidget
- **Files:** `ui/mainwindow.ui`
- **Action:** Replace `centralwidget` with QStackedWidget. This is the only XML change — all pages added in C++.
- **QA:** UIC compiles

### Task 2.9: Modify `mainwindow.h`
- **Files:** `ui/mainwindow.h`
- **Action:** Add `QStackedWidget* m_stackedWidget;`, `MainMenuWidget* m_menu;`, `LevelSelectWidget* m_levelSelect;`, `GameScene* m_scene;`, `int m_currentLevelId;`. Add private slots: `onStartGame()`, `onLevelSelected(int id, QString file)`, `onGameEnded(bool won, int levelId)`, `onRetry()`, `onNextLevel()`, `onMenu()`. Add `loadAndStartLevel(int id, const QString& file)`.
- **QA:** No compile errors from missing includes

### Task 2.10: Rewrite `mainwindow.cpp` — Phase A (page setup)
- **Files:** `ui/mainwindow.cpp`
- **Action:** Replace constructor body: create QStackedWidget, create MainMenuWidget/LevelSelectWidget/GameScene, add to stack. Set centralWidget to stackedWidget. Show page 0 on launch. Wire all signals.
- **QA:** Program shows main menu at startup

### Task 2.11: Rewrite `mainwindow.cpp` — Phase B (level loading)
- **Files:** `ui/mainwindow.cpp`
- **Action:** `loadAndStartLevel()`: call `DataManager::instance().loadLevel(file)`, call `m_scene->resetGame()`, switch to page 2. Remove old Start Wave button and toolbar (game now auto-starts or via in-game button).
- **QA:** Clicking level card starts correct level

### Task 2.12: Rewrite `mainwindow.cpp` — Phase C (end screen overlay)
- **Files:** `ui/mainwindow.cpp`
- **Action:** `onGameEnded(won, levelId)`: disable toolbar, create `GameResultWidget` overlay matching GameScene geometry, show it, connect its signals to `onRetry/onNextLevel/onMenu`.
- **QA:** Game ends → overlay appears with correct buttons

### Task 2.13: Rewrite `mainwindow.cpp` — Phase D (retry/next/menu)
- **Files:** `ui/mainwindow.cpp`
- **Action:** `onRetry()`: delete overlay, reload same level. `onNextLevel()`: delete overlay, load levelId+1. `onMenu()`: delete overlay, switch to page 0.
- **QA:** Each button navigates correctly

### Task 2.14: Update `CMakeLists.txt`
- **Files:** `CMakeLists.txt`
- **Action:** Add 6 new files to PROJECT_SOURCES
- **QA:** Build passes

### Task 2.15: Final build & manual QA
- **Action:** Full build, run. Test flow: Main menu → Level Select → choose level 1 → play → die/win → overlay → retry/menu. Test "Start Game" shortcut.
- **QA:** All flows work, no crashes

---

## Dependency Graph

```
Wave 1:
  1.1 ──→ 1.2 ──→ 1.3 ──→ 1.4 ──→ 1.5
                                       │
Wave 2 (all depend on Wave 1):         │
                                       │
  ┌─ 2.1 ──→ 2.2 ─────────────────────┐
  ├─ 2.3 ──→ 2.4 ─────────────────────┤
  └─ 2.5 ──→ 2.6 ─────────────────────┤
                                       │
  2.7 ──→ 2.8 ──→ 2.9 ──→ 2.10 ──→ 2.11 ──→ 2.12 ──→ 2.13 ──→ 2.14 ──→ 2.15
           (2.7 is independent of 2.1-2.6)
```

**Parallel opportunities:** Tasks 2.1+2.3+2.5 (.ui XML) can be done simultaneously. Tasks 2.2+2.4+2.6 (C++ wrappers) can follow simultaneously.

---

## Verification Checklist

- [ ] `main.cpp` starts without JSON load errors
- [ ] Main menu appears with 3 buttons
- [ ] "开始游戏" launches level 1
- [ ] "选择关卡" shows level list with 2 items
- [ ] Clicking level card loads correct JSON and starts game
- [ ] "返回" navigates back to main menu
- [ ] Game over → overlay with result text
- [ ] Victory overlay shows 3 buttons, defeat shows 2 (no "下一关")
- [ ] "重试" restarts same level
- [ ] "下一关" loads next level
- [ ] "返回主菜单" goes to main menu
- [ ] Build passes cleanly

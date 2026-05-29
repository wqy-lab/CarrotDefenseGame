# Main Menu & Level Select — Design Spec

## Overview

Replace the current "launch directly into game" flow with a proper main menu and level selection system.

## Game Flow

```
Launch → MainMenu → Start Game → Level 1 (shortcut)
                  → Level Select → choose level → Battle
                                                 ↓
                                    End Screen: VICTORY / GAME OVER
                                    [重试] [下一关] [返回主菜单]
```

## Page Architecture

**QStackedWidget** inside MainWindow manages 3 pages:

| Index | Page | Type |
|-------|------|------|
| 0 | MainMenuWidget | New `.ui` QWidget |
| 1 | LevelSelectWidget | New `.ui` QWidget |
| 2 | GameScene | Existing C++ QWidget |

## Each Page

### MainMenuWidget (Page 0)

```
┌──────────────────────┐
│      保卫萝卜         │
│                      │
│   [ 开始游戏 ]        │  → starts level 1 directly
│   [ 选择关卡 ]        │  → switches to page 1
│   [ 退  出 ]          │  → QApplication::quit()
└──────────────────────┘
```

- `.ui` XML defines layout (QVBoxLayout + QLabel + 3 QPushButtons)
- C++ class `MainMenuWidget` emits signals: `startGameClicked()`, `levelSelectClicked()`, `exitClicked()`
- Style: dark background, centered, matches existing app dark theme

### LevelSelectWidget (Page 1)

```
┌──────────────────────────────────┐
│  选择关卡                    [返回]│
│                                  │
│  ┌────────┐ ┌────────┐          │
│  │ 第1关  │ │ 第2关  │  ...     │
│  │ 蛇形关 │ │ 障碍关 │          │
│  └────────┘ └────────┘          │
└──────────────────────────────────┘
```

- Level list comes from `config/levels.json` index file
- `.ui` XML defines an empty `QListWidget` in **IconMode** + **Wrapping**
- C++ dynamically calls `addItem()` in a loop — no hardcoded buttons in XML
- **Debug build** (`#ifdef QT_DEBUG`): all levels unlocked
- **Release build**: level N+1 locked until level N is completed
- Click unlocked level → emit `levelSelected(int id, QString file)` → MainWindow loads level and switches to page 2
- "返回" button → switch to page 0

### GameResultWidget — End-screen overlay

```
┌──────────────────────────────────┐
│  ╔══════════════════════════╗    │
│  ║     VICTORY!             ║    │
│  ║                          ║    │
│  ║  [重试] [下一关] [返回]   ║    │
│  ╚══════════════════════════╝    │
│  (GameScene dimmed behind)       │
└──────────────────────────────────┘
```

- New `.ui` QWidget, NOT inside QStackedWidget
- Created dynamically by MainWindow when it receives `gameEnded()` signal
- Positioned as an overlay on top of GameScene (same geometry, raised)
- **GameScene itself is untouched** — it only emits signals, no UI changes
- Buttons emit signals back to MainWindow: `retryClicked()`, `nextLevelClicked()`, `menuClicked()`
- MainWindow destroys the overlay and takes appropriate action

## Data Files

### config/levels.json (new)

```json
{
    "levels": [
        { "id": 1, "name": "蛇形关", "file": "levels/level1.json" },
        { "id": 2, "name": "障碍关", "file": "levels/level2.json" }
    ]
}
```

Loaded by DataManager via new `loadLevelsIndex(path)` method. Provides `levels()` → `std::vector<LevelEntry>`.

### DataManager additions

```cpp
struct LevelEntry {
    int id;
    QString name;
    QString file;  // relative path to level JSON
};

bool loadLevelsIndex(const QString& path);
const std::vector<LevelEntry>& levels() const;
```

## Files to Create/Modify

| File | Action | Description |
|------|--------|-------------|
| `config/levels.json` | **New** | Level index |
| `ui/mainmenuwidget.ui/.h/.cpp` | **New** | Main menu (title + 3 buttons) |
| `ui/levelselectwidget.ui/.h/.cpp` | **New** | Level select (QListWidget IconMode, dynamic) |
| `ui/gameresultwidget.ui/.h/.cpp` | **New** | End-screen overlay (result text + buttons) |
| `ui/mainwindow.ui` | **Modify** | QStackedWidget replacing centralwidget |
| `ui/mainwindow.h` | **Modify** | Page navigation, overlay management, game end logic |
| `ui/mainwindow.cpp` | **Modify** | Wire all signals, load level on selection, create/destroy overlay |
| `ui/gamescene.h` | **Modify** | Add `gameEnded(bool won, int levelId)` signal (levelId for "next level" logic) |
| `game/config/datamanager.h` | **Modify** | Add `LevelEntry`, `loadLevelsIndex()`, `levels()` |
| `game/config/datamanager.cpp` | **Modify** | Parse `levels.json` |
| `main.cpp` | **Modify** | Call `loadLevelsIndex()` |
| `CMakeLists.txt` | **Modify** | Add 6 new source files |

> **GameScene gets NO UI changes** — only a signal signature change. End-screen is `GameResultWidget`, a separate overlay widget.

## Unlock Logic

```cpp
#ifdef QT_DEBUG
    // All levels unlocked
#else
    // Level 1 always unlocked
    // Level N unlocked if player has beaten level N-1
    // (requires storing unlock progress — future enhancement, hardcode for now)
#endif
```

## Implementation Order

1. Data layer: `levels.json` + DataManager `loadLevelsIndex`
2. Main menu: `mainmenuwidget.ui/.h/.cpp`
3. Level select: `levelselectwidget.ui/.h/.cpp`
4. MainWindow: QStackedWidget wiring, page navigation
5. GameScene: end-screen buttons and signals
6. Integration: `main.cpp` load call, CMakeLists update

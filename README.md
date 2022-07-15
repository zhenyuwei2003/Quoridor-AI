# Quoridor AI


## 简介

- 上海交通大学，2021级CS，MeteorVanish
- 2021-2022学年夏季学期，PPCA Project 2
- 用 C++ 实现棋类游戏 `Quoridor` 的简易AI
- 使用算法：`Minimax + Alpha-beta Pruning` / `Monte Carlo Tree Search`

## 游戏介绍

### 基本玩法

[Quoridor](https://en.wikipedia.org/wiki/Quoridor)，中文名为步步为营或是墙棋。棋盘由9×9的格子构成，格子间有凹槽。游戏双方各有一枚1×1的棋子，初始位于上下底边中间；各有10片长度为2格的木板（墙），可放置在凹槽中。玩家每一步可以向四个方向移动一格棋子或是放置一片木板，率先抵达对方底线的玩家获胜。

### 细节规则

- **移动**
  玩家移动方向不能有墙阻挡（边界也算墙），若移动方向被对方棋子阻挡，在对方身后没有墙阻挡的情况下，可以跳过对方棋子，否则可以跳到对方棋子左右两侧（左右两侧没有墙阻挡的情况下）

- **放墙**
木板之间不能重叠或交叉，每一片木板放置后须保证双方棋子均存在一条抵达对方底线的路线

### 游戏特点

- 与围棋、象棋等棋类游戏一样，为**Combinatorial Game**，即具有**零和**（zero-sum），**透明信息**（fully information），**确定性**（determinism），顺序性（sequential），**离散性**（discrete）这些特点，因此适合用 `Minimax` 或是 `MCTS` 这两种算法，来逼近博弈过程的纳什均衡点。

## 文件内容

- **`down` 文件夹**：助教下发的所有内容，包括项目要求、代码框架、本地评测程序和学习资料
- **`src` 文件夹**：实现两种算法的AI源程序
- **`exe` 文件夹**：两种算法在 `Windows` 下编译的 `exe` 文件、助教下发的评测程序

## 如何运行

``` shell
py judge.py "human/AI name 1" "human/AI name 2"
```

*（注：运行评测程序需要本地安装 `python` 的 `numpy` 库）*

### 示例

- 两种不同算法AI对战 `py judge.py "Yosoro! [minimax].exe" "Yosoro! [MCTS].exe"`
- 人人对战 `py judge.py human human`
- 人机对战（先手） `py judge.py human "Yosoro! [minimax].exe"`

## 代码框架

见 `down\sample.cpp` 文件

- `init()` 函数：在对局开始前调用一次
- `action()`函数：接受对方操作 `loc`，返回己方操作，操作码含义见 `README` 文件
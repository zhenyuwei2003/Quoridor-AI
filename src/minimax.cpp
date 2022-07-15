// minimax + alpha/beta pruning

#include <bits/stdc++.h>
#include "AIController.h"

#define mp std::make_pair
#define mpp(a, b, c) std::make_pair(a, std::make_pair(b, c))

using std::pair;
using std::vector;
using std::priority_queue;
using std::max;
using std::min;

extern int ai_side;
std::string ai_name = "Yosoro! [minimax]";

enum DIRECTION {UP, DOWN, LEFT, RIGHT};

int Distance(const pair<int, int> pos1, const pair<int, int> pos2)
{ return abs(pos1.first - pos2.first) + abs(pos1.second - pos2.second); }

pair<int, int> NewPosition(const pair<int, int> pos, const DIRECTION dir)
{
    switch (dir)
    {
        case UP: return mp(pos.first - 1, pos.second);
        case DOWN: return mp(pos.first + 1, pos.second);
        case LEFT: return mp(pos.first, pos.second - 1);
        case RIGHT: return mp(pos.first, pos.second + 1);
    }
}

bool CheckWall(const pair<int, int> pos, const DIRECTION dir, const bool vertical[8][9], const bool horizontal[8][9])
{
    if (dir == UP && (pos.first == 0 || horizontal[pos.first - 1][pos.second])) return true;
    if (dir == DOWN && (pos.first == 8 || horizontal[pos.first][pos.second])) return true;
    if (dir == LEFT && (pos.second == 0 || vertical[pos.second - 1][pos.first])) return true;
    if (dir == RIGHT && (pos.second == 8 || vertical[pos.second][pos.first])) return true;
    return false;
}

class STATE
{
private:
    pair<int, int> Position[2];
    bool WallVertical[8][9];   // [col][row]
    bool WallHorizontal[8][9]; // [row][col]
    bool WallPoint[8][8];
    int Round, WallNum[2];

public:
    STATE()
    {
        Position[0] = mp(8, 4);
        Position[1] = mp(0, 4);
        Round = 1;
        WallNum[0] = WallNum[1] = 10;
        memset(WallVertical, false, sizeof(WallVertical));
        memset(WallHorizontal, false, sizeof(WallHorizontal));
        memset(WallPoint, false, sizeof(WallPoint));
    }
    ~STATE() = default;

    int GetRound() const { return Round; }
    pair<int, int> GetPosition(const int Index) const { return Position[Index]; }

    void Update(const int id, const pair<int, pair<int, int> > act)
    {
        ++Round;
        pair<int, int> pos = act.second;
        if (act.first == 0) Position[id] = pos;
        else if(act.first == 1)
        {
            --WallNum[id];
            WallPoint[pos.first][pos.second] = true;
            WallVertical[pos.second][pos.first] = WallVertical[pos.second][pos.first + 1] = true;
        }
        else if(act.first == 2)
        {
            --WallNum[id];
            WallPoint[pos.first][pos.second] = true;
            WallHorizontal[pos.first][pos.second] = WallHorizontal[pos.first][pos.second + 1] = true;
        }
    }

    bool IsTerminal() const
    {
        if (Position[0].first == 0 || Position[1].first == 8) return true;
        else return false;
    }

    // return <state> after <act>
    STATE Result(const int id, const pair<int, pair<int, int> > act) const
    {
        STATE state = *this;
        pair<int, int> pos = act.second;
        if (act.first == 0) state.Position[id] = pos;
        else if(act.first == 1)
        {
            --state.WallNum[id];
            state.WallPoint[pos.first][pos.second] = true;
            state.WallVertical[pos.second][pos.first] = state.WallVertical[pos.second][pos.first + 1] = true;
        }
        else if(act.first == 2)
        {
            --state.WallNum[id];
            state.WallPoint[pos.first][pos.second] = true;
            state.WallHorizontal[pos.first][pos.second] = state.WallHorizontal[pos.first][pos.second + 1] = true;
        }
        return state;
    }

    // implemented with A-star algorithm
    int ToSteps(const int id, const bool IsWin) const
    {
        struct POINT
        {
            pair<int, int> pos;
            int f, g, h;

            POINT() = default;
            POINT(pair<int, int> &pos_, int g_, int h_) : pos(pos_), f(g_ + h_), g(g_), h(h_) {}
            void Init(pair<int, int> &pos_, int g_, int h_) { pos = pos_; f = g_ + h_; g = g_; h = h_; }
            bool operator<(const POINT &rhs) const { return f > rhs.f; }
        };
        bool vis[9][9];
        memset(vis, false, sizeof(vis));
        pair<int, int> pos = Position[id], posNew;
        priority_queue<POINT> PQ;
        if (IsWin) PQ.push(POINT(pos, 0, (id ? 8 - pos.first : pos.first)));
        else PQ.push(POINT(pos, 0, (id ? pos.first : 8 - pos.first)));
        vis[pos.first][pos.second] = true;
        while (!PQ.empty())
        {
            POINT cur = PQ.top();
            PQ.pop();
            for (int i = 0; i < 4; ++i)
            {
                auto dir = static_cast<DIRECTION>(i);
                if (!CheckWall(cur.pos, dir, WallVertical, WallHorizontal))
                {
                    posNew = NewPosition(cur.pos, dir);
                    if (vis[posNew.first][posNew.second]) continue;
                    vis[posNew.first][posNew.second] = true;
                    POINT nxt;
                    if (IsWin) nxt.Init(posNew, cur.g + 1, (id ? 8 - posNew.first : posNew.first));
                    else nxt.Init(posNew, cur.g + 1, (id ? posNew.first : 8 - posNew.first));
                    if (posNew.first == 8 * id) return nxt.g;
                    PQ.push(nxt);
                }
            }
        }
        if (IsWin) return -1;
        else return 20;
    }

    pair<int, int> LegalAction(const int id, pair<int, pair<int, int> > act[]) const
    {
        int cnt1 = 0, cnt2 = 0;
        pair<int, int> pos = Position[id], posEnemy = Position[id ^ 1], posNew;
        for (int i = 0; i < 4; ++i)
        {
            auto dir = static_cast<DIRECTION>(i);
            posNew = NewPosition(pos, dir);
            if (CheckWall(pos, dir, WallVertical, WallHorizontal)) continue;
            else if (posNew == posEnemy)
            {
                if (dir == UP && !CheckWall(pos, UP, WallVertical, WallHorizontal))
                {
                    if (!CheckWall(posEnemy, UP, WallVertical, WallHorizontal))
                        act[++cnt1] = mp(0, NewPosition(posEnemy, UP));
                    else
                    {
                        if (!CheckWall(posEnemy, LEFT, WallVertical, WallHorizontal))
                            act[++cnt1] = mp(0, NewPosition(posEnemy, LEFT));
                        if (!CheckWall(posEnemy, RIGHT, WallVertical, WallHorizontal))
                            act[++cnt1] = mp(0, NewPosition(posEnemy, RIGHT));
                    }
                }
                else if (dir == DOWN && !CheckWall(pos, DOWN, WallVertical, WallHorizontal))
                {
                    if (!CheckWall(posEnemy, DOWN, WallVertical, WallHorizontal))
                        act[++cnt1] = mp(0, NewPosition(posEnemy, DOWN));
                    else
                    {
                        if (!CheckWall(posEnemy, LEFT, WallVertical, WallHorizontal))
                            act[++cnt1] = mp(0, NewPosition(posEnemy, LEFT));
                        if (!CheckWall(posEnemy, RIGHT, WallVertical, WallHorizontal))
                            act[++cnt1] = mp(0, NewPosition(posEnemy, RIGHT));
                    }
                }
                else if (dir == LEFT && !CheckWall(pos, LEFT, WallVertical, WallHorizontal))
                {
                    if (!CheckWall(posEnemy, LEFT, WallVertical, WallHorizontal))
                        act[++cnt1] = mp(0, NewPosition(posEnemy, LEFT));
                    else
                    {
                        if (!CheckWall(posEnemy, UP, WallVertical, WallHorizontal))
                            act[++cnt1] = mp(0, NewPosition(posEnemy, UP));
                        if (!CheckWall(posEnemy, DOWN, WallVertical, WallHorizontal))
                            act[++cnt1] = mp(0, NewPosition(posEnemy, DOWN));
                    }
                }
                else if (dir == RIGHT && !CheckWall(pos, RIGHT, WallVertical, WallHorizontal))
                {
                    if (!CheckWall(posEnemy, RIGHT, WallVertical, WallHorizontal))
                        act[++cnt1] = mp(0, NewPosition(posEnemy, RIGHT));
                    else
                    {
                        if (!CheckWall(posEnemy, UP, WallVertical, WallHorizontal))
                            act[++cnt1] = mp(0, NewPosition(posEnemy, UP));
                        if (!CheckWall(posEnemy, DOWN, WallVertical, WallHorizontal))
                            act[++cnt1] = mp(0, NewPosition(posEnemy, DOWN));
                    }
                }
            }
            else act[++cnt1] = mp(0, posNew);
        }
        if (WallNum[id])
        {
            // vertical
            for (int i = 0; i < 8; ++i)
                for (int j = 0; j < 8; ++j)
                    if (!WallVertical[j][i] && !WallVertical[j][i + 1] && !WallPoint[i][j])
                    {
                        STATE stateTmp = Result(id, mpp(1, i, j));
                        if (stateTmp.ToSteps(0, true) != -1 && stateTmp.ToSteps(1, true) != -1)
                            act[cnt1 + ++cnt2] = mpp(1, i, j);
                    }
            // parallel
            for (int i = 0; i < 8; ++i)
                for (int j = 0; j < 8; ++j)
                    if (!WallHorizontal[i][j] && !WallHorizontal[i][j + 1] && !WallPoint[i][j])
                    {
                        STATE stateTmp = Result(id, mpp(2, i, j));
                        if (stateTmp.ToSteps(0, true) != -1 && stateTmp.ToSteps(1, true) != -1)
                            act[cnt1 + ++cnt2] = mpp(2, i, j);
                    }
        }
        return mp(cnt1, cnt2);
    }

    int Evaluate() const
    {
        int ret = 0;
        int stepsWin[2] = {ToSteps(0, true), ToSteps(1, true)};
        int stepsHome[2] = {ToSteps(0, false), ToSteps(1, false)};

        if (ai_side == 0) ret += 2 * (stepsHome[ai_side] - stepsHome[ai_side ^ 1]);
        if (ai_side == 1) ret += 1 * (stepsHome[ai_side] - stepsHome[ai_side ^ 1]);
        ret += 3 * (stepsWin[ai_side ^ 1] - stepsWin[ai_side]);
        if (WallNum[ai_side] + WallNum[ai_side ^ 1] > 14)
            ret += 1 * (WallNum[ai_side] - WallNum[ai_side ^ 1]);
        else
            ret += 2 * (WallNum[ai_side] - WallNum[ai_side ^ 1]);
        return ret;
    }

    int Utility() const
    {
        if (Position[ai_side].first == 8 * ai_side) return 2000000000;
        else return -2000000000;
    }
} State;

void GetSequence(const STATE &state, vector<int> &sequence, const pair<int, pair<int, int> > act[], const int cnt1, const int cnt2)
{
    bool visit[150];
    memset(visit, false, sizeof(visit));
    for (int i = 1; i <= cnt1; ++i) sequence.push_back(i);
    for (int i = 1; i <= cnt2; ++i)
    {
        int index = 0, dis, disMin = 20, dis0, dis1;
        for (int j = cnt1 + 1; j <= cnt1 + cnt2; ++j)
        {
            if (visit[j]) continue;
            dis0 = Distance(act[j].second, state.GetPosition(0));
            dis1 = Distance(act[j].second, state.GetPosition(1));
            dis = min(dis0, dis1);
            if (dis <= disMin) disMin = dis, index = j;
        }
        sequence.push_back(index);
        visit[index] = true;
    }
}

pair<int, pair<int, pair<int, int> > > minimax(const int depth, const STATE &state, int alpha, int beta, const bool flag)
{
    if (state.IsTerminal())
        return mp(state.Utility(), mpp(-1, 0, 0));
    if (depth == 0)
        return mp(state.Evaluate(), mpp(-1, 0, 0));
    if (flag) // max
    {
        pair<int, pair<int, int> > act[150];
        pair<int, int> cnt = state.LegalAction(ai_side, act);
        vector<int> sequence;
        GetSequence(state, sequence, act, cnt.first, cnt.second);
        int Value, maxValue = -2147483648, index = 0;
        int SearchTimes = min(cnt.first + cnt.second, 80);
        for (int i = 0; i < SearchTimes; ++i)
        {
            Value = minimax(depth - 1, state.Result(ai_side, act[sequence[i]]), alpha, beta, !flag).first;
            if (Value > maxValue) maxValue = Value, index = sequence[i];
            alpha = max(alpha, Value);
            if (alpha >= beta) break;
        }
        return mp(maxValue, act[index]);
    }
    else // min
    {
        pair<int, pair<int, int> > act[150];
        pair<int, int> cnt = state.LegalAction(ai_side ^ 1, act);
        vector<int> sequence;
        GetSequence(state, sequence, act, cnt.first, cnt.second);
        int Value, minValue = 2147483647, index = 0;
        int SearchTimes = min(cnt.first + cnt.second, 80);
        for (int i = 0; i < SearchTimes; ++i)
        {
            Value = minimax(depth - 1, state.Result(ai_side ^ 1, act[sequence[i]]), alpha, beta, !flag).first;
            if (Value < minValue) minValue = Value, index = sequence[i];
            beta = min(beta, Value);
            if (alpha >= beta) break;
        }
        return mp(minValue, act[index]);
    }
}

pair<int, pair<int, int> > Action()
{
    pair<int, pair<int, pair<int, int> > > ret;
    if (State.GetRound() <= 6)
    {
        ret.first = 2147483647;
        if (ai_side == 0) ret.second = mpp(0, State.GetPosition(0).first - 1, 4);
        if (ai_side == 1) ret.second = mpp(0, State.GetPosition(1).first + 1, 4);
    }
    else if ((State.GetRound() == 7 || State.GetRound() == 8) && State.GetPosition(0) == mp(5, 4) && State.GetPosition(1) == mp(3, 4))
    {
        ret.first = 2147483647;
        if (ai_side == 0) ret.second = mpp(2, 5, 4);
        if (ai_side == 1) ret.second = mpp(2, 2, 3);
    }
    else ret = minimax(3, State, -2147483648, 2147483647, true);
    fprintf(stderr, "Player: %d, Round: %d, Value: %d\n", ai_side, State.GetRound(), ret.first);
    return ret.second;
}

// init function is called once at the beginning
void init() { fprintf(stderr, "[Minimax Algorithm]\n\n"); }

/*
 * The following notation is based on player 0's perspective
 * Rows are labeled 0 through 8 from player 1's side to player 0's side
 * Columns are labeled 0 through 8 from player 0's left to right
 * A coordinate is recorded as the row followed by the column, for example, player 0's pawn starts on (8, 4)
 * A pawn move is recorded as the new coordinate occupied by the pawn
 * A fence placement is recorded as the coordinate of the square whose bottom-right corner fits the center of the wall
 * A typecode is defined as follows: 0: pawn move
 *                                   1: vertical fence placement
 *                                   2: horizontal fence placement
 * An action is defined as (typecode, (row-coordinate, column-coordinate))
 * You need to analyze your opponent's action and return your own action
 * If the typecode of your opponent's action is '-1', it means that you are on the offensive.
 */

pair<int, pair<int, int> > action(pair<int, pair<int, int> > loc)
{
    if (loc.first != -1) State.Update(ai_side ^ 1, loc);
    pair<int, pair<int, int> > ret = Action();
    State.Update(ai_side, ret);
    return ret;
}
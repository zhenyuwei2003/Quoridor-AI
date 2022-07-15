// Monte Carlo Tree Search

#include <bits/stdc++.h>
#include "AIController.h"

using std::string;
using std::pair;
using std::make_pair;
using std::priority_queue;

extern int ai_side;
string ai_name = "Yosoro! [MCTS]";

enum DIRECTION {UP, DOWN, LEFT, RIGHT};

pair<int, int> NewPosition(const pair<int, int> pos, const DIRECTION dir)
{
    switch (dir)
    {
        case UP: return make_pair(pos.first - 1, pos.second);
        case DOWN: return make_pair(pos.first + 1, pos.second);
        case LEFT: return make_pair(pos.first, pos.second - 1);
        case RIGHT: return make_pair(pos.first, pos.second + 1);
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
        else return 25;
    }

    // only for SetLegalAction()
    int LegalAction(const int id, pair<int, pair<int, int> > act[]) const
    {
        int cnt = 0;
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
                        act[++cnt] = make_pair(0, NewPosition(posEnemy, UP));
                    else
                    {
                        if (!CheckWall(posEnemy, LEFT, WallVertical, WallHorizontal))
                            act[++cnt] = make_pair(0, NewPosition(posEnemy, LEFT));
                        if (!CheckWall(posEnemy, RIGHT, WallVertical, WallHorizontal))
                            act[++cnt] = make_pair(0, NewPosition(posEnemy, RIGHT));
                    }
                }
                else if (dir == DOWN && !CheckWall(pos, DOWN, WallVertical, WallHorizontal))
                {
                    if (!CheckWall(posEnemy, DOWN, WallVertical, WallHorizontal))
                        act[++cnt] = make_pair(0, NewPosition(posEnemy, DOWN));
                    else
                    {
                        if (!CheckWall(posEnemy, LEFT, WallVertical, WallHorizontal))
                            act[++cnt] = make_pair(0, NewPosition(posEnemy, LEFT));
                        if (!CheckWall(posEnemy, RIGHT, WallVertical, WallHorizontal))
                            act[++cnt] = make_pair(0, NewPosition(posEnemy, RIGHT));
                    }
                }
                else if (dir == LEFT && !CheckWall(pos, LEFT, WallVertical, WallHorizontal))
                {
                    if (!CheckWall(posEnemy, LEFT, WallVertical, WallHorizontal))
                        act[++cnt] = make_pair(0, NewPosition(posEnemy, LEFT));
                    else
                    {
                        if (!CheckWall(posEnemy, UP, WallVertical, WallHorizontal))
                            act[++cnt] = make_pair(0, NewPosition(posEnemy, UP));
                        if (!CheckWall(posEnemy, DOWN, WallVertical, WallHorizontal))
                            act[++cnt] = make_pair(0, NewPosition(posEnemy, DOWN));
                    }
                }
                else if (dir == RIGHT && !CheckWall(pos, RIGHT, WallVertical, WallHorizontal))
                {
                    if (!CheckWall(posEnemy, RIGHT, WallVertical, WallHorizontal))
                        act[++cnt] = make_pair(0, NewPosition(posEnemy, RIGHT));
                    else
                    {
                        if (!CheckWall(posEnemy, UP, WallVertical, WallHorizontal))
                            act[++cnt] = make_pair(0, NewPosition(posEnemy, UP));
                        if (!CheckWall(posEnemy, DOWN, WallVertical, WallHorizontal))
                            act[++cnt] = make_pair(0, NewPosition(posEnemy, DOWN));
                    }
                }
            }
            else act[++cnt] = make_pair(0, posNew);
        }
        if (WallNum[id])
        {
            // vertical
            for (int i = 0; i < 8; ++i)
                for (int j = 0; j < 8; ++j)
                    if (!WallVertical[j][i] && !WallVertical[j][i + 1] && !WallPoint[i][j])
                    {
                        STATE* StateTmp = NextState(make_pair(1, make_pair(i, j)));
                        if (StateTmp->ToSteps(0, true) != -1 && StateTmp->ToSteps(1, true) != -1)
                            act[++cnt] = make_pair(1, make_pair(i, j));
                        delete StateTmp;
                    }
            // horizontal
            for (int i = 0; i < 8; ++i)
                for (int j = 0; j < 8; ++j)
                    if (!WallHorizontal[i][j] && !WallHorizontal[i][j + 1] && !WallPoint[i][j])
                    {
                        STATE* StateTmp = NextState(make_pair(2, make_pair(i, j)));
                        if (StateTmp->ToSteps(0, true) != -1 && StateTmp->ToSteps(1, true) != -1)
                            act[++cnt] = make_pair(2, make_pair(i, j));
                        delete StateTmp;
                    }
        }
        return cnt;
    }

public:
    pair<int, int> Position[2];
    int WallNum[2];
    bool WallVertical[8][9];   // [col][row]
    bool WallHorizontal[8][9]; // [row][col]
    bool WallPoint[8][8];

    int Side;
    int Round;
    int LegalActionNum;
    pair<int, pair<int, int> > Act[150];

    STATE()
    {
        Position[0] = make_pair(8, 4);
        Position[1] = make_pair(0, 4);
        WallNum[0] = WallNum[1] = 10;
        memset(WallVertical, false, sizeof(WallVertical));
        memset(WallHorizontal, false, sizeof(WallHorizontal));
        memset(WallPoint, false, sizeof(WallPoint));
        Side = 0;
        Round = 1;
    }
    STATE(const STATE &rhs) : Side(1 - rhs.Side), Round(rhs.Round + 1)
    {
        Position[0] = rhs.Position[0];
        Position[1] = rhs.Position[1];
        WallNum[0] = rhs.WallNum[0];
        WallNum[1] = rhs.WallNum[1];
        for (int i = 0; i < 8; ++i)
            for (int j = 0; j < 9; ++j)
                WallVertical[i][j] = rhs.WallVertical[i][j], WallHorizontal[i][j] = rhs.WallHorizontal[i][j];
        for (int i = 0; i < 8; ++i)
            for (int j = 0; j < 8; ++j)
                WallPoint[i][j] = rhs.WallPoint[i][j];
    }
    ~STATE() = default;

    void SetLegalAction() { LegalActionNum = LegalAction(Side, Act); }

    bool IsTerminal() const
    {
        if (Position[0].first == 0 || Position[1].first == 8) return true;
        return false;
    }

    STATE* NextState(const pair<int, pair<int, int> > act) const
    {
        auto ret = new STATE(*this);
        pair<int, int> pos = act.second;
        if (act.first == 0) ret->Position[Side] = pos;
        else if(act.first == 1)
        {
            --ret->WallNum[Side];
            ret->WallPoint[pos.first][pos.second] = true;
            ret->WallVertical[pos.second][pos.first] = ret->WallVertical[pos.second][pos.first + 1] = true;
        }
        else if(act.first == 2)
        {
            --ret->WallNum[Side];
            ret->WallPoint[pos.first][pos.second] = true;
            ret->WallHorizontal[pos.first][pos.second] = ret->WallHorizontal[pos.first][pos.second + 1] = true;
        }
        return ret;
    }

    double Evaluate() const
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

        return 1 / (1 + exp(-(double)ret / 4));
    }

    double Utility() const
    {
        if (Position[ai_side].first == 8 * ai_side) return 3;
        else return -2;
    }
};

class NODE
{
public:
    STATE* State;
    NODE* Parent;
    NODE* Child[150];
    int ChildNum;

    int Visit;
    double Quality;
    pair<int, pair<int, int> > PreAction;

    NODE()
    {
        State = nullptr;
        Parent = nullptr;
        ChildNum = 0;
        Visit = 0;
        Quality = 0;
        PreAction = make_pair(-1, make_pair(0, 0));
    }
    ~NODE()
    {
        delete State;
        for (int i = 1; i <= ChildNum; ++i) delete Child[i];
    }

    NODE* Expand()
    {
        auto ExpandNode = new NODE;
        Child[++ChildNum] = ExpandNode;
        ExpandNode->Parent = this;
        ExpandNode->State = State->NextState(State->Act[ChildNum]);
        ExpandNode->State->SetLegalAction();
        ExpandNode->PreAction = State->Act[ChildNum];
        return ExpandNode;
    }

    NODE* BestChild() const
    {
        double Value, MaxValue = 0;
        int BestChildList[150], cnt = 0;
        for (int i = 1; i <= ChildNum; ++i)
        {
            NODE* ChildTmp = Child[i];
            Value = ChildTmp->Quality / ChildTmp->Visit + sqrt(log(Visit) / ChildTmp->Visit);
            if (Value > MaxValue)
            {
                MaxValue = Value;
                BestChildList[cnt = 1] = i;
            }
            else if (Value == MaxValue) BestChildList[++cnt] = i;
        }
        return Child[BestChildList[rand() % cnt + 1]];
    }

    pair<int, pair<int, int> > BestAction() const
    {
        double Value, MaxValue = 0;
        int BestChildList[150], cnt = 0;
        for (int i = 1; i <= ChildNum; ++i)
        {
            NODE* ChildTmp = Child[i];
            Value = ChildTmp->Quality / ChildTmp->Visit;
            if (Value > MaxValue)
            {
                MaxValue = Value;
                BestChildList[cnt = 1] = i;
            }
            else if (Value == MaxValue) BestChildList[++cnt] = i;
        }
        return Child[BestChildList[rand() % cnt + 1]]->PreAction;
    }

    NODE* FindNextNode(pair<int, pair<int, int> > act)
    {
        for (int i = 1; i <= ChildNum; ++i)
            if (Child[i]->PreAction == act) return Child[i];
        NODE* NextNode = new NODE;
        NextNode->State = State->NextState(act);
        NextNode->State->SetLegalAction();
        NextNode->PreAction = act;
        NextNode->Parent = this;
        Child[++ChildNum] = NextNode;
        return NextNode;
    }
} *NowNode;

NODE* SelectionAndExpansion(NODE* Node)
{
    while (!Node->State->IsTerminal())
    {
        if (Node->ChildNum == Node->State->LegalActionNum) Node = Node->BestChild();
        else return Node->Expand();
    }
    return Node;
}

double Simulation(NODE* Node)
{
    auto State = Node->State;
    if (State->IsTerminal()) return State->Utility();
    return State->Evaluate();
}

void BackPropagation(NODE* Root, NODE* Node, double Utility)
{
    while (Node != Root->Parent)
    {
        ++Node->Visit;
        Node->Quality += Utility;
        Node = Node->Parent;
    }
}

pair<int, pair<int, int> > MonteCarloTreeSearch(NODE* Node)
{
    int Rollouts = 800;
    if (Node->State->Round <= 10) Rollouts = 1200;
    else if (Node->State->Round >= 50) Rollouts = 1500;
    for (int i = 1; i <= Rollouts; ++i)
    {
        if(i % 100 == 0) fprintf(stderr, "***** ");
        NODE* ExpandNode = SelectionAndExpansion(Node);
        double Utility = Simulation(ExpandNode);
        BackPropagation(Node, ExpandNode, Utility);
    }
    fprintf(stderr, "\n");
    return Node->BestAction();
}

// init function is called once at the beginning
void init()
{
    fprintf(stderr, "[MCTS Algorithm]\n\n");
    srand(time(nullptr));
    NowNode = new NODE;
    NowNode->State = new STATE;
    NowNode->State->SetLegalAction();
}

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
    if (loc.first != -1) NowNode = NowNode->FindNextNode(loc);
    pair<int, pair<int, int> > ret;
    if (NowNode->State->Round <= 6)
    {
        if (ai_side == 0) ret = make_pair(0, make_pair(NowNode->State->Position[0].first - 1, 4));
        if (ai_side == 1) ret = make_pair(0, make_pair(NowNode->State->Position[1].first + 1, 4));
    }
    else if ((NowNode->State->Round == 7 || NowNode->State->Round == 8) && NowNode->State->Position[0] == make_pair(5, 4) && NowNode->State->Position[1] == make_pair(3, 4))
    {
        if (ai_side == 0) ret = make_pair(2, make_pair(5, 4));
        if (ai_side == 1) ret = make_pair(2, make_pair(2, 3));
    }
    else ret = MonteCarloTreeSearch(NowNode);
    NowNode = NowNode->FindNextNode(ret);
    fprintf(stderr, "Player: %d, ret: <%d, %d, %d>\n", ai_side, ret.first, ret.second.first, ret.second.second);
    return ret;
}
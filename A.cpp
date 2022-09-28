#include <iostream>
#include <vector>
#include <array>
#include <chrono>
#include <set>
#include <algorithm>
#include <cstdio>
using namespace std;
using namespace std::chrono;

int xor64() {
    static uint64_t x = 88172645463345263ULL;
    x ^= x<<13;
    x ^= x>> 7;
    x ^= x<<17;
    return int(x&0x7fffffff);
}

class Paper
{
public:
    int N;
    int M;
    vector<char> P;
    // ｜－＼／
    // l[0]<l[1]
    vector<vector<array<int, 2>>> lines[4];
    set<array<int, 4>> moves;
    vector<char> X; // x
    vector<char> Y; // y
    vector<char> Z; // x+y
    vector<char> W; // x-y+N-1
    // movesに追加した長方形
    vector<vector<array<int, 4>>> hist_add;
    // movesから削除した長方形
    // 先頭は今回描いた長方形
    vector<vector<array<int, 4>>> hist_remove;

    Paper(int N, int M, vector<int> mark)
        : N(N)
        , M(M)
        , P(N*N)
    {
        for (int m: mark)
            P[m] = 1;
        lines[0].resize(N);
        lines[1].resize(N);
        lines[2].resize(2*N-1);
        lines[3].resize(2*N-1);

        for (int p=0; p<N*N; p++)
        {
            X.push_back(p%N);
            Y.push_back(p/N);
            Z.push_back(p%N+p/N);
            W.push_back(p%N-p/N+N-1);
        }

        for (int p1=0; p1<N*N; p1++)
            if (!P[p1])
            {
                int x1 = X[p1];
                int y1 = Y[p1];

                for (int p3=0; p3<N*N; p3++)
                    if (P[p3])
                    {
                        int x3 = X[p3];
                        int y3 = Y[p3];

                        if (x3!=x1 && y3!=y1)
                        {
                            int x2 = x1;
                            int y2 = y3;
                            int p2 = y2*N+x2;

                            int x4 = x3;
                            int y4 = y1;
                            int p4 = y4*N+x4;

                            if (P[p2] && P[p4])
                            {
                                if (canLine(p1, p2) &&
                                    canLine(p2, p3) &&
                                    canLine(p3, p4) &&
                                    canLine(p4, p1))
                                {
                                    moves.insert({p1, p2, p3, p4});
                                }
                            }
                        }
                    }
            }

        for (int p1=0; p1<N*N; p1++)
            if (!P[p1])
            {
                int x1 = X[p1];
                int y1 = Y[p1];

                for (int p3=0; p3<N*N; p3++)
                    if (P[p3])
                    {
                        int x3 = X[p3];
                        int y3 = Y[p3];

                        if ((x1+y1)%2==(x3+y3)%2 &&
                            x1-x3!=y1-y3 &&
                            x1-x3!=y3-y1)
                        {
                            int x2 = (x1+x3+y1-y3)/2;
                            int y2 = (x1-x3+y1+y3)/2;
                            int p2 = y2*N+x2;

                            int x4 = (x1+x3-y1+y3)/2;
                            int y4 = (-x1+x3+y1+y3)/2;
                            int p4 = y4*N+x4;

                            if (0<=x2 && x2<N && 0<=y2 && y2<N &&
                                0<=x4 && x4<N && 0<=y4 && y4<N &&
                                P[p2] && P[p4])
                            {
                                if (canLine(p1, p2) &&
                                    canLine(p2, p3) &&
                                    canLine(p3, p4) &&
                                    canLine(p4, p1))
                                {
                                    moves.insert({p1, p2, p3, p4});
                                }
                            }
                        }
                    }
            }
    }

    long long score() const
    {
        long long S = 0;
        long long s = 0;
        int c = (N-1)/2;
        for (int p=0; p<N*N; p++)
        {
            int x = X[p];
            int y = Y[p];
            long long t = (x-c)*(x-c)+(y-c)*(y-c)+1;
            S += t;
            if (P[p])
                s += t;
        }
        return 1'000'000LL*N*N*s/(M*S);
    }

    vector<array<int, 4>> getMoves() const
    {
        return vector<array<int, 4>>(moves.begin(), moves.end());
    }

    void move(array<int, 4> m)
    {
        P[m[0]] = 1;
        for (int i=0; i<4; i++)
        {
            int p1 = m[i];
            int p2 = m[(i+1)%4];
            if (p1>p2)
                swap(p1, p2);

            if (X[p1]==X[p2])
                lines[0][X[p1]].push_back({p1, p2});
            if (Y[p1]==Y[p2])
                lines[1][Y[p1]].push_back({p1, p2});
            if (Z[p1]==Z[p2])
                lines[2][Z[p1]].push_back({p1, p2});
            if (W[p1]==W[p2])
                lines[3][W[p1]].push_back({p1, p2});
        }

        hist_add.push_back(vector<array<int, 4>>());
        hist_remove.push_back(vector<array<int, 4>>());

        hist_remove.back().push_back(m);
        moves.erase(m);

        // 作れなくなった正方形を削除
        for (auto it=moves.begin(); it!=moves.end(); )
        {
            if ((*it)[0]==m[0] ||
                !(canLine((*it)[0], (*it)[1]) &&
                  canLine((*it)[1], (*it)[2]) &&
                  canLine((*it)[2], (*it)[3]) &&
                  canLine((*it)[3], (*it)[0])))
            {
                hist_remove.back().push_back(*it);
                it = moves.erase(it);
            }
            else
                it++;
        }

        // 作れるようになった長方形を追加
        int p1 = m[0];
        int x1 = X[p1];
        int y1 = Y[p1];
        for (int p3=0; p3<N*N; p3++)
        {
            int x3 = X[p3];
            int y3 = Y[p3];

            if (x3!=x1 && y3!=y1)
            {
                int x2 = x1;
                int y2 = y3;
                int p2 = y2*N+x2;

                int x4 = x3;
                int y4 = y1;
                int p4 = y4*N+x4;

                if (!P[p2] && P[p3] && P[p4] ||
                    P[p2] && !P[p3] && P[p4] ||
                    P[p2] && P[p3] && !P[p4])
                {
                    if (canLine(p1, p2) &&
                        canLine(p2, p3) &&
                        canLine(p3, p4) &&
                        canLine(p4, p1))
                    {
                        array<int, 4> m;
                        if (!P[p2] && P[p3] && P[p4])
                            m = {p2, p3, p4, p1};
                        if (P[p2] && !P[p3] && P[p4])
                            m = {p3, p4, p1, p2};
                        if (P[p2] && P[p3] && !P[p4])
                            m = {p4, p1, p2, p3};
                        hist_add.back().push_back(m);
                        moves.insert(m);
                    }
                }
            }
        }

        for (int p3=0; p3<N*N; p3++)
        {
            int x3 = X[p3];
            int y3 = Y[p3];

            if ((x1+y1)%2==(x3+y3)%2 &&
                x1-x3!=y1-y3 &&
                x1-x3!=y3-y1)
            {
                int x2 = (x1+x3+y1-y3)/2;
                int y2 = (x1-x3+y1+y3)/2;
                int p2 = y2*N+x2;

                int x4 = (x1+x3-y1+y3)/2;
                int y4 = (-x1+x3+y1+y3)/2;
                int p4 = y4*N+x4;

                if (0<=x2 && x2<N && 0<=y2 && y2<N &&
                    0<=x4 && x4<N && 0<=y4 && y4<N)
                    if (!P[p2] && P[p3] && P[p4] ||
                        P[p2] && !P[p3] && P[p4] ||
                        P[p2] && P[p3] && !P[p4])
                        if (canLine(p1, p2) &&
                            canLine(p2, p3) &&
                            canLine(p3, p4) &&
                            canLine(p4, p1))
                        {
                            array<int, 4> m;
                            if (!P[p2] && P[p3] && P[p4])
                                m = {p2, p3, p4, p1};
                            if (P[p2] && !P[p3] && P[p4])
                                m = {p3, p4, p1, p2};
                            if (P[p2] && P[p3] && !P[p4])
                                m = {p4, p1, p2, p3};
                            hist_add.back().push_back(m);
                            moves.insert(m);
                        }
            }
        }
    }

    void undo()
    {
        array<int, 4> m = hist_remove.back().front();

        for (array<int, 4> &t: hist_add.back())
            moves.erase(t);
        for (array<int, 4> &t: hist_remove.back())
            moves.insert(t);

        hist_add.pop_back();
        hist_remove.pop_back();

        for (int i=0; i<4; i++)
        {
            int p1 = m[i];
            int p2 = m[(i+1)%4];
            if (p1>p2)
                swap(p1, p2);

            if (X[p1]==X[p2])
                lines[0][X[p1]].pop_back();
            if (Y[p1]==Y[p2])
                lines[1][Y[p1]].pop_back();
            if (Z[p1]==Z[p2])
                lines[2][Z[p1]].pop_back();
            if (W[p1]==W[p2])
                lines[3][W[p1]].pop_back();
        }

        P[m[0]] = 0;
    }

    bool canLine(int p1, int p2) const
    {
        if (p1>p2)
            swap(p1, p2);

        int x1 = X[p1];
        int y1 = Y[p1];
        int x2 = X[p2];
        int y2 = Y[p2];

        if (X[p1]==X[p2])
        {
            for (int p=p1+N; p<p2; p+=N)
                if (P[p])
                    return false;
            for (array<int, 2> l: lines[0][X[p1]])
                if (p1<l[1] && l[0]<p2)
                    return false;
        }

        if (Y[p1]==Y[p2])
        {
            for (int p=p1+1; p<p2; p++)
                if (P[p])
                    return false;
            for (array<int, 2> l: lines[1][Y[p1]])
                if (p1<l[1] && l[0]<p2)
                    return false;
        }

        if (Z[p1]==Z[p2])
        {
            for (int p=p1+N-1; p<p2; p+=N-1)
                if (P[p])
                    return false;
            for (array<int, 2> l: lines[2][Z[p1]])
                if (p1<l[1] && l[0]<p2)
                    return false;
        }

        if (W[p1]==W[p2])
        {
            for (int p=p1+N+1; p<p2; p+=N+1)
                if (P[p])
                    return false;
            for (array<int, 2> l: lines[3][W[p1]])
                if (p1<l[1] && l[0]<p2)
                    return false;
        }
        return true;
    }
};

int main()
{
    int N, M;
    cin>>N>>M;
    vector<int> mark;
    for (int i=0; i<M; i++)
    {
        int x, y;
        cin>>x>>y;
        mark.push_back(y*N+x);
    }

    system_clock::time_point start = system_clock::now();

    Paper paper(N, M, mark);

    vector<array<int, 4>> bestA;
    long long bestScore = 0;

    const double TIME = 4.8;

    for (int pattern=0; ; pattern++)
    {
        if (chrono::duration_cast<chrono::nanoseconds>(system_clock::now()-start).count()*1e-9>TIME)
            break;

        auto isPattern = [&](array<int, 4> m) -> bool
        {
            sort(m.begin(), m.end());
            if (m[1]==m[0]+N-1 &&
                m[2]==m[0]+N+1 &&
                m[3]==m[0]+2*N)
            {
                int c = (m[0]+m[1]+m[2]+m[3])/4;
                int dx = paper.X[c]-(N-1)/2;
                int dy = paper.Y[c]-(N-1)/2;
                if (abs(dx)<=abs(dy))
                {
                    if (paper.X[c]%2==pattern%2)
                        return true;
                }
                if (abs(dx)>=abs(dy))
                {
                    if (paper.Y[c]%2==pattern/2%2)
                        return true;
                }
            }
            if (m[1]==m[0]+1 &&
                m[2]==m[0]+N &&
                m[3]==m[0]+N+1)
            {
                if (paper.Z[m[0]]%2==pattern/4%2)
                    return true;
            }
            return false;
        };

        auto getPatternLength = [&]() -> int
        {
            int n = 0;
            while (true)
            {
                array<int, 4> mm = {-1, 0, 0, 0};

                for (array<int, 4> m: paper.getMoves())
                {
                    if (mm[0]!=-1)
                        break;
                    if (isPattern(m))
                        mm = m;
                }
                if (mm[0]==-1)
                    break;
                paper.move(mm);
                n++;
            }
            for (int i=0; i<n; i++)
                paper.undo();
            return n;
        };

        vector<array<int, 4>> A;

        while (true)
        {
            if (chrono::duration_cast<chrono::nanoseconds>(system_clock::now()-start).count()*1e-9>TIME)
                break;

            vector<array<int, 4>> moves = paper.getMoves();
            if (moves.empty())
                break;

            int mi = -1;
            for (int i=0; i<(int)moves.size() && mi==-1; i++)
                if (isPattern(moves[i]))
                    mi = i;

            if (mi==-1)
            {
                int maxn = 0;
                vector<int> cand;

                for (int i=0; i<(int)moves.size(); i++)
                {
                    paper.move(moves[i]);
                    int n = getPatternLength();
                    paper.undo();

                    if (n>maxn)
                    {
                        maxn = n;
                        cand.clear();
                    }
                    if (n>0 && n==maxn)
                        cand.push_back(i);
                }

                if (!cand.empty())
                    mi = cand[xor64()%(int)cand.size()];
            }

            if (mi==-1)
            {
                long long ms = -1;
                vector<int> cand;

                for (int i=0; i<(int)moves.size(); i++)
                {
                    paper.move(moves[i]);
                    long long s = (long long)paper.getMoves().size()*1000000000;
                    // 2周目以降はscoreの評価を止めてランダム性を出す。
                    if (pattern>=8)
                        s += paper.score();
                    paper.undo();

                    if (s>ms)
                    {
                        ms = s;
                        cand.clear();
                    }
                    if (s==ms)
                        cand.push_back(i);
                }

                if (!cand.empty())
                    mi = cand[xor64()%(int)cand.size()];
            }

            array<int, 4> move = moves[mi];
            A.push_back(move);
            paper.move(move);
        }

        if (paper.score()>bestScore)
        {
            bestScore = paper.score();
            bestA = A;
        }

        while (!paper.hist_add.empty())
            paper.undo();
    }

    cout<<bestA.size()<<endl;
    for (array<int, 4> a: bestA)
    {
        for (int i=0; i<4; i++)
            cout<<(i==0?"":" ")<<a[i]%N<<" "<<a[i]/N;
        cout<<endl;
    }

    double time = chrono::duration_cast<chrono::nanoseconds>(system_clock::now()-start).count()*1e-9;
    fprintf(stderr, "%2d %3d %4d %7d %6.3f\n", N, M, (int)bestA.size(), (int)bestScore, time);

    return 0;
}

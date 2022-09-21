#include <iostream>
#include <vector>
#include <array>
#include <chrono>
#include <set>
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
public:
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

        moves.erase(m);

        // 作れなくなった正方形を削除
        for (auto it=moves.begin(); it!=moves.end(); )
        {
            if ((*it)[0]==m[0] ||
                !(canLine((*it)[0], (*it)[1]) &&
                  canLine((*it)[1], (*it)[2]) &&
                  canLine((*it)[2], (*it)[3]) &&
                  canLine((*it)[3], (*it)[0])))
                it = moves.erase(it);
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
                        if (!P[p2] && P[p3] && P[p4])
                            moves.insert({p2, p3, p4, p1});
                        if (P[p2] && !P[p3] && P[p4])
                            moves.insert({p3, p4, p1, p2});
                        if (P[p2] && P[p3] && !P[p4])
                            moves.insert({p4, p1, p2, p3});
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
                            if (!P[p2] && P[p3] && P[p4])
                                moves.insert({p2, p3, p4, p1});
                            if (P[p2] && !P[p3] && P[p4])
                                moves.insert({p3, p4, p1, p2});
                            if (P[p2] && P[p3] && !P[p4])
                                moves.insert({p4, p1, p2, p3});
                        }
            }
        }
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

    vector<array<int, 4>> A;
    while (true)
    {
        if (chrono::duration_cast<chrono::nanoseconds>(system_clock::now()-start).count()*1e-9>4.0)
            break;

        vector<array<int, 4>> moves = paper.getMoves();
        if (moves.empty())
            break;

        array<int, 4> move = moves[xor64()%int(moves.size())];
        A.push_back(move);
        paper.move(move);
    }

    cout<<A.size()<<endl;
    for (array<int, 4> a: A)
    {
        for (int i=0; i<4; i++)
            cout<<(i==0?"":" ")<<a[i]%N<<" "<<a[i]/N;
        cout<<endl;
    }

    cerr<<"N: "<<N<<endl;
    cerr<<"M: "<<M<<endl;
    cerr<<"num: "<<A.size()<<endl;
    cerr<<"Score: "<<paper.score()<<endl;
    cerr<<"Time: "<<chrono::duration_cast<chrono::nanoseconds>(system_clock::now()-start).count()*1e-9<<endl;

    return 0;
}

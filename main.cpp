// Даны две строки, состоящие из символов 0, 1 и ?.
// Требуется заменить символы ? на 0 и 1 так, чтобы расстояние Хемминга между ними было мимнимальным
// Ввод: две строки
// Вывод: расстояние Хемминга, и две строки (с 0 или 1 вместо ?).

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <limits>

struct EdgeWithSream
{
    int Weight;
    int Stream;
};

struct Vertex
{
    bool IsVisited;
    std::vector<int> Neighbours;
};

struct VertexOfNetwork: public Vertex
{
    int Level;
    int curInd;
};

class myGraph
{
private:
    int NumberOfVertex;
    Vertex* Graph;
    VertexOfNetwork* Network; //Слоистая сеть, построенная на основе графа
    EdgeWithSream** Edges; // Матрица текущий потоков
    void CleanGraph();
    void AddToNetworkFrom_Xto_Y(int _x, int _y);
    void AddEdgeToNetwork(int _x, int _y);
    void AddEdgeFrom_Xto_Y(int _x, int _y, int _weight_);
    void CreateNetwork();
    int dfs(int v, int flow);
public:
    myGraph(int _size);
    void AddEdge(int _x, int _y, int _weight);

    int FindStreams();
    std::vector<int> bfs();
    //void Print();
    //void PrintNetwork();
    ~myGraph();
};

std::vector<int> myGraph::bfs()
{
    std::vector<int> res (NumberOfVertex - 2, 0);
    CleanGraph();
    std::queue<int> q;
    q.push(0);
    Graph[0].IsVisited = true;
    int _size = q.size();
    while(_size)
    {
        Vertex cur;
        for(int i = 0; i < _size; ++i)
        {
            int indFrom = q.front();
            cur = Graph[indFrom];
            q.pop();
            for(unsigned j = 0; j < cur.Neighbours.size(); ++j)
            {
                int indTo = cur.Neighbours[j];

                if(Edges[indFrom][indTo].Weight - Edges[indFrom][indTo].Stream)
                {
                    if(!Graph[indTo].IsVisited)
                    {
                        Graph[indTo].IsVisited = true;
                        q.push(indTo);
                    }
                }

            }
        }
        _size = q.size();
    }
    for(int i = 1; i < NumberOfVertex - 1; ++i)
    {
        if(!Graph[i].IsVisited)
            res[i - 1] = 1;
    }
    return res;
}

myGraph::myGraph(int _size)
{
    NumberOfVertex = _size;
    Graph = new Vertex [NumberOfVertex];
    Edges = new EdgeWithSream* [NumberOfVertex];
    for(int i = 0; i < NumberOfVertex; ++i)
    {
        Edges[i] = new EdgeWithSream [NumberOfVertex];
        for(int j = 0; j < NumberOfVertex; ++j)
        {
            Edges[i][j].Stream = 0;
            Edges[i][j].Weight = 0;
        }
    }
    CleanGraph();
}

void myGraph::CleanGraph()
{
    for ( int i = 0; i < NumberOfVertex; ++i )
    {
        Graph[i].IsVisited = false;
    }
}

void myGraph::AddEdgeFrom_Xto_Y(int _x, int _y, int _weight_)
{
    Graph[_x].Neighbours.push_back(_y);
    Edges[_x][_y].Weight += _weight_;
}

void myGraph::AddEdge(int _x, int _y, int _weight)
{
    AddEdgeFrom_Xto_Y(_x, _y, _weight);
    AddEdgeFrom_Xto_Y(_y, _x, _weight);
}

void myGraph::AddToNetworkFrom_Xto_Y(int _x, int _y)
{
    Network[_x].Neighbours.push_back(_y);
}

void myGraph::AddEdgeToNetwork(int _x, int _y)
{
    AddToNetworkFrom_Xto_Y(_x, _y);
}

myGraph::~myGraph()
{
    delete[] Graph;
    for(int i = 0; i < NumberOfVertex; ++i)
        delete[] Edges[i];
    delete[] Edges;
}

void myGraph::CreateNetwork()
{
    Network = new VertexOfNetwork [NumberOfVertex];
    CleanGraph();
    std::queue<int> q;
    int _level = 0;
    q.push(0);
    Graph[0].IsVisited = true;
    Network[0].Level = _level;
    ++_level;
    int _size = q.size();
    while(_size)
    {
        Vertex cur;
        for(int i = 0; i < _size; ++i)
        {
            int indFrom = q.front();
            cur = Graph[indFrom];
            q.pop();
            for(unsigned j = 0; j < cur.Neighbours.size(); ++j)
            {
                int indTo = cur.Neighbours[j];

                if(Edges[indFrom][indTo].Weight - Edges[indFrom][indTo].Stream)
                {
                    if(!Graph[indTo].IsVisited)
                    {
                        Graph[indTo].IsVisited = true;
                        Network[indTo].Level = _level;
                        q.push(indTo);
                    }
                    if(Network[indTo].Level - Network[indFrom].Level == 1)
                    {
                        AddEdgeToNetwork(indFrom, indTo);
                    }
                }
            }
        }
        ++_level;
        _size = q.size();
    }
}

int myGraph::dfs(int v, int flow)
{
    if(!flow)
    {
        return 0;
    }
    if(v == NumberOfVertex - 1)
        {
            return flow;
        }
    int to = -1;
    for(int i = Network[v].curInd; i < Network[v].Neighbours.size(); ++i)
    {
        to = Network[v].Neighbours[i];
        if (Network[to].Level != Network[v].Level + 1)
        {
            Network[v].curInd++;
            continue;
        }
        int pushed = dfs(to, std::min(flow, Edges[v][to].Weight - Edges[v][to].Stream));
        if(pushed != 0)
        {
            Edges[v][to].Stream += pushed;
            Edges[to][v].Stream -= pushed;
        return pushed;
        }
        Network[v].curInd++;
    }
    return 0;
}

int myGraph::FindStreams()
{
    CreateNetwork();
    for(int i = 0; i < NumberOfVertex; ++i)
        Network[i].curInd = 0;
    int result = 0;
    int cur = dfs(0, std::numeric_limits<int>::max());
    while(cur)
    {
        result += cur;
        cur = dfs(0, std::numeric_limits<int>::max());
    }
    delete[] Network;
    return result;
}

std::vector<int> PrepareSTr (std::string _str, int &_numberOfUnknown)
{
    std::vector<int> res(_str.length(), 0);
    for(int i = 0; i < _str.length(); ++i)
    {
        int d = _str[i] - '0';
        switch(d)
        {
            case 0:
                res[i] = 0;
                break;
            case 1:
                res[i] = -1;
                break;
            default:
                res[i] = ++_numberOfUnknown;
        }
    }
    return res;
}

void Convert(std::vector<int> &str, int n, int start, std::vector<int> &Unknown)
{
    for(int i = 0; i < str.size(); ++i)
    {
        if(str[i] > 0)
            Unknown.push_back(i + start);
        if(str[i] < 0)
            str[i] = n + 1;
    }
}

int Hemming(std::vector<int> &FStr, std::vector<int> &SStr, int NumberOfUnknown, int d, std::vector<int> &Unknown)
{
    myGraph graph(NumberOfUnknown + 2);
    for(int i = 0; i < SStr.size(); ++i)
    {
        for(int j = 0; j <= d; ++ j)
        {
            if(SStr[i] != FStr[i + j])
            {
                graph.AddEdge(SStr[i], FStr[i + j], 1);
            }
        }
    }

    //graph.CreateNetwork();
    int c = graph.FindStreams();
    int r = 0;
    while(c)
    {
        r += c;
        //graph.CreateNetwork();
        c = graph.FindStreams();
    }

    std::vector<int> vv = graph.bfs();

    for(int i = 0; i < vv.size(); ++i)
    {
        int ind = Unknown[i];
        if(ind < FStr.size())
        {
            FStr[ind] = vv[i];
        }
        else
        {
            SStr[ind - FStr.size()] = vv[i];
        }
    }

    for(int i = 0; i < FStr.size(); ++i)
    {
        FStr[i] = std::min(FStr[i], 1);
    }
    for(int i = 0; i < SStr.size(); ++i)
    {
        SStr[i] = std::min(SStr[i], 1);
    }
    return r;
}

int main()
{
    std::string x, y;
    std::cin >> x >> y;
    std::vector<int> Unknown;
    int NumberOfUnknown = 0;
    std::vector<int> FStr = PrepareSTr(x, NumberOfUnknown);
    std::vector<int> SStr = PrepareSTr(y, NumberOfUnknown);
    Convert(FStr, NumberOfUnknown, 0, Unknown);
    Convert(SStr, NumberOfUnknown, FStr.size(), Unknown);

    int d = x.length() - y.length();
    int r = Hemming(FStr, SStr, NumberOfUnknown, d, Unknown);

    std::cout << r << "\n";
    for(auto item : FStr)
    {
        std:: cout << item;
    }
    std:: cout << "\n";
    for(auto item : SStr)
    {
        std:: cout << item;
    }
    return 0;
}

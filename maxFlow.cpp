/* Find the max flow in the graph.

Найдите максимальный поток в сети.
Первая строка содержит два числа 2≤v≤50 и 0≤e≤1000 — число вершин
и число рёбер сети. Следующие e строк описывают рёбра:
каждая из них содержит три целых числа через пробел:
0≤ui<v, 0≤vi<v, 0<ci<50 — исходящую и входящую вершины для этого ребра,
а так же его пропускную способность.

Выведите единственное число — величину максимального потока из вершины 0 в вершину v−1.

Sample Input:
4 5
0 1 3
1 2 1
0 2 1
1 3 1
2 3 3

Sample Output:
3
*/

#include <iostream>
#include <exception>
#include <vector>
#include <algorithm>
#include <cstring>
#include <climits>


struct ogException : std::exception {
  std::string msg;
  ogException(const char* msg): msg(msg) {};
  const char* what() const noexcept {return msg.c_str();}
};


struct edge {
	int from;
  int to;
  int cap; 
  int flow;
};

std::ostream& operator<<(std::ostream& os, struct edge& e) {
  os << "From " << (e.from)+1 << " to " << (e.to)+1 << ". Capacity " << e.cap << ". Flow " << e.flow;
  return os;
}

typedef std::vector<struct edge*> pEdges; // array of pointers to edges
typedef std::vector<int> EdgesOfVertexes; // array of indexies of edges which incidents to vertex


class Orgraph {
private:
  int eCount; // edges number
  int vCount; // vertices number
  
  pEdges edges; // pointer of edges of orgraph. Stores edges. It size is eCount.
  EdgesOfVertexes* vToEs; // vector of lists of pointers to edges. It size is vCount.
  
  bool bfs(int s, int t, int* d, int* ptr, int* q) {
    int qh=0, qt=0;
    q[qt++] = s;
    memset (d, -1, vCount * sizeof d[0]);
    d[s] = 0;
    while (qh < qt && d[t] == -1) {
      int v = q[qh++];
      for (size_t i=0; i<vToEs[v].size(); ++i) {
        int id = vToEs[v][i],
          to = edges[id]->to;
        if (d[to] == -1 && edges[id]->flow < edges[id]->cap) {
          q[qt++] = to;
          d[to] = d[v] + 1;
        }
      }
    }
    return d[t] != -1;
  }
   
  int dfs (int t, int* d, int* ptr, int* q, int v, int flow) {
    if (!flow)  return 0;
    if (v == t)  return flow;
    for (; ptr[v]<(int)vToEs[v].size(); ++ptr[v]) {
      int id = vToEs[v][ptr[v]],
        to = edges[id]->to;
      if (d[to] != d[v] + 1)  continue;
      int pushed = dfs (t, d, ptr, q, to, std::min(flow, edges[id]->cap - edges[id]->flow));
      if (pushed) {
        edges[id]->flow += pushed;
        edges[id^1]->flow -= pushed;
        return pushed;
      }
    }
    return 0;
  }


public:
  Orgraph(int eCount=0, int vCount=0): eCount(eCount), vCount(vCount), vToEs(new EdgesOfVertexes[vCount]) {};
  
  virtual ~Orgraph(){
    clear();
  }
  
  int edgesCount() { return edges.size(); }
  int vertexesCount() { return vCount; }
  
  void clear() {
    for(auto it = edges.cbegin(); it!=edges.cend(); ++it){
      delete *it;
    }
    
    edges.clear();
    
    delete[] vToEs;
    
    vCount = 0;
    eCount = 0;
  }
  
  // resize number of vertexes  and edges.
  // clears all data in orgraph!
  void setSize(int vCount, int eCount) {
    clear();
    this->vCount = vCount;
    this->eCount = eCount;
    vToEs = new EdgesOfVertexes[vCount];
  };
  
  void add_edge (int from, int to, int cap) {
    if(from >= vCount || to >= vCount){
      throw new ogException("Two large number of vertex!");
    }
    
    struct edge* pe1 = new edge { from, to, cap, 0 };
    struct edge* pe2 = new edge { to, from, 0, 0 }; // virtual edge

    vToEs[from].push_back((int) edges.size());
    edges.push_back(pe1);
    vToEs[to].push_back((int) edges.size());
    edges.push_back(pe2);
  };
  
  const pEdges getEdges() const {
    return edges;
  }
  
  // Yefim Dinic algorithm to find max flow in graph
  int dinic(int s, int t) {
    int flow = 0;
    int* d = new int[vCount];
    int* ptr = new int[vCount];
    int* q = new int[vCount];
    
    while(1) {
      if(!bfs(s, t, d, ptr, q)) break;
      memset(ptr, 0, vCount * sizeof ptr[0]);
      while(int pushed = dfs(t, d, ptr, q, s, INT_MAX))
        flow += pushed;
    }
    
    delete[] d;
    delete[] ptr;
    delete[] q;
    
    return flow;
  }


};


std::istream& operator>>(std::istream& is, Orgraph& og){
  int v, e;
  is >> v >> e;

  og.setSize(v, e); // set size and clear all data.
  
  // read data of edges
  int from, to, cap;
  for(int i=0; i < e; ++i){
    is >> from >> to >> cap;
    og.add_edge(from, to, cap);
  }
  
  return is;
}
  
  
std::ostream& operator<<(std::ostream& os, Orgraph& og){
  int v = og.vertexesCount();
  if(v== 0){
    os << "Empty graph" << std::endl;
    return os;
  }
  
  os << "Vertexes " << v << ". Edges " << og.edgesCount() << "." << std::endl;
  os << "Edges list:" << std::endl;
  
  const pEdges elist = og.getEdges();

  for(auto it = elist.cbegin(); it != elist.cend(); ++it) {
    os << *(*it) << std::endl;
  }
  
  return os;
}


int main(int argc, char** argv){
	Orgraph og;
	
  std::cin >> og;
  //std::cout << og;
  
  std::cout << og.dinic(0, og.vertexesCount()-1) << std::endl;
  
	return 0;
}

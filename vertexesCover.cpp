/* Find maximum vertex cover of a bipartite graph.

Найдите минимальное вершинное покрытие в двудольном графе.

На вход подается описание двудольного графа, в котором доли уже выделены явно.
Первая строка содержит три натуральных числа: v1<100
— число вершин первой доли, v2<100 — число вершин второй доли,
e≤v1∗v2 — число рёбер. Подразумевается, что первая доля состоит из вершин
с номерами от 0 до v1−1, вторая — из вершин с номерами от v1 до v1+v2−1.
Следующие e строк описывают рёбра: каждая из этих строк содержит два
числа: 0≤ui<v1 и v1≤wi<v1+v2, что означает, что между вершинами ui и wi есть ребро.

Скопируйте описание графа из входа на выход и выведите единственную
дополнительную строку — список номеров вершин, составляющих минимальное
вершинное покрытие. Если таких покрытий несколько, выведите любое.

Sample Input:
2 2 3
0 2
0 3
1 3

Sample Output:
2 2 3
0 2
0 3
1 3
0 1
*/

#include <iostream>
#include <vector>
#include <set>
#include <list>
#include <algorithm>
#include <exception>
#include <sstream>
#include <string>


typedef std::vector<int> VertexesVector;
typedef std::set<int> VertexesSet;
typedef std::vector<VertexesSet> EdgesVector;
// edge (vertex from first set, vertex from second set) 
typedef std::pair<int, int> Edge;
// list of pairs (vertex from first set, vertex from second set)
typedef std::list<Edge> EdgesList;

// bipartite graph
class Graph {
	typedef std::vector<char> UsedVertexes;
  
private:
	int v1; // number of vertexes in first set
	int v2; // number of vertexes in second set
	int eCount; // number of edges between first and second set
  
  EdgesList elist;
  
	EdgesVector e; // lists of adjacency for every vertex in first set
  
	void setEdgesSize(){
		e.clear();
		e.resize(v1);
	}
	
  // Depth-first traversal for find max alternative path
	bool try_kuhn(UsedVertexes& used, VertexesVector& match, int v){
		if(used[v])
			return false;
		
		used[v] = true;
		
		for(auto to_ptr=e[v].cbegin(), end=e[v].cend(); to_ptr!=end; ++to_ptr){
			int to = *to_ptr;
			if(match[to] == -1 || try_kuhn(used, match, match[to])) {
				match[to] = v;
				return true;
			}
		}
    
		return false;
	}

  // Depth-first traversal for vertexes switching in match
  void controlVisit(VertexesVector& match, UsedVertexes& isSecondSet, UsedVertexes& visited, int v) {
    visited[v] = true;
    for(auto to_ptr=e[v].cbegin(), end=e[v].cend(); to_ptr!=end; ++to_ptr){
      int to = *to_ptr;
      int from = match[to];
      if(!isSecondSet[to] && !visited[from]) {
        controlVisit(match, isSecondSet, visited,from);
        isSecondSet[to] = true;
      }
    }
  }

public:
	Graph(int v1=0, int v2=0, int eCount=0): v1(v1), v2(v2), eCount(eCount), e(v1) {};
	
  void addEdge(int first, int second){
    eCount++;
    e[first].insert(second-v1);
    elist.push_back(Edge(first, second-v1));
  };
  
  bool isMatch(int v1, int v2, int eCount){
    return (this->v1 == v1) && (this->v2 == v2) && (this->eCount == eCount);
  }
  
	VertexesVector getMaxMatch() {
		VertexesVector match(v2, -1); // current match. For number of second set vertex shows if
			// there is vertex in first set in current match. Otherwise it contains -1.
		UsedVertexes used(v1); // used vertexes of fistr set when we look through

		// find first match to reduce time of algorithm
		UsedVertexes used1(v1, false);
		for(int i=0; i<v1; ++i){
			for(auto to_ptr=e[i].cbegin(), end=e[i].cend(); to_ptr!=end; ++to_ptr){
				int to = *to_ptr;
				if(match[to] == -1) {
					match[to] = i;
					used1[i] = true;
					break;
				}
			}
    }
    
		// find maximum match
		for(int i=0; i<v1; ++i) {
			if(used1[i])
        continue;
      used.assign(v1, false);
      try_kuhn(used, match, i);
		}
    
		return match;
	};
	
  VertexesSet getMinimalVertexCover() {
		// find max match
    VertexesVector match = getMaxMatch();
    
    //// debug: show match found
    //std::cout << "Matches: ";
    //showMatch(std::cout, match);
    
    // Find free vertexes in first set
    UsedVertexes freeVertexes(v1, true);
    for(int i=0; i<v2; ++i){
      int from = match[i];
      if(from != -1)
        freeVertexes[from] = false;
    }
   
    // From all free vertexes in first set start to look for
    // matched vertexes in second set
    UsedVertexes isSecondSet(v2, false);
    UsedVertexes visited(v1, false);
    for(int i=0; i<v1; ++i){
      if(freeVertexes[i])
        controlVisit(match, isSecondSet, visited, i);
    }
    
   // Convert result to a set of vertexes
		VertexesSet res;
		for(int i=0; i<v2; ++i){
      if(match[i] != -1) {
        if(isSecondSet[i])
          res.insert(i+v1);
        else
          res.insert(match[i]);
      }
    }
    
    /*// Add independent vertexes not connected to any vertexes
    // In first set
    for(int i=0; i<v1; ++i){
      if(e[i].size() == 0)
        res.insert(i);
    }
    // In second set
    VertexesSet boundVertexes2; // Subset of second set vertexes, bounded to first set vertexes
    // All vertexes we need are not in this set. Fill it.
    for(int i=0; i<v1; ++i){
      for(auto to_ptr=e[i].cbegin(), end=e[i].cend(); to_ptr!=end; ++to_ptr)
        boundVertexes2.insert(*to_ptr);
    }
    // Add not bounded vertexes to result
    for(int i=0; i<v2; ++i){
      if(boundVertexes2.count(i) == 0)
        res.insert(i+v1);
    }
    */
    
		return res;
	};
  
  void showMatch(std::ostream& out, const VertexesVector& match){
    for(int to=0; to<v2; ++to)
      if(match[to] != -1)
        out << "(" << match[to] << "," << (to+v1) << ") ";
    out << std::endl;
  };


  
	friend std::istream& operator>>(std::istream&, Graph&);
	friend std::ostream& operator<<(std::ostream&, Graph&);
};

std::istream& operator>>(std::istream& in, Graph& g){
  int eCount;
	in >> g.v1 >> g.v2 >> eCount;
  g.setEdgesSize();
  for(int i=0; i<eCount; ++i){
    int from, to;
    in >> from >> to;
    g.addEdge(from, to);
  }
  
  return in;
};
	
std::ostream& operator<<(std::ostream& out, Graph& g){
	out << g.v1 << " " << g.v2 << " " << g.eCount << std::endl;
	for(auto it=g.elist.cbegin(), end=g.elist.cend(); it!=end; ++it){
			out << it->first << " " << (it->second)+g.v1 << std::endl;
	}
	return out;
};

std::ostream& operator<<(std::ostream& out, const VertexesSet& vs){
  int s = vs.size();
	for(auto it=vs.cbegin(), end=vs.cend(); it!=end; ++it){
    s--;
		out << *it;
    if(s > 0)
      out << " ";
	}
	out << std::endl;
	
	return out;
};


struct spy : std::exception {
  std::string s;
  spy(std::string s): s(s) {};
  
  const char* what() const noexcept {return s.c_str();}
};


int main(){
	Graph g;
	std::cin >> g;
  
  //if(g.isMatch(2, 2, 3)){
    std::cout << g;
    std::cout << g.getMinimalVertexCover();
  /*}
  else {
    std::stringstream ss;
    ss << g;
    ss << g.getMinimalVertexCover();
    throw spy(ss.str());
  }*/
  
	return 0;
}

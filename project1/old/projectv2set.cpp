//Project

#include <vector>
#include <list>
#include <iostream>
#include <stack>
#include <set>
#include <stdio.h>
#include <stdlib.h>

class Vertex {

		int  _key;
		int  _discovery;
		int  _lowlink;
		int  _minKey;
		bool _onStack;

		std::list<Vertex*> _adjacency;

	public:

		Vertex(int key) :
			_key(key),
			_discovery(-1),
			_lowlink(-1),
			_minKey(-1),
			_onStack(false) {}

		int key()       const { return _key;       }
		int discovery() const { return _discovery; }
		int lowlink()   const { return _lowlink;   }
		int minKey()    const { return _minKey;    }
		
		std::list<Vertex*> adjacency() const { return _adjacency; }

		void key(int key)     { _key       = key; }
		void discovery(int d) { _discovery = d;   }
		void lowlink(int ll)  { _lowlink   = ll;  }
		void onStack(bool b)  { _onStack   = b;   }
		void minKey(int key)  { _minKey    = key; }
		
		bool onStack()      const { return _onStack;         }
		bool undiscovered() const { return _discovery == -1; }
		bool isValid()      const { return _key >= 1;        }
		bool inSCC()        const { return _minKey != -1;    }

		void lowerlink(const Vertex &v) {
			if (_lowlink > v.lowlink())
				_lowlink = v.lowlink();
		}

		void addEdge(Vertex *end) {
			_adjacency.push_front(end);
		}

		friend bool operator!=(const Vertex &v1, const Vertex &v2) {
			return v1.key() != v2.key();
		}

		friend bool operator==(const Vertex &v1, const Vertex &v2) {
			return v1.key() == v2.key();
		}

};

class Edge {
		
		int _start;
		int _end;

	public:
		
		Edge(int v1, int v2) :
			_start(v1),
			_end(v2) {}

		int start() const { return _start; }
		int end() const { return _end; }

		int vertix(int i) {
			if(i == 1)
				return _start;
			else if(i == 2)
				return _end;
			else
				exit(1);
		}

		friend bool operator==(const Edge &e1, const Edge &e2) {
			return e1.start() == e2.start() && e1.end() == e2.end();
		}
};

struct comparator {
	bool operator()(const Edge* e1, const Edge* e2) const {
		if(e1->start() == e2->start())
			return e1->end() < e2->end();
		else return e1->start() < e2->start();
	}
};
	
class Graph {

		int _vertexNo;
		int _edgeNo;
		int _sccNo;

		std::vector<Vertex*> _vertices;
		std::vector<Edge*>   _output;
		
	public:

		Graph(int nv, int ne) :
			_vertexNo(nv),
			_edgeNo(ne),
			_sccNo(0),
			_vertices(nv),
			_output(ne) {}

		int vertexNo() const { return _vertexNo; }
		int edgeNo()   const { return _edgeNo;   }
		int sccNo()    const { return _sccNo;    }

		std::vector<Vertex*> vertices() const { return _vertices;          }
		Vertex *vertex(int key)         const { return _vertices[key - 1]; }

		void incrementSCC()       { _sccNo++;                     }
		void addVertex(Vertex* v) { _vertices[v->key() - 1] = v;  }

		void addEdge(Vertex *start, Vertex *end) {
			if(start->isValid() && end->isValid())
				start->addEdge(end);
			else
				exit(1);
		}

		void getOutput() {
			int i = 0;
			for (Vertex *v : _vertices)
				for (Vertex *w : v->adjacency()) {
					if (v->minKey() != w->minKey()) {
						_output[i] = new Edge(v->minKey(), w->minKey());
						i++;
					}
				}
			
			std::set<Edge*, comparator> s;
			unsigned size = _output.size();
			for(unsigned i = 0; i < size; i++) {
				if(_output[i] == NULL) break;
				s.insert(_output[i]);
			}
			_output.assign(s.begin(), s.end());
						
			printEdges(s.size());

			for(Edge* e : _output)
				delete(e);

			/*
			std::cout << "Before sort\n";
			printEdges();			

		  sort(2);
			std::cout << "After 1st sort\n";
			printEdges();

			sort(1);

			std::cout << "After 2nd sort\n";
			printEdges();
			*/
		}

		/*void sort(int n) {

			Edge* aux[_edgeNo];

			int i, count[_vertexNo+1] = {0};			

			for (i = 0; i < _edgeNo; i++) {
				if (_output[i] == NULL) break;
				++count[_output[i]->vertix(n)];
			}
				
								
			for (i = 1; i < _vertexNo+1; i++)
				count[i] += count[i-1];

			for (i = ((int) _output.size()) - 1; i >= 0; i--) {
				if(_output[i] == NULL) continue;
				aux[count[_output[i]->vertix(n)] - 1] = _output[i];
				--count[_output[i]->vertix(n)];
			}

			for (i = 0; i < _edgeNo; i++) {
				if(_output[i] == NULL) break;
				_output[i] = aux[i];
			}
		}
		*/

		void printEdges(unsigned int size) {
			
			fprintf(stdout, "%d\n", (int) size);	

			for(unsigned int i = 0; i < size; i++) {
				if(_output[i] == NULL) break;
				std::cout << _output[i]->start() << " " << _output[i]->end() << "\n";
			}
		}
 
		void print() {
			for (int i = 0; i < _vertexNo; i++) {
				std::cout << _vertices[i]->key() << " | ";
				for(Vertex* vertex : _vertices[i]->adjacency())
					std::cout << vertex->key() << " ";
				std::cout << "\n";
			}
		}
};

class SCCFinder {

		Graph               *_graph;
		std::stack<Vertex*>  _stack;
		int                  _index;
		
	public:

		SCCFinder(Graph *g) :
			_graph(g),
			_index(0) {}

		void tarjan() {
			for (Vertex* v : _graph->vertices())
				if (v->undiscovered())
					strongconnect(v);
		}

		void strongconnect(Vertex *v) {
			v->discovery(_index);
			v->lowlink(_index);
			_index += 1;
			_stack.push(v);
			v->onStack(true);

			for (Vertex *neighbour : v->adjacency()) {
				if (neighbour->undiscovered()) {
					strongconnect(neighbour);
					v->lowerlink(*neighbour);
				}
				else if (neighbour->onStack())
					v->lowerlink(*neighbour);
			}
	
			if(v->lowlink() == v->discovery()) {
				Vertex *w;
				std::list<Vertex*> members;
				int minKey = v->key();
				_graph->incrementSCC();
				do {
					w = _stack.top();
					members.push_front(w);
					w->onStack(false);
					_stack.pop();
					if (w->key() < minKey)
						minKey = w->key();
				} while (w != v);
				for (Vertex *m : members)
					m->minKey(minKey);
			}
		}
};

int main() {

	int vertexNo;
	int edgeNo;

	int edgeStart;
	int edgeEnd;

	fscanf(stdin, "%d", &vertexNo);
	
	fscanf(stdin, "%d", &edgeNo);

	Graph g(vertexNo, edgeNo);

	for (int i = 0; i < vertexNo; i++)
		g.addVertex(new Vertex(i + 1));

	for(int i = 0; i < edgeNo; i++) {
		fscanf(stdin, "%d %d", &edgeStart, &edgeEnd);
		g.addEdge(g.vertex(edgeStart), g.vertex(edgeEnd));
	}

	SCCFinder finder(&g);

  finder.tarjan();

	fprintf(stdout, "%d\n", g.sccNo());

	g.getOutput();

	for (int i = 0; i < g.vertexNo(); i++)
		delete g.vertex(i + 1);

	return 0;
}	

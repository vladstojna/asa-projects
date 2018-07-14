// Project

#include <vector>
#include <list>
#include <iostream>
#include <stack>

class Vertex {

		int  _key;
		int  _discovery;
		int  _lowlink;
		int  _minKey;
		bool _onStack;

		int  _outDeg;
		int  _inDeg;

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



class Graph {

		int _vertexNo;
		int _edgeNo;
		int _sccNo;

		std::vector<Vertex*> _vertices;
		std::vector<Vertex*> _connects;
		
	public:

		Graph(int nv, int ne) :
			_vertexNo(nv),
			_edgeNo(ne),
			_sccNo(0),
			_vertices(nv),
			_connects(nv) {}

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

		void addConnect(

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
					std::cout << w->key() << " ";
				} while (w != v);
				std::cout << "| ";
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

	std::cout << "Vertex No input\n";
	fscanf(stdin, "%d", &vertexNo);
	
	std::cout << "Edge No input\n";
	fscanf(stdin, "%d", &edgeNo);

	Graph g(vertexNo, edgeNo);

	std::cout << "Vertex initialization\n";
	for (int i = 0; i < vertexNo; i++)
		g.addVertex(new Vertex(i + 1));

	std::cout << "Adding Edges\n";
	for(int i = 0; i < edgeNo; i++) {
		fscanf(stdin, "%d %d", &edgeStart, &edgeEnd);
		g.addEdge(g.vertex(edgeStart), g.vertex(edgeEnd));
	}

	SCCFinder finder(&g);

	std::cout << "Before printing\n";
	g.print();
	std::cout << "Print successful\n";

	std::cout << "Before Tarjan\n";
	finder.tarjan();
	std::cout << "\nTarjan successful\n";

	fprintf(stdout, "sccNo: %d\n", g.sccNo());

	std::cout << "Min Keys\n";
	for (int i = 0; i < g.vertexNo(); i++)
		std::cout << g.vertex(i + 1)->minKey() << "\n";

	for (int i = 0; i < g.vertexNo(); i++)
		delete g.vertex(i + 1);

	return 0;
}	

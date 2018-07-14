// Project

#include <vector>
#include <list>
#include <stack>
#include <iostream>

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
		
		std::list<Vertex*> &adjacency() { return _adjacency; }

		void discovery(int d) { _discovery = d;   }
		void lowlink(int ll)  { _lowlink   = ll;  }
		void onStack(bool b)  { _onStack   = b;   }
		void minKey(int key)  { _minKey    = key; }
		
		bool onStack()      const { return _onStack;         }
		bool undiscovered() const { return _discovery == -1; }
		bool isValid()      const { return _key >= 1;        }

		int compareMK(Vertex *v) const { return _minKey - v->minKey(); }

		void lowerlink(const Vertex &v) {
			if (_lowlink > v.lowlink())
				_lowlink = v.lowlink();
		}

		void addEdge(Vertex *end) { _adjacency.push_front(end); }

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
		int _connectNo;
		int _maxMinKey;

		std::vector<Vertex*>  _vertices;
		std::vector<Vertex*> *_connections;
		
		bool sortedUniqueInsert(Vertex *w, std::list<Vertex*> &list) {
			std::list<Vertex*>::iterator current = list.begin();
			std::list<Vertex*>::iterator end     = list.end();
			while ( (current != end) && (w->compareMK(*current) > 0) )
				current++;
			if ( (current == end) || (w->compareMK(*current) < 0) ) {
				list.insert(current, w);
				return true;
			}
			return false;
		}
		
	public:

		Graph(int nv, int ne) :
			_vertexNo(nv),
			_edgeNo(ne),
			_sccNo(0),
			_connectNo(0),
			_maxMinKey(-1),
			_vertices(nv),
			_connections(NULL) {}

		~Graph() {
			if (_connections != NULL) {
				for (Vertex *v : *_connections)
					delete v;
				delete _connections;
			}
		}


		std::vector<Vertex*> vertices() const { return _vertices;  }

		Vertex *vertex(int key) const { return _vertices[key - 1]; }


		void incrementSCC()       { _sccNo++;                     }
		void addVertex(Vertex* v) { _vertices[v->key() - 1] = v;  }


		void maxMinKey(int mk)    {
			if (mk > _maxMinKey)
				_maxMinKey = mk;
		}

		void addEdge(Vertex *start, Vertex *end) {
			if(start->isValid() && end->isValid())
				start->addEdge(end);
			else
				exit(1);
		}
		
		void createConnections() {
			if (_maxMinKey != -1)
				_connections = new std::vector<Vertex*>(_maxMinKey, NULL);
		}

		void insertConnections() {
			int mk;
			if (_connections == NULL)
				return;
			for (Vertex *v : _vertices) {
				mk = v->minKey();
				if ((*_connections)[mk - 1] == NULL)
					(*_connections)[mk - 1] = new Vertex(mk);
				for (Vertex *w : v->adjacency())
					if (w->minKey() != mk)
						if ( sortedUniqueInsert(w, (*_connections)[mk - 1]->adjacency()) )
							_connectNo += 1;
			}
		}

		void printConnections() {
		fprintf(stdout, "%d\n%d\n", _sccNo, _connectNo);
		for (Vertex *v : *_connections)
			if (v != NULL)
				for (Vertex *w : v->adjacency())
					fprintf(stdout, "%d %d\n", v->key(), w->minKey());
		}

		void output() {
			createConnections();
			insertConnections();
			printConnections();
		}

};



class SCCFinder {

		Graph               &_graph;
		std::stack<Vertex*>  _stack;
		int                  _index;
		
	public:

		SCCFinder(Graph &g) :
			_graph(g),
			_index(0) {}

		void tarjan() {
			for (Vertex* v : _graph.vertices())
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
				_graph.incrementSCC();

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

				_graph.maxMinKey(minKey);
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
	SCCFinder finder(g);

	for (int i = 0; i < vertexNo; i++)
		g.addVertex(new Vertex(i + 1));

	for (int i = 0; i < edgeNo; i++) {
		fscanf(stdin, "%d %d", &edgeStart, &edgeEnd);
		g.addEdge(g.vertex(edgeStart), g.vertex(edgeEnd));
	}

	finder.tarjan();
	g.output();

	for (int i = 1; i <= vertexNo; i++)
		delete g.vertex(i);

	return 0;

}	

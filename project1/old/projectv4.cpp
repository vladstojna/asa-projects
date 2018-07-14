// Project

#include <vector>
#include <list>
#include <stack>
#include <iostream>

class Vertex {

		int  _key;
		int  _discovery;
		int  _lowlink;
		bool _onStack;

	public:

		Vertex(int key) :
			_key(key),
			_discovery(-1),
			_lowlink(-1),
			_onStack(false) {}

		int key()       const { return _key;       }
		int discovery() const { return _discovery; }
		int lowlink()   const { return _lowlink;   }

		void key(int k)       { _key       = k;   }
		void discovery(int d) { _discovery = d;   }
		void lowlink(int ll)  { _lowlink   = ll;  }
		void onStack(bool b)  { _onStack   = b;   }
		
		bool onStack()      const { return _onStack;         }
		bool undiscovered() const { return _discovery == -1; }
		bool isValid()      const { return _key >= 1;        }

		void lowerlink(const Vertex &v) {
			if (_lowlink > v.lowlink())
				_lowlink = v.lowlink();
		}

		friend bool operator!=(const Vertex &v1, const Vertex &v2) {
			return v1.key() != v2.key();
		}

		friend bool operator==(const Vertex &v1, const Vertex &v2) {
			return v1.key() == v2.key();
		}
		
		friend std::ostream &operator<<(std::ostream &stream, const Vertex &v) {
			return stream << v.key();
		}

};

class Edge {

		Vertex *_begin;
		Vertex *_end;

	public:

		Edge(Vertex *begin, Vertex *end) :
			_begin(begin),
			_end(end) {}

		inline Vertex *begin() const { return _begin; }
		inline Vertex *end()   const { return _end; }

		friend bool operator==(const Edge &e1, const Edge &e2) {
			return *e1.begin() == *e2.begin() && *e1.end() == *e2.end();
		}

		friend std::ostream &operator<<(std::ostream &stream, const Edge &e) {
			return stream << *e.begin() << " " << *e.end();
		}
};



class Graph {

		const int _vertexNo;
		const int _edgeNo;
		int _sccNo;
		int _connectNo;
		int _maxMinKey;


		std::vector<Vertex*> _vertices;
		std::vector<Edge*>   _edges;
		std::vector<int>     _adjacency;

	public:

		Graph(int nv, int ne) :
			_vertexNo(nv),
			_edgeNo(ne),
			_sccNo(0),
			_connectNo(0),
			_maxMinKey(-1),
			_vertices(nv),
			_edges(ne),
			_adjacency(nv + 1) {}

		std::vector<Edge*>   edges()    const { return _edges; }
		std::vector<Vertex*> vertices() const { return _vertices; }

		Edge *edge(int position) {
			if (position >= 0 && position < _edgeNo)
				return _edges[position];
			exit(1);
		}

		Vertex *vertex(int key) {
			if (key > 0 && key <= _vertexNo)
				return _vertices[key - 1];
			exit(1);
		}

		int adjStart(Edge *e)   const { return _adjacency[e->begin()->key() - 1]; }
		int adjSize (Edge *e)   const { return _adjacency[e->begin()->key()] - _adjacency[e->begin()->key() - 1]; }
		int adjStart(Vertex *v) const { return _adjacency[v->key() - 1]; }
		int adjSize (Vertex *v) const { return _adjacency[v->key()] - _adjacency[v->key() - 1]; }
		
		void incrementSCC() { _sccNo++; }

		void addVertex(Vertex *v) {
			if (v->isValid())
				_vertices[v->key() - 1] = v;
			else
				exit(1);
		}

		void addEdge(Edge *edge, int position) {
			if (position >= 0 && position < _edgeNo) {
				_edges[position] = edge;
				_adjacency[edge->begin()->key() - 1] += 1;
			}
			else
				exit(1);
		}

		void sortByEnd() {

			int range = _vertexNo + 1;
			std::vector<Edge*> aux(_edgeNo);
			std::vector<int>   count(range);

			for (int i = 0; i < _edgeNo; i++)
				++count[_edges[i]->end()->key()];
			
			for (int i = 1; i < range; i++)
				count[i] += count[i - 1];

			for (int i = _edgeNo - 1; i >= 0; i--) {
				int key = _edges[i]->end()->key();
				aux[count[key] - 1] = _edges[i];
				--count[key];
			}

			for (int i = 0; i < _edgeNo; i++)
				_edges[i] = aux[i];

		}

		void sortByStart() {

			int range = _vertexNo + 1;
			std::vector<Edge*> aux(_edgeNo);
			std::vector<int>   count(range);

			for (int i = 0; i < _edgeNo; i++)
				++count[_edges[i]->begin()->key()];
			
			for (int i = 1; i < range; i++)
				count[i] += count[i - 1];

			for (int i = _edgeNo - 1; i >= 0; i--) {
				int key = _edges[i]->begin()->key();
				aux[count[key] - 1] = _edges[i];
				--count[key];
			}

			for (int i = 0; i < _edgeNo; i++)
				_edges[i] = aux[i];

		}

		void generateAdjacency() {
			for (int i = _vertexNo; i > 0; i--)
				_adjacency[i] = _adjacency[i - 1];
			_adjacency[0] = 0;

			for (int i = 1; i < _vertexNo + 1; i++)
				_adjacency[i] += _adjacency[i - 1];
		}

		void connectionsNumber() {
			Edge *prev = NULL;

			for (Edge *e : _edges) {
				if (e->begin()->key() == e->end()->key() || (prev != NULL && *e == *prev)) continue;
				
				_connectNo++;
				prev = e;
			}

		}

		void print() {
			Edge *prev = NULL;

			fprintf(stdout, "%d\n", _sccNo);
			fprintf(stdout, "%d\n", _connectNo);

			for (Edge *e : _edges) {
				if (e->begin()->key() == e->end()->key() || (prev != NULL && *e == *prev)) continue;

				fprintf(stdout, "%d %d\n", e->begin()->key(), e->end()->key());
				prev = e;
			}
		}

		void debugprint() {
			for (Edge *e : _edges)
				std::cout << *e << "\t| " << _adjacency[e->begin()->key() - 1] << "\n";
			std::cout << "\n";
		}

};

class SCCFinder {

		Graph               &_graph;
		std::stack<Vertex*>  _stack;
		int                  _index;
		int					 _adjStart;
		
	public:

		SCCFinder(Graph &g) :
			_graph(g),
			_index(0),
			_adjStart(0) {}

		void tarjan() {
			for (Vertex *v : _graph.vertices())
				if (v->undiscovered())
					strongconnect(v, _graph.adjSize(v));
			std::cout << "\n";
		}

		void strongconnect(Vertex *v, int adjSize) {

			v->discovery(_index);
			v->lowlink(_index);
			v->onStack(true);
			_index += 1;
			_stack.push(v);
			int start = _graph.adjStart(v);
			int end   = start + adjSize;


			for (int i = start; i < end; i++) {

				Vertex *neighbour = _graph.edge(i)->end();

				if (neighbour->undiscovered()) {
					strongconnect(neighbour, _graph.adjSize(neighbour));
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
					std::cout << w->key() << " ";
				} while (*w != *v);
				std::cout << "| ";
				for (Vertex *m : members)
					m->key(minKey);
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

	std::cout << "Creating vertices\n";
	for (int i = 1; i <= vertexNo; i++)
		g.addVertex(new Vertex(i));

	std::cout << "Adding Edges\n";
	for(int i = 0; i < edgeNo; i++) {
		fscanf(stdin, "%d %d", &edgeStart, &edgeEnd); 
		g.addEdge(new Edge(g.vertex(edgeStart), g.vertex(edgeEnd)), i);
	}

	g.sortByStart();
	g.generateAdjacency();
	g.debugprint();
	std::cout << "------------------------------------------\n";

	SCCFinder finder(g);

	std::cout << "Before Tarjan\n";
	finder.tarjan();
	std::cout << "\nTarjan successful\n";

	g.debugprint();
	std::cout << "------------------------------------------\n";
	g.sortByEnd();
	g.sortByStart();
	g.connectionsNumber();
	std::cout << "------------------------------------------\n";
	g.print();

	for (int i = 0; i < edgeNo; i++)
		delete g.edge(i);

	for (int i = 1; i <= vertexNo; i++)
		delete g.vertex(i);

	std::cout << "End successful";

	return 0;
}

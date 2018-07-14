// Project

#include <vector>
#include <list>
#include <queue>
#include <stack>

#include <climits>
#include <iostream>
#include <chrono>

/*---------------------------------------
|
|    Vertex
|
\----------------------------------------*/

class Vertex {

		const int  _key;
		int  _level;

	public:

		Vertex(int key) :
			_key(key),
			_level(-1) {}

		inline const int  key()     const { return _key;          }
		inline const int  level()   const { return _level;        }
		inline const bool visited() const { return _level != -1;  }
		inline const bool isValid() const { return _key >= 0;     }

		void level(int l) { _level = l; }

		friend bool operator!=(const Vertex &v1, const Vertex &v2) {
			return v1.key() != v2.key();
		}

		friend bool operator==(const Vertex &v1, const Vertex &v2) {
			return v1.key() == v2.key();
		}

};

/*---------------------------------------
|
|    Edge
|
\----------------------------------------*/
class Edge {

		Vertex *_source;
		Vertex *_target;

		int _capacity;
		int _flow;

		Edge *_rev;

	public:

		Edge(Vertex *s, Vertex *t, int cap) :
			_source(s),
			_target(t),
			_capacity(cap),
			_flow(0),
			_rev(NULL) {}

		inline Vertex *source() const { return _source; }
		inline Vertex *target() const { return _target; }

		inline int cap()  const { return _capacity; }
		inline int flow() const { return _flow;     }
		
		inline Edge *rev()   const { return _rev; }
		inline bool hasRev() const { return _rev != NULL; }

		void cap(int c)     { _capacity += c;  }
		void addFlow(int f) { _flow += f; }
		void subFlow(int f) { _flow -= f; }
		void rev(Edge *r)   { _rev = r; }

		friend bool operator==(const Edge &e1, const Edge &e2) {
			return *e1.source() == *e2.source() && *e1.target() == *e2.target();
		}

		void print() {
			fprintf(stdout, "%d\t%d\t%d\t%d\n", _source->key(), _target->key(), _capacity, _flow);
		}

};

/*---------------------------------------
|
|    Graph
|
\----------------------------------------*/
class Graph {

		const int _vertexNo;
		const int _edgeNo;

		std::vector<Vertex*> _vertices;
		std::vector< std::vector<Edge*> > _edges;

	public:

		Graph(int nv, int ne) :
			_vertexNo(nv),
			_edgeNo(ne),
			_vertices(nv),
			_edges(nv) {}

		// Getters -------------------------------------------------------------

		inline const std::vector<Vertex*> &vertices()           const { return _vertices; }
		inline const std::vector<Edge*>   &adjacency(Vertex *v) const { return _edges[v->key()]; }

		inline Vertex *vertex(int key) const {
			if (key >= 0 && key < _vertexNo)
				return _vertices[key];
			exit(1);
		}

		Vertex *source() const { return vertex(0); }
		Vertex *target() const { return vertex(_vertexNo - 1); }

		bool fromSource(Edge *e) const { return e->source()->key() == source()->key(); }
		bool toTarget(Edge *e)   const { return e->target()->key() == target()->key(); }

		inline const int size() const { return _vertexNo; }

		//-----------------------------------------------------------------------

		void addVertex(Vertex *v) {
			if (v->isValid())
				_vertices[v->key()] = v;
			else
				exit(1);
		}

		void addEdge(Edge *e) {
			_edges[e->source()->key()].push_back(e);
		}

		void setReverse(Edge *e1, Edge *e2) {
			if (e1->source() == e2->target() &&
			e2->source() == e1->target()) {
				e1->rev(e2);
				e2->rev(e1);
			}
		}

		void loadVertex(int key) {
			addVertex(new Vertex(key));
		}

		void loadSingleEdge(Vertex *s, Vertex *t, int cap) {
			if (cap == 0)
				return;
			addEdge(new Edge(s, t, cap));
		}

		void loadDoubleEdge(Vertex *s, Vertex *t, int cap) {
			if (cap == 0)
				return;
			Edge *normal;
			Edge *reverse;
			normal  = new Edge(s, t, cap);
			reverse = new Edge(t, s, cap);
			addEdge(normal);
			addEdge(reverse);
			setReverse(normal, reverse);
		}

		void unload() {
			for (Vertex *v : _vertices) {
				for (Edge *e : _edges[v->key()])
					delete e;
			delete v;
			}
		}

		void print() {
			for (Vertex *v : _vertices) {
				fprintf(stdout, "%2d | ", v->key());
				for (Edge *e : adjacency(v))
					fprintf(stdout, "%2d ", e->target()->key());
				std::cout << "\n";
			}
		}

		void output(int n) {
			for (int i = 1; i < _vertexNo - 1; i++) {
				if (vertex(i)->visited())
					fprintf(stdout, "C ");
				else
					fprintf(stdout, "P ");
				if (i % n == 0)
					fprintf(stdout, "\n");
			}
		}
};

class MinCutMaxFlow {

		Graph  &_graph;
		Vertex *_source;
		Vertex *_target;
		int     _maxFlow;

	public:


		MinCutMaxFlow(Graph &g, Vertex *s, Vertex *t) :
			_graph(g),
			_source(s),
			_target(t),
			_maxFlow(0) {}


		int min(int i1, int i2) { return i1 < i2 ? i1 : i2; }

		void addStartingFlow() {
			Vertex *s = _graph.source();
			Vertex *t = _graph.target();

			Edge *vt;

			int flow;

			for (Edge *sv : _graph.adjacency(s)) {
				if (_graph.adjacency(sv->target()).size() > 0) {
					/* Since edge (v, t) is added first, target is always at 0, if exists */
					vt = _graph.adjacency(sv->target())[0];
					if (vt->target() == t) {
						flow = min(sv->cap(), vt->cap());
						sv->addFlow(flow);
						vt->addFlow(flow);
						_maxFlow += flow;
					}
				}
			}
		}

		/* Dinic methods -------------------------------------------------------------------------- */

		bool BFS_dinic() {
			for (Vertex *v : _graph.vertices())
				v->level(-1);

			std::queue<Vertex*> queue;

			_source->level(0);
			queue.push(_source);

			while (!queue.empty()) {

				Vertex *curr = queue.front();
				queue.pop();

				for (Edge *e : _graph.adjacency(curr)) {
					if (e->target()->level() < 0 && e->flow() < e->cap()) {
						e->target()->level(curr->level() + 1);
						queue.push(e->target());
					}
				}
			}
			return _target->level() >= 0;
		}
		
		
		int sendFlow_dinic(Vertex *v, int flow, unsigned int restart[]) {

			if (v == _target) return flow;

			unsigned int size = _graph.adjacency(v).size();

			for ( ; restart[v->key()] < size; ++restart[v->key()]) {
				Edge *e = _graph.adjacency(v)[restart[v->key()]];
				if (e->target()->level() == v->level() + 1 && e->flow() < e->cap()) {
					int currFlow = min(flow, e->cap() - e->flow());
					int tempFlow = sendFlow_dinic(e->target(), currFlow, restart);
					if (tempFlow > 0) {
						e->addFlow(tempFlow);
						if (!_graph.fromSource(e) && !_graph.toTarget(e))
							e->rev()->subFlow(tempFlow);
						return tempFlow;

					}
				}
			}
			return 0;
		}

		int dinic_DFS_iter(Vertex *v, int flow, unsigned int restart[]) {

			std::stack<Vertex*> vstack; // vertex stack

			Edge **pred = new Edge*[_graph.size()];

			unsigned int size;

			vstack.push(v);
			pred[v->key()] = NULL;

			// Minimize flow
			while (vstack.top() != _target) {
				v = vstack.top();
				vstack.pop();
				size = _graph.adjacency(v).size();

				for ( ; restart[v->key()] < size; restart[v->key()]++) {
					Edge *e = _graph.adjacency(v)[restart[v->key()]];
					if (e->target()->level() == v->level() + 1 && e->flow() < e->cap()) {
						flow = min(flow, e->cap() - e->flow());
						vstack.push(e->target());
						pred[e->target()->key()] = e;
					}
				}
				if (flow == INT_MAX) {
					delete[] pred;
					return 0;
				}
			}

			// Update path edges with current flow
			Edge *e = pred[_target->key()];
			while (e != NULL) {
				e->addFlow(flow);
				if (!_graph.fromSource(e) && !_graph.toTarget(e))
					e->rev()->subFlow(flow);
				e = pred[e->source()->key()];
			}

			delete[] pred;
			return flow;
		}

		int dinic_iter() {
			unsigned int *restart = new unsigned int[_graph.size()];
			while (BFS_dinic()) {

				std::fill_n(restart, _graph.size(), 0);

				while (int flow = dinic_DFS_iter(_source, INT_MAX, restart))
					_maxFlow += flow;
			}
			delete[] restart;
			return _maxFlow;
		}		

		int dinic() {
			unsigned int *restart = new unsigned int[_graph.size()];
			while (BFS_dinic()) {

				std::fill_n(restart, _graph.size(), 0);

				while (int flow = sendFlow_dinic(_source, INT_MAX, restart))
					_maxFlow += flow;
			}
			delete[] restart;
			return _maxFlow;
		}

		/* ---------------------------------------------------------------------------------------- */
		/* E-K methods ---------------------------------------------------------------------------- */
		bool BFS_ek(std::vector<Edge*> &pred) {

			std::queue<Vertex*> queue;

			int size = _graph.size();
			for (int i = 0; i < size; i++) {
				pred[i] = NULL;
				_graph.vertex(i)->level(-1);
			}

			queue.push(_source);
			_source->level(0);

			while (!queue.empty()) {
				Vertex *curr = queue.front();
				queue.pop();

				for (Edge *e : _graph.adjacency(curr)) {
					if (pred[e->target()->key()] == NULL && e->cap() > e->flow()) {
						pred[e->target()->key()] = e;
						e->target()->level(0);
						queue.push(e->target());
					}
				}
			}
			return _target->visited();
		}

		int edmondsKarp() {
			std::vector<Edge*> pred(_graph.size());
			while (BFS_ek(pred)) {
				int df = INT_MAX;
				for (Edge *e = pred[_target->key()]; e != NULL; e = pred[e->source()->key()])
					df = min(df, e->cap() - e->flow());
				for (Edge *e = pred[_target->key()]; e != NULL; e = pred[e->source()->key()]) {
					e->addFlow(df);
					if (!_graph.fromSource(e) && !_graph.toTarget(e))
						e->rev()->subFlow(df);
				}
				_maxFlow += df;
			}
			return _maxFlow;
		}

};

void printTime(std::chrono::high_resolution_clock::time_point &start, std::chrono::high_resolution_clock::time_point &end, const char *message) {
	end = std::chrono::high_resolution_clock::now();
	std::cout << message << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "\n";	
	start = std::chrono::high_resolution_clock::now();
}

/*---------------------------------------
|
|    main
|
\----------------------------------------*/
int main() {

	int n;
	int m;
	int vertexNo;
	int edgeNo;

	int capacity;

	//std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
	//std::chrono::high_resolution_clock::time_point endTime;

	// Reading input; creation of objects

	fscanf(stdin, "%d %d", &m, &n);

	vertexNo = n * m + 2;

	edgeNo =
		2 * n * m + /* universal source and sink edges */
		2 * n * (m - 1) + 2 * (n - 1) * m; /* interior edges */

		/* 2mn + 2 (mn + mn - n - m) = 6mn - 2n - 2m */

	Graph g(vertexNo, edgeNo);

	//printTime(startTime, endTime, "Creating graph (ms): ");

	for (int i = 0; i < vertexNo; i++)
		g.loadVertex(i);

	//printTime(startTime, endTime, "Loading vertices (ms): ");

	MinCutMaxFlow mf(g, g.source(), g.target());

	//printTime(startTime, endTime, "Creating MinCutMaxFlow (ms): ");

	for (int i = 1; i < vertexNo - 1; i++) {
		fscanf(stdin, "%d", &capacity);
		g.loadSingleEdge(g.source(), g.vertex(i), capacity);
	}

	//printTime(startTime, endTime, "Adding source - vertex (ms): ");

	for (int i = 1; i < vertexNo - 1; i++) {
		fscanf(stdin, "%d", &capacity);
		g.loadSingleEdge(g.vertex(i), g.target(), capacity);
	}

	//printTime(startTime, endTime, "Adding vertex - target (ms): ");

	for (int i = 1; i < vertexNo - 1; i++) {
		if (i % n == 0)
			continue;
		fscanf(stdin, "%d", &capacity);
		g.loadDoubleEdge(g.vertex(i), g.vertex(i + 1), capacity);
	}

	//printTime(startTime, endTime, "Adding horizontal edges (ms): ");

	for (int i = 1; i < n * (m - 1) + 1; i++) {
		fscanf(stdin, "%d", &capacity);
		g.loadDoubleEdge(g.vertex(i), g.vertex(i + n), capacity);
	}

	//printTime(startTime, endTime, "Adding vertical edges (ms): ");

	//g.print();

	// Main procedure
	mf.addStartingFlow();

	//printTime(startTime, endTime, "Adding starting flow (ms): ");

	fprintf(stdout, "%d\n\n", mf.edmondsKarp());
	//g.output(n);

	//printTime(startTime, endTime, "Main algorithm (ms): ");

	// Deletion of objects
	g.unload();

	//printTime(startTime, endTime, "Deletion of objects (ms): ");

	return 0;

}

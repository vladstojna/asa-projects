// Project 2 - v1 - Dinic

import java.util.Queue;
import java.util.Stack;
import java.util.List;

import java.util.ArrayList;
import java.util.ArrayDeque;

import java.io.IOException;
import java.io.StreamTokenizer;

import java.io.BufferedReader;
import java.io.InputStreamReader;

import java.io.BufferedWriter;
import java.io.OutputStreamWriter;

/*---------------------------------------
|
|    Vertex
|
\----------------------------------------*/
class Vertex {

	private int _key;
	private int _level;
	private int _adjSize;


	private Vertex(int key, int level, int adj) {
		_key     = key;
		_level   = level;
		_adjSize = adj;
	}

	public Vertex(int key) {
		this(key, -1, 0);
	}


	public int key()     { return _key;   }
	public int level()   { return _level; }
	public int adjSize() { return _adjSize; }

	public boolean visited() { return _level != -1; }
	public boolean isValid() { return _key >= 0;    }

	public void level(int l) { _level = l; }
	public void addEdge() { _adjSize += 1; }


	@Override
	public boolean equals(Object other) {
		if (other instanceof Vertex) {
			Vertex v = (Vertex) other;
			return v.key() == _key;
		}
		return false;
	}

	@Override
	public String toString() {
		return _key + ", [" + _level + "]";
	}

}


/*---------------------------------------
|
|    Edge
|
\----------------------------------------*/
class Edge {

	private Vertex _source;
	private Vertex _target;

	private int _capacity;
	private int _flow;

	private Edge _rev;


	private Edge(Vertex s, Vertex t, int c, int f, Edge rev) {
		_source = s;
		_target = t;
		_capacity = c;
		_flow = f;
		_rev = rev;		
	}

	public Edge(Vertex s, Vertex t, int c) {
		this(s, t, c, 0, null);
	}


	public Vertex source() { return _source;   }
	public Vertex target() { return _target;   }
	public int cap()    { return _capacity; }
	public int flow()   { return _flow;     }

	public Edge rev() { return _rev; }

	void addFlow(int f) { _flow += f; }
	void subFlow(int f) { _flow -= f; }

	void rev(Edge r) { _rev = r; }

	@Override
	public boolean equals(Object other) {
		if (other instanceof Edge) {
			Edge e = (Edge) other;
			return e.source() == _source && e.target() == _target;
		}
		return false;
	}

	@Override
	public String toString() {
		return "<" + _source.toString() + " - " + _target.toString() + ">";
	}

}


/*---------------------------------------
|
|    Graph
|
\----------------------------------------*/
class Graph {

	private int _vertexNo;

	private Vertex[] _vertices;
	private Edge[][] _edges;

	private Graph(int size, Vertex[] vertices, Edge[][] edges) {
		_vertexNo = size;
		_vertices = vertices;
		_edges    = edges;

		_edges[0]        = new Edge[size - 2];
		_edges[size - 1] = new Edge[0];

		for (int i = 1; i < size - 1; i++)
			_edges[i] = new Edge[5];
	}

	public Graph(int size) {
		this(size, new Vertex[size], new Edge[size][]);
	}


	public Vertex[] vertices() { return _vertices; }

	public Edge[] adjacency(Vertex v) { return _edges[v.key()]; }

	public Vertex vertex(int key) {
		if (key >= 0 && key < _vertexNo)
			return _vertices[key];
		else
			return null;
	}

	public Vertex source() { return _vertices[0]; }
	public Vertex target() { return _vertices[_vertexNo - 1]; }

	public int size() { return _vertexNo; }


	public void addVertex(Vertex v) {
		if (v.isValid())
			_vertices[v.key()] = v;
		else
			System.out.println("Invalid vertex");
	}

	public void addEdge(Edge e) {
		_edges[e.source().key()][e.source().adjSize()] = e;
		e.source().addEdge();
	}

	public void setReverse(Edge e1, Edge e2) {
		if (e1.source() == e2.target() && e2.source() == e1.target()) {
			e1.rev(e2);
			e2.rev(e1);
		}
	}

	public void loadEdge(Vertex s, Vertex t, int cap) {
		if (cap == 0)
			return;
		Edge normal;
		Edge reverse;
		normal  = new Edge(s, t, cap);
		reverse = new Edge(t, s, cap);
		this.addEdge(normal);
		this.addEdge(reverse);
		this.setReverse(normal, reverse);
	}

	public void loadToSingle(Vertex s, Vertex t, int cap) {
		if (cap == 0)
			return;
		this.addEdge(new Edge(s, t, cap));
	}

	public void output(int h) throws IOException {
		BufferedWriter log = new BufferedWriter(new OutputStreamWriter(System.out));
		for (int i = 1; i < _vertexNo - 1; i++) {
			if (_vertices[i].visited())
				log.write("C ");
			else
				log.write("P ");
			if (i % h == 0)
				log.write("\n");
		}
		log.flush();
	}

	
	public void print() {
		for (Vertex v : _vertices) {
			System.out.printf("%2d | ", v.key());
			int size = v.adjSize();
			for (int i = 0; i < size; i++) {
				Edge e = adjacency(v)[i];
				System.out.printf("%2d ", e.target().key());
			}
			System.out.println();
		}
	}
	

}

/*---------------------------------------
|
|    MinCutMaxFlow
|
\----------------------------------------*/
class MinCutMaxFlow {

	private Graph _g;
	private Vertex _source;
	private Vertex _target;

	private int _maxFlow;


	private MinCutMaxFlow(Graph g, Vertex s, Vertex t, int flow) {
		_g = g;
		_source = s;
		_target = t;
		_maxFlow = flow;
	}

	public MinCutMaxFlow(Graph g) {
		this(g, g.source(), g.target(), 0);
	}


	public int maxFlow() { return _maxFlow; }

	public void addFlow(int f) { _maxFlow += f; }

	public int min(int a, int b) { return a < b ? a : b; }


	public void addStartingFlow() {
		Vertex s = _g.source();
		Vertex t = _g.target();

		Edge vt;
		Edge sv;

		int flow;

		int sizeS = s.adjSize();
		int sizeV;

		for (int i = 0 ; i < sizeS ; i++) {
			sv = _g.adjacency(s)[i];
			vt = _g.adjacency(sv.target())[0];
			if (vt.target().equals(t)) {
				flow = min(sv.cap(), vt.cap());
				sv.addFlow(flow);
				vt.addFlow(flow);
				_maxFlow += flow;
			}
		}
	}


	public boolean dinicBFS() {

		Queue<Vertex> queue = new ArrayDeque<Vertex>();

		for (Vertex v : _g.vertices())
			v.level(-1);

		_source.level(0);

		queue.add(_source);

		while (!queue.isEmpty()) {
			
			Vertex v = queue.poll();

			int size = v.adjSize();

			for (int i = 0 ; i < size ; i++) {
				Edge e = _g.adjacency(v)[i];
				if (e.target().level() < 0 && e.cap() > e.flow()) {
					e.target().level(v.level() + 1);
					queue.add(e.target());
				}
			}

		}

		return _target.level() >= 0;

	}


	public int dinicDFS(Vertex v, int flow, int[] restart) {

		if (v.equals(_target)) return flow;

		for ( ; restart[v.key()] < v.adjSize() ; ++restart[v.key()]) {
			Edge e = _g.adjacency(v)[restart[v.key()]];
			if (e.target().level() == v.level() + 1 && e.cap() > e.flow()) {
				int currFlow = min(flow, e.cap() - e.flow());
				int tempFlow = dinicDFS(e.target(), currFlow, restart);
				if (tempFlow > 0) {
					e.addFlow(tempFlow);
					if (e.source().key() > 0 && e.target().key() < _g.size() - 1)
						e.rev().subFlow(tempFlow);
					return tempFlow;

				}
			}
		}
		return 0;

	}


	public int dinic() {

		int[] restart = new int[_g.size()];

		int flow;

		while (dinicBFS()) {

			for (int i = 0; i < _g.size(); i++)
				restart[i] = 0;

			while ( (flow = dinicDFS(_source, Integer.MAX_VALUE, restart)) > 0)
				_maxFlow += flow;

		}

		return _maxFlow;
	}

}

/*---------------------------------------
|
|    Main
|
\----------------------------------------*/
public class ProjectDinicPrimitive {

	public static void main(String[] args) throws IOException {

		long startTime = System.currentTimeMillis();
		long endTime;

		int m;
		int n;

		int vertexNo;
		int capacity;

		Graph g;
		MinCutMaxFlow mf;
		StreamTokenizer st = new StreamTokenizer(new BufferedReader(new InputStreamReader(System.in)));

		st.parseNumbers();

		st.nextToken();
		m = (int) st.nval;
		st.nextToken();
		n = (int) st.nval;

		vertexNo = m * n + 2;

		g = new Graph(vertexNo);

		//endTime = System.currentTimeMillis();
		//System.out.println("Start & creating graph took (ms): " + (endTime - startTime));


		//startTime = System.currentTimeMillis();

		// Adding vertices
		for (int i = 0; i < vertexNo; i++)
			g.addVertex(new Vertex(i));

		//endTime = System.currentTimeMillis();
		//System.out.println("Adding vertices took (ms): " + (endTime - startTime));


		//startTime = System.currentTimeMillis();

		// Adding to source
		for (int i = 1; i < vertexNo - 1; i++) {
			st.nextToken();
			capacity = (int) st.nval;
			g.loadToSingle(g.source(), g.vertex(i), capacity);
		}

		//endTime = System.currentTimeMillis();
		//System.out.println("Adding edges from source took (ms): " + (endTime - startTime));

		//startTime = System.currentTimeMillis();

		// Adding to target
		for (int i = 1; i < vertexNo - 1; i++) {
			st.nextToken();
			capacity = (int) st.nval;
			g.loadToSingle(g.vertex(i), g.target(), capacity);
		}

		//endTime = System.currentTimeMillis();
		//System.out.println("Adding edges from target took (ms): " + (endTime - startTime));

		//startTime = System.currentTimeMillis();

		// Adding interior horizontal edges
		for (int i = 1; i < vertexNo - 1; i++) {
			if (i % n == 0)
				continue;
			st.nextToken();
			capacity = (int) st.nval;
			g.loadEdge(g.vertex(i), g.vertex(i + 1), capacity);
		}

		//endTime = System.currentTimeMillis();
		//System.out.println("Adding horizontal edges took (ms): " + (endTime - startTime));

		//startTime = System.currentTimeMillis();

		// Adding interior vertical edges
		for (int i = 1; i < n * (m - 1) + 1; i++) {
			st.nextToken();
			capacity = (int) st.nval;
			g.loadEdge(g.vertex(i), g.vertex(i + n), capacity);
		}

		//endTime = System.currentTimeMillis();
		//System.out.println("Adding vertical edges took (ms): " + (endTime - startTime));

		//startTime = System.currentTimeMillis();

		//g.print();

		mf = new MinCutMaxFlow(g);
		mf.addStartingFlow();

		//endTime = System.currentTimeMillis();
		//System.out.println("Adding starting flow took (ms): " + (endTime - startTime));

		//startTime = System.currentTimeMillis();

		System.out.printf("%d\n\n", mf.dinic());
		g.output(n);

		//endTime = System.currentTimeMillis();
		//System.out.println("Dinic took (ms): " + (endTime - startTime));
	}

}

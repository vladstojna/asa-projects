/*====================================================================*/
/* 				            Projeto1 ASA																		*/
/*                                                                    */
/* Grupo: 45																													*/
/* Rodrigo Moreira  81686																							*/
/* Vasco Silva 	    86525																							*/
/*====================================================================*/

#include <stdio.h>
#include <stdlib.h>

#define INFINITE -1
#define IN 1
#define OUT 0
#define TRUE 1
#define FALSE 0

/*--------------------------------------------------------------------
| Struct: Vertex
---------------------------------------------------------------------*/
typedef struct vertex{
	int _id;
	struct vertex* _next;
}Vertex;

#define id(v)   (v->_id) 
#define next(v) (v->_next)

/*--------------------------------------------------------------------
| Struct: Graph
---------------------------------------------------------------------*/
typedef struct graph{
	int _numV;
	int _numE;
	Vertex* _edges;	
	Vertex* _adjList;
	Vertex** _tails; /*  Needed for insertions at the end of
											 | the adjacencies of each vertex in O(1)
                       */
}Graph;

#define numV(g) 	    (g->_numV)
#define numE(g) 	    (g->_numE)
#define edges(g)			(g->_edges)
#define adjList(g)		(g->_adjList)
#define tails(g)			(g->_tails)
#define adj_of(g,i)		((g->_adjList)+i)
#define tail_of(g,i)	(*((g->_tails)+i))

void initGraph(Graph* g);
void printGraph(Graph* g);
void freeGraph(Graph* g);

/*--------------------------------------------------------------------
| Struct: Stack
---------------------------------------------------------------------*/
typedef struct stack{
	Vertex* _head;
	int* _inStack;
}Stack;

#define head(s)					(s->_head)
#define inStack(s)			(s->_inStack)
#define isInStack(s,i)	(s->_inStack[i])

void initStack(Stack* s, int maxDim);
void Push(Stack* s, int i);
int  Pop(Stack* s);
void freeStack(Stack* s);

/*--------------------------------------------------------------------
| StronglyConnectedComponents (SCC) Funtions
---------------------------------------------------------------------*/
int  SCC_Tarjan(Graph* g);
void SCC_Tarjan_Visit(Graph* g, int index, int* visited, \
			Stack* stack, int d[], int low[], int* numSCC);
void SCC_Compress(Graph* g, Stack* stack, int minIndex);
int  SCC_Filter(Graph* g, int numSCC, int leadersOfSCC[]);
void printGraphSCC(Graph* g, int leadersOfSCC[]);


/*--------------------------------------------------------------------
|                                                     ,.  ,.
|                                                     ||  ||
|                                                    .''--''.
|                                                   : (.)(.) :
|                 MAIN                             .'        '.
|                                                  :          :
|                                                  :          :
|                                                  :          :
|                                                   '-m----m-' 
---------------------------------------------------------------------*/
int main(){
	Graph g;
	initGraph(&g);
	int numSCC = SCC_Tarjan(&g);
	int leadersOfSCC[numSCC];
	int edgesBetweenSCC = SCC_Filter(&g, numSCC, leadersOfSCC);
	numV((&g)) = numSCC;
	numE((&g)) = edgesBetweenSCC;
	//printGraphSCC(&g, leadersOfSCC);
	freeGraph(&g);
	return 0;
}

/*--------------------------------------------------------------------
| Function: initGraphi
---------------------------------------------------------------------*/
void initGraph(Graph* g){
	scanf("%d", &(numV(g)));
	scanf("%d", &(numE(g)));

	/*The list of Vertexes*/
	adjList(g) = (Vertex*)malloc(sizeof(Vertex)*numV(g));

	/*The tails of the adjecencies*/
	tails(g) = (Vertex**)malloc(sizeof(Vertex*)*numV(g));

	/*Array of edges so we dont have to allocate constantly*/
	edges(g) = (Vertex*)malloc(sizeof(Vertex)*numE(g));
	
	int i;
	for(i=0; i < numV(g); i++){
		next(adj_of(g,i)) = NULL;
		tail_of(g,i) = NULL;
		id(adj_of(g,i)) = i;	
	} 

	int origin, destiny;
	for(i=0; i < numE(g); i++){
		scanf("%d %d", &origin, &destiny);
		origin--; destiny--;
		id(((edges(g))+i)) = destiny;
		if(tail_of(g,origin) == NULL)
			tail_of(g,origin) = (edges(g))+i;		
		next((edges(g)+i)) = next(adj_of(g,origin));
		next(adj_of(g,origin)) = (edges(g)+i); 
	}
}

/*--------------------------------------------------------------------
| Function: freeGraph
---------------------------------------------------------------------*/
void freeGraph(Graph* g){
	free(adjList(g));
	free(edges(g));
	free(tails(g));
}

/*--------------------------------------------------------------------
| Function: printGraph
---------------------------------------------------------------------*/
void printGraph(Graph* g){
	int id;
	for(id=0; id < numV(g); id++){
		Vertex* v = next(adj_of(g,id));
		while(v != NULL){
			printf("%d %d\n", id+1, id(v)+1);
			v = next(v);
		}
	}
}

/*--------------------------------------------------------------------
| Function: printGraphSSC
|  More eficient version of 'printGraph' for SCC graphs 
---------------------------------------------------------------------*/
void printGraphSCC(Graph* g, int leadersOfSCC[]){
	printf("%d\n%d\n", numV(g), numE(g));
	Vertex* v;
	int i;
	for(i=0; i<numV(g); i++){
		v = next(adj_of(g,leadersOfSCC[i]));
		while(v != NULL){
			printf("%d %d\n", leadersOfSCC[i]+1, id(v)+1);
			v = next(v);
		}
	}

}
/*--------------------------------------------------------------------
| Function: initStack
---------------------------------------------------------------------*/
void initStack(Stack* s, int maxDim){
	head(s) = NULL;
	if(maxDim > 0){
		inStack(s) = (int*)malloc(sizeof(int)*maxDim);
		int i;
		for(i=0; i < maxDim; i++)
			isInStack(s,i) = OUT;
	}
	else
		inStack(s) = NULL;
}

/*--------------------------------------------------------------------
| Function: Push
---------------------------------------------------------------------*/
void Push(Stack* s, int id){
	Vertex* v = (Vertex*)malloc(sizeof(Vertex));
	next(v) = head(s);
	id(v) = id;
	head(s) = v;
	if(inStack(s) != NULL)
		isInStack(s,id) = IN;
}

/*--------------------------------------------------------------------
| Function: Pop
---------------------------------------------------------------------*/
int Pop(Stack* s){
	if(head(s) == NULL) 
		return -1;
	Vertex* v = head(s);
	int id = id(v);
	head(s) = next(v);
	free(v);
	if(inStack(s) != NULL)
		isInStack(s,id) = OUT;
	return id;
}

/*--------------------------------------------------------------------
| Function: Push
---------------------------------------------------------------------*/
void freeStack(Stack* s){
	/* devido ao algoritmo de tarjan utilizado 
	a stack e sempre esvaziada*/
	free(inStack(s));
}

/*--------------------------------------------------------------------
| Function: SCC_Compress
|  Condensates all the edges of all the vertexes in a SCC in one of
| them (the one with the lowest id)
---------------------------------------------------------------------*/
void SCC_Compress(Graph* g, Stack* stack, int minIndex){
	int newPop;
	while((newPop = Pop(stack)) != -1){
		if(newPop != minIndex){
			next(tail_of(g,minIndex)) = next(adj_of(g, newPop));
			tail_of(g,minIndex) = tail_of(g,newPop);
			next(adj_of(g,newPop)) = NULL;
			id(adj_of(g,newPop)) = minIndex; /* mudar o id para menor*/
		}
	}
}

/*--------------------------------------------------------------------
| Function: SCC_Tarjan
---------------------------------------------------------------------*/	
int SCC_Tarjan(Graph* g){
	int visited = 1;  
	int numV = numV(g);
	Stack stack;
	initStack(&stack, numV);

	int d[numV], low[numV];
	int numSCC = 0;

	int i;
	for(i=0; i < numV; i++)
		d[i] = INFINITE;
	for(i=0; i < numV; i++)
		if(d[i] == INFINITE)
			SCC_Tarjan_Visit(g, i, &visited, &stack, d, low, &numSCC);
	freeStack(&stack);
	return numSCC;
}


/*--------------------------------------------------------------------
| Function: SCC Tarjan
---------------------------------------------------------------------*/
void SCC_Tarjan_Visit(Graph* g, int index, int* visited, \
		Stack* stack, int d[], int low[], int* numSCC){
	d[index] = low[index] = *visited;
	*visited += 1;
	
	Vertex* vertex = adj_of(g, index);
	Push(stack, index);
	Vertex* next = next(vertex);
	while(next != NULL){
		if(d[id(next)] == INFINITE || isInStack(stack,id(next)) == IN){
			if(d[id(next)] == INFINITE)
				SCC_Tarjan_Visit(g, id(next), visited, stack, d, low, numSCC);
			if(low[index] > low[id(next)])
				low[index] = low[id(next)];
		}
		next = next(next);
	}
	int minIndex, newPop;
	if(d[index] == low[index]){
		/*
		 | Auxiliar Stack where we keep track of "POPed" vertexes
		 | from a same SCC in order to update to compress them all
		 | under the one with lowest id
		 */
		Stack auxStack;
		initStack(&auxStack, 0);
		minIndex = Pop(stack);
		newPop = minIndex;
		while(newPop != index){
			Push(&auxStack, newPop);
			newPop = Pop(stack);
			if(minIndex > newPop)	
				minIndex = newPop;
		}
		Push(&auxStack, newPop);
		SCC_Compress(g, &auxStack, minIndex);	
		(*numSCC)++;
	}
}


/*--------------------------------------------------------------------
| Function: SCC_Filter 
|  Prepares the SCC graph for printing by counting all the eddges
| between SCCs and by eliminating all the redundent edges.
---------------------------------------------------------------------*/
int SCC_Filter(Graph* g, int numSCC, int leadersOfSCC[]){
	int edgesFromThisSCC = 0;
	int totalEdgesSCC = 0;

	int remainingPossibleEdges[numSCC];
	int checkEdgeFromSCC[numSCC];
	int map[numV(g)];
	
	int indexForSccThings = 0; 
	int id;
	for(id=0; id<numV(g); id++){
		/*
		 | This comparison ensures that id is a "SCC id"
		 */
		if(id == id(adj_of(g,id))){
			leadersOfSCC[indexForSccThings] = id;
			/*
			 | A SCC can only have numSCC-1 (non-redundant) edges
			 | to other SCCs
			 */
			remainingPossibleEdges[indexForSccThings] = numSCC-1;
			checkEdgeFromSCC[indexForSccThings] = FALSE;
			map[id] = indexForSccThings;
			indexForSccThings++;
			/*
			 | Optimization: once all the SCC representatives have
			 | been found, there's no need to iterates more vertexes
			 */
			if(indexForSccThings == numSCC)
				break;
		}
	}

	int 	originLeaderId;
	Vertex* originLeader;
	int 	destinyLeaderId;
	Vertex* next;
	for(id=0; id<numSCC; id++){
		originLeaderId = leadersOfSCC[id]; 
		originLeader = adj_of(g, originLeaderId);
		next = next(originLeader);
		/*
		 | In case there's an isolated vertex
		 */
		if(next == NULL)
			continue;
		while(next != NULL){
			destinyLeaderId = id(adj_of(g, id(next)));
			/*
			 | Edges between vertexes of the same SCC do not count 
			 */
			if(destinyLeaderId != originLeaderId &&
				checkEdgeFromSCC[map[destinyLeaderId]] == FALSE){
				/*
				 | If SCC1 has and edge to SCC2 => 
				 | =>	SCC2 doesn't have an edge to SCC1
				 */
				remainingPossibleEdges[map[originLeaderId]] -= 1;
				remainingPossibleEdges[map[destinyLeaderId]] -= 1;
				checkEdgeFromSCC[map[destinyLeaderId]] = TRUE;
				edgesFromThisSCC++;
				totalEdgesSCC++;
				/*
				 | Optimization: This SCC can't connect to any others
				 */
				if(remainingPossibleEdges[map[originLeaderId]] == 0)
					break;			
			}
			next = next(next);
		}

		next = originLeader;
		int j; 
		for(j=0; j<numSCC; j++){
			if(checkEdgeFromSCC[j] == TRUE){
				next = next(next);
				id(next) = leadersOfSCC[j];
				checkEdgeFromSCC[j] = FALSE;
				edgesFromThisSCC--;
			}
			/*
			 | Optimization: once all the edges have been found,
			 | there's no need to iterates more SCCs
			 */
			if(edgesFromThisSCC == 0){
				/*
				 | This comparison ensures that the compressed edges
				 | (redundant or in the same SCC) are "lost"
				 */
				next(next) = NULL;
				break;
			}
		}
	}
	return totalEdgesSCC;
} 

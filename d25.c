#include "aoc.h"
#include "parse.h"

#include <time.h>

const char* DefaultInputPath = "d25.txt";

static bool IsLower(char C)
{
    return C >= 'a' && C <= 'z';
}

typedef struct
{
    int* Vertices;
    int VertexCount;
} vertex_lookup;

static void InitVertexLookup(vertex_lookup* Lookup)
{
    Lookup->Vertices = (int*)calloc(26 * 26 * 26, sizeof(int));
    Lookup->VertexCount = 0;
}

static void FreeVertexLookup(vertex_lookup* Lookup)
{
    free(Lookup->Vertices);
}

static int VertexLookupGet(vertex_lookup* Lookup, int Id)
{
    int Vertex = Lookup->Vertices[Id];
    if(!Vertex)
    {
        Vertex = ++Lookup->VertexCount;
        Lookup->Vertices[Id] = Vertex;
    }
    return Vertex - 1;
}

static const char* ParseVertex(const char* Input, vertex_lookup* Lookup, int* OutVertex)
{
    int Id = 0;
    while(IsLower(*Input)) Id = Id * 26 + *Input++ - 'a';
    *OutVertex = VertexLookupGet(Lookup, Id);
    return Input;
}

typedef struct
{
    int From;
    int To;
} edge;

typedef struct
{
    size_t Count;
    size_t Capacity;
    int* Buffer;
    int Head;
    int Tail;
} vertex_queue;

static void VertexQueueReset(vertex_queue* Queue)
{
    Queue->Count = 0;
    Queue->Head = 0;
    Queue->Tail = 0;
}

static void InitVertexQueue(vertex_queue* Queue)
{
    Queue->Capacity = 0;
    Queue->Buffer = NULL;
    VertexQueueReset(Queue);
}

static void FreeVertexQueue(vertex_queue* Queue)
{
    free(Queue->Buffer);
}

static void VertexQueuePush(vertex_queue* Queue, int Vertex)
{
    if(Queue->Count == Queue->Capacity)
    {
        size_t NewCapacity = Queue->Capacity ? Queue->Capacity * 2 : 64;
        int* NewBuffer = (int*)malloc(sizeof(int) * NewCapacity);
        if(Queue->Head < Queue->Tail)
        {
            memcpy(NewBuffer, Queue->Buffer + Queue->Head, sizeof(int) * (Queue->Tail - Queue->Head));
            Queue->Tail -= Queue->Head;
        }
        else
        {
            size_t ToCapacity = Queue->Capacity - Queue->Head;
            memcpy(NewBuffer, Queue->Buffer + Queue->Head, sizeof(int) * ToCapacity);
            memcpy(NewBuffer + ToCapacity, Queue->Buffer, sizeof(int) * Queue->Tail);
            Queue->Tail += ToCapacity;
        }
        Queue->Head = 0;
        free(Queue->Buffer);
        Queue->Capacity = NewCapacity;
        Queue->Buffer = NewBuffer;
    }
    Queue->Buffer[Queue->Tail] = Vertex;
    Queue->Tail = (Queue->Tail + 1) & (Queue->Capacity - 1);
    Queue->Count++;
}

static int VertexQueuePull(vertex_queue* Queue)
{
    int Vertex = Queue->Buffer[Queue->Head];
    Queue->Head = (Queue->Head + 1) & (Queue->Capacity - 1);
    Queue->Count--;
    return Vertex;
}

static int CountConnected(int Vertex, int VertexCount, int* Adj, uint8_t* Visited)
{
    Visited[Vertex] = true;
    int* VertexAdj = &Adj[Vertex * VertexCount];
    int Count = 1;
    for(int Neighbor = 0; Neighbor < VertexCount; Neighbor++)
    {
        if(Visited[Neighbor]) continue;
        if(!VertexAdj[Neighbor]) continue;
        Count += CountConnected(Neighbor, VertexCount, Adj, Visited);
    }
    return Count;
}

AOC_SOLVER(Part1)
{
    // Parse all vertices and edges from the input.
    vertex_lookup Lookup;
    InitVertexLookup(&Lookup);
    size_t EdgeCount = 0;
    size_t EdgeCapacity = 8;
    edge* Edges = (edge*)malloc(sizeof(edge) * EdgeCapacity);
    while(IsLower(*Input))
    {
        int FromVertex;
        Input = ParseVertex(Input, &Lookup, &FromVertex) + 2;
        while(IsLower(*Input))
        {
            int ToVertex;
            Input = SkipPastWhitespace(ParseVertex(Input, &Lookup, &ToVertex));
            if(EdgeCount == EdgeCapacity)
            {
                EdgeCapacity *= 2;
                Edges = (edge*)realloc(Edges, sizeof(edge) * EdgeCapacity);
            }
            Edges[EdgeCount++] = (edge){.From = FromVertex, .To = ToVertex};
        }
        Input = SkipPastNewline(Input);
    }

    // Form an adjacency matrix from the edges.
    int* Adj = (int*)calloc(Lookup.VertexCount * Lookup.VertexCount, sizeof(int));
    for(int EdgeIndex = 0; EdgeIndex < EdgeCount; EdgeIndex++)
    {
        edge Edge = Edges[EdgeIndex];
        Adj[Edge.From * Lookup.VertexCount + Edge.To] = EdgeIndex + 1;
        Adj[Edge.To * Lookup.VertexCount + Edge.From] = EdgeIndex + 1;
    }

    // Find paths between random vertices in the graph. Count the frequency of
    // edges encountered. Edges in the min-cut have a high chance of appearing
    // in one of these paths because they are bridges between the two connected
    // sub-graphs, which will be taken if the randomly selected vertices are
    // in different halves of the graph.
    int* Frequency = (int*)malloc(sizeof(int) * EdgeCount);
    int* Prev = (int*)malloc(sizeof(int) * Lookup.VertexCount);
    int* Dist = (int*)malloc(sizeof(int) * Lookup.VertexCount);
    vertex_queue Queue;
    InitVertexQueue(&Queue);
    uint8_t* Visited = (uint8_t*)malloc(sizeof(uint8_t) * Lookup.VertexCount);
    srand(time(NULL));
    int NumIterations = 5;
    int RemovedEdgeIndices[3];
    int Connected;
    for(;;)
    {
        memset(Frequency, 0, sizeof(int) * EdgeCount);

        for(int Phase = 0; Phase < 3; Phase++)
        {
            for(int Iteration = 0; Iteration < NumIterations; Iteration++)
            {
                // Reset the BFS state.
                for(int Index = 0; Index < Lookup.VertexCount; Index++)
                {
                    Dist[Index] = INT32_MAX;
                }
                VertexQueueReset(&Queue);

                // Select a random pair of vertices.
                int Source = rand() % Lookup.VertexCount;
                int Sink = rand() % (Lookup.VertexCount - 1);
                if(Sink == Source) Sink++;

                // Perform BFS, searching for a path between source and sink.
                Dist[Source] = 0;
                VertexQueuePush(&Queue, Source);
                while(Queue.Count > 0)
                {
                    int Vertex = VertexQueuePull(&Queue);
                    if(Vertex == Sink) break;
                    int* VertexAdj = &Adj[Vertex * Lookup.VertexCount];
                    for(int Neighbor = 0; Neighbor < Lookup.VertexCount; Neighbor++)
                    {
                        if(!VertexAdj[Neighbor]) continue;
                        int AltDist = Dist[Vertex] + 1;
                        if(AltDist < Dist[Neighbor])
                        {
                            Dist[Neighbor] = AltDist;
                            Prev[Neighbor] = Vertex;
                            VertexQueuePush(&Queue, Neighbor);
                        }
                    }
                }

                // Increase the frequency on every edge connecting source and
                // sink.
                int Vertex = Sink;
                while(Vertex != Source)
                {
                    int PrevVertex = Prev[Vertex];
                    int EdgeIndex = Adj[PrevVertex * Lookup.VertexCount + Vertex] - 1;
                    Frequency[EdgeIndex]++;
                    Vertex = PrevVertex;
                }
            }

            // Find the edge with the highest frequency.
            int MaxEdgeIndex = 0;
            int MaxEdgeFreq = 0;
            for(int EdgeIndex = 0; EdgeIndex < EdgeCount; EdgeIndex++)
            {
                int EdgeFreq = Frequency[EdgeIndex];
                if(EdgeFreq > MaxEdgeFreq)
                {
                    MaxEdgeIndex = EdgeIndex;
                    MaxEdgeFreq = EdgeFreq;
                }
            }

            // Remove the highest frequency edge from the graph.
            edge MaxEdge = Edges[MaxEdgeIndex];
            Adj[MaxEdge.From * Lookup.VertexCount + MaxEdge.To] = 0;
            Adj[MaxEdge.To * Lookup.VertexCount + MaxEdge.From] = 0;
            Frequency[MaxEdgeIndex] = 0;
            RemovedEdgeIndices[Phase] = MaxEdgeIndex;
        }

        // Stop iterating if we've split the graph.
        memset(Visited, 0, sizeof(uint8_t) * Lookup.VertexCount);
        Connected = CountConnected(0, Lookup.VertexCount, Adj, Visited);
        if(Connected < Lookup.VertexCount) break;

        // The graph isn't split - reset the clock! Restore the adjacency
        // matrix and increase the number of iterations.
        for(int Phase = 0; Phase < 3; Phase++)
        {
            int EdgeIndex = RemovedEdgeIndices[Phase];
            edge Edge = Edges[EdgeIndex];
            Adj[Edge.From * Lookup.VertexCount + Edge.To] = EdgeIndex + 1;
            Adj[Edge.To * Lookup.VertexCount + Edge.From] = EdgeIndex + 1;
        }
        NumIterations++;
    }

    // Count the number of vertices connected to the first vertex. Assuming
    // we've successfully split the graph into two, use this to work out the
    // number of vertices in the other half of the graph.
    int64_t Result = (Lookup.VertexCount - Connected) * Connected;

    // On the first day of Christmas, my true love gave to me: dynamic memory.
    free(Visited);
    FreeVertexQueue(&Queue);
    free(Dist);
    free(Prev);
    free(Frequency);
    free(Adj);
    free(Edges);
    FreeVertexLookup(&Lookup);

    return Result;
}

AOC_SOLVER(Part2)
{
    AOC_UNUSED(Input);
    return -1;
}

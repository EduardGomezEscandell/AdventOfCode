#include "day15.h"

#include <stdio.h>
#include <stdlib.h>

#include "common/file_utils.h"

DEFINE_TEST(1, 40)
DEFINE_TEST(2, 315)

#define RIGHT 0
#define DOWN 1
#define LEFT 2
#define UP 3

void PrintNode(Node * node)
{
    printf("Node #%ld\n", node->id);
    printf(" - Danger: %d\n", node->danger);
    printf(" - Neigh:");
    for(size_t i=0; i<4; ++i) {
        if (node->neighbours[i]) printf(" %ld", node->neighbours[i]->id);
        else                     printf(" NULL");
    }
    printf("\n");
    printf(" - Visited: %d\n", node->visited);
    printf(" - Distance: %d\n", node->distance);
    printf("\n");
}

int CompareDistances(const Node * const A, const Node * const B)
{
    // Negative distances
    if(A->distance < 0 && B->distance < 0) return 0;
    if(A->distance < 0) return 1;
    if(B->distance < 0) return -1;

    // Unvisited
    if(A->visited && B->visited) return 0;
    if(A->visited) return 1;
    if(B->visited) return -1;

    // Real comparison
    if(A->distance > B->distance) return 1;
    if(A->distance < B->distance) return -1;
    return 0;
}

Node ** MinEntry(Node ** begin, Node ** end)
{
    Node ** min = begin;
    for(++begin; begin != end; ++begin)
    {
        if(CompareDistances(*min, *begin) == 1) min = begin;
    }
    return min;
}

size_t GetId(const size_t row, const size_t col, const size_t ncols)
{
    return row*ncols + col;
}

Graph CreateGraph(Matrix * data, const size_t row_folds, const size_t col_folds)
{
    Graph g;
    NEW_VECTOR(g);

    const size_t total_cols = data->ncols * row_folds;
    const size_t total_rows = data->ncols * col_folds;
    
    size_t id_ = 0;
    
    for(size_t row=0; row<total_rows; ++row) {
        for(size_t col=0; col<total_cols; ++col)
        {
            const size_t i = row % data->nrows;
            const size_t j = col % data->ncols;
            const size_t n_folds = row / data->nrows + col / data->ncols;

            Node new_node;
            new_node.danger = (data->data[i][j] + n_folds - 1) % 9 + 1;
            new_node.id = id_++;
            
            PUSH(g, new_node);
        }
    }

    id_ = 0;
    for(size_t row=0; row<total_rows; ++row) {
        for(size_t col=0; col<total_cols; ++col)
        {
            Node * node = g.begin + (id_++);
            node->neighbours[UP]    = (row   != 0)          ? g.begin + GetId(row-1, col, total_cols) : NULL;
            node->neighbours[DOWN]  = (row+1 != total_rows) ? g.begin + GetId(row+1, col, total_cols) : NULL;
            node->neighbours[LEFT]  = (col   != 0)          ? g.begin + GetId(row, col-1, total_cols) : NULL;
            node->neighbours[RIGHT] = (col+1 != total_cols) ? g.begin + GetId(row, col+1, total_cols) : NULL;
        }
    }

    return g;
}

long long int Distance(const Node * const A, const Node * const B)
{
    if(!A || !B)
    {
        fprintf(stderr, "Computing distance to a NULL pointer!");
        return -1;
    }
    return B->danger;
}

void Dijkstra(Node * begin, Node * end, Node * start, Node * target)
{
    Queue Q;
    NEW_VECTOR(Q);
    const size_t sz = end-begin;
    RESERVE(Q, sz);

    for(Node * it = begin; it != end; ++it)
    {
        it->distance = -1;
        it->prev = NULL;
        it->visited = false;

        PUSH(Q, it);
    }
    start->distance = 0;

    Node ** visited_end = Q.begin;

    while(visited_end != Q.end)
    {
        Node ** it = MinEntry(visited_end, Q.end);
        
        /*Popping it from queue*/
        {
            Node *tmp = *it;
            *it = *visited_end;
            *visited_end = tmp;
            it = visited_end;
            ++visited_end;
        }

        Node * u = *it;
        u->visited = true;

        if(u->id == target->id) {
            CLEAR(Q);
            return;
        }

        for(size_t i=0; i<4; ++i)
        {
            Node * neigh = u->neighbours[i];
            if(!neigh || neigh->visited) continue;

            long long int alt = u->distance + Distance(u, neigh);

            if(neigh->distance == -1 || alt < neigh->distance)
            {
                neigh->distance = alt;
                neigh->prev = u;
            }
        }
    }

    CLEAR(Q);

    printf("DIJKSTRA: Failed to find a shortest path\n");

}

solution_t Solve(const bool is_test, const size_t row_folds, const size_t col_folds)
{
    FILE * file = GetFile(is_test, 15);

    Matrix data = ReadMatrix(file);
    Graph graph = CreateGraph(&data, row_folds, col_folds);
    ClearMatrix(&data);

    Dijkstra(graph.begin, graph.end, graph.begin, graph.end-1);

    Node * node = graph.end-1;
    long long int total_distance = node->distance;

    CLEAR(graph);
    return total_distance;
}

solution_t SolvePart1(const bool is_test)
{
    return Solve(is_test, 1, 1);
}

solution_t SolvePart2(const bool is_test)
{
    return Solve(is_test, 5, 5);
}

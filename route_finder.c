/*
 * City Navigation / Shortest Route Finder
 * Mumbai - Navi Mumbai - Panvel region
 *
 * Data structures: Graph (adjacency list using linked lists),
 *                  Min-Heap (array based), Queue (BFS), Stack (DFS)
 * Compile: gcc route_finder.c -o route_finder
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_NODES 50
#define NAME_LEN  30
#define HEAP_MAX  2000
#define INF       INT_MAX

/* ---------- Graph: adjacency list ---------- */
typedef struct Edge {
    int to;
    int km;
    struct Edge *next;
} Edge;

char names[MAX_NODES][NAME_LEN];
Edge *adj[MAX_NODES];
int n = 0;   /* number of locations */

int addLocation(const char *name) {
    if (n >= MAX_NODES) {
        printf("Location limit reached.\n");
        return -1;
    }
    strncpy(names[n], name, NAME_LEN - 1);
    names[n][NAME_LEN - 1] = '\0';
    adj[n] = NULL;
    return n++;
}

void addOneWay(int u, int v, int km) {
    Edge *e = (Edge *)malloc(sizeof(Edge));
    e->to = v;
    e->km = km;
    e->next = adj[u];
    adj[u] = e;
}

void addRoad(int u, int v, int km) {
    addOneWay(u, v, km);
    addOneWay(v, u, km);   /* two-way road */
}

/* ---------- Min-Heap (for Dijkstra) ---------- */
typedef struct {
    int dist;
    int node;
} HeapItem;

HeapItem heap[HEAP_MAX];
int heapSize = 0;

void heapSwap(int a, int b) {
    HeapItem t = heap[a];
    heap[a] = heap[b];
    heap[b] = t;
}

void heapPush(int dist, int node) {
    int i = heapSize++;
    heap[i].dist = dist;
    heap[i].node = node;
    while (i > 0 && heap[(i - 1) / 2].dist > heap[i].dist) {
        heapSwap(i, (i - 1) / 2);
        i = (i - 1) / 2;
    }
}

HeapItem heapPop(void) {
    HeapItem top = heap[0];
    heap[0] = heap[--heapSize];
    int i = 0;
    while (1) {
        int l = 2 * i + 1, r = 2 * i + 2, small = i;
        if (l < heapSize && heap[l].dist < heap[small].dist) small = l;
        if (r < heapSize && heap[r].dist < heap[small].dist) small = r;
        if (small == i) break;
        heapSwap(i, small);
        i = small;
    }
    return top;
}

/* ---------- Queue (for BFS) ---------- */
int queueArr[MAX_NODES];
int qFront = 0, qRear = 0;

void enqueue(int x) { queueArr[qRear++] = x; }
int  dequeue(void)  { return queueArr[qFront++]; }
int  queueEmpty(void) { return qFront == qRear; }

/* ---------- Stack (for DFS) ---------- */
int stackArr[HEAP_MAX];
int top = -1;

void push(int x) { stackArr[++top] = x; }
int  pop(void)   { return stackArr[top--]; }
int  stackEmpty(void) { return top == -1; }

/* ---------- Helpers ---------- */
void printPath(int parent[], int src, int dest) {
    int path[MAX_NODES], len = 0, v;
    for (v = dest; v != -1; v = parent[v])
        path[len++] = v;

    printf("\nRoute: ");
    for (int i = len - 1; i >= 0; i--) {
        printf("%s", names[path[i]]);
        if (i > 0) printf(" -> ");
    }
    printf("\nStops : %d road segment(s)\n", len - 1);
    (void)src;
}

void showLocations(void) {
    printf("\n--- Locations ---\n");
    for (int i = 0; i < n; i++)
        printf("%2d. %s\n", i + 1, names[i]);
}

void showMap(void) {
    printf("\n===== CITY MAP (Adjacency List) =====\n");
    for (int i = 0; i < n; i++) {
        printf("%2d. %-12s -> ", i + 1, names[i]);
        for (Edge *e = adj[i]; e != NULL; e = e->next)
            printf("%s(%dkm)  ", names[e->to], e->km);
        printf("\n");
    }
}

/* ---------- Dijkstra: shortest distance ---------- */
void shortestPath(int src, int dest) {
    int dist[MAX_NODES], parent[MAX_NODES];
    for (int i = 0; i < n; i++) {
        dist[i] = INF;
        parent[i] = -1;
    }
    heapSize = 0;
    dist[src] = 0;
    heapPush(0, src);

    while (heapSize > 0) {
        HeapItem cur = heapPop();
        int u = cur.node;
        if (cur.dist > dist[u]) continue;
        if (u == dest) break;
        for (Edge *e = adj[u]; e != NULL; e = e->next) {
            if (dist[u] + e->km < dist[e->to]) {
                dist[e->to] = dist[u] + e->km;
                parent[e->to] = u;
                heapPush(dist[e->to], e->to);
            }
        }
    }

    if (dist[dest] == INF) {
        printf("\nNo route found.\n");
        return;
    }
    printPath(parent, src, dest);
    printf("Total distance : %d km\n", dist[dest]);
    printf("Approx. time   : %d minutes (at 30 km/h)\n", dist[dest] * 60 / 30);
}

/* ---------- BFS: fewest stops ---------- */
void fewestStops(int src, int dest) {
    int parent[MAX_NODES], seen[MAX_NODES];
    for (int i = 0; i < n; i++) {
        parent[i] = -1;
        seen[i] = 0;
    }
    qFront = qRear = 0;
    enqueue(src);
    seen[src] = 1;

    while (!queueEmpty()) {
        int u = dequeue();
        if (u == dest) break;
        for (Edge *e = adj[u]; e != NULL; e = e->next) {
            if (!seen[e->to]) {
                seen[e->to] = 1;
                parent[e->to] = u;
                enqueue(e->to);
            }
        }
    }

    if (!seen[dest]) {
        printf("\nNo route found.\n");
        return;
    }
    printPath(parent, src, dest);
}

/* ---------- DFS: reachable places ---------- */
void reachable(int src) {
    int seen[MAX_NODES] = {0}, count = 0;
    top = -1;
    push(src);
    printf("\nPlaces reachable from %s (DFS order):\n", names[src]);

    while (!stackEmpty()) {
        int u = pop();
        if (seen[u]) continue;
        seen[u] = 1;
        printf("  %d. %s\n", ++count, names[u]);
        for (Edge *e = adj[u]; e != NULL; e = e->next)
            if (!seen[e->to]) push(e->to);
    }
}

/* ---------- Input helper ---------- */
int readLocation(const char *prompt) {
    int x;
    do {
        printf("%s (1-%d): ", prompt, n);
        if (scanf("%d", &x) != 1) {
            while (getchar() != '\n');
            x = 0;
        }
    } while (x < 1 || x > n);
    return x - 1;
}

/* ---------- Default map (distances are approximate road km) ---------- */
void loadDefaultMap(void) {
    const char *places[] = {
        "Panvel", "Kalamboli", "Kamothe", "Kharghar", "Taloja", "Belapur",
        "Seawoods", "Nerul", "Juinagar", "Sanpada", "Vashi", "Turbhe",
        "Ghansoli", "Airoli", "Thane", "Mankhurd", "Chembur", "Dadar",
        "Mumbai CST", "Bandra"
    };
    for (int i = 0; i < 20; i++) addLocation(places[i]);

    addRoad(0, 2, 5);     /* Panvel - Kamothe */
    addRoad(0, 1, 6);     /* Panvel - Kalamboli */
    addRoad(0, 4, 8);     /* Panvel - Taloja */
    addRoad(1, 2, 4);     /* Kalamboli - Kamothe */
    addRoad(2, 3, 4);     /* Kamothe - Kharghar */
    addRoad(3, 4, 5);     /* Kharghar - Taloja */
    addRoad(3, 5, 7);     /* Kharghar - Belapur */
    addRoad(5, 6, 3);     /* Belapur - Seawoods */
    addRoad(6, 7, 2);     /* Seawoods - Nerul */
    addRoad(5, 7, 4);     /* Belapur - Nerul */
    addRoad(7, 8, 3);     /* Nerul - Juinagar */
    addRoad(8, 9, 3);     /* Juinagar - Sanpada */
    addRoad(9, 10, 4);    /* Sanpada - Vashi */
    addRoad(9, 11, 3);    /* Sanpada - Turbhe */
    addRoad(10, 11, 3);   /* Vashi - Turbhe */
    addRoad(11, 12, 6);   /* Turbhe - Ghansoli */
    addRoad(12, 13, 4);   /* Ghansoli - Airoli */
    addRoad(13, 14, 8);   /* Airoli - Thane */
    addRoad(10, 15, 6);   /* Vashi - Mankhurd */
    addRoad(15, 16, 5);   /* Mankhurd - Chembur */
    addRoad(16, 17, 10);  /* Chembur - Dadar */
    addRoad(17, 18, 9);   /* Dadar - Mumbai CST */
    addRoad(17, 19, 8);   /* Dadar - Bandra */
    addRoad(16, 18, 16);  /* Chembur - CST (Eastern Freeway) */
    addRoad(14, 17, 22);  /* Thane - Dadar */
}

int main(void) {
    int choice;
    loadDefaultMap();

    do {
        printf("\n=========================================\n");
        printf("   CITY NAVIGATION / SHORTEST ROUTE FINDER\n");
        printf("=========================================\n");
        printf("1. Show all locations\n");
        printf("2. Show city map (adjacency list)\n");
        printf("3. Shortest route by distance (Dijkstra)\n");
        printf("4. Route with fewest stops (BFS)\n");
        printf("5. Places reachable from a location (DFS)\n");
        printf("6. Add a new location\n");
        printf("7. Add a new road\n");
        printf("0. Exit\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            choice = -1;
        }

        switch (choice) {
        case 1:
            showLocations();
            break;
        case 2:
            showMap();
            break;
        case 3: {
            showLocations();
            int s = readLocation("Source");
            int d = readLocation("Destination");
            shortestPath(s, d);
            break;
        }
        case 4: {
            showLocations();
            int s = readLocation("Source");
            int d = readLocation("Destination");
            fewestStops(s, d);
            break;
        }
        case 5:
            showLocations();
            reachable(readLocation("Start"));
            break;
        case 6: {
            char name[NAME_LEN];
            printf("New location name: ");
            scanf(" %29[^\n]", name);
            if (addLocation(name) != -1)
                printf("Added %s.\n", name);
            break;
        }
        case 7: {
            int km;
            showLocations();
            int a = readLocation("From");
            int b = readLocation("To");
            printf("Distance in km: ");
            scanf("%d", &km);
            addRoad(a, b, km);
            printf("Road added.\n");
            break;
        }
        case 0:
            printf("Thank you!\n");
            break;
        default:
            printf("Invalid choice.\n");
        }
    } while (choice != 0);

    return 0;
}
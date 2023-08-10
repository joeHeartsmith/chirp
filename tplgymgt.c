#include "pddd.h"
#include "stdcntrl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>


char charlib[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
char separator = '-';

int seeded = 0;
int stale_convergence = 0;

static void startseed(void) { /*@-unrecog@*/
    struct timeval cur_time;
    gettimeofday(&cur_time, NULL); /*@+unrecog@ @-usedef -compdef@*/
    srand(cur_time.tv_usec);
    seeded = 1;
} /*@+unrecog@*/

static idstr genID(int ID_TYPE) {
    char* TMP_ID = (char *) calloc(IDSTRLEN, sizeof(char));

    if (ID_TYPE == ID_NULLID) { /*@-null@*/
        (void) snprintf(TMP_ID, IDSTRLEN, "%s", NULLID);
        return TMP_ID;
    } /*@+null@*/

    if (ID_TYPE != ID_NULLID && ID_TYPE != ID_NODE && ID_TYPE != ID_NLINK && ID_TYPE != ID_ADJ) {
        ID_TYPE = ID_OTHER;
    }

    /*@-boolops@*/
	if (!seeded || stale_convergence) {
		startseed();
	} /*@+boolops@*/

	if (TMP_ID != NULL) {
        switch (ID_TYPE) {
            case ID_NODE:
                TMP_ID[0] = 'n';
                break;
            case ID_NLINK:
                TMP_ID[0] = 'l';
                break;
            case ID_ADJ:
                TMP_ID[0] = 'a';
                break;
            default:
                TMP_ID[0] = 'x';
                break;
        }

        TMP_ID[1] = separator;

        int i = 0;
        for(i = 2; i < (IDSTRLEN - 1); i++) {
            if (i != (2 + 8 + 0) && i != (2 + 16 + 1) && i != (2 + 24 + 2) && i != (2 + 28 + 3) && i != (2 + 30 + 4)) {
                TMP_ID[i] = charlib[rand() % sizeof(charlib)];
            }
            else {
                TMP_ID[i] = separator;
            }
        }

        TMP_ID[IDSTRLEN - 1] = '\0';
        return TMP_ID;
    }
    else { /*@-nullret@*/
        return NULL;
    } /*@+nullret@*/
}

node* create_node(int NODE_KEY) {
    node *TMP_NOD = (node *) malloc(sizeof(node));
    if (TMP_NOD != NULL) {
        memset(TMP_NOD, 0, sizeof(node));
        TMP_NOD->KEY = NODE_KEY;
        TMP_NOD->ROOT_METRIC = BAD_ROOT_METRIC;
        TMP_NOD->IS_ROOT = 0;
        /*@-mustfreeonly -nullret@*/
        TMP_NOD->NEXT = NULL;
        TMP_NOD->ID = genID(ID_NODE); /*@+mustfreeonly@*/
        return TMP_NOD; /*@+nullret@*/
    }
    else {
        log_pr(LOG_CRIT, "Unable to allocate memory. Exiting.");
        exit(EXIT_FAILURE);
    } /*@-nullret -unreachable@*/
    return NULL; /* wtf */
} /*@+nullret +unreachable@*/

void destroy_nodechain(/*@null@*/ node *node_lib) {
/*@-branchstate -temptrans@*/
    if (node_lib != NULL) {
        node* tmp = NULL;
        for(tmp = node_lib; node_lib != NULL; node_lib = tmp) {
            tmp = node_lib->NEXT;
            free(node_lib->ID);
            free(node_lib);
        }
    }
} /*@-branchstate -temptrans@*/


nlink* create_nlink(int NLINK_KEY) {
    nlink *TMP_NLNK = (nlink *) malloc(sizeof(nlink));
    if (TMP_NLNK != NULL) {
        memset(TMP_NLNK, 0, sizeof(nlink));
        TMP_NLNK->KEY = NLINK_KEY;
        TMP_NLNK->PRI = DEFAULT_PRIORITY;
        /*@-mustfreeonly -nullret@*/
        TMP_NLNK->S_NODE = NULL;
        TMP_NLNK->D_NODE = NULL;
        TMP_NLNK->ID = genID(ID_NLINK); /*@+mustfreeonly@*/
        return TMP_NLNK; /*@+nullret@*/
    }
    else {
        log_pr(LOG_CRIT, "Unable to allocate memory. Exiting.");
        exit(EXIT_FAILURE);
    } /*@-nullret -unreachable@*/
    return NULL; /* wtf */
} /*@+nullret +unreachable@*/


void destroy_nlinkchain(/*@null@*/ nlink *nlink_lib) {
/*@-branchstate -temptrans@*/
    if (nlink_lib != NULL) {
        nlink* tmp = NULL;
        for(tmp = nlink_lib; nlink_lib != NULL; nlink_lib = tmp) {
            tmp = nlink_lib->NEXT;
            free(nlink_lib->ID);
            free(nlink_lib);
        }
    }
} /*@-branchstate -temptrans@*/

adj* create_adj(int ADJ_KEY) {
    adj *TMP_ADJ = (adj *) malloc(sizeof(adj));
    if (TMP_ADJ != NULL) {
        memset(TMP_ADJ, 0, sizeof(adj));
        TMP_ADJ->KEY = ADJ_KEY;
        /*@-mustfreeonly -nullret@*/
        TMP_ADJ->S_NODE = NULL;
        TMP_ADJ->D_NODE = NULL;
        TMP_ADJ->VIA = NULL;
        TMP_ADJ->ID = genID(ID_ADJ); /*@+mustfreeonly@*/
        return TMP_ADJ; /*@+nullret@*/
    }
    else {
        log_pr(LOG_CRIT, "Unable to allocate memory. Exiting.");
        exit(EXIT_FAILURE);
    } /*@-nullret -unreachable@*/
    return NULL; /* wtf */
} /*@+nullret +unreachable@*/


void destroy_adjchain(/*@null@*/ adj *adj_lib) {
/*@-branchstate -temptrans@*/
    if (adj_lib != NULL) {
        adj* tmp = NULL;
        for(tmp = adj_lib; adj_lib != NULL; adj_lib = tmp) {
            tmp = adj_lib->NEXT;
            free(adj_lib->ID);
            free(adj_lib);
        }
    }
} /*@-branchstate -temptrans@*/

node* find_node_by_key(node* node_lib, int NODE_KEY) {
/*@-temptrans@*/
    if (NODE_KEY == INVALID_KEY) {
        return NULL;
    }
    while (node_lib != NULL) {
        if (node_lib->KEY == NODE_KEY) {
            return node_lib;
        }
        node_lib = node_lib->NEXT;
    }
    return NULL;
} /*@+temptrans@*/

node* find_node_by_id(node* node_lib, idstr NODE_ID) {
/*@-temptrans@*/
    while (node_lib != NULL) {
        if (strncmp(node_lib->ID, NODE_ID, IDSTRLEN) == 0) {
            return node_lib;
        }
        node_lib = node_lib->NEXT;
    }
    return NULL;
} /*@+temptrans@*/

node* find_root_node(node* node_lib) {
/*@-temptrans@*/
    while (node_lib != NULL) {
        if (node_lib->IS_ROOT == 1) {
            return node_lib;
        }
        node_lib = node_lib->NEXT;
    }
    return NULL;
} /*@+temptrans@*/

nlink* find_nlink_by_key(nlink* nlink_lib, int NLINK_KEY) {
/*@-temptrans@*/
    while (nlink_lib != NULL) {
        if (nlink_lib->KEY == NLINK_KEY) {
            return nlink_lib;
        }
        nlink_lib = nlink_lib->NEXT;
    }
    return NULL;
} /*@+temptrans@*/

nlink* find_nlink_by_id(nlink* nlink_lib, idstr NLINK_ID) {
/*@-temptrans@*/
    while (nlink_lib != NULL) {
        if (strncmp(nlink_lib->ID, NLINK_ID, IDSTRLEN) == 0) {
            return nlink_lib;
        }
        nlink_lib = nlink_lib->NEXT;
    }
    return NULL;
} /*@+temptrans@*/

adj* find_adj_by_key(adj* adj_lib, int NLINK_KEY) {
/*@-temptrans@*/
    while (adj_lib != NULL) {
        if (adj_lib->KEY == NLINK_KEY) {
            return adj_lib;
        }
        adj_lib = adj_lib->NEXT;
    }
    return NULL;
} /*@+temptrans@*/

adj* find_adj_by_id(adj* adj_lib, idstr NLINK_ID) {
/*@-temptrans@*/
    while (adj_lib != NULL) {
        if (strncmp(adj_lib->ID, NLINK_ID, IDSTRLEN) == 0) {
            return adj_lib;
        }
        adj_lib = adj_lib->NEXT;
    }
    return NULL;
} /*@+temptrans@*/

/*@null@*/ adj* find_adjs_by_snode(/*@null@*/ adj* adj_lib, /*@null@*/ node *src) {
    /*@-branchstate -compmempass@*/
    if (adj_lib != NULL && src != NULL) {
        static int adj_count = 0;
        adj *local_adj_lib = create_adj(adj_count);
        adj *local_adj_ptr = local_adj_lib;
        adj *adj_lib_ptr = adj_lib;
        strncpy(local_adj_ptr->ID, NULLID, IDSTRLEN);

        while (adj_lib_ptr != NULL) {
            if (adj_lib_ptr->S_NODE == src) {
                local_adj_ptr->NEXT = create_adj(++adj_count);
                local_adj_ptr = local_adj_ptr->NEXT;
                strncpy(local_adj_ptr->ID, adj_lib_ptr->ID, IDSTRLEN);
                if (adj_lib_ptr->S_NODE != NULL) {
                    local_adj_ptr->S_NODE = adj_lib_ptr->S_NODE;
                }
                else {
                    local_adj_ptr->S_NODE = NULL;
                }
                if (adj_lib_ptr->D_NODE != NULL) {
                    local_adj_ptr->D_NODE = adj_lib_ptr->D_NODE;
                }
                else {
                    local_adj_ptr->D_NODE = NULL;
                }
                if (adj_lib_ptr->VIA != NULL) {
                    local_adj_ptr->VIA = adj_lib_ptr->VIA;
                }
                else {
                    local_adj_ptr->VIA = NULL;
                }
            }
            adj_lib_ptr = adj_lib_ptr->NEXT;
        }

        if (local_adj_lib->NEXT == NULL && local_adj_lib->S_NODE == NULL) {
            return NULL;
        }
        else {
        return local_adj_lib;
        }
        /*@+branchstate@*/
        /*@notreached@*/ return NULL;
    }
    else {
        return NULL;
    }
} /*@+compmempass@*/

/*@null@*/ adj* find_adjs_by_dnode(/*@null@*/ adj* adj_lib, /*@null@*/ node *dst) {
    /*@-branchstate -compmempass@*/
    if (adj_lib != NULL && dst != NULL) {
        static int adj_count = 0;
        adj *local_adj_lib = create_adj(adj_count);
        adj *local_adj_ptr = local_adj_lib;
        adj *adj_lib_ptr = adj_lib;
        strncpy(local_adj_ptr->ID, NULLID, IDSTRLEN);
        while (adj_lib_ptr != NULL) {
            if (adj_lib_ptr->D_NODE == dst) {
                local_adj_ptr->NEXT = create_adj(++adj_count);
                local_adj_ptr = local_adj_ptr->NEXT;
                strncpy(local_adj_ptr->ID, adj_lib_ptr->ID, IDSTRLEN);
                if (adj_lib_ptr->S_NODE != NULL) {
                    local_adj_ptr->S_NODE = adj_lib_ptr->S_NODE;
                }
                else {
                    local_adj_ptr->S_NODE = NULL;
                }
                if (adj_lib_ptr->D_NODE != NULL) {
                    local_adj_ptr->D_NODE = adj_lib_ptr->D_NODE;
                }
                else {
                    local_adj_ptr->D_NODE = NULL;
                }
                if (adj_lib_ptr->VIA != NULL) {
                    local_adj_ptr->VIA = adj_lib_ptr->VIA;
                }
                else {
                    local_adj_ptr->VIA = NULL;
                }
            }
            adj_lib_ptr = adj_lib_ptr->NEXT;
        }
        if (local_adj_lib->NEXT == NULL && local_adj_lib->D_NODE == NULL) {
            return NULL;
        }
        else {
        return local_adj_lib;
        }
        /*@+branchstate@*/
        /*@notreached@*/ return NULL;
    }
    else {
        return NULL;
    }
} /*@+compmempass@*/

int find_lower_idstr(idstr entity_a, idstr entity_b) {
    if (entity_a != NULL && entity_b != NULL) {
        if (strncmp((char*)entity_a, (char*)entity_b, IDSTRLEN) < 0)
            return ENTITY_A;
        if (strncmp((char*)entity_a, (char*)entity_b, IDSTRLEN) > 0)
            return ENTITY_B;
        if (strncmp((char*)entity_a, (char*)entity_b, IDSTRLEN) == 0)
            return SAMEIDSTR;
    }
    else {
        return 0;
    }
    return 0;
}

int find_higher_idstr(idstr entity_a, idstr entity_b) {
    if (entity_a != NULL && entity_b != NULL) {
        if (strncmp((char*)entity_a, (char*)entity_b, IDSTRLEN) > 0)
            return ENTITY_A;
        if (strncmp((char*)entity_a, (char*)entity_b, IDSTRLEN) < 0)
            return ENTITY_B;
        if (strncmp((char*)entity_a, (char*)entity_b, IDSTRLEN) == 0)
            return SAMEIDSTR;
    }
    else {
        return 0;
    }
    return 0;
}

void assign_adj(adj* adj_lib, node* src, node* dst, nlink* via) {
    if (adj_lib != NULL) { /*@-mustfreeonly -temptrans -branchstate@*/
        adj_lib->S_NODE = src;
        adj_lib->D_NODE = dst;
        adj_lib->VIA = via;
    } /*@+mustfreeonly +temptrans +branchstate@*/
}

int count_node_keys(node* node_lib) {
    int node_count = 0;
    if (node_lib != NULL) {
        while (node_lib != NULL) {
                node_count++;
            node_lib = node_lib->NEXT;
        }
    }
    return node_count;
}

int count_adj_keys(adj* adj_lib) {
    int adj_count = 0;
    if (adj_lib != NULL) {
        while (adj_lib != NULL) {
                adj_count++;
            adj_lib = adj_lib->NEXT;
        }
    }
    return --adj_count;
}

void cleanup_tplgy_database(topology_db db) {
    destroy_nodechain(db.node_lib);
    destroy_nlinkchain(db.nlink_lib);
    destroy_adjchain(db.adj_lib);
}


void set_root_node_by_key(topology_db db, int key) { /*@-nullderef@*/
    int i;
    if (find_node_by_key(db.node_lib, key) != NULL) {
        for (i = 0; i < count_node_keys(db.node_lib); i++) {
            find_node_by_key(db.node_lib, i)->IS_ROOT = 0;
        }
        find_node_by_key(db.node_lib, key)->IS_ROOT = 1;
        log_pr(5, "root node assigned");
    }
    else {
        log_pr(4, "set_root_node_by_key called on topology_db with no key found");
    }
} /*@+nullderef@*/

void set_root_node_by_id(topology_db db, idstr id) { /*@-nullderef@*/
    int i;
    if (find_node_by_id(db.node_lib, id) != NULL) {
        for (i = 0; i < count_node_keys(db.node_lib); i++) {
            find_node_by_key(db.node_lib, i)->IS_ROOT = 0;
        }
        find_node_by_key(db.node_lib, find_node_by_id(db.node_lib, id)->KEY)->IS_ROOT = 1;
        log_pr(5, "root node assigned");
    }
    else {
        log_pr(4, "set_root_node_by_id called on topology_db with no id found");
    }
} /*@+nullderef@*/

void unset_root_node_by_key(topology_db db, int key) { /*@-nullderef@*/
    if (find_node_by_key(db.node_lib, key) != NULL) {
        find_node_by_key(db.node_lib, key)->IS_ROOT = 0;
        log_pr(5, "root node unassigned");
    }
    else {
        log_pr(4, "unset_root_node_by_key called on topology_db with no key found");
    }
} /*@+nullderef@*/


void unset_root_node_by_id(topology_db db, idstr id) { /*@-nullderef@*/
    if (find_node_by_id(db.node_lib, id) != NULL) {
        find_node_by_id(db.node_lib, id)->IS_ROOT = 0;
        log_pr(5, "root node unassigned");
    }
    else {
        log_pr(4, "unset_root_node_by_id called on topology_db with no id found");
    }
} /*@+nullderef@*/


/*@null@*/ node* find_nearest_neighbor(topology_db db, node* src) { /*@-nullderef -mustfreefresh -temptrans@*/
    adj *neighbor_adjs, *adj_ptr;
    if ((neighbor_adjs = find_adjs_by_snode(db.adj_lib, src))->NEXT != NULL) {
        adj_ptr = neighbor_adjs->NEXT;
    }
    else {
        log_pr(LOG_CRIT, "fed find_nearest_neighbor() with bad graph");
        return src;
    }
    int best_cost = WORST_PRI;
    int node_id = INITIAL_NODE;

    while (adj_ptr != NULL) {
        if (adj_ptr->VIA->PRI < best_cost) {
            best_cost = adj_ptr->VIA->PRI;
            node_id = adj_ptr->D_NODE->KEY;
        }
        if (adj_ptr->VIA->PRI == best_cost) {
            best_cost = adj_ptr->VIA->PRI;
            node_id = (find_lower_idstr(find_node_by_key(db.node_lib, adj_ptr->D_NODE->KEY)->ID, find_node_by_key(db.node_lib, node_id)->ID) == ENTITY_A ) ? adj_ptr->D_NODE->KEY : node_id;
        }
        adj_ptr = adj_ptr->NEXT;
    }
    destroy_adjchain(neighbor_adjs);
    return find_node_by_key(db.node_lib, node_id);
} /*@+nullderef +mustfreefresh +temptrans@*/

static int pathpage_counter = 0;
static int pathpages[MAXHOPS][MAXHOPS];
static int visited_nodes[MAXHOPS];
static int pathpage_terminators[MAXHOPS];
static int living_pathpages[MAXHOPS];
char pathids[MAXHOPS][MAXHOPS][IDSTRLEN];
int pathpagerestore[MAXHOPS];

static inline void reset_pathpage_counter(void) {
    pathpage_counter = 0;
}

static inline void reset_pathids(void) {
    int x, y;
    for (x = 0; x < MAXHOPS; x++) {
    for (y = 0; y < MAXHOPS; y++) {
        pathids[x][y][0] = '\0';
    }}
}

static inline void reset_pathpage_restore(void) {
    int i;
    for (i = 0; i < MAXHOPS; i++) {
        pathpagerestore[i] = INVALID_KEY;
    }
}

static inline void initialize_pathpages(void) {
    int x, y;
    for (x = 0; x < MAXHOPS; x++) {
    for (y = 0; y < MAXHOPS; y++) {
        pathpages[x][y] = INVALID_KEY;
    }}
    pathpage_counter = 0;
}

static inline void initialize_visited(void) {
    int i;
    for (i = 0; i < MAXHOPS; i++) {
        visited_nodes[i] = INVALID_KEY;
    }
}

static inline void initialize_pathpage_terminators(void) {
    int i;
    for (i = 0; i < MAXHOPS; i++) {
        pathpage_terminators[i] = INVALID_KEY;
    }
}

static inline void initialize_living_pathpages(void) {
    int i;
    for (i = 0; i < MAXHOPS; i++) {
        living_pathpages[i] = INVALID_KEY;
    }
}

static inline void init_all_pathcounters(void) {
    reset_pathpage_counter();
    initialize_pathpages();
    initialize_visited();
    initialize_pathpage_terminators();
    initialize_living_pathpages();
}

static inline void kill_pathpage(int pathpage_index) {
    pathpages[pathpage_index][PATH_STATUS] = DEAD_PATH_KEY;
}

static inline void activate_pathpage(int pathpage_index) {
    pathpages[pathpage_index][PATH_STATUS] = ACTIVE_PATH_KEY;
}

static inline void create_pathpage(void) {
    int offset = 0, next_avail_pathpage = 0;
    while (pathpages[next_avail_pathpage][H_OFFSET + 1] != INVALID_KEY && offset <= MAXHOPS) {
        next_avail_pathpage++;
    }
    activate_pathpage(next_avail_pathpage);
    pathpages[next_avail_pathpage][COUNT] = 0;
    pathpage_counter = next_avail_pathpage;
}

static inline void append_pathpage(int pathpage_index, int node_key) {
    int offset = H_OFFSET + 1;
    if (pathpages[pathpage_index][offset] != INVALID_KEY) {
        while (pathpages[pathpage_index][offset] != INVALID_KEY && offset <= MAXHOPS) {
            offset++;
        }
        pathpages[pathpage_index][offset] = node_key;
        ++pathpages[pathpage_index][COUNT];
    }
    else {
        pathpages[pathpage_index][H_OFFSET + 1] = node_key;
        ++pathpages[pathpage_index][COUNT];
    }
}

static inline void append_visited(int node_key) {
    int i, noinsert = 0, offset = 1;

    for (i = 1; visited_nodes[i] >= 0; i++) {
        if (visited_nodes[i] == node_key) {
            noinsert = 1;
        }
    }
    if (noinsert != 1) {
        if (visited_nodes[offset] == INVALID_KEY) {
            visited_nodes[offset] = node_key;
        }
        else {
            while (visited_nodes[offset] != INVALID_KEY && offset <= MAXHOPS) {
                ++offset;
            }
            visited_nodes[offset] = node_key;
        }
        visited_nodes[COUNT] = offset;
    }
}

static inline void remove_visited(int node_key) {
    int i, tmpi = 1, tmp[MAXHOPS];

    for (i = 0; i < MAXHOPS; i++) {
        tmp[i] = INVALID_KEY;
    }
    tmp[COUNT] = visited_nodes[COUNT];

    for (i = 1; visited_nodes[i] != INVALID_KEY; i++) {
        if (visited_nodes[i] != node_key) {
            tmp[tmpi] = visited_nodes[i];
            ++tmpi;
        }
        else {
            --tmp[COUNT];
        }
    }

    for (i = 0; i < MAXHOPS; i++) {
        visited_nodes[i] = tmp[i];
    }
    visited_nodes[COUNT] = tmp[COUNT];
}

static inline void append_terminators(int node_key) {
    int offset = 1;
    if (pathpage_terminators[offset] == INVALID_KEY) {
        pathpage_terminators[offset] = node_key;
    }
    else {
        while (pathpage_terminators[offset] != INVALID_KEY && offset <= MAXHOPS) {
            ++offset;
        }
        pathpage_terminators[offset] = node_key;
    }
    pathpage_terminators[COUNT] = offset - 1;
}

static inline void append_livingpath(int pathpage_key) {
    int offset = 1;
    if (living_pathpages[offset] == INVALID_KEY) {
        living_pathpages[offset] = pathpage_key;
    }
    else {
        while (living_pathpages[offset] != INVALID_KEY && offset <= MAXHOPS) {
            ++offset;
        }
        living_pathpages[offset] = pathpage_key;
    }
    living_pathpages[COUNT] = offset;
}

static inline int node_is_visited(int node_key) {
    int i, visited = 0;
    for (i = 1; i < visited_nodes[COUNT] + 1; i++) {
        if (visited_nodes[i] == node_key) {
            visited = 1;
        }
    }
    if (node_key < 0) {visited = 0;}
    return visited;
}

static inline int all_nodes_are_visited(node** node_lib) {
    return visited_nodes[COUNT] != count_node_keys(*node_lib) ? 0 : 1;
}

static inline void find_living_pathpages(void) {
    int i;
    initialize_living_pathpages();
    for (i = 0; i < MAXHOPS; i++) {
        if (pathpages[i][PATH_STATUS] == ACTIVE_PATH_KEY) {
            append_livingpath(i);
        }
    }
}

static inline void copy_pathpage(int pathpage_index) {
    int i, next_avail_pathpage = 0;
    while (pathpages[next_avail_pathpage][H_OFFSET + 1] != INVALID_KEY) {
        ++next_avail_pathpage;
    }
    pathpages[next_avail_pathpage][COUNT] = 0;
    for (i = H_OFFSET + 1; i < MAXHOPS; i++) {
        pathpages[next_avail_pathpage][i] = pathpages[pathpage_index][i];
        if (pathpages[pathpage_index][i] >= 0) {
            ++pathpages[next_avail_pathpage][COUNT];
        }
    }
    pathpages[next_avail_pathpage][BRANCH_FROM] = pathpage_index;
    kill_pathpage(pathpage_index);
    activate_pathpage(next_avail_pathpage);
    ++pathpage_counter;
}

// static inline void clone_pathpage(int pathpage_index) {
//     copy_pathpage(pathpage_index);
//     activate_pathpage(pathpage_index);
// }
// 
// static int cmpintp(const void *i1, const void *i2) {
//     return i1 < i2 ? 1 : 0;
// }
/*
static inline void sort_pathpage_terminators(void) {
    qsort(&pathpage_terminators[1], MAXHOPS, sizeof(int), cmpintp);
}*/

static inline void find_pathpage_terminators(void) {
    int i, offset = H_OFFSET + 1;
    initialize_pathpage_terminators();
    for (i = 0; i < MAXHOPS; i++) {
        if (pathpages[i][PATH_STATUS] != DEAD_PATH_KEY) {
            while (pathpages[i][offset] != INVALID_KEY && offset < MAXHOPS) {
                offset++;
            }
            if (pathpages[i][offset] != DEAD_PATH_KEY) {
                append_terminators(pathpages[i][offset - 1]);
            }
            offset = H_OFFSET + 1;
        }
    }
    //sort_pathpage_terminators();
}

static inline void find_unvisited_pathpage_terminators(void) {
    int i, t, v, found = 0, tmp[MAXHOPS];
    find_pathpage_terminators();
    for (i = 0; i < MAXHOPS; i++) {
        tmp[i] = INVALID_KEY;
    } i = 1;
    for (t = 1; t <= pathpage_terminators[COUNT]; t++) {
        for (v = 1; v <= visited_nodes[COUNT]; v++) {
            if (pathpage_terminators[t] == visited_nodes[v]) {
                found = 1;
            }
        }
        if (found != 1) {
            tmp[i] = pathpage_terminators[t];
            ++i;
        }
        else {
            found = 0;
        }
    }
    tmp[0] = i - 1;
    initialize_pathpage_terminators();
    for (i = 0; i < MAXHOPS; i++) {
        pathpage_terminators[i] = tmp[i];
    }
    //sort_pathpage_terminators();
}

static inline void dedup_terminators(void) {
    int i, j, p, found[MAXHOPS], out[MAXHOPS];
    for (i = 0; i < MAXHOPS; i++) {
        out[i] = -1;
        found[i] = 0;
    } i = 1;
    for (p = 1; p < MAXHOPS; p++) {
        ++found[pathpage_terminators[p]];
    }
    for (j = 0; j < MAXHOPS; j++) {
        if (found[j] > 0) {
            out[i] = j;
            i++;
        }
    }
    out[0] = i - 1;
    initialize_pathpage_terminators();
    for (i = 0; i < MAXHOPS; i++) {
        pathpage_terminators[i] = out[i];
    }
}

static inline int find_pathpage_by_terminator(int terminator) {
    int i;
    for (i = 0; i <= pathpage_counter; i++) {
        if (pathpages[i][BRANCH_FROM] != -1 && pathpages[i][COUNT] == terminator) {
            return i;
        }
    }
    return -1;
}

static inline int loop_check(int pathpage_index, int kill_flag) {
    int i, j, dflag = 0, ret = 0;
    for (i = H_OFFSET + 1; pathpages[pathpage_index][i] != INVALID_KEY; i++) {
            dflag = 0;
            for (j = H_OFFSET + 1; pathpages[pathpage_index][j] != INVALID_KEY; j++) {
                if (pathpages[pathpage_index][j] == pathpages[pathpage_index][i]) {
                    //printf(" FOUND %d @ %d\n", i, j);
                    ++dflag;
                }
            }
            if (dflag >= 2) {
                ret = 1;
                if (kill_flag == 1) {
                    kill_pathpage(pathpage_index);
                }
            }
        }
    return ret;
}

static inline void kill_loop_pages(int norevisit_flag) {
    /* REMINDER: norevisit_flag means terminators for pathpages w/loops get added back to the visited set */
    int i, j;
    for (i = 0; i <= pathpage_counter; i++) {
        if (loop_check(i, KILL_LOOP)) {
            if (norevisit_flag) {
                for (j = H_OFFSET + 1; pathpages[i][j] != INVALID_KEY; j++) {;}
                append_visited(pathpages[i][--j]);
            }
        }
    }
}

/*
 TODO: 1) this doesn't handle the case of two nodes being connected via 
          two, separate, equal links

       2) inline this function
*/
int calc_pathlen(topology_db db, int pathpage_index) { /*@-mustfreefresh -nullderef@*/
    int i = 1, dist = 0;
    adj *tmps = NULL, *tmpd = NULL;
    while (pathpages[pathpage_index][H_OFFSET + i + 1] != INVALID_KEY) {
        tmps = find_adjs_by_snode(db.adj_lib, find_node_by_key(db.node_lib, pathpages[pathpage_index][H_OFFSET + i]));
        if (tmps->NEXT != NULL) {
            tmpd = find_adjs_by_dnode(tmps, find_node_by_key(db.node_lib, pathpages[pathpage_index][H_OFFSET + i + 1]));
            if (tmpd->NEXT != NULL) {
                dist += tmpd->NEXT->VIA->PRI;
            }
            else {
                break; // BIG ERROR  (TODO: Populate this)
            }
            destroy_adjchain(tmps);
            destroy_adjchain(tmpd);
            tmps = NULL;
            tmpd = NULL;
        }
        else {
            break; // BIG ERROR  (TODO: Populate this)
        }
        ++i;
    }
    pathpages[pathpage_index][DISTANCE_METRIC] = dist;
    return dist;
} /*@+mustfreefresh +nullderef@*/

void calc_all_pathlens(topology_db db) {
    int i;
    for (i = 0; pathpages[i][COUNT] > 0; i++) {
        (void) calc_pathlen(db, i);
    }
}

// TODO add tons of error-handling to this function
int find_shortest_pathpage(topology_db db) {
    int i, j = H_OFFSET + 1, x = 0, y = 0, leasthops = MAXHOPS, shortest = MAXHOPS * WORST_PRI, resultpage = INVALID_KEY;
    int page[MAXHOPS];

    char* lowestID = (char *) calloc(IDSTRLEN, sizeof(char));
    if (lowestID != NULL) {
        (void) snprintf(lowestID, IDSTRLEN, "%s", TOPID);
    }
    else {
        return -1; // BIG ERROR
    }
    reset_pathids();

    for (i = 0; i < MAXHOPS; i++) {
        page[i] = INVALID_KEY;
    }
    page[COUNT] = 0;

    for (i = 0; pathpages[i][COUNT] > 0; i++) {
        if (pathpages[i][DISTANCE_METRIC] <= shortest && pathpages[i][PATH_STATUS] != DEAD_PATH_KEY) {
            shortest = pathpages[i][DISTANCE_METRIC];
        }
    }

    for (i = 0; pathpages[i][COUNT] > 0; i++) {
        if (pathpages[i][DISTANCE_METRIC] == shortest) {
            ++page[COUNT];
            page[page[COUNT]] = i;
        }
    }

    if (page[COUNT] < 1) {
        free(lowestID);
        return -1;  // BIG ERROR  TODO add details to this?
    }

    if (page[COUNT] == 1) {
        free(lowestID);
        return page[1];
    }

    for (i = 1; i < page[COUNT]; i++) {
        if (pathpages[page[i]][COUNT] < MAXHOPS) {
            leasthops = pathpages[page[i]][COUNT];
        }
    }

    reset_pathpage_restore();
    for (i = 1; page[i] > INVALID_KEY; i++) {
        if (pathpages[page[i]][COUNT] <= leasthops) {
            pathpagerestore[i - 1] = page[i];
            while (j < pathpages[page[i]][COUNT] + 4) {/*@-nullderef@*/
                if (find_node_by_key(db.node_lib, pathpages[page[i]][j])->ID != NULL) {
                    (void) snprintf(pathids[x][y], IDSTRLEN, "%s", find_node_by_key(db.node_lib, pathpages[page[i]][j])->ID);
                } /*@+nullderef@*/
                ++y;
                ++j;
            }
        }
        j = H_OFFSET + 1;
        ++x; 
        y = 0;
    }
    (void) snprintf(lowestID, IDSTRLEN, "%s", TOPID);

    for (x = 0; x < leasthops; x++) {
    for (y = 0; pathids[y][x][0] != '\0'; y++) {
        if (pathids[y][x][0] == 'n') {
            }
        if (x >= 1 && (find_lower_idstr(pathids[y][x], lowestID) == ENTITY_A || find_lower_idstr(pathids[y][x], lowestID) == SAMEIDSTR)) {
             (void) snprintf(lowestID, IDSTRLEN, "%s", pathids[y][x]);
             resultpage = y;
        }
    }
    }
    resultpage = pathpagerestore[resultpage];
    free(lowestID);
    return resultpage;
}



/******************************************************************************/
static inline void rundebug(void) {
    int i, j;
    //printf("\n\n##### DEBUG: pathpage_counter = %d\n", pathpage_counter);
    printf("\n\n               ##### DEBUG: #####################\n\n");

    printf("                visited_nodes: ");
    for (i = 1; visited_nodes[i] != -1; i++) {
        printf("%d, ", visited_nodes[i]);
    } printf("\n");
printf("\n");
    for (i = 0; i <= pathpage_counter; i++) {

        if (pathpages[i][BRANCH_FROM] == -1) { printf("                pathpage[%d] (BRANCH_FROM: X): ", i); }
        if (pathpages[i][BRANCH_FROM] != -1) { printf("                pathpage[%d] (BRANCH_FROM: %d): ", i, pathpages[i][BRANCH_FROM]); }


    for (j = H_OFFSET + 1; pathpages[i][j] > -1; j++) {
        //printf("pathpage[%d][%d] = %d", i, j, pathpages[i][j]);
        printf("%d", pathpages[i][j]);
        //if (pathpages[i][PATH_STATUS] == DEAD_PATH_KEY) {
        //    printf(" (DEAD) ");
        //} else {
            printf(", ");
        //}
    
    }
if (pathpages[i][PATH_STATUS] == DEAD_PATH_KEY) { printf("X"); } 
printf("\n"); }

find_unvisited_pathpage_terminators();
    printf("\n                living_pathpage_terminators: ");
    for (i = 1; i <= pathpage_terminators[COUNT]; i++) {
        printf("%d, ", pathpage_terminators[i]);
    }

//printf("\nfind_pathpage_by_terminator(3) = %d\n", find_pathpage_by_terminator(3));

    printf("\n               ##################################\n\n");
}
/******************************************************************************/



/*@-fixedformalarray@*/
int get_shortest_path(topology_db db, node* start, node* end, path (*pathinfo)[MAXHOPS]) {
    int pagetrack[MAXHOPS];
    int c, i, j, s, loop_counter = 0, step = 0, complete = 0, vcount, tcount, max_iterations = (count_node_keys(db.node_lib) * count_node_keys(db.node_lib));
        printf("**max_iterations = %d\n", max_iterations);
    init_all_pathcounters();
    if (pathinfo != NULL) {
        for (i = 0; i < MAXHOPS; i++) {
            (*pathinfo)[i] = INVALID_KEY;
        }
    }

    printf(" ** SEARCHING FROM (%d) to (%d)\n", start->KEY, end->KEY);


    // STEP 0: find_nearest_neighbor() on the starting node.
    printf(" ** NEAREST NEIGHBOR FOR (%d) is (%d)\n", start->KEY, find_nearest_neighbor(db, start)->KEY);
    if (find_nearest_neighbor(db, start)->KEY == end->KEY) {
        printf(" ** NEAREST NEIGHBOR WAS TERMINAL NODE\n");
        (*pathinfo)[COUNT] = 2;
        (*pathinfo)[1] = start->KEY;
        (*pathinfo)[2] = end -> KEY;
        return SUCCESS;     /* TODO: get better return macros */
    }

    // STEP 0: find_node_by_src() on the starting node and count the results; if NULL return NOGRAPH
    adj *probe_ptr, *probe = find_adjs_by_snode(db.adj_lib, start);
    probe_ptr = probe;
    printf(" ** FOUND %d NODES:\n", count_adj_keys(probe));

    if (count_adj_keys(probe) < 1) {
        return NOGRAPH;
    }

    // STEP 0: create_pathpage() for that many found nodes, and append the staring node to it (append the starting node to the new pathpages)
    //         append each node found above to the new pathpages
    //         add the starting node to the visited set
    for (i = 0; i < count_adj_keys(probe); i++) {
        probe_ptr = probe_ptr->NEXT;
        printf("     NODE (%d)\n", probe_ptr->D_NODE->KEY);

        create_pathpage();
        append_pathpage(pathpage_counter, start->KEY);
        append_pathpage(pathpage_counter, probe_ptr->D_NODE->KEY);
        append_visited(start->KEY);

        pagetrack[i] = pathpage_counter;

    }
    destroy_adjchain(probe);


//rundebug();


    step = 1;
    /* LOOP-PREP COMPLETE */

    while (!complete && loop_counter < max_iterations) {

        /************** STEP 1 **************/
        while (step == 1) {

            //rundebug();


            printf("\n ***** ENTERED STEP 1 *****\n\n");

            find_unvisited_pathpage_terminators();
            
            printf("  FOUND %d TERMINATORS\n", pathpage_terminators[COUNT]);
            for (c = 1; c <= pathpage_terminators[COUNT]; c++) {
                printf("      FOUND *UNVISITED* *TERMINATING* NODE[%d]: %d\n", c, pathpage_terminators[c]);
            } --c;
            printf("       (THAT WAS %d NODES...)\n", c);

            if (c < 1) {
                /* TODO: wat do??  ...see if terminators' neighbors aren't in the visited set */
            } 


            step = 2;
        }

rundebug();
getchar();
        /************** STEP 2 **************/
        while (step == 2) {
//find_unvisited_pathpage_terminators();
            s = 0;
            tcount = 0;

            printf("\n ***** ENTERED STEP 2 *****\n\n");
            printf(" *** RUNNING SEARCH ON %d NODES (NON-VISITED)\n\n", c);
            
            //for (i = 1; i <= pathpage_terminators[COUNT]; i++) {
            //    printf("TEMRINATOR: %d\n", pathpage_terminators[i]);
            //}


            for (i = 1; i <= pathpage_terminators[COUNT]; i++) {
                vcount = 0;

                printf("  ** SRC (%d)\n", pathpage_terminators[i]);
                //printf("  ** SRC (%d)\n", pathpage_terminators[i+1]);
                //printf("  ** SRC (%d)\n", pathpage_terminators[i+2]);

                
                probe = find_adjs_by_snode(db.adj_lib, find_node_by_key(db.node_lib, pathpage_terminators[i]));
                probe_ptr = probe;

                for (j = 0; j < count_adj_keys(probe); j++) {
                    probe_ptr = probe_ptr->NEXT;
                    if (!node_is_visited(probe_ptr->D_NODE->KEY)) {
                        printf("      DST (%d)\n", probe_ptr->D_NODE->KEY);
                        ++vcount;

                    } 
                }

                printf("    FOUND %d APPLICABLE NODES\n", vcount);
                append_visited(pathpage_terminators[i]);

                tcount += vcount;
                if (vcount < 1) {
                    /* TODO: add current node to the visited set. 
                             then check to see if this node is the destination. if not, kill the applicable pathpage */
                }
                else {
                    printf("    WILL NEED TO ADD %d TOTAL NEW PATHPAGES\n", vcount);
//                     for (i = 0; i < tcount; i++) {
//                         copy_pathpage(pathpage_counter);
//                     }
//                     pathpage_counter -= tcount;
                    probe_ptr = probe;



                        for (j = 0; j < count_adj_keys(probe); j++) {
                        probe_ptr = probe_ptr->NEXT;
                        if (!node_is_visited(probe_ptr->D_NODE->KEY)) {
                            ++s;

                            // if (d_node->key is adjacent to 
                            copy_pathpage(pagetrack[i - 1]);

                            //copy_pathpage(find_pathpage_by_terminator(pathpage_terminators[i]));

                            append_pathpage(pathpage_counter, probe_ptr->D_NODE->KEY);

                        

                          //printf("     current pathpage: %d\tcurrent terminator: %d\n", pathpage_counter, pagetrack[i - 1]);
            //printf("         SRC = %d   |   searched pathpage %d\n", pathpage_terminators[i], find_pathpage_by_terminator(pathpage_terminators[i]));
                              printf("      APPEND: (%d) from page %d to page %d\n", probe_ptr->D_NODE->KEY, pagetrack[i - 1], pathpage_counter);
                            // printf("      APPEND: (%d) from page %d to page %d\n", probe_ptr->D_NODE->KEY, find_pathpage_by_terminator(pathpage_terminators[i]), pathpage_counter);

                            

                            // end if

                        }

                        //++pathpage_counter;
                    } pagetrack[loop_counter] = pathpage_counter;

                }

//getc(stdin);
rundebug();

                destroy_adjchain(probe);
                printf("\n");
            }

            ++loop_counter;
            if (loop_counter >= max_iterations) {
                return EPATHFAIL;
            }

            step = 3;

//            rundebug(); exit(0);  /* TODO: get rid of this */

        }



        /************** STEP 3 **************/
        while (step == 3) {
            printf("\n ***** ENTERED STEP 3 *****\n\n");

            if (!all_nodes_are_visited(&db.node_lib)) {
                step = 1;
            }
            else {
                step = 4;
                complete = 1;
            }



            //getc(stdin);


        }



//rundebug();



    }







    if (all_nodes_are_visited(&db.node_lib)) {
        printf("YES - all nodes were visited\n");
    }

    int shortest = find_shortest_pathpage(db);
    printf(">>>>> SHORTEST PAGE = %d\n", shortest);


    (*pathinfo)[COUNT] = pathpages[shortest][COUNT];
    volatile int p;
    for (p = 1; p < MAXHOPS; p++) {
        (*pathinfo)[p] = pathpages[shortest][H_OFFSET + p];
    }


    printf("\n");
    return 1; /* TODO: get rid of this */
} /*@+fixedformalarray@*/


// void assign_root_metrics(topology_db db) {
// }




// void assign_root_metrics(topology_db db) { /*@-nullderef -compdef@*/
//     if (find_root_node(db.node_lib) != NULL) {
//         int i;
//         int* metric_chain = (int*) calloc(MAXHOPS, sizeof(int));
//         if (metric_chain != NULL) {
//             init_metric_chain(metric_chain);
//         }
//         else {
//             log_pr(LOG_CRIT, "calloc failed in assign_root_metrics()");
//             exit(EXIT_FAILURE);
//         }
//         int root_node_key = find_root_node(db.node_lib)->KEY;
// 
// 
//         for (i = 0; i < count_node_keys(db.node_lib); i++) {
//             if (i != root_node_key) {
//                 get_ls_metric(db, root_node_key, i, metric_chain);
//                 find_node_by_key(db.node_lib, i)->ROOT_METRIC = get_metric_chain_len(metric_chain);
//                 init_metric_chain(metric_chain);
//             }
//         }
//         find_node_by_key(db.node_lib, find_root_node(db.node_lib)->KEY)->ROOT_METRIC = 0;
//         free(metric_chain);
//     }
//     else {
//         log_pr(4, "assign_root_metrics called on topology_db with no root node set");
//     }
// } *@+nullderef +compdef@*/

static int pp = 3;

void debugmatrix(void) {
    printf("\n");

    init_all_pathcounters();

    create_pathpage();

    append_pathpage(pathpage_counter, 1);
    append_pathpage(pathpage_counter, 0);

    create_pathpage();

    append_pathpage(pathpage_counter, 1);
    append_pathpage(pathpage_counter, 2);

    create_pathpage();

    append_pathpage(pathpage_counter, 1);
    append_pathpage(pathpage_counter, 3);

    create_pathpage();

    append_pathpage(pathpage_counter, 1);
    append_pathpage(pathpage_counter, 3);
    append_pathpage(pathpage_counter, 4);

    create_pathpage();

    append_pathpage(pathpage_counter, 1);
    append_pathpage(pathpage_counter, 3);
    append_pathpage(pathpage_counter, 5);

    copy_pathpage(2);
    append_pathpage(pathpage_counter, 4);

    create_pathpage();

    append_pathpage(pathpage_counter, 1);
    append_pathpage(pathpage_counter, 3);
    append_pathpage(pathpage_counter, 4);
    append_pathpage(pathpage_counter, 5);

    create_pathpage();

    append_pathpage(pathpage_counter, 1);
    append_pathpage(pathpage_counter, 3);
    append_pathpage(pathpage_counter, 5);
    append_pathpage(pathpage_counter, 6);

    create_pathpage();
    append_pathpage(pathpage_counter, 1);
    append_pathpage(pathpage_counter, 0);

    find_living_pathpages();

    append_visited(0);
    append_visited(2);
    append_visited(3);
    append_visited(4);
//     append_visited(1);
//     append_visited(5);
//     append_visited(6);

    if (node_is_visited(0) == 1) {
        printf("Node 0 was visited\n");
    }

    find_pathpage_terminators();


    int i;
    for (i = 1; i <= pathpage_terminators[COUNT]; i++) {
        printf("found terminating node: %d\n", pathpage_terminators[i]);
    }

    for (i = 1; i <= visited_nodes[COUNT]; i++) {
        printf("node in visited set: %d\n", visited_nodes[i]);
    }

        find_unvisited_pathpage_terminators();

    for (i = 1; i <= pathpage_terminators[COUNT]; i++) {
        printf("found unvisited terminating node[%d]: %d\n", i, pathpage_terminators[i]);
    }

    dedup_terminators();

    printf("DEDUP!\n");
    for (i = 1; i <= pathpage_terminators[COUNT]; i++) {
        printf("found unvisited terminating node[%d]: %d\n", i, pathpage_terminators[i]);
    }


    for (i = H_OFFSET + 1; i <= H_OFFSET + (pathpages[pp][COUNT]); i++) {
        printf("found node in pathpage %d: %d\n", pp, pathpages[pp][i]);
    }
}

void mytest(void)
{
    int i;
    printf("\nUPDATE!\n\n");
//     for (i = 0; i <= H_OFFSET + (pathpages[pp][COUNT]); i++) {
//         printf("found node in pathpage %d: %d\n", pp, pathpages[pp][i]);
//     }
    int j;
    for (i = 0; i < pathpage_counter; i++) {
        for (j = H_OFFSET + 1; j <= pathpages[i][COUNT] + H_OFFSET; j++) {
            printf("pathpage [%d][%d] = %d\n", i, j, pathpages[i][j]);
        }
        printf("***pathpage [%d] length = %d\n", i, pathpages[i][DISTANCE_METRIC]);
    }

    create_pathpage();
    //printf("***************pathpage: %d\n", pathpage_counter);
    append_pathpage(pathpage_counter, 0);
    append_pathpage(pathpage_counter, 1);
    append_pathpage(pathpage_counter, 3);
    append_pathpage(pathpage_counter, 4);
    append_pathpage(pathpage_counter, 5);
    append_pathpage(pathpage_counter, 3);     // COMMENT THIS OUT

//     kill_loop_pages(APPEND_LOOP_TERMS_TO_VISITED);
// 
//     printf("***********LOOP_CHECK_PREAMBLE: pathpage[9] status = %d\n", pathpages[9][PATH_STATUS]);
//     loop_check(9, KILL_LOOP);
//     printf("***********LOOP_CHECK_POSTAMBLE: pathpage[9] status = %d\n", pathpages[9][PATH_STATUS]);
//     //printf("XXXX = %d\n", XXXX);
// 
//     printf("visited set: %d %d %d %d %d %d %d\n", visited_nodes[COUNT], visited_nodes[1], visited_nodes[2], visited_nodes[3], visited_nodes[4], visited_nodes[5], visited_nodes[6]);
//     remove_visited(2);
//     printf("visited set: %d %d %d %d %d %d %d\n", visited_nodes[COUNT], visited_nodes[1], visited_nodes[2], visited_nodes[3], visited_nodes[4], visited_nodes[5], visited_nodes[6]);

}

#include "stdcntrl.h"

#define BAD_ROOT_METRIC -1
#define MAXHOPS 64
#define DEFAULT_PRIORITY 1000

#define IDSTRLEN 1 + 32 + 6 + 1
#define NULLID "x-00000000-00000000-00000000-0000-00-00"
#define TOPID "n-zzzzzzzz-zzzzzzzz-zzzzzzzz-zzzz-zz-zz"
#define SAMEIDSTR 0
#define ENTITY_A 1
#define ENTITY_B 2
#define ID_NULLID 0
#define ID_NODE 1
#define ID_NLINK 2
#define ID_ADJ 3
#define ID_OTHER 9

#define BEST_PRI 0
#define WORST_PRI 16384

#define COUNT 0
#define PATH_STATUS 1
#define BRANCH_FROM 2
#define DISTANCE_METRIC 3
#define H_OFFSET 3
#define INVALID_KEY -1
#define DEAD_PATH_KEY -2
#define ACTIVE_PATH_KEY -3

#define NO_KILL_LOOP 0
#define NO_APPEND_LOOP_TERMS_TO_VISITED 0
#define KILL_LOOP 1
#define APPEND_LOOP_TERMS_TO_VISITED 1

#define NOGRAPH -1
#define EPATHFAIL -2
#define SUCCESS 1

typedef char* idstr;

/*
    TODO: add array to 'node' to indicate path to root
*/
struct node {
    int KEY, ROOT_METRIC, IS_ROOT;
    idstr ID;
    struct node *NEXT;
}; typedef struct node node;

struct nlink {
    int KEY, PRI;
    idstr ID;
    struct node *S_NODE, *D_NODE;
    struct nlink *NEXT;
}; typedef struct nlink nlink;

struct adj {
    int KEY;
    idstr ID;
    struct node *S_NODE, *D_NODE;
    struct nlink *VIA;
    struct adj *NEXT;
}; typedef struct adj adj;

struct topology_db {
    int KEY;
    struct node *node_lib;
    struct nlink *nlink_lib;
    struct adj *adj_lib;
}; typedef struct topology_db topology_db;

typedef int path;

node* create_node(int);
nlink* create_nlink(int);
adj* create_adj(int);
/*@unused@*/ void destroy_nodechain(/*@null@*/ node*);
/*@unused@*/ void destroy_nlinkchain(/*@null@*/ nlink*);
/*@unused@*/ void destroy_adjchain(/*@null@*/ adj*);
/*@null@*/ node* find_node_by_key(node*, int);
/*@null@*/ node* find_node_by_id(node*, idstr);
/*@null@*/ node* find_root_node(node*);
/*@null@*/ nlink* find_nlink_by_key(nlink*, int);
/*@null@*/ nlink* find_nlink_by_id(nlink*, idstr);
/*@null@*/ adj* find_adj_by_key(adj*, int);
/*@null@*/ adj* find_adj_by_id(adj*, idstr);
/*@null@*/ adj* find_adjs_by_snode(/*@null@*/ adj*, /*@null@*/ node*);
/*@null@*/ adj* find_adjs_by_dnode(/*@null@*/ adj*, /*@null@*/ node*);

int find_lower_idstr(idstr, idstr);
int find_higher_idstr(idstr, idstr);


void assign_adj(adj*, node*, node*, nlink*);
int count_node_keys(node*);
int count_adj_keys(adj*);
void cleanup_tplgy_database(topology_db);
//void init_metric_chain(int*);
int get_metric_chain_len(int*);

void get_ls_metric(topology_db, int, int, int*);
void get_ls_metric2(topology_db, int, int, int*);

void assign_root_metrics(topology_db);
void set_root_node_by_key(topology_db, int);
void set_root_node_by_id(topology_db, idstr);
void unset_root_node_by_key(topology_db, int);
void unset_root_node_by_id(topology_db, idstr);
void init_metric_chain(int*);

/*@null@*/ node* find_nearest_neighbor(topology_db, node*);

void debugmatrix(void);
int calc_pathlen(topology_db, int);
void calc_all_pathlens(topology_db);
void mytest(void);
int find_shortest_pathpage(topology_db);
/*@-fixedformalarray@*/
int get_shortest_path(topology_db, node*, node*, path (*p)[MAXHOPS]);

/*@+fixedformalarray@*/

//idstr genID(int);

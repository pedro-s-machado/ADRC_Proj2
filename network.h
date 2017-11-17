#ifndef network_h
#define network_h


typedef struct _networkNode networkNode;

typedef struct _nodeTree nodeTree;

typedef struct _nodeList nodeList;

typedef struct _network network;

network * createNetwork(char *filename);

void printNetwork(nodeTree * tree);

nodeTree* getNetwork(network *n);

void findTierOnes(nodeTree * root, nodeList ** list, int * count );

nodeTree* produceStats(networkNode* , int );
nodeList* makeNodeListFromNodeTree(nodeTree* , nodeList* );
void freeNodeList(nodeList*);
void freeNodeTree(nodeTree*);
nodeList* removeFromNodeList(nodeList*, networkNode*);
int countNodesInTree(nodeTree* tree);
int countNodesInList(nodeList* list);
int countNoPeerRoutesInTree(nodeTree* );
int countNoCustomerRoutesInTree(nodeTree* );
#endif

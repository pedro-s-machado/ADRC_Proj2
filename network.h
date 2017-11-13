#ifndef network_h
#define network_h


typedef struct _networkNode networkNode;

typedef struct _nodeTree nodeTree;

typedef struct _network network;

network * createNetwork(char *filename);

void printNetwork(nodeTree * tree);

nodeTree* getNetwork(network *n);

#endif
#include <stdio.h>
#include <stdlib.h>


struct _nodeTree;

typedef struct _networkNode{

    int id;
    struct _nodeTree * costumers;
    struct _nodeTree * peers;
    struct _nodeTree * providers;

}networkNode;

typedef struct _nodeTree{
    
    networkNode * node;
    struct _nodeTree * left;
    struct _nodeTree * right;

}nodeTree;

typedef struct _network{

    int numberNodes;
    int costumerCicles;
    int comercialyConected;
    nodeTree * nodes;

}network;


/*  
    searches a node tree for a specific node
    input:
        root, node to search for
    output:
        the node if found
        the 'parent node' if not found
        NULL if root == NULL
*/
nodeTree * searchNode(nodeTree * root, networkNode * lost){
    
    nodeTree * searchPointer = NULL;
    nodeTree * nextNode = root;
    
    while(nextNode != NULL){
        
        searchPointer = nextNode;
        if(lost->id < searchPointer->node->id){
            nextNode = searchPointer->left;
        }
        else if(lost->id > searchPointer->node->id){
            nextNode = searchPointer->right;
        }
        else{
            nextNode = NULL;
        }

    }
    return(searchPointer);
}

networkNode * nodeTreeInsert(nodeTree ** root, networkNode* node){

    nodeTree * searchPointer = searchNode(*root, node);
    nodeTree * new;
    
    if(searchPointer == NULL){
        new = (nodeTree*)malloc(sizeof(nodeTree));
        new->node = node;
        new->left = NULL;
        new->right = NULL;
        *root = new;
        return node;
    }
    else if(searchPointer->node->id != node->id){

        new = (nodeTree*)malloc(sizeof(nodeTree));
        new->node = node;
        new->left = NULL;
        new->right = NULL;

        if(node->id < searchPointer->node->id){
            searchPointer->left = new;
        }
        else{
            searchPointer->right = new;
        }
        return node;
    }else{
        
    }

    return searchPointer->node;
}



/*
    inserts a node into the network basead on a connection
    input:
        network, tailID, headID, type of connection (1-provider, 2-peer, 3-costumer)
    output:
        updated network
*/
network * networkConnectionInsert(network * n ,int tail, int head, int relation){

    networkNode * tailNode = malloc(sizeof(networkNode));
    networkNode* headNode = malloc(sizeof(networkNode));

    /*repensar inserção (procurar primeiro e depois inserir (mudar função de inserção))*/
    tailNode->id = tail;
    tailNode->costumers = NULL;
    tailNode->providers = NULL;
    tailNode->peers = NULL;

    headNode->id = head;
    headNode->costumers = NULL;
    headNode->providers = NULL;
    headNode->peers = NULL;

    tailNode = nodeTreeInsert(&n->nodes, tailNode);
    
    headNode = nodeTreeInsert(&n->nodes, headNode);

    switch(relation){
        case 1:
            nodeTreeInsert(&headNode->providers, tailNode);
            break;
        case 2:
            nodeTreeInsert(&headNode->peers, tailNode);
            break;
        case 3:
            nodeTreeInsert(&headNode->costumers, tailNode);
            break;
        default:
            break;
    }
    return(n);
}

int countNodes(nodeTree * root, int count){
    if(root != NULL){
        count ++;
        count = countNodes(root->left, count);
        count = countNodes(root->right, count);    
    }
    return count;
}

network * createNetwork(char *filename){

    FILE * file;
    char buff[200];
    int tail, head, relation;
    network * n = NULL;

    /*Opening file*/
    file = fopen(filename, "r");
    if(file == NULL){
        printf("Error opening file %s", filename);
        exit(1);
    }

    /*Initializing network*/
    n = (network*)malloc(sizeof(network));
    n->numberNodes = 0;
    n->comercialyConected = 0;
    n->costumerCicles = 0;
    n->nodes = NULL;

    /*Reading from file*/
    while(fgets(buff, 200, file)){

        sscanf(buff, "%d %d %d", &tail, &head, &relation);
        //printf("%d %d %d\n", tail, head, relation);
        n = networkConnectionInsert(n, tail, head, relation);

    }
    n->numberNodes = countNodes(n->nodes,0);
    printf("Nodes count = %d\n", n->numberNodes);
    printf("Network Created!");

    return(n);
}

void printTree(nodeTree * tree){
    if(tree != NULL){
        printf("\t %d\n", tree->node->id);
        printTree(tree->left);
        printTree(tree->right);
    }
    return;
}


void printNetwork(nodeTree * tree){

    if(tree != NULL){
        printf(" %d costumers:\n", tree->node->id);
        printTree(tree->node->costumers);
        printf(" %d peers:\n", tree->node->id);
        printTree(tree->node->peers);
        printf(" %d providers:\n", tree->node->id);
        printTree(tree->node->providers);
        
        printNetwork(tree->left);
        printNetwork(tree->right);
    }
}

nodeTree* getNetwork(network *n){
    return n->nodes;
}
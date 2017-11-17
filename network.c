#ifndef network_c
#define network_c

#include <stdio.h>
#include <stdlib.h>
#include "network.h"


struct _nodeTree;
struct _routeNode;

typedef struct _networkNode{
    
    /* Intrinsic information of an AS (node) */
    int id;
    
    /* Used in the cycle-detection procedure */
    int visited;
    int finished;
    
    /* BST's of all direct neighbours at a node */
    struct _nodeTree * costumers;
    struct _nodeTree * peers;
    struct _nodeTree * providers;
    
    /* BST's to list (in trees) nodes according to type of prefered-route */
    struct _nodeTree * viaCustomers_tree;
    struct _nodeList * viaPeers_list;
    /* And the number of elements in these BST's */
    int customerRouteNodes, peerRouteNodes;
    
    /* Routing table (BST) at a node (will not contain route to all nodes, too inefficient) */
    struct _routeNode * routes;

}networkNode;

typedef struct _routeNode{

    int routeType;
    int hopCount;
    networkNode * destination;
    networkNode * nextHop;
    
    struct _routeNode * left;
    struct _routeNode * right;

}routeNode;

typedef struct _nodeTree{
    
    networkNode * node;
    struct _nodeTree * left;
    struct _nodeTree * right;

}nodeTree;


typedef struct _nodeList{

    networkNode * node;
    struct _nodeList * next;
}nodeList;

typedef struct _network{

    int numberNodes;
    int tierOneCount;
    int costumerCicles;
    int comercialyConected;
    
    /* BST of all nodes */
    nodeTree * nodes;
    /* BST of all tier-1 nodes */
    nodeList * tierOnes;

}network;

nodeList * nodeListInsert(nodeList * head, networkNode * node){

    nodeList * new = (nodeList*)malloc(sizeof(nodeList));
    new->node = node;
    new->next = head;

    return new;
}

void printList(nodeList* head){

    nodeList * iterator = head;
    while(iterator != NULL){
        printf("%d ", iterator->node->id);
        iterator = iterator->next;
    }
    printf("\n");
}


void printTree(nodeTree * tree){
    if(tree != NULL){
        printf("\t %d\n", tree->node->id);
        printTree(tree->left);
        printTree(tree->right);
    }
    return;
}

/* only frees the list, nodes only get deleted via network */
nodeList * freeList(nodeList * head){
    nodeList * iterator = head;
    nodeList * deleter;
    
    while(iterator != NULL){
        deleter = iterator;
        iterator = iterator->next;
        free(deleter->next);
        free(deleter);
    }
    return NULL;
}

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

routeNode * searchRoute(routeNode * root, networkNode * destination){
    
    routeNode * searchPointer = NULL;
    routeNode * nextNode = root;
    
    while(nextNode != NULL){
        
        searchPointer = nextNode;
        if(destination->id < searchPointer->destination->id){
            nextNode = searchPointer->left;
        }
        else if(destination->id > searchPointer->destination->id){
            nextNode = searchPointer->right;
        }
        else{
            nextNode = NULL;
        }

    }
    return(searchPointer);
}

networkNode * nodeTreeInsert(nodeTree ** root, networkNode* node, int * old){

    nodeTree * searchPointer = searchNode(*root, node);
    nodeTree * new;
    
    if(searchPointer == NULL){
        new = (nodeTree*)malloc(sizeof(nodeTree));
        new->node = node;
        new->left = NULL;
        new->right = NULL;
        *root = new;
        *old = 0;
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
        *old = 0;
        return node;
    }else{
        
    }
    *old = 1;
    return searchPointer->node;
}

void updateRoute(networkNode * root, networkNode* nextHop, networkNode * destination, int hopCount, int routeType);

/*annouce route to a determined type of neib*/
void announce(nodeTree * root, networkNode * nextHop, networkNode * destination, int hopCount, int routeType){

    if(root != NULL){
        updateRoute(root->node, nextHop, destination, hopCount, routeType);
        announce(root->left, nextHop, destination, hopCount, routeType);
        announce(root->right, nextHop, destination, hopCount, routeType);
    }
    return;
}

void updateRoute(networkNode * receiver, networkNode* nextHop, networkNode * destination, int hopCount, int routeType){

    routeNode * searchPointer = searchRoute(receiver->routes, destination);
    routeNode * new;
    int better = 0;
    
    /*no routes yet*/
    if(searchPointer == NULL){
        new = (routeNode*)malloc(sizeof(routeNode));
        new->nextHop = nextHop;
        new->destination = destination;
        new->hopCount = hopCount;
        new->routeType = routeType;
        new->left = NULL;
        new->right = NULL;
        receiver->routes = new;
        
        better = 1;
    }
    /*new route*/
    else if(searchPointer->destination->id != destination->id){

        new = (routeNode*)malloc(sizeof(routeNode));
        new->nextHop = nextHop;
        new->destination = destination;
        new->hopCount = hopCount;
        new->routeType = routeType;
        new->left = NULL;
        new->right = NULL;

        if(destination->id < searchPointer->destination->id){
            searchPointer->left = new;
        }
        else{
            searchPointer->right = new;
        }        
        better = 1;
    }
    /*route exists*/
    else{
        
        /*if new routType is better*/
        if(searchPointer->routeType < routeType){
            better = 1;
        }
        /*if routeType is the same*/
        else if(searchPointer->routeType == routeType){
            if(searchPointer->hopCount > hopCount){
                better = 1;
            }
        }

        /*if better update*/
        if(better){
            searchPointer->nextHop = nextHop;
            searchPointer->hopCount = hopCount;
            searchPointer->routeType = routeType;

            new = searchPointer;
        }

    }

    if(better){
        switch(routeType){
            case 1:
            case 2:
                announce(receiver->costumers, receiver, new->destination, new->hopCount+1, 1);
                break;
            case 3:
                announce(receiver->costumers, receiver, new->destination, new->hopCount+1, 1);
                announce(receiver->peers, receiver, new->destination, new->hopCount+1, 2);
                announce(receiver->providers, receiver, new->destination, new->hopCount+1, 3);
                break;
            default:
                break;
        }
    }
    return;
}

void printBestRouteTo(nodeTree * root, networkNode * destination){
    
    routeNode * route;
    if(root != NULL){
        route = searchRoute(root->node->routes, destination);
        printf("from: %d, to: %d, use: %d, cost: %d\n", root->node->id, destination->id, route->nextHop->id, route->hopCount);
        printBestRouteTo(root->left, destination);
        printBestRouteTo(root->right, destination);
    }
    return;
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
    int old = 0;

    /*repensar inserção (procurar primeiro e depois inserir (mudar função de inserção))*/
    tailNode->id = tail;
    tailNode->visited = 0;
    tailNode->finished = 0;
    tailNode->costumers = NULL;
    tailNode->providers = NULL;
    tailNode->peers = NULL;
    tailNode->routes = NULL;
    tailNode->viaCustomers_tree = NULL;
    tailNode->viaPeers_list = NULL;

    headNode->id = head;
    headNode->visited = 0;
    headNode->finished = 0;
    headNode->costumers = NULL;
    headNode->providers = NULL;
    headNode->peers = NULL;
    headNode->routes = NULL;
    tailNode->viaCustomers_tree = NULL;
    tailNode->viaPeers_list = NULL;

    tailNode = nodeTreeInsert(&n->nodes, tailNode, &old);
    
    headNode = nodeTreeInsert(&n->nodes, headNode, &old);
    
    switch(relation){
        case 1:
            nodeTreeInsert(&headNode->providers, tailNode, &old);
            break;
        case 2:
            nodeTreeInsert(&headNode->peers, tailNode, &old);
            break;
        case 3:
            nodeTreeInsert(&headNode->costumers, tailNode, &old);
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

void findTierOnes(nodeTree * root, nodeList **   list, int * count ){
    
    if(root != NULL){
        
        if(root->node->providers == NULL){
            *count = *count + 1;
            *list = nodeListInsert(*list, root->node);
        }
        
        findTierOnes(root->left, list, count);
        findTierOnes(root->right, list, count);
    }
    return;
}

/*
    checks if all tierOne nodes have peer connections to all tierOne nodes
    assumes a peer connection works both ways, 
    i.e. 
    if A has peer to B then is assumed that B has peer to A

    input - network
    output - 0 if false
             1 if true
*/
int checkPeerBetweenTierOnes(network * n){

    nodeList * out_iterator = n->tierOnes;
    nodeList * in_iterator;
    nodeTree * found;

    while(out_iterator != NULL){

        in_iterator = out_iterator->next;
        //printf("Checking %d\n", out_iterator->node->id);
        while( in_iterator != NULL){

            //printf("\t%d\n", in_iterator->node->id);
            found = searchNode(out_iterator->node->peers, in_iterator->node);
            if((found == NULL) || (found->node->id != in_iterator->node->id)){
                printf("\tNot found!\n");
                return 0;
            }
            in_iterator = in_iterator->next;    
        }
        out_iterator = out_iterator->next;
    }
    return 1;
}

/*
    from the internet, just to check how deep it was
*/
int maxDepth(nodeTree * node) {
   if (node==NULL) 
       return 0;
   else
   {
       /* compute the depth of each subtree */
       int lDepth = maxDepth(node->left);
       int rDepth = maxDepth(node->right);
 
       /* use the larger one */
       if (lDepth > rDepth) 
           return(lDepth+1);
       else return(rDepth+1);
   }
}
void findReachability(networkNode * current, nodeTree ** found, int from);
void callReachability(nodeTree * tree, nodeTree ** found, int from){

    if(tree != NULL){
        findReachability(tree->node, found, from);
        callReachability(tree->left, found, from);
        callReachability(tree->right, found, from);
    }
    return;

}

void findReachability(networkNode * current, nodeTree ** found, int from){

    int old = 0;
    networkNode * inserted = nodeTreeInsert(found, current, &old);
    
    /*if is not visited, keep searching*/
    //printf("\n%d\n", current->id);
    if(!old){
        //printf("  %d\n", current->id);
        switch(from){
            case 1:
                callReachability(inserted->costumers, found, 1);
                break;
            case 2:
                callReachability(inserted->costumers, found, 1);
                break;
            case 3:
                callReachability(inserted->peers, found, 2);
                callReachability(inserted->costumers, found, 1);
                break;
            default:
                break;
        }
    }
    return;
}

/*
    Checks if a network is comercially connected

    input - network
    output - 0 if false
             1 if true
*/
int checkComercialConnected(network * n){

    nodeTree * reached = NULL;
    if(checkPeerBetweenTierOnes(n)){
        
        findReachability(n->tierOnes->node, &reached, 3);
        
        if(n->numberNodes == countNodes(reached,0)){
            n->comercialyConected = 1;
            return 1;
        }
    }
    n->comercialyConected = 0;
    return 0;
}

/*
void routeInfo(){

}*/

void updateAllRoutes(nodeTree * root, int * count){
    
    if(root != NULL){
        updateRoute(root->node, root->node, root->node, 0, 3);
        *count = *count + 1;
        printf("%d\n", *count);
        updateAllRoutes(root->left, count);
        updateAllRoutes(root->right, count);
    }
}

void specialDFS_part_two(nodeTree * root, int id, int * cicle);
void specialDFS_part_one(networkNode * root, int id, int * cicle){
    
    if(root->visited == 0){
        //printf("\t%d\n", root->id);
        root->visited = id;
        if((root != NULL) && !(*cicle)){
            specialDFS_part_two(root->costumers, id, cicle);
        }
        root->finished = id;
    }
    return;
}

void specialDFS_part_two(nodeTree * root, int id, int * cicle){

    if((root != NULL) && !(*cicle)){
        if(root->node->visited == id){
            if(!(root->node->finished == id)){
                *cicle = 1;
                return;
            }
        }
        else{
            specialDFS_part_one(root->node, id, cicle);
        }
    
    specialDFS_part_two(root->left, id, cicle);
    specialDFS_part_two(root->right, id, cicle);
    }
    return;

}


void checkCostumerCicles(nodeTree * root, int * costumerCicle){

    if((root != NULL) && (*costumerCicle == 0)){
        //printf("%d\n", root->node->id );
        specialDFS_part_one(root->node, root->node->id, costumerCicle);
        checkCostumerCicles(root->left, costumerCicle);
        checkCostumerCicles(root->right, costumerCicle);
    }

    return;

}
network * createNetwork(char *filename){

    FILE * file;
    char buff[200];
    int tail, head, relation;
    network * n = NULL;
    int count = 0;

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
    n->tierOneCount = 0;
    n->nodes = NULL;
    n->tierOnes = NULL;

    /*Reading from file*/
    while(fgets(buff, 200, file)){

        sscanf(buff, "%d %d %d", &tail, &head, &relation);
        //printf("%d %d %d\n", tail, head, relation);
        n = networkConnectionInsert(n, tail, head, relation);

    }
    n->numberNodes = countNodes(n->nodes,0);
    
    findTierOnes(n->nodes, &(n->tierOnes), &(n->tierOneCount));
    /*
    printf("\nNodes count = %d\n", n->numberNodes);
    printf("Tier One count = %d\n", n->tierOneCount);
    printList(n->tierOnes);
    printf("\nNetwork Created!\n");

    printf("\nMax Depht = %d\n", maxDepth(n->nodes));
    if(checkComercialConnected(n)){
        printf("Commercialy connected!\n");
    }else{
        printf("Not Commercialy connected\n");
    }
    printf("\nUpdating routes\n");
    updateRoute(n->nodes->node, n->nodes->node, n->nodes->node, 0, 3);
    updateAllRoutes(n->nodes, &count);
    printf("Acabou\n");
    printBestRouteTo(n->nodes, n->nodes->node);
    */
    checkCostumerCicles(n->nodes, &(n->costumerCicles));
    if(n->costumerCicles){
        printf("Costumer Cicle found!\n");
    }
    else{
        printf("No costumer cicle!\n");
    }

    return(n);
}

void printNetwork(nodeTree * tree){

    if(tree != NULL){
        printf(" %d costumers:\n", tree->node->id);
        printTree(tree->node->costumers);
        printf(" %d peers:\n", tree->node->id);
        printTree(tree->node->peers);
        printf(" %d providers:\n", tree->node->id);
        printTree(tree->node->providers);
        
        //printf("%d\n",tree->node->id);
        printNetwork(tree->left);
        printNetwork(tree->right);
    }
}

nodeTree* getNetwork(network *n){
    return n->nodes;
}

nodeList* makeNodeListFromNodeTree(nodeTree* tree, nodeList* ptr) {
    if (tree != NULL) {
        if (tree->left != NULL) {
            ptr = makeNodeListFromNodeTree(tree->left, ptr);
        }
        if (tree->right != NULL) {
            ptr = makeNodeListFromNodeTree(tree->right, ptr);
        }
        nodeList *new_entry = malloc(sizeof(nodeList));
        new_entry->node = tree->node;
        new_entry->next = ptr;
        return new_entry;
    }
    else
        return NULL;
}

nodeTree* produceStats(networkNode* node, int resetCounters) {
    
    if (!resetCounters && (node->viaCustomers_tree != NULL || node->viaPeers_list != NULL)) {
        return node->viaCustomers_tree;
    }
    else {
        
        nodeTree *tempViaCustomersTree = NULL, *tempViaPeersTree = NULL;
        nodeList *tempViaPeersList = NULL;
        
        // Listing of the neighbours to whom to make a request
        networkNode *nodePtr = NULL;
        nodeList *customers = NULL, *peers = NULL, *listPtr = NULL;
        customers = makeNodeListFromNodeTree(node->costumers, customers);
        peers = makeNodeListFromNodeTree(node->peers, peers);
        listPtr = customers;
        while (listPtr != NULL) {
            nodePtr = listPtr->node;
            tempViaCustomersTree = produceStats(nodePtr, resetCounters);
            listPtr = listPtr->next;
        }
        if (!resetCounters) {
            // Adding my own customers to the tree
            listPtr = customers;
            while (listPtr != NULL) {
                nodePtr = listPtr->node;
                int i=0;
                nodeTreeInsert(&tempViaCustomersTree, nodePtr, &i);
                listPtr = listPtr->next;
            }
            
            // Saving the customers tree
            node->viaCustomers_tree = tempViaCustomersTree;
        }
        
        listPtr = peers;
        while (listPtr != NULL) {
            nodePtr = listPtr->node;
            tempViaPeersTree = produceStats(nodePtr, resetCounters);
            listPtr = listPtr->next;
        }
        if (!resetCounters) {
            
            // Now find the peers
            tempViaPeersList = makeNodeListFromNodeTree(tempViaPeersTree, tempViaPeersList);
            listPtr = tempViaPeersList;
            while (listPtr != NULL) {
                nodePtr = listPtr->node;
                // Check presence of nodePtr in tempViaCustomersTree, if it is there, remove it from tempViaPeersList;
                if (nodePtr == searchNode(tempViaCustomersTree, nodePtr)->node) {
                    tempViaPeersList = removeFromNodeList(tempViaPeersList, nodePtr);
                }
                
                listPtr = listPtr->next;
            }
            
            // Saving the peers list...
            node->viaPeers_list = tempViaPeersList;
            
            // Counting....
            node->customerRouteNodes = countNodesInTree(tempViaCustomersTree);
            node->peerRouteNodes = countNodesInList(tempViaPeersList);
            
            return node->viaCustomers_tree;
        }
        else {
            node->customerRouteNodes = 0;
            node->peerRouteNodes = 0;
            freeNodeTree(node->viaCustomers_tree);
            node->viaCustomers_tree = NULL;
            freeNodeList(node->viaPeers_list);
            node->viaPeers_list = NULL;
            return NULL;
        }
    }
}

void freeNodeList(nodeList* list) {
    if (list!=NULL) {
        if (list->next != NULL)
            freeNodeList(list->next);
        free(list);
    }
}

void freeNodeTree(nodeTree* tree) {
    if (tree!=NULL) {
        if (tree->right != NULL)
            freeNodeTree(tree->right);
        if (tree->left != NULL)
            freeNodeTree(tree->left);
        free(tree);
    }
}

nodeList* removeFromNodeList(nodeList* list, networkNode* node) {
    if (list->node == node) {
        nodeList* ret_value = list->next;
        free(list);
        return ret_value;
    }
    nodeList *ptr1 = list, *ptr2 = list->next;
    while (ptr2 != NULL) {
        if (ptr2->node == node) {
            ptr1->next = ptr2->next;
            free(ptr2);
            return list;
        }
        ptr2 = ptr2->next;
    }
    return list;
}

int countNodesInTree(nodeTree* tree) {
    if (tree == NULL)
        return 0;
    else
        return 1 + countNodesInTree(tree->left)+countNodesInTree(tree->right);
}
int countNodesInList(nodeList* list) {
    if (list == NULL)
        return 0;
    else
        return 1 + countNodesInList(list->next);
}

void showStats(network* n) {
    printf("Entered showStats\n");
    produceStats(n->tierOnes->node, 0);
    printf("Statsproduced\n");
    int a=countNoCustomerRoutesInTree(n->nodes),b=countNoPeerRoutesInTree(n->nodes),c;
    c = (n->numberNodes)*((n->numberNodes) - 1) - a - b;
    printf("Total number of nodes :\t\t\t\t\t\t\t\t\t%i\n",n->numberNodes);
    printf("Total number of customer elected-routes :\t%i (%i%%)\n", a, 100*a/(a+b+c));
    printf("Total number of peer elected-routes :\t\t%i (%i%%)\n", b, 100*b/(a+b+c));
    printf("Total number of provider elected-routes :\t%i (%i%%)\n", c, 100*c/(a+b+c));
    printf("Resetting stat variables\n");
   // produceStats(n->tierOnes->node, 1);
    printf("Stats variables reset\n");
}

int countNoCustomerRoutesInTree(nodeTree* node) {
    if (node == NULL)
        return 0;
    else
        return node->node->customerRouteNodes + countNoCustomerRoutesInTree(node->left) + countNoCustomerRoutesInTree(node->right);
}
int countNoPeerRoutesInTree(nodeTree* node) {
    if (node == NULL)
        return 0;
    else
        return node->node->peerRouteNodes + countNoPeerRoutesInTree(node->left) + countNoPeerRoutesInTree(node->right);
}

#endif


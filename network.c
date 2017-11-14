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

typedef struct _nodeList{

    networkNode * node;
    struct _nodeList * next;
}nodeList;

typedef struct _network{

    int numberNodes;
    int tierOneCount;
    int costumerCicles;
    int comercialyConected;
    nodeTree * nodes;
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

/*
    only frees the list, nodes only get deleted via network
*/
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
    tailNode->costumers = NULL;
    tailNode->providers = NULL;
    tailNode->peers = NULL;

    headNode->id = head;
    headNode->costumers = NULL;
    headNode->providers = NULL;
    headNode->peers = NULL;

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
    if(!checkPeerBetweenTierOnes(n)){
        n->comercialyConected = 0;
        return 0;
    }else{
        findReachability(n->tierOnes->node, &reached, 3);
        n->comercialyConected = 1;
        return 1;
    }
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
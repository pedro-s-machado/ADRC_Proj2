#include <stdio.h>
#include <stdlib.h>
#include "network.h"

int main(int argc, char * argv[]){

    network *n;
    if(argc >= 1){
    
        n = createNetwork(argv[1]);
        //printNetwork(getNetwork(n));
    }

    exit(0);
}
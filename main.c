#include <stdio.h>
#include <stdlib.h>
#include "network.h"

int main(int argc, char * argv[]){

    network *n;
    if(argc > 1){
    
        n = createNetwork(argv[1]);
        //printNetwork(getNetwork(n));
        showStats(n);
<<<<<<< HEAD
<<<<<<< HEAD
    }
    else {
        printf("No argument passed\n");
=======
>>>>>>> 195a9242aba484dba7f1214be2a3a61c42e7a856
=======
>>>>>>> 195a9242aba484dba7f1214be2a3a61c42e7a856
    }

    exit(0);
}

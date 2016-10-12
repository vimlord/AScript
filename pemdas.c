#include "pemdas.h"

#include "asmcommands.h"
#include "srccompile.h"

#include <string.h>
#include <stdlib.h>

void addOperation(FILE* execfile, char* partA, char* partB) {

    //Compute the two subcomponents
    pemdas(execfile, partA);
    pemdas(execfile, partB);
    
    //Grab the results
    stackPop(execfile, 17);
    stackPop(execfile, 16);

    //Add, then store.
    addReg(execfile, 16, 17);
    stackPush(execfile, 16);

}


void subOperation(FILE* execfile, char* partA, char* partB) {

    //Compute the two subcomponents
    pemdas(execfile, partA);
    pemdas(execfile, partB);
    
    //Grab the results
    stackPop(execfile, 17);
    stackPop(execfile, 16);

    //Add, then store.
    subReg(execfile, 16, 17);
    stackPush(execfile, 16);

}


void mulOperation(FILE* execfile, char* partA, char* partB) {

    //Compute the two subcomponents
    pemdas(execfile, partA);
    pemdas(execfile, partB);

    //Grab the results
    stackPop(execfile, 17);
    stackPop(execfile, 16);

    //Mult, then store.
    mulRegs(execfile, 16, 17);
    stackPush(execfile, 0);
}

void bitAndOperation(FILE* execfile, char* partA, char* partB) {

    //Compute the two subcomponents
    pemdas(execfile, partA);
    pemdas(execfile, partB);

    //Grab the results
    stackPop(execfile, 17);
    stackPop(execfile, 16);

    //Add, then store.
    andReg(execfile, 16, 17);
    stackPush(execfile, 16);
}

void bitOrOperation(FILE* execfile, char* partA, char* partB) {

    //Compute the two subcomponents
    pemdas(execfile, partA);
    pemdas(execfile, partB);

    //Grab the results
    stackPop(execfile, 17);
    stackPop(execfile, 16);

    //Add, then store.
    orReg(execfile, 16, 17);
    stackPush(execfile, 16);
}
void bitXorOperation(FILE* execfile, char* partA, char* partB) {
    
    //Compute the two subcomponents
    pemdas(execfile, partA);
    pemdas(execfile, partB);
        
    //Grab the results
    stackPop(execfile, 17);
    stackPop(execfile, 16);

    //Add, then store.
    xorReg(execfile, 16, 17);
    stackPush(execfile, 16);

}

void boolEqOperation(FILE* execfile, char* partA, char* partB) {
    
    if(*partB != '=') {
        //Bad input
        printf("Error during compilation: Invalid use of arithmetic operators.\n");
        exit(EINVAL);
    }

    //Compute the two subcomponents
    pemdas(execfile, partA);
    pemdas(execfile, &partB[1]);
        
    //Grab the results
    stackPop(execfile, 17);
    stackPop(execfile, 16);
        
    //Add, then store.
    eqBool(execfile, 16, 17, 18);
    stackPush(execfile, 18);
}

void boolNeOperation(FILE* execfile, char* partA, char* partB) {
    
    if(*partB != '=') {
        //Something like computing !x
        boolEqOperation(execfile, partA, partB);

        return;
    }

    //Compute the two subcomponents
    pemdas(execfile, partA);
    pemdas(execfile, &partB[1]);
        
    //Grab the results
    stackPop(execfile, 17);
    stackPop(execfile, 16);
        
    //Add, then store.
    neBool(execfile, 16, 17, 18);
    stackPush(execfile, 18);
}

void boolGeOperation(FILE* execfile, char* partA, char* partB) {
    
    //Compute the two subcomponents
    pemdas(execfile, partA);
    pemdas(execfile, partB);
        
    //Grab the results
    stackPop(execfile, 17);
    stackPop(execfile, 16);

    //Add, then store.
    geBool(execfile, 16, 17, 18);
    stackPush(execfile, 18);
}

void boolGtOperation(FILE* execfile, char* partA, char* partB) {
    
    if(*partB == '=') {
        boolGeOperation(execfile, partA, &partB[1]);
        return;
    }

    //Compute the two subcomponents
    pemdas(execfile, partA);
    pemdas(execfile, partB);
        
    //Grab the results
    stackPop(execfile, 17);
    stackPop(execfile, 16);
        
    //Add, then store.
    gtBool(execfile, 16, 17, 18);
    stackPush(execfile, 18);

}

void boolLeOperation(FILE* execfile, char* partA, char* partB) {

    //Compute the two subcomponents
    pemdas(execfile, partA);
    pemdas(execfile, partB);
        
    //Grab the results
    stackPop(execfile, 17);
    stackPop(execfile, 16);

    //Add, then store.
    leBool(execfile, 16, 17, 18);
    stackPush(execfile, 18);
}

void boolLtOperation(FILE* execfile, char* partA, char* partB) {

    if(*partB == '=') {
        boolLeOperation(execfile, partA, &partB[1]);
        return;
    }

    //Compute the two subcomponents
    pemdas(execfile, partA);
    pemdas(execfile, partB);
        
    //Grab the results
    stackPop(execfile, 17);
    stackPop(execfile, 16);
        
    //Add, then store.
    ltBool(execfile, 16, 17, 18);
    stackPush(execfile, 18);
}



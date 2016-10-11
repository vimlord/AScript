#include "pemdas.h"

#include "asmcommands.h"
#include "srccompile.h"

#include <string.h>
#include <stdlib.h>

void addOperation(FILE* execfile, char* partA, char* partB, int dst) {

    //Compute the two subcomponents
    pemdas(execfile, partA, dst + 1);
    pemdas(execfile, partB, dst + 2);
    
    //Grab the results
    copyRegFromMem(execfile, 16, dst + 1); //A
    copyRegFromMem(execfile, 17, dst + 2); //B
    
    //Add, then store.
    addReg(execfile, 16, 17);
    copyRegToMem(execfile, dst, 16);
        
}


void subOperation(FILE* execfile, char* partA, char* partB, int dst) {

    //Compute the two subcomponents
    pemdas(execfile, partA, dst + 1);
    pemdas(execfile, partB, dst + 2);
    
    //Grab the results
    copyRegFromMem(execfile, 16, dst + 1); //A
    copyRegFromMem(execfile, 17, dst + 2); //B
    
    //Add, then store.
    subReg(execfile, 16, 17);
    copyRegToMem(execfile, dst, 16);
        
}


void mulOperation(FILE* execfile, char* partA, char* partB, int dst) {

    //Compute the two subcomponents
    pemdas(execfile, partA, dst + 1);
    pemdas(execfile, partB, dst + 2);

    //Grab the results
    copyRegFromMem(execfile, 16, dst + 1); //A
    copyRegFromMem(execfile, 17, dst + 2); //B

    //Mult, then store.
    mulRegs(execfile, 16, 17);
    copyRegToMem(execfile, dst, 0);

}

void bitAndOperation(FILE* execfile, char* partA, char* partB, int dst) {

    //Compute the two subcomponents
    pemdas(execfile, partA, dst + 1);
    pemdas(execfile, partB, dst + 2);
        
    //Grab the results
    copyRegFromMem(execfile, 16, dst + 1); //A
    copyRegFromMem(execfile, 17, dst + 2); //B
        
    //Add, then store.
    andReg(execfile, 16, 17);
    copyRegToMem(execfile, dst, 16);

}

void bitOrOperation(FILE* execfile, char* partA, char* partB, int dst) {

    //Compute the two subcomponents
    pemdas(execfile, partA, dst + 1);
    pemdas(execfile, partB, dst + 2);
        
    //Grab the results
    copyRegFromMem(execfile, 16, dst + 1); //A
    copyRegFromMem(execfile, 17, dst + 2); //B
        
    //Add, then store.
    orReg(execfile, 16, 17);
    copyRegToMem(execfile, dst, 16);

}
void bitXorOperation(FILE* execfile, char* partA, char* partB, int dst) {

    //Compute the two subcomponents
    pemdas(execfile, partA, dst + 1);
    pemdas(execfile, partB, dst + 2);
        
    //Grab the results
    copyRegFromMem(execfile, 16, dst + 1); //A
    copyRegFromMem(execfile, 17, dst + 2); //B
        
    //Add, then store.
    xorReg(execfile, 16, 17);
    copyRegToMem(execfile, dst, 16);

}

void boolEqOperation(FILE* execfile, char* partA, char* partB, int dst) {
    
    if(*partB != '=') {
        //Bad input
        printf("Error during compilation: Invalid use of arithmetic operators.\n");
        exit(EINVAL);
    }

    //Compute the two subcomponents
    pemdas(execfile, partA, dst + 1);
    pemdas(execfile, &partB[1], dst + 2);
        
    //Grab the results
    copyRegFromMem(execfile, 16, dst + 1); //A
    copyRegFromMem(execfile, 17, dst + 2); //B
        
    //Add, then store.
    eqBool(execfile, 16, 17, 18);
    copyRegToMem(execfile, dst, 18);

}

void boolNeOperation(FILE* execfile, char* partA, char* partB, int dst) {
    
    if(*partB != '=') {
        //Something like computing !x
        boolEqOperation(execfile, partA, partB, dst);

        return;
    }

    //Compute the two subcomponents
    pemdas(execfile, partA, dst + 1);
    pemdas(execfile, &partB[1], dst + 2);
        
    //Grab the results
    copyRegFromMem(execfile, 16, dst + 1); //A
    copyRegFromMem(execfile, 17, dst + 2); //B
        
    //Add, then store.
    neBool(execfile, 16, 17, 18);
    copyRegToMem(execfile, dst, 18);

}

void boolGeOperation(FILE* execfile, char* partA, char* partB, int dst) {
    
    //Compute the two subcomponents
    pemdas(execfile, partA, dst + 1);
    pemdas(execfile, partB, dst + 2);
        
    //Grab the results
    copyRegFromMem(execfile, 16, dst + 1); //A
    copyRegFromMem(execfile, 17, dst + 2); //B
        
    //Add, then store.
    geBool(execfile, 16, 17, 18);
    copyRegToMem(execfile, dst, 18);

}

void boolGtOperation(FILE* execfile, char* partA, char* partB, int dst) {
    
    if(*partB == '=') {
        boolGeOperation(execfile, partA, &partB[1], dst);
        return;
    }

    //Compute the two subcomponents
    pemdas(execfile, partA, dst + 1);
    pemdas(execfile, partB, dst + 2);
        
    //Grab the results
    copyRegFromMem(execfile, 16, dst + 1); //A
    copyRegFromMem(execfile, 17, dst + 2); //B
        
    //Add, then store.
    gtBool(execfile, 16, 17, 18);
    copyRegToMem(execfile, dst, 18);

}

void boolLeOperation(FILE* execfile, char* partA, char* partB, int dst) {

    //Compute the two subcomponents
    pemdas(execfile, partA, dst + 1);
    pemdas(execfile, partB, dst + 2);
        
    //Grab the results
    copyRegFromMem(execfile, 16, dst + 1); //A
    copyRegFromMem(execfile, 17, dst + 2); //B
        
    //Add, then store.
    leBool(execfile, 16, 17, 18);
    copyRegToMem(execfile, dst, 18);

}

void boolLtOperation(FILE* execfile, char* partA, char* partB, int dst) {

    if(*partB == '=') {
        boolLeOperation(execfile, partA, &partB[1], dst);
        return;
    }

    //Compute the two subcomponents
    pemdas(execfile, partA, dst + 1);
    pemdas(execfile, partB, dst + 2);
        
    //Grab the results
    copyRegFromMem(execfile, 16, dst + 1); //A
    copyRegFromMem(execfile, 17, dst + 2); //B
        
    //Add, then store.
    ltBool(execfile, 16, 17, 18);
    copyRegToMem(execfile, dst, 18);

}



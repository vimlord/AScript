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



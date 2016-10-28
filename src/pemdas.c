#include "pemdas.h"

#include "asmcommands.h"
#include "srccompile.h"

#include <string.h>
#include <stdlib.h>

int pemdascomps = 0;

void addOperation(FILE* execfile, char* partA, char* partB, int n) {

    //Compute the two subcomponents
    pemdas(execfile, partA, n);
    pemdas(execfile, partB, n);
    
    //Grab the results
    int i = 0;
    while(i < 2*n) stackPop(execfile, 16+(i++));

    //Add sequentially
    addReg(execfile, 16, 16+n);
    i = 1;
    while(i < n) {
        char buff[64];
        sprintf(buff, "adc %i, %i\n", 16+i, 16+i+n);
        i++;
    }
    
    //Push to the stack
    i = n-1;
    while(i >= 0)
        stackPush(execfile, 16+i--);

}


void subOperation(FILE* execfile, char* partA, char* partB, int n) {

    //Compute the two subcomponents
    pemdas(execfile, partA, n);
    pemdas(execfile, partB, n);
    
    //Grab the results
    int i = 0;
    while(i < 2*n) stackPop(execfile, 16+i++);

    //Add sequentially
    subReg(execfile, 16, 16+n);
    i = 1;
    while(i < n) {
        char buff[64];
        sprintf(buff, "sbc %i, %i\n", 16+i, 16+i+n);
    }
    
    //Push to the stack
    i = n-1;
    while(i >= 0)
        stackPush(execfile, 16+i--);
    }


void mulOperation(FILE* execfile, char* partA, char* partB, int n) {

    //Compute the two subcomponents
    pemdas(execfile, partA, n);
    pemdas(execfile, partB, n);
    
    //Grab the results
    int i = 0;
    while(i < 2*n) stackPop(execfile, 18+i++);
    
    writeComment(execfile, "Perform multiplication");

    i = n;
    while(i) {
        int j = n-i;
        while(j < i) {
            //Multiply index j of first w/ n-j of second
            mulRegs(execfile, 18+j, 18+2*n-j-1);
            
            writeAsmBlock(execfile, "add r16, r0\nadc r17, r1\n");

            j++;
        }

        if(i < n)
            stackPush(execfile, 17);
        copyReg(execfile, 17, 16); 

        i--;
    }
    
    stackPush(execfile, 17);
}

void bitAndOperation(FILE* execfile, char* partA, char* partB, int n) {

    //Compute the two subcomponents
    pemdas(execfile, partA, n);
    pemdas(execfile, partB, n);

    //Grab the results
    int i = 0;
    while(i < 2*n) {
        stackPop(execfile, 16+i++);
    }

    //Add, then store.
    i = 0;
    while(i < n) {
        andReg(execfile, 16+i, 16+n+i);
        i++;
    }

    i = n;
    while(i)
        stackPush(execfile, --i);
}

void bitOrOperation(FILE* execfile, char* partA, char* partB, int n) {

    //Compute the two subcomponents
    pemdas(execfile, partA, n);
    pemdas(execfile, partB, n);

    //Grab the results
    int i = 0;
    while(i < 2*n) {
        stackPop(execfile, 16+i++);
    }

    //Add, then store.
    i = 0;
    while(i < n) {
        orReg(execfile, 16+i, 16+n+i);
        i++;
    }

    i = n;
    while(i)
        stackPush(execfile, --i);

}

void bitXorOperation(FILE* execfile, char* partA, char* partB, int n) {
    
    //Compute the two subcomponents
    pemdas(execfile, partA, n);
    pemdas(execfile, partB, n);

    //Grab the results
    int i = 0;
    while(i < 2*n) {
        stackPop(execfile, 16+i++);
    }

    //Add, then store.
    i = 0;
    while(i < n) {
        xorReg(execfile, 16+i, 16+n+i);
        i++;
    }

    i = n;
    while(i)
        stackPush(execfile, --i);


}

void boolEqOperation(FILE* execfile, char* partA, char* partB, int n) {
    
    if(*partB != '=') {
        //Bad input
        printf("Error during compilation: Invalid use of arithmetic operators.\n");
        exit(EINVAL);
    }
    char label[64];
    sprintf(label, "pemdcmp%i", pemdascomps++);

    //Compute the two subcomponents
    pemdas(execfile, partA, n);
    pemdas(execfile, &partB[1], n);
    
    //Grab the results
    int i = 0;
    while(i < 2*n) {
        stackPop(execfile, 16+i++);
    }
    
    loadReg(execfile, 16+2*n, "1");

    i = 0;
    while(i < n) {
        branchEQ(execfile, 16+i, 16+n+i, label);
        i++;
    }
    
    loadReg(execfile, 16+2*n, "0");
    writeAsmBlock(execfile, label);
    writeAsmBlock(execfile, ":\n");

    //Add, then store.
    stackPush(execfile, 16+2*n);
}

void boolNeOperation(FILE* execfile, char* partA, char* partB, int n) {
    
    if(*partB != '=') {
        //Something like computing !x
        boolEqOperation(execfile, partA, partB, n);

        return;
    }

    char label[64];
    sprintf(label, "pemdcmp%i", pemdascomps++);

    //Compute the two subcomponents
    pemdas(execfile, partA, n);
    pemdas(execfile, &partB[1], n);
    
    //Grab the results
    int i = 0;
    while(i < 2*n) {
        stackPop(execfile, 16+i++);
    }
    
    loadReg(execfile, 16+2*n, "1");

    i = 0;
    while(i < n) {
        branchNE(execfile, 16+i, 16+n+i, label);
        i++;
    }
    
    loadReg(execfile, 16+2*n, "0");
    writeAsmBlock(execfile, label);
    writeAsmBlock(execfile, ":\n");

    //Add, then store.
    stackPush(execfile, 16+2*n);
}

void boolGeOperation(FILE* execfile, char* partA, char* partB, int n) {
    
    //Compute the two subcomponents
    pemdas(execfile, partA, n);
    pemdas(execfile, partB, n);
 
    char label[64];
    sprintf(label, "pemdcmp%i", pemdascomps++);

    //Compute the two subcomponents
    pemdas(execfile, partA, n);
    pemdas(execfile, &partB[1], n);
    
    //Grab the results
    int i = 0;
    while(i < 2*n) {
        stackPop(execfile, 16+i++);
    }
    
    loadReg(execfile, 16+2*n, "1");

    i = 0;
    while(i < n) {
        branchNE(execfile, 16+i, 16+n+i, label);
        i++;
    }
    
    loadReg(execfile, 16+2*n, "0");
    writeAsmBlock(execfile, label);
    writeAsmBlock(execfile, ":\n");

    //Add, then store.
    stackPush(execfile, 16+2*n);       
}

void boolGtOperation(FILE* execfile, char* partA, char* partB, int n) {
    
    if(*partB == '=') {
        boolGeOperation(execfile, partA, &partB[1], n);
        return;
    }

    char label[64];
    sprintf(label, "pemdcmp%i", pemdascomps++);

    //Compute the two subcomponents
    pemdas(execfile, partA, n);
    pemdas(execfile, &partB[1], n);
    
    //Grab the results
    int i = 0;
    while(i < 2*n) {
        stackPop(execfile, 16+i++);
    }
    
    loadReg(execfile, 16+2*n, "1");

    i = 0;
    while(i < n) {
        branchNE(execfile, 16+i, 16+n+i, label);
        i++;
    }
    
    loadReg(execfile, 16+2*n, "0");
    writeAsmBlock(execfile, label);
    writeAsmBlock(execfile, ":\n");

    //Add, then store.
    stackPush(execfile, 16+2*n);
}

void boolLeOperation(FILE* execfile, char* partA, char* partB, int n) {

    //Compute the two subcomponents
    pemdas(execfile, partA, n);
    pemdas(execfile, partB, n);
 
    char label[64];
    sprintf(label, "pemdcmp%i", pemdascomps++);

    //Compute the two subcomponents
    pemdas(execfile, partA, n);
    pemdas(execfile, &partB[1], n);
    
    //Grab the results
    int i = 0;
    while(i < 2*n) {
        stackPop(execfile, 16+i++);
    }
    
    loadReg(execfile, 16+2*n, "1");

    i = 0;
    while(i < n) {
        branchNE(execfile, 16+i, 16+n+i, label);
        i++;
    }
    
    loadReg(execfile, 16+2*n, "0");
    writeAsmBlock(execfile, label);
    writeAsmBlock(execfile, ":\n");

    //Add, then store.
    stackPush(execfile, 16+2*n); 
}

void boolLtOperation(FILE* execfile, char* partA, char* partB, int n) {

    if(*partB == '=') {
        boolLeOperation(execfile, partA, &partB[1], n);
        return;
    }

    //Compute the two subcomponents
    pemdas(execfile, partA, n);
    pemdas(execfile, partB, n);
 
    char label[64];
    sprintf(label, "pemdcmp%i", pemdascomps++);

    //Compute the two subcomponents
    pemdas(execfile, partA, n);
    pemdas(execfile, &partB[1], n);
    
    //Grab the results
    int i = 0;
    while(i < 2*n) {
        stackPop(execfile, 16+i++);
    }
    
    loadReg(execfile, 16+2*n, "1");

    i = 0;
    while(i < n) {
        branchNE(execfile, 16+i, 16+n+i, label);
        i++;
    }
    
    loadReg(execfile, 16+2*n, "0");
    writeAsmBlock(execfile, label);
    writeAsmBlock(execfile, ":\n");

    //Add, then store.
    stackPush(execfile, 16+2*n);       
}



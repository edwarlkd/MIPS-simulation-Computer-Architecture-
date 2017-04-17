#include <stdio.h>
#include <stdlib.h>
#include "computer.h"
	
#define TRUE 1
#define FALSE 0



int main (int argc, char *argv[]) {
    int argIndex;
    int printingRegisters = FALSE;
    int printingMemory = FALSE;
    int debugging = FALSE;
    int interactive = FALSE;
    FILE *filein;
    

     for(int i = 0; i < argc; i++){
	     const char *filename1 = argv[i];
	     fprintf(stdout, "\"%s\"\n", filename1);  
     }  
   

    if (argc < 2) {
        fprintf (stderr, "Not enough arguments.\n");
        exit (1);
    }
    for (argIndex=1; argIndex<argc && argv[argIndex][0]=='-'; argIndex++) {
        //Argument is an option, we hope one of -r, -m, -i, -d. 
        switch (argv[argIndex][1]) {
            case 'r':	//print all registers; without it only affected one printed
            printingRegisters = TRUE;
            break;
            case 'm': //print all data memory locations with nonzero values after instruction
            printingMemory = TRUE;
            break;
         case 'i': //run in interactive mode, type a return before simulating each insturction 
            interactive = TRUE;
            break;
            case 'd': // debugging
            debugging = TRUE;
            break;
            default:
            fprintf (stderr, "Invalid option \"%s\".\n", argv[argIndex]);
            fprintf (stderr, "Correct options are -r, -m, -i, -d.\n");
            exit (1);
        }
    }
    if (argIndex == argc) {
        fprintf (stderr, "No file name given.\n");
        exit (1);
    } else if (argIndex < argc-1) {
        fprintf (stderr, "Too many arguments.\n");
        exit (1);
    }
    
    filein = fopen (argv[argIndex], "r");
    if (filein == NULL) {
        fprintf (stderr, "Can't open file: %s\n", argv[argIndex]);
        exit (1);
    }

    InitComputer (filein, printingRegisters, printingMemory,
	debugging, interactive);
    Simulate ();

  
  return 0;
}

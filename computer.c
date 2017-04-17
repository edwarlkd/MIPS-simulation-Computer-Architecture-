#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include "computer.h"

#undef mips			/* gcc already has a def for mips */


unsigned int endianSwap(unsigned int);
int numbersToBits(int _newpc, int _add);
void PrintInfo (int changedReg, int changedMem);
unsigned int Fetch (int);
void Decode (unsigned int, DecodedInstr*, RegVals*);
int Execute (DecodedInstr*, RegVals*);
int Mem(DecodedInstr*, int, int *);
void RegWrite(DecodedInstr*, int, int *);
void UpdatePC(DecodedInstr*, int);
void PrintInstruction (DecodedInstr*);

/*Globally accessible Computer variable*/
Computer mips;
RegVals rVals;

/*
 *  Return an initialized computer with the stack pointer set to the
 *  address of the end of data memory, the remaining registers initialized
 *  to zero, and the instructions read from the given file.
 *  The other arguments govern how the program interacts with the user.
 */
void InitComputer (FILE* filein, int printingRegisters, int printingMemory,
  int debugging, int interactive) {
    int k;
    unsigned int instr;
	    
    /* stack pointer - Initialize to highest address of data segment */
    mips.registers[29] = 0x00400000 + (MAXNUMINSTRS+MAXNUMDATA)*4;

    for (k=0; k<MAXNUMINSTRS+MAXNUMDATA; k++) {
        mips.memory[k] = 0;
    }

    k = 0;
    while (fread(&instr, 4, 1, filein)) {
	/*swap to big endian, convert to host byte order. Ignore this.*/
        mips.memory[k] = ntohl(endianSwap(instr));
        k++;
        if (k>MAXNUMINSTRS) {
            fprintf (stderr, "Program too big.\n");
            exit (1);
        }
    }

	
    mips.printingRegisters = printingRegisters;
    mips.printingMemory = printingMemory;
    mips.interactive = interactive;
    mips.debugging = debugging;
}

unsigned int endianSwap(unsigned int i) {
    return (i>>24)|(i>>8&0x0000ff00)|(i<<8&0x00ff0000)|(i<<24);
}

/*
 *  Run the simulation.
 */
void Simulate () {
    char s[40];  /* used for handling interactive input */
    unsigned int instr;
    int changedReg=-1, changedMem=-1, val;
    DecodedInstr d;
    rVals.R_rs = 0;
    rVals.R_rt = 0;
    rVals.R_rd = 0;

    
    /* Initialize the PC to the start of the code section */
    mips.pc = 0x00400000;
    while (1) {
        if (mips.interactive) {
            printf ("> ");
            fgets (s,sizeof(s),stdin);
            if (s[0] == 'q') {
                return;
            }
        }


        instr = Fetch (mips.pc);

	printf("pc: %8.8x   instr: %8.8x \n", mips.pc, instr);

        /* 
	 * Decode instr, putting decoded instr in d
	 * Note that we reuse the d struct for each instruction.
	 */
        Decode (instr, &d, &rVals);	

	//Printing the decoded insturction
	PrintInstruction(&d);

        /* 
	 * Perform computation needed to execute d, returning computed value 
	 * in val 
	 */
        val = Execute(&d, &rVals);

	int forbug = val;
	UpdatePC(&d,val);

        /* 
	 * Perform memory load or store. Place the
	 * address of any updated memory in *changedMem, 
	 * otherwise put -1 in *changedMem. 
	 * Return any memory value that is read, otherwise return -1 (to *changedMem, not return value);
         */
        val = Mem(&d, val, &changedMem);

        /* 
	 * Write back to register. If the instruction modified a register--
	 * (including jal, which modifies $ra) --
         * put the index of the modified register in *changedReg,
         * otherwise put -1 in *changedReg.
         */
	val = forbug;
        RegWrite(&d, val, &changedReg);

        PrintInfo (changedReg, changedMem);

    }
}

/*
 *  Print relevant information about the state of the computer.
 *  changedReg is the index of the register changed by the instruction
 *  being simulated, otherwise -1.
 *  changedMem is the address of the memory location changed by the
 *  simulated instruction, otherwise -1.
 *  Previously initialized flags indicate whether to print all the
 *  registers or just the one that changed, and whether to print
 *  all the nonzero memory or just the memory location that changed.
 */
void PrintInfo ( int changedReg, int changedMem) {
    int k, addr;
    printf ("New pc = %8.8x \n", mips.pc);
    if (!mips.printingRegisters && changedReg == -1) {
        printf ("No register was updated.\n");
    } else if (!mips.printingRegisters) {
        printf ("Updated r%2.2d to %8.8x\n",
        changedReg, mips.registers[changedReg]);
    } else {
        for (k=0; k<32; k++) {
            printf ("r%2.2d: %8.8x  ", k, mips.registers[k]);
            if ((k+1)%4 == 0) {
                printf ("\n");
            }
        }
    }
    if (!mips.printingMemory && changedMem == -1) {
        printf ("No memory location was updated.\n");
    } else if (!mips.printingMemory) {
        printf ("Updated memory at address %8.8x to %8.8x\n",
        changedMem, Fetch (changedMem));
    } else {
        printf ("Nonzero memory\n");
        printf ("ADDR	  CONTENTS\n");
        for (addr = 0x00400000+4*MAXNUMINSTRS;
             addr < 0x00400000+4*(MAXNUMINSTRS+MAXNUMDATA);
             addr = addr+4) {
            if (Fetch (addr) != 0) {
                printf ("%8.8x  %8.8x\n", addr, Fetch (addr));
            }
        }
    }
}

/*
 *  Return the contents of memory at the given address. Simulates
 *  instruction fetch.
 */
unsigned int Fetch ( int addr) {
    return mips.memory[(addr-0x00400000)/4]; 
}


//from 0
int bitsToDecimal(char *c, int from, int n)
{			//      5        6 so, 0-5

	char *temp = c;
	int bitvalue = 1;
	int sum = 0;

		int counter = n-1;
		//check every bits up from 0 to index

		int forloopcount = 0;
		for(int i = from; i > from - n; i--)
		{	forloopcount++;
//			printf("%c", c[from - counter ]);
			
			//check the valuex
			if(temp[i] == '1'){
				sum = sum + bitvalue;	
			}
			bitvalue *= 2;
			counter--;
		}
//		printf("total # of loop was %d", forloopcount);

		
		for(int i = 0; i < n; i++)
		{
//			printf("%c", c[i]);

		}
	
	return sum;
}

/* Decode instr, returning decoded instruction. */
void Decode ( unsigned int instr, DecodedInstr* d, RegVals* rVals) {

//	printf("\n***Decode().........\n");

	//now convert the instruction into a binary. 
	unsigned int temp = instr;
	char bit32[]  = "00000000000000000000000000000000";
	for (int pos = 31; pos >= 0; --pos)
	{
		if (temp % 2) 
		    bit32[pos] = '1';
	
		temp /= 2;
	}

	//output	
/*	for(int i = 0; i < 32; i++)
	{
		printf("%c", bit32[i]);
	}
*/

	//Fill in the 'd'
	//now translate opcode and find its appropriate InstraType 'type'		
	int opcodesum = bitsToDecimal(&bit32, 5, 6);

	if(opcodesum == 0){ //R-type
		d->type = 0;		  //start of index, number of value
		d->op = bitsToDecimal(&bit32, 5, 6);
		d->regs.r.rs = bitsToDecimal(&bit32, 10, 5);
		d->regs.r.rt = bitsToDecimal(&bit32, 15, 5); 
		d->regs.r.rd = bitsToDecimal(&bit32, 20, 5);
		d->regs.r.shamt = bitsToDecimal(&bit32, 25, 5);
		d->regs.r.funct = bitsToDecimal(&bit32, 31, 6);
		
		rVals->R_rs = mips.registers[d->regs.r.rs];
		rVals->R_rt = mips.registers[d->regs.r.rt];
		rVals->R_rd = mips.registers[d->regs.r.rd];
		
//printf("R-type d.op: %d rs: %d  rt: %d  rd: %d  shamt: %d  funct: %d \n", d->op, d->regs.r.rs, d->regs.r.rt, d->regs.r.rd, d->regs.r.shamt, d->regs.r.funct); 		

	}	
	else if(opcodesum == 2 || opcodesum == 3){ //J-type --- j or jal
		d->type = 2;
		d->op = opcodesum; 
		unsigned int addressinDec = bitsToDecimal(bit32, 31, 16);
		d->regs.j.target = bitsToDecimal(bit32, 31, 26);
	}	
	else{ //I-type
		d->type = 1;
		d->op = opcodesum;		   	
		d->regs.i.rs = bitsToDecimal(bit32, 10, 5);
		d->regs.i.rt = bitsToDecimal(bit32, 15, 5);
		d->regs.i.addr_or_immed = bitsToDecimal(bit32, 31, 16);
				
		rVals->R_rs = mips.registers[d->regs.r.rs];
		rVals->R_rt = mips.registers[d->regs.r.rt];

//		printf("I-type  d.op = %d   rs = %d   rt = %d   addr_or_immed = %d  \n", d->op, d->regs.i.rs, d->regs.i.rt, d->regs.i.addr_or_immed);   

	}
	


}

/*
 *  Print the disassembled version of the given instruction
 *  followed by a newline.
 */
void PrintInstruction ( DecodedInstr* d) {
	switch(d->type){
	case R:
//		printf("In PrintInstruction():  d.type: %d   d.funct: %d \n", d->type, d->regs.r.funct);

		if(d->regs.r.funct == 0)
		{	
		printf("sll	$%d, $%d, %d \n", d->regs.r.rd, d->regs.r.rt, d->regs.r.shamt);
		}
		else if(d->regs.r.funct== 2)
		{ 
		printf("srl	$%d, $%d, $%d \n", d->regs.r.rd, d->regs.r.rt, d->regs.r.shamt);
   		}
		else if(d->regs.r.funct == 8)
		{ //jr $rs; value in rs
		printf("jr	$%d \n", d->regs.r.rs);			
		}
		else if(d->regs.r.funct == 33)
		{
		printf("addu	$%d, $%d, $%d \n", d->regs.r.rd, d->regs.r.rs, d->regs.r.rt);
		}
		else if(d->regs.r.funct == 35)
		{
		printf("subu	$%d, $%d, $%d \n", d->regs.r.rd, d->regs.r.rs, d->regs.r.rt);
		}
		else if(d->regs.r.funct == 36)
		{
		printf("and	$%d, $%d, $%d \n", d->regs.r.rd, d->regs.r.rs, d->regs.r.rt);
		}
		else if(d->regs.r.funct == 37)
		{
		printf("or	$%d, $%d, $%d \n", d->regs.r.rd, d->regs.r.rs, d->regs.r.rt);
		}
		else if(d->regs.r.funct == 42)
		{//slt?
		printf("slt	$%d, $%d, $%d \n", d->regs.r.rd, d->regs.r.rs, d->regs.r.rt);
		}
		else{
			exit(1);
		}
		break;

	case I:
//		printf("In PrintInstruction():  d.type: %d   d.op: %d \n", d->type, d->op);
		if(d->op == 4){
	printf("beq	$%d, $%d, 0x%08x \n", d->regs.i.rs, d->regs.i.rt, d->regs.i.addr_or_immed);
		}	
		else if(d->op == 5){
	printf("bne	$%d, $%d, 0x%08x \n", d->regs.i.rs, d->regs.i.rt, d->regs.i.addr_or_immed);
		}
		else if(d->op == 9){
	printf("addiu	$%d, $%d, %d \n", d->regs.i.rt, d->regs.i.rs, (short int)d->regs.i.addr_or_immed);
		}
		else if(d->op == 12){
	printf("andi	$%d, $%d, %d \n", d->regs.i.rt, d->regs.i.rs, d->regs.i.addr_or_immed);
		}
		else if(d->op == 13){
	printf("ori	$%d, $%d, 0x%04x \n", d->regs.i.rt, d->regs.i.rs, d->regs.i.addr_or_immed);
		}
		else if(d->op == 15){
	printf("lui	$%d, 0x%04x \n", d->regs.i.rt, d->regs.i.addr_or_immed);
		}
		else if(d->op == 35){
	printf("lw	$%d, %d($%d) \n", d->regs.i.rt, d->regs.i.addr_or_immed, d->regs.i.rs);
		}
		else if(d->op == 43){
	printf("sw	$%d, %d($%d) \n", d->regs.i.rt, d->regs.i.addr_or_immed, d->regs.i.rs);
		}
		else{
			exit(1);
		}
		
		break;

	case J:
		printf("In PrintInstruction():  d.type: %d   d.op: %d \n", d->type, d->op);
		if(d->op == 2){
			int x = numbersToBits(mips.pc+4, d->regs.j.target);
			printf("j	0x%08x \n", x);	
		}
		else if(d->op == 3){
			int x = numbersToBits(mips.pc+4, d->regs.j.target);
			//printf("jal	%8.8x \n", x);
			printf("jal	0x%08x \n", x);
		}
		else{
			exit(1);
		}
		break;
	}	

}




/* Perform computation needed to execute d, returning computed value */
int Execute ( DecodedInstr* d, RegVals* rVals) {
    /* Your code goes here */

	//check the type
	//and perform each instruction 
	switch(d->type){

	case R:
//		printf("inside R PrintInstruc(), d->op: %d \n", d->op);
		if(d->regs.r.funct == 0)
		{	
//		printf("sll	$%s, $%s, $%s \n", rdName, rtName, d->regs.r.shamt//);
			return rVals->R_rt << d->regs.r.shamt;
		}
		else if(d->regs.r.funct== 2)
		{ //srl	$rd, $rt, shamt
//		printf("srl	$%s, $%s, $%s \n", rdName, rtName, d->regs.r.shamt);
			return rVals->R_rt >> d->regs.r.shamt;		
   		}
		else if(d->regs.r.funct == 8)
		{
//		printf("jr	$%d \n", rsName);
			return mips.registers[d->regs.r.rs];
		}
		else if(d->regs.r.funct == 32)
		{
//		printf("and	$%s, $%s, $%s \n", rdName, rsName, rtName);
			return 	rVals->R_rs & rVals->R_rt;
		}
		else if(d->regs.r.funct == 33)
		{
//		printf("addu	$%s, $%s, $%s \n", rdName, rsName, rtName);
			return 	rVals->R_rs + rVals->R_rt;

		}
		else if(d->regs.r.funct == 35)
		{
//		printf("subu	$%s, $%s, $%s \n", rdName, rsName, rtName);
			return 	rVals->R_rs - rVals->R_rt;

		}
		else if(d->regs.r.funct == 37)
		{
//		printf("or	$%s, $%s, $%s \n", rdName, rsName, rtName);
			return rVals->R_rs | rVals->R_rt;
		}
		else if(d->regs.r.funct == 42) // slt
		{ 
			//if rs < rt, then rd = 1			
			if(rVals->R_rs < rVals->R_rt)
			{
				//mips.registers[d->regs.r.rd] = 1;
				return 1; 
			}
			else
			{
				//mips.registers[d->regs.r.rd] = 0;
				return 0; 			
			}
		}
		break;

	case I:
		if(d->op == 4){
//	printf("beq	$%s, $%s, %d \n", rsName, rtName, d->regs.i.addr_or_immed);
			if(rVals->R_rs == rVals->R_rt)
			{				 
				return (((unsigned int)d->regs.i.addr_or_immed << 2) + 4);
			}
			else
			{
				return 4; // for pc += 4
			}
		}

		if(d->op == 5){
//	printf("bne	$%s, $%s, %d \n", rsName, rtName, d->regs.i.addr_or_immed);
			if(rVals->R_rs != rVals->R_rt)
			{
				return 	((((unsigned int)d->regs.i.addr_or_immed) << 2) + 4);
			}
			else{
				return 4; // for pc += 4
			}
		}
		if(d->op == 9){
//	printf("addiu	$%s, $%s, %d \n", rdName, rsName, d->regs.i.addr_or_immed);
			return ((unsigned int)rVals->R_rs + (unsigned int)d->regs.i.addr_or_immed);
		}
		if(d->op == 12){
//	printf("andi	$%s, $%s, %d \n", rtName, rsName, d->regs.i.addr_or_immed);
			return (rVals->R_rs & d->regs.i.addr_or_immed);
		}
		if(d->op == 13){
//	sprintf("ori	$%s, $%s, %d \n", rtName, rsName, d->regs.i.addr_or_immed);
			return rVals->R_rs | d->regs.i.addr_or_immed;
		}
		if(d->op == 15){ //lui
//	printf("lui	$%s, %d \n", rtName, d->regs.i.addr_or_immed);
			return d->regs.i.addr_or_immed << 16;
		}
		if(d->op == 35){ //LW
		//R[$rt] <- Mem 4B (R[$rs] + SignExt 16b (imm))
			return ((unsigned int)rVals->R_rs + (unsigned int)d->regs.i.addr_or_immed); //used in Mem()

		}
		if(d->op == 43){ //SW
//	printf("sw	$%s, %d($%d) \n", rtName, d->regs.i.addr_or_immed, rsName);
			 return rVals->R_rt;
		}
		break;

	case J:
//		printf("inside J PrintInstruc(), d->op: %d \n", d->op);
			return mips.pc+4; //jump execution done in UpdatePC

		break;
	}
			
  return 0;
}

//*bit32 = numbersToBits(mips.pc+4, d->regs.i.addr_or_immed);
int numbersToBits(int _newpc, int _add){

	char finalBit32[] = "00000000000000000000000000000000";

	//use first 4 byte of the _newpc [31:28]... or [0-3] for our case.	
	char bit2_32[] = "00000000000000000000000000000000";
	int temp = _newpc;
	
	//first conver _newpc into binary, then extract the first 4 bits
	char pc4[4];
	for(int pos =31; pos >= 0; --pos)
	{
		if (_newpc % 2) 
		   bit2_32[pos] = '1';
	
		temp /= 2;
	}	

	pc4[0] = bit2_32[3];
	pc4[1] = bit2_32[2];
	pc4[2] = bit2_32[1];
	pc4[3] = bit2_32[0];
	
	//this time for _add (jump address)... Last 2 bits are 0.
	temp = _add;
	for (int pos = 31; pos >= 0; --pos) // changed from 31
	{

		if (temp % 2) 
		   bit2_32[pos] = '1';
	
		temp /= 2;
	}

	//now insert them all into finalBit32
	finalBit32[0] = pc4[0];
	finalBit32[1] = pc4[1];
	finalBit32[2] = pc4[2];
	finalBit32[3] = pc4[3]; 
	finalBit32[30] = pc4[3]; //works in placement of 0
	finalBit32[31] = pc4[3];

	int countdown = 31;
	for(int i = 29; i >= 4; --i){
		finalBit32[i] = bit2_32[countdown];
		countdown--;
	}
		
	//output
//	printf("	----------->  Finalbit32: "); 
/*	for(int i = 0; i < 32; i++)
	{
		printf("%c", finalBit32[i]);
	}
	printf("\n");
*/
	//now convert from the 32-bits to integer for the configured set.
	int huh = bitsToDecimal(finalBit32, 31, 32);
	return huh;
	
}


/* 
 * Update the program counter based on the current instruction. For
 * instructions other than branches and jumps, for example, the PC
 * increments by 4 (which we have provided).
 */
void UpdatePC ( DecodedInstr* d, int val) {
	char bit32[] = "00000000000000000000000000000";	
	switch(d->type){
	case R:
		if(d->regs.r.funct == 8)
		{
			mips.pc = val;		
		}
		else
		{
			mips.pc += 4;
		}
		break;

	case I:
		if(d->op == 4 || d->op == 5){
 				mips.pc += val;
		}
		else{
			mips.pc += 4;
		}

		break;

	case J:			
		mips.pc = numbersToBits(mips.pc+4, d->regs.j.target);
		break;
	}

}

/*
 * Perform memory load or store. Place the address of any updated memory 
 * in *changedMem, otherwise put -1 in *changedMem. Return any memory value 
 * that is read, otherwise return -1. 
 *
 * Remember that we're mapping MIPS addresses to indices in the mips.memory 
 * array. mips.memory[0] corresponds with address 0x00400000, mips.memory[1] 
 * with address 0x00400004, and so forth.
 *
 */
int Mem( DecodedInstr* d, int val, int *changedMem){
    /* Your code goes here */
	switch(d->type){

	//case R and J has no Mem association
	case I:
		if(d->op == 43){ // sw: from register to memory
			int immd = (short int)d->regs.i.addr_or_immed;
			int x = 0x00400000+(mips.registers[d->regs.i.rt] + immd);
			int x2 = (x-0x00400000)/4;
			mips.memory[x2] = val;
			*changedMem = x;
			return val;
		}
		else if(d->op == 35){ //lw: load memory value being read; return that. otherwise -1
			*changedMem = -1;
			int x = 0x00400000+val; //0x00400000 is the start of the memory[] after bigendian swap from Initiate()		
			return Fetch(x);
		}				

		break;
	default:
		//no memory need for R and J types
		*changedMem = -1;
		return val; //without this, 0 gets in placed for val...
	}

  return 0;
}

/* 
 * Write back to register. If the instruction modified a register--
 * (including jal, which modifies $ra) --
 * put the index of the modified register in *changedReg,
 * otherwise put -1 in *changedReg.
 */
void RegWrite( DecodedInstr* d, int val, int *changedReg) {
    /* Your code goes here */

switch(d->type){

	case R:
		if(d->regs.r.funct == 0 || d->regs.r.funct == 2 || d->regs.r.funct == 32 || d->regs.r.funct == 33 || d->regs.r.funct == 35 || d->regs.r.funct == 37 || d->regs.r.funct == 42 )
		{	//from execute(), val is already configured.
			//so here, we just push it to the correct register.
			mips.registers[d->regs.r.rd] = val;
			*changedReg = d->regs.r.rd;
		}
		else{
			//no change in registers.
			*changedReg = -1;
		}
		break;

	case I:
		if(d->op == 9 || d->op == 12 || d->op == 13 || d->op == 15){
			mips.registers[d->regs.i.rt] = val;
			*changedReg = d->regs.r.rt;			
		}	
		else if(d->op == 35){ // lw 
			mips.registers[d->regs.r.rd] = val;
			*changedReg = d->regs.r.rd;
		}
		else{
			*changedReg = -1;
		}
		break;

	case J:
		if(d->op == 3){ // jal
			mips.registers[31] = val;
			*changedReg = 31;			
		}
		else{
			*changedReg = -1;
		}
		break;
	}
	

}

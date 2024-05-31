/* Kod inspirovany testovacim programom na kontrolu maticoveho displeja a kodami z cviceni predmetu IMP */

/* Header file with all the essential definitions for a given type of MCU */
//#include "MK60DZ10.h"
#include "MK60D10.h"


/* Macros for bit-level registers manipulation */
#define GPIO_PIN_MASK	0x1Fu
#define GPIO_PIN(x)		(((1)<<(x & GPIO_PIN_MASK)))


/* Constants specifying delay loop duration */
#define	tdelay1			10000
#define tdelay2 		20


/* Row values for writing letters */
#define LetterSpace {0,0,0,0}

#define LetterA {255, 144, 144, 255}
#define LetterB {255, 145, 145, 255}
#define LetterC {255, 129, 129, 129}
#define LetterD {255, 129, 66, 60}
#define LetterE {255, 145, 145, 145}
#define LetterF {255, 144, 144, 144}
#define LetterG {255, 129, 137, 143}
#define LetterH {255, 24, 24, 255}
#define LetterI {0, 129, 255, 129}
#define LetterJ {135, 129, 129, 255}
#define LetterK {255, 24, 36, 195}
#define LetterL {255, 1, 1, 1}
#define LetterM {255, 32, 32, 255}
#define LetterN {255, 120, 30, 255}
#define LetterO {255, 129, 129, 255}
#define LetterP {255, 144, 144, 240}
#define LetterQ {254, 130, 134, 255}
#define LetterR {255, 152, 148, 243}
#define LetterS {249, 137, 137, 143}
#define LetterT {0, 128, 255, 128}
#define LetterU {255, 1, 1, 255}
#define LetterV {252, 3, 3, 252}
#define LetterW {255, 3, 3, 255}
#define LetterX {231, 24, 24, 231}
#define LetterY {128, 64, 63, 192}
#define LetterZ {159, 145, 145, 241}

#define Letter1 {0, 32, 64, 255}
#define Letter3 {145, 145, 145, 255}
#define Letter4 {24, 40, 79, 136}
#define Letter6 {255, 137, 137, 143}
#define Letter7 {128, 136, 255, 8}
#define Letter9 {240, 144, 144, 255}

/* Button masks for interrupts */
#define BUTTON1_MASK 0x400
#define BUTTON2_MASK 0x800
#define BUTTON3_MASK 0x1000
#define BUTTON4_MASK 0x4000000
#define BUTTON5_MASK 0x8000000

/* Variable used for changing printing text */
/* Value 2 is starting text */
int PrintText = 2;


/* Configuration of the necessary MCU peripherals */
void SystemConfig() {
	/* Turn on all port clocks */
	SIM->SCGC5 = SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTE_MASK;
	/*Enable clock on PIT*/
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;


	/* Set corresponding PTA pins (column activators of 74HC154) for GPIO functionality */
	PORTA->PCR[8] = ( 0|PORT_PCR_MUX(0x01) );  // A0
	PORTA->PCR[10] = ( 0|PORT_PCR_MUX(0x01) ); // A1
	PORTA->PCR[6] = ( 0|PORT_PCR_MUX(0x01) );  // A2
	PORTA->PCR[11] = ( 0|PORT_PCR_MUX(0x01) ); // A3

	/* Set corresponding PTA pins (rows selectors of 74HC154) for GPIO functionality */
	PORTA->PCR[26] = ( 0|PORT_PCR_MUX(0x01) );  // R0
	PORTA->PCR[24] = ( 0|PORT_PCR_MUX(0x01) );  // R1
	PORTA->PCR[9] = ( 0|PORT_PCR_MUX(0x01) );   // R2
	PORTA->PCR[25] = ( 0|PORT_PCR_MUX(0x01) );  // R3
	PORTA->PCR[28] = ( 0|PORT_PCR_MUX(0x01) );  // R4
	PORTA->PCR[7] = ( 0|PORT_PCR_MUX(0x01) );   // R5
	PORTA->PCR[27] = ( 0|PORT_PCR_MUX(0x01) );  // R6
	PORTA->PCR[29] = ( 0|PORT_PCR_MUX(0x01) );  // R7

	/* Set corresponding PTE pins (output enable of 74HC154) for GPIO functionality */
	PORTE->PCR[28] = ( 0|PORT_PCR_MUX(0x01) ); // #EN


	/* Change corresponding PTA port pins as outputs */
	PTA->PDDR = GPIO_PDDR_PDD(0x3F000FC0);

	/* Change corresponding PTE port pins as outputs */
	PTE->PDDR = GPIO_PDDR_PDD( GPIO_PIN(28) );

	/* Setup for button interrupts */
	MCG->C4 |= (MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x01)); /* Setup Clock system */
    SIM->CLKDIV1 |= SIM_CLKDIV1_OUTDIV1(0x00);

    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK; /* Activate Clock for PORTE */

    PORTE->PCR[10] = ( PORT_PCR_ISF(0x01) /* Reset ISF (Interrupt Status Flag) */
                | PORT_PCR_IRQC(0x0A) /* Interrupt enable on failing edge */
                | PORT_PCR_MUX(0x01) /* Pin Mux Control to GPIO */
                | PORT_PCR_PE(0x01) /* Pull resistor enable... */
                | PORT_PCR_PS(0x01)); /* ...select Pull-Up */
    /* Same for other ports */

    PORTE->PCR[11] = ( PORT_PCR_ISF(0x01)
                | PORT_PCR_IRQC(0x0A)
                | PORT_PCR_MUX(0x01)
                | PORT_PCR_PE(0x01)
                | PORT_PCR_PS(0x01));

    PORTE->PCR[12] = ( PORT_PCR_ISF(0x01)
                | PORT_PCR_IRQC(0x0A)
                | PORT_PCR_MUX(0x01)
                | PORT_PCR_PE(0x01)
                | PORT_PCR_PS(0x01));

    PORTE->PCR[26] = ( PORT_PCR_ISF(0x01)
                | PORT_PCR_IRQC(0x0A)
                | PORT_PCR_MUX(0x01)
                | PORT_PCR_PE(0x01)
                | PORT_PCR_PS(0x01));

    PORTE->PCR[27] = ( PORT_PCR_ISF(0x01)
                | PORT_PCR_IRQC(0x0A)
                | PORT_PCR_MUX(0x01)
                | PORT_PCR_PE(0x01)
                | PORT_PCR_PS(0x01));

    NVIC_ClearPendingIRQ(PORTE_IRQn); /* Clear interrupt signal on port E */
    NVIC_EnableIRQ(PORTE_IRQn);       /* Enable interrupt on port E */

	/*Enables clock for timers*/
	PIT->MCR = 0x0;
	/*Sets 6_000_000 in hex as timer value and starts timer*/
	PIT->CHANNEL[0].TCTRL = 0x0u;
	PIT->CHANNEL[0].LDVAL = 0x5B8D80u;
	PIT->CHANNEL[0].TCTRL = 0x1u;
}

/* Variable delay loop */
void delay(int t1, int t2)
{
	int i, j;

	for(i=0; i<t1; i++) {
		for(j=0; j<t2; j++);
	}
}

void PORTE_IRQHandler() {
	//Waits if false positive
	delay(20,1);
	// If button 1 is pressed change value of control variable
    if (PORTE->ISFR & BUTTON1_MASK && !(GPIOE_PDIR & BUTTON1_MASK))
     {
        PrintText = 1;
        //Reset interrupt
        PORTE->ISFR = BUTTON1_MASK;
     }
    //Same as above for others
    else if (PORTE->ISFR & BUTTON2_MASK && !(GPIOE_PDIR & BUTTON2_MASK))
    {
        PrintText = 2;
        PORTE->ISFR = BUTTON2_MASK;
    }

    else if (PORTE->ISFR & BUTTON3_MASK && !(GPIOE_PDIR & BUTTON3_MASK))
    {
        PrintText = 3;
        PORTE->ISFR = BUTTON3_MASK;
    }

    else if (PORTE->ISFR & BUTTON4_MASK && !(GPIOE_PDIR & BUTTON4_MASK))
    {
        PrintText = 4;
        PORTE->ISFR = BUTTON4_MASK;
    }

    else if (PORTE->ISFR & BUTTON5_MASK && !(GPIOE_PDIR & BUTTON5_MASK))
    {
        PrintText = 5;
        PORTE->ISFR = BUTTON5_MASK;
    }

}
//Returns size of string
int strlen(char *string){
	int i = 0;
	while(string[i] != 0){
		i++;
	}
	return i;
}


/* Conversion of requested column number into the 4-to-16 decoder control.  */
void column_select(unsigned int col_num)
{
	unsigned i, result, col_sel[4];

	for (i =0; i<4; i++) {
		result = col_num / 2;	  // Whole-number division of the input number
		col_sel[i] = col_num % 2;
		col_num = result;

		switch(i) {

			// Selection signal A0
		    case 0:
				((col_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(8))) : (PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(8)));
				break;

			// Selection signal A1
			case 1:
				((col_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(10))) : (PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(10)));
				break;

			// Selection signal A2
			case 2:
				((col_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(6))) : (PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(6)));
				break;

			// Selection signal A3
			case 3:
				((col_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(11))) : (PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(11)));
				break;

			// Otherwise nothing to do...
			default:
				break;
		}
	}
}

void row_select(unsigned int row_num)
{
	unsigned i, result, row_sel[8];

	for (i =0; i<8; i++) {
		result = row_num / 2;	  // Whole-number division of the input number
		row_sel[i] = row_num % 2;
		row_num = result;

		switch(i) {

			// Selection signal R0
		    case 0:
				((row_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(26))) : (PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(26)));
				break;

			// Selection signal R1
			case 1:
				((row_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(24))) : (PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(24)));
				break;

			// Selection signal R2
			case 2:
				((row_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(9))) : (PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(9)));
				break;

			// Selection signal R3
			case 3:
				((row_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(25))) : (PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(25)));
				break;

			// Selection signal R4
			case 4:
				((row_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(28))) : (PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(28)));
				break;

			// Selection signal R5
			case 5:
				((row_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(7))) : (PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(7)));
				break;

			// Selection signal R6
			case 6:
				((row_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(27))) : (PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(27)));
				break;

			// Selection signal R7
			case 7:
				((row_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(29))) : (PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(29)));
				break;

			// Otherwise nothing to do...
			default:
				break;
		}
	}
}

void PrintCol(int row, int col){
	//Selects row and col
	column_select(col);
	row_select(row);
	//Switch on pixels
	PTE->PDDR &= ~GPIO_PDDR_PDD( GPIO_PIN(28) );
	delay(20,30);
	//Switch off pixels
	PTE->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(28));
	//Reset row_select
	row_select(0);
}

void PrintLetter(int i, int rowValue[]){
	//Check if they are on screen and print cols of letter plus empty col for space
	if(i <= 15 && i >= 0){
		PrintCol(rowValue[0], i);
	}

	if(i-1 <= 15 && i-1 >= 0){
		PrintCol(rowValue[1], i-1);
	}

	if(i-2 <= 15 && i-2 >= 0){
		PrintCol(rowValue[2], i-2);
	}

	if(i-3 <= 15 && i-3 >= 0){
		PrintCol(rowValue[3], i-3);
	}

	if(i-4 <= 15 && i-4 >= 0){
		PrintCol(0, i-4);
	}
}


void PrintLine(char *word){
	//Size of word
	int sizeofLine = strlen(word);
	//Variable for offsetting letters
	int offset = 0;
	//Variable for checking if interrupt happened
	int origText = PrintText;
	//i must be less than all cols in word + empty screen
	for (int i=0; i<(16+sizeofLine*5); i++) {
		for (;;){
			//All letters in word
			for (int j = 0; j < sizeofLine; j++){
				//Switch for comparing letters
				switch(word[j]){
				case ' ':
					PrintLetter(i-offset, (int []) LetterSpace);
					break;

				case 'A':
				case 'a':
					PrintLetter(i-offset, (int []) LetterA);
					break;

				case 'B':
				case 'b':
				case '8':
					PrintLetter(i-offset, (int []) LetterB);
					break;


				case 'C':
				case 'c':
					PrintLetter(i-offset, (int []) LetterC);
					break;

				case 'D':
				case 'd':
					PrintLetter(i-offset, (int []) LetterD);
					break;

				case 'E':
				case 'e':
					PrintLetter(i-offset, (int []) LetterE);
					break;

				case 'F':
				case 'f':
					PrintLetter(i-offset, (int []) LetterF);
					break;

				case 'G':
				case 'g':
					PrintLetter(i-offset, (int []) LetterG);
					break;

				case 'H':
				case 'h':
					PrintLetter(i-offset, (int []) LetterH);
					break;

				case 'I':
				case 'i':
					PrintLetter(i-offset, (int []) LetterI);
					break;

				case 'J':
				case 'j':
					PrintLetter(i-offset, (int []) LetterJ);
					break;

				case 'K':
				case 'k':
					PrintLetter(i-offset, (int []) LetterK);
					break;

				case 'L':
				case 'l':
					PrintLetter(i-offset, (int []) LetterL);
					break;

				case 'M':
				case 'm':
					PrintLetter(i-offset, (int []) LetterM);
					break;

				case 'N':
				case 'n':
					PrintLetter(i-offset, (int []) LetterN);
					break;


				case 'O':
				case 'o':
				case '0':
					PrintLetter(i-offset, (int []) LetterO);
					break;

				case 'P':
				case 'p':
					PrintLetter(i-offset, (int []) LetterP);
					break;

				case 'Q':
				case 'q':
					PrintLetter(i-offset, (int []) LetterQ);
					break;

				case 'R':
				case 'r':
					PrintLetter(i-offset, (int []) LetterR);
					break;

				case 'S':
				case 's':
				case '5':
					PrintLetter(i-offset, (int []) LetterS);
					break;

				case 'T':
				case 't':
					PrintLetter(i-offset, (int []) LetterT);
					break;

				case 'U':
				case 'u':
					PrintLetter(i-offset, (int []) LetterU);
					break;

				case 'V':
				case 'v':
					PrintLetter(i-offset, (int []) LetterV);
					break;

				case 'W':
				case 'w':
					PrintLetter(i-offset, (int []) LetterW);
					break;

				case 'X':
				case 'x':
					PrintLetter(i-offset, (int []) LetterX);
					break;

				case 'Y':
				case 'y':
					PrintLetter(i-offset, (int []) LetterY);
					break;

				case 'Z':
				case 'z':
				case '2':
					PrintLetter(i-offset, (int []) LetterZ);
					break;

				case '1':
					PrintLetter(i-offset, (int []) Letter1);
					break;

				case '3':
					PrintLetter(i-offset, (int []) Letter3);
					break;

				case '4':
					PrintLetter(i-offset, (int []) Letter4);
					break;

				case '6':
					PrintLetter(i-offset, (int []) Letter6);
					break;

				case '7':
					PrintLetter(i-offset, (int []) Letter7);
					break;

				case '9':
					PrintLetter(i-offset, (int []) Letter9);
					break;

				default:
					PrintLetter(i-offset, (int []) LetterSpace);
					break;
				}
				//When letter is written next is offsetted
				offset += 5;
			}
			//Reset offset after word is written
			offset = 0;
			//check if timer is done, if yes reset and push cols to left
			if(PIT->CHANNEL[0].TFLG){
				PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF_MASK;
				break;
			}
		}
		//check if interrupt happened
		if(origText != PrintText){
			break;
		}
	}
}


int main(void)
{
	SystemConfig();
	//Lines to print
	char *line1 = "XDURIC05";
	char *line2 = "Zvolte si vypisovany text stlacenim jedneho zo 4 smerovych tlacitok";
	char *line3 = "IMP Projekt Svetelna tabula";
	char *line4 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789";
	char *line5 = "VUT FIT";
	//Endless loop
    for (;;) {
    	//Switch to compare control variable
    	switch (PrintText){
			case 1:
				PrintLine(line1);
				break;
			case 2:
				PrintLine(line2);
				break;
			case 3:
				PrintLine(line3);
				break;
			case 4:
				PrintLine(line4);
				break;
			case 5:
				PrintLine(line5);
				break;
			default:
				break;
    	}

    }
    /* Never leave main */
    return 0;
}

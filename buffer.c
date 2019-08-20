/*File name: buffer.c
* Compiler: MS Visual Studio 2015 (x86)
* Author: Justin Angus - 040690837
* Course: CST 8152 – Compilers, Lab Section: 14
* Assignment: Assignment #2
* Date: November 5th, 2018
* Professor: Sv. Ranev
* Purpose: Allocating and preforming operations on a buffer data structure and it's assosiated functions
* Function list: b_allocate(); b_addc(); b_clear(); b_free(); b_isfull(); b_limit(); b_capacity();
* b_mark(); b_mode(); b_incfactor(); b_load(); b_isempty(); b_getc(); b_eob(); b_print();
* b_compact(); b_rflag(); b_retract(); b_reset(); b_getcoffset(); b_rewind(); b_location();
*/
#include <stdio.h>
#include "buffer.h"

/* Purpose: This function creates a new buffer in memory (on the program heap), checks the operational mode,
* and sets the buffers mode and inc_factor.
* History/Versions: Version 1.0 September 27th, 2018
* Called functions: calloc();
* Parameters: init_capacity: short /  inc_factor: char, depends on mode / o_mode: char, a/f/m
* Return value: This function returns the Buffer that we allocate.
* Algorithm: This function check that the init_capacity is valid, if the o_mode is one of the three valid modes, and
* checks that is the mode is fixed and the init_capacity is zero. The function then check the operational mode then setting
* the Buffer's mode and inc_factor. This function also sets the Buffer's capacity then sets the default flags.
*/
Buffer * b_allocate(short init_capacity, char inc_factor, char o_mode) {
	/*Error check all parameters, if invalid tell the user why and return NULL*/
	if (init_capacity < 0 || init_capacity > SHRT_MAX_N_1) {
		return NULL;
	}
	else if (o_mode != 'f' && o_mode != 'a' &&  o_mode != 'm') {
		return NULL;
	}
	else if (o_mode == 'f' && init_capacity == 0) {
		return NULL;
	}

	/*Creating a new buffer and check that it succeeds*/
	Buffer *newBuffer = calloc(1, sizeof(Buffer));
	if (newBuffer == NULL) {
		return NULL;
	}

	/*Malloc the buffer to the size of the init_capacity and check that it succeeds*/
	newBuffer->cb_head = malloc(init_capacity);
	if (newBuffer->cb_head == NULL) {
		free(newBuffer);
		return NULL;
	}

	/*Setting the operational mode and increment factor*/
	if (o_mode == 'f' || (unsigned char)inc_factor == 0) {
		newBuffer->mode = FIXED;
		newBuffer->inc_factor = 0;
	}
	else if (o_mode == 'a' && (unsigned char)inc_factor >= 1 && (unsigned char)inc_factor <= 255) {
		newBuffer->mode = ADDITIVE;
		newBuffer->inc_factor = inc_factor;
	}
	else if (o_mode == 'm' && inc_factor >= 1 && inc_factor <= 100) {
		newBuffer->mode = MULTIPLICATIVE;
		newBuffer->inc_factor = inc_factor;
	}
	else {
		free(newBuffer);
		return NULL;
	}

	/*Setting current total size (in bytes)*/
	newBuffer->capacity = init_capacity;

	/*Setting the flags to their default value*/
	newBuffer->flags = DEFAULT_FLAGS;

	return newBuffer;
}

/* Purpose: This function is where symbols are added to the buffer. Depending on the operational mode when the
* capacity is reached the function calcutes the new capacity then expands the buffer.
* Author: Justin Angus
* History/Versions: Version 1.0 September 27th, 2018
* Called functions: realloc();
* Parameters: pBD: pBuffer / symbol: char
* Return value: This function returns a pointer to a Buffer.
* Algorithm: If the buffer is not full the function adds the symbol parameter to the next availiable space in
* the buffer. If the buffer is full the function checks the operational mode and calculates the new capacity.
* Once the new capacity is calculated the buffer is reallocated to the new capacity, the symbol is added, and the function returns
* the buffer pointer.
*/
pBuffer b_addc(pBuffer const pBD, char symbol) {
	char *temp;/*Temp buffer*/
	unsigned long availiableSpace;/*For calculations*/
	unsigned long newInc;/*For calculations*/
	unsigned long newCapacity;/*For calculations*/

							  /*Error checking, symbol can be anything*/
	if (pBD == NULL) {
		return NULL;
	}

	/*Re-set r_flag to 0*/
	pBD->flags = pBD->flags & RESET_R_FLAG;

	/*If the buffer is not full add the symbol to the next available location and increment addc_offset*/
	if (pBD->addc_offset * sizeof(char) < pBD->capacity * sizeof(char)) {
		*(pBD->cb_head + pBD->addc_offset) = symbol;
		++pBD->addc_offset;
		return pBD;
	}
	/*If the buffer is full we need to increase the capacity*/
	else {
		/*Operational mode 0*/
		if (pBD->mode == FIXED) {
			return NULL;
		}
		/*Operational mode 1*/
		else if (pBD->mode == ADDITIVE) {

			/*Do the math in longs*/
			newCapacity = pBD->capacity + (unsigned char)pBD->inc_factor;

			if ((short)newCapacity > 0 && (short)newCapacity < SHRT_MAX_N_1) {
				/*Made sure the newCapacity was less then what a short can accommodate*/
				pBD->capacity = (short)newCapacity;
			}
			else if ((short)newCapacity > 0 && (short)newCapacity > SHRT_MAX_N_1) {
				pBD->capacity = SHRT_MAX_N_1;
			}
			else {
				return NULL;
			}
		}
		/*Operational mode -1*/
		else if (pBD->mode == MULTIPLICATIVE) {
			if (pBD->capacity == SHRT_MAX_N_1) {
				return NULL;
			}
			else {

				/*Do the math in longs*/
				availiableSpace = SHRT_MAX_N_1 - pBD->capacity;
				newInc = (long)availiableSpace * (unsigned char)pBD->inc_factor / 100;
				newCapacity = pBD->capacity + newInc;

				/*If newInc is zero the newCapacity has not been incremented succesfully (stayed the same)*/
				if (newInc != 0 && (short)newCapacity < SHRT_MAX_N_1) {
					pBD->capacity = (short)newCapacity;
				}
				else if (newInc == 0 && pBD->capacity < SHRT_MAX_N_1) {
					pBD->capacity = SHRT_MAX_N_1;
				}
				else {
					return NULL;
				}
			}
		}

		/*Re-allocate this temp buffer to the newCapacity and check if it succeeds*/
		temp = realloc(pBD->cb_head, pBD->capacity);
		if (temp == NULL) {
			return NULL;
		}

		/*If the location in mem has been changed by the reallocation, set r_flag bit to 1*/
		if (pBD->cb_head != temp) {
			pBD->flags = pBD->flags | SET_R_FLAG;
		}

		/*Set pBD->cb_head to equal the buffer that has been reallocated, add the symbol, increment pBD->addc_offset, and free the temp buffer*/
		pBD->cb_head = temp;
		*(pBD->cb_head + pBD->addc_offset) = symbol;
		++pBD->addc_offset;
	}
	return pBD;
}

/* Purpose: This function re-initializes all the appropriate data members of the given Buffer structure.
* Author: Justin Angus
* History/Versions: Version 1.0 September 27th, 2018
* Called functions: N/A
* Parameters: pBD: Buffer
* Return value: int: -1 for error
* Algorithm: N/A
*/
int b_clear(Buffer * const pBD) {
	/*Error checking*/
	if (pBD == NULL) {
		return RT_FAIL_1;
	}

	/*Re-initialize offsets to make the buffer seem empty (start overwriting memory from cb_head)*/
	pBD->addc_offset = 0;
	pBD->getc_offset = 0;
	pBD->markc_offset = 0;
	pBD->flags = pBD->flags & DEFAULT_FLAGS;
	return CLEAR_PASS;
}

/* Purpose: This funciton free's the dyncamically allocated memory on the heap.
* Author: Justin Angus
* History/Versions: Version 1.0 September 27th, 2018
* Called functions: free();
* Parameters: pBD: Buffer
* Return value: void: NULL for error
* Algorithm: N/A
*/
void b_free(Buffer * const pBD) {
	/*Error checking*/
	if (pBD == NULL || pBD->cb_head == NULL) {
		return;
	}

	/*Free mem*/
	free(pBD->cb_head);
	free(pBD);
}

/* Purpose: This function check to see if the buffer structure is full.
* Author: Justin Angus
* History/Versions: Version 1.0 September 27th, 2018
* Called functions: N/A
* Parameters: pBD: Buffer
* Return value: int: -1 for errors
* Algorithm: Checks if addc_offset is equal to the capacity .
*/
int b_isfull(Buffer * const pBD) {
	/*Error checking*/
	if (pBD == NULL || pBD->cb_head == NULL) {
		return RT_FAIL_1;
	}

	/*Return 1 if the buffer is full, otherwise return 0, pBD->capacity is 1:1 ratio in bytes so I did not convert*/
	if (pBD->addc_offset * sizeof(char) == pBD->capacity * sizeof(char)) {
		return BUFFER_FULL;
	}
	return BUFFER_NOT_FULL;
}

/* Purpose: This function returns the current limit of the buffer.
* Author: Justin Angus
* History/Versions: Version 1.0 September 27th, 2018
* Called functions: N/A
* Parameters: pBD: Buffer
* Return value: int: -1 for errors
* Algorithm: N/A
*/
short b_limit(Buffer * const pBD) {
	/*Error checking*/
	if (pBD == NULL || pBD->cb_head == NULL) {
		return RT_FAIL_1;
	}

	/*Return the limit of added chars in the buffer*/
	return pBD->addc_offset;
}

/* Purpose: This function returns the current capacity of the buffer.
* Author: Justin Angus
* History/Versions: Version 1.0 September 27th, 2018
* Called functions: N/A
* Parameters: pBD: Buffer
* Return value: short: -1 for errors
* Algorithm: N/A
*/
short b_capacity(Buffer * const pBD) {
	/*Error checking*/
	if (pBD == NULL || pBD->cb_head == NULL) {
		return RT_FAIL_1;
	}

	/*Return the current capacity of the function*/
	return pBD->capacity;
}

/* Purpose: The function sets the markc_offset to the mark parameter.
* Author: Justin Angus
* History/Versions: Version 1.0 September 27th, 2018
* Called functions: N/A
* Parameters: pBD: pBuffer
* Return value: short: -1 for errors
* Algorithm: N/A
*/
short b_mark(pBuffer const pBD, short mark) {
	/*Error checking*/
	if (pBD == NULL || pBD->cb_head == NULL) {
		return RT_FAIL_1;
	}
	else if (mark < 0 || mark > pBD->addc_offset) {
		return RT_FAIL_1;
	}

	/*Set pBD->markc_offset to mark then return pBD->markc_offset*/
	return pBD->markc_offset = mark;
}

/* Purpose: This function returns the value of the mode to the calling function.
* Author: Justin Angus
* History/Versions: Version 1.0 September 27th, 2018
* Called functions: N/A
* Parameters: pBD: Buffer
* Return value: int: -1 for errors
* Algorithm: N/A
*/
int b_mode(Buffer * const pBD) {
	int intMode;/*To calulate the mode*/

				/*Error checking*/
	if (pBD == NULL || pBD->cb_head == NULL) {
		return MODE_FAIL;
	}

	/*Cast pBD->mode into and int and return the int*/
	intMode = (int)pBD->mode;
	return intMode;
}

/* Purpose: The function returns the non-negative number of the value inc_factor to the calling function
* Author: Justin Angus
* History/Versions: Version 1.0 September 27th, 2018
* Called functions: N/A
* Parameters: pBD: Buffer
* Return value: size_t: NULL for errors
* Algorithm: N/A
*/
size_t b_incfactor(Buffer * const pBD) {
	/*Error checking*/
	if (pBD == NULL || pBD->cb_head == NULL) {
		return RT_FAIL_HEX;
	}

	/*This return value cannot be < 0 because it size_t is unsigned*/
	return (unsigned char)pBD->inc_factor;
}

/* Purpose: This function loads the file and adds the characters one at a time using fgetc. If the eob flag is set
* the function stops trying to add to the buffer and uses ungetc to retrive the character. Also, if the function reaches
* end of file (feof();) the function breaks out of the while loop.
* Author: Justin Angus
* History/Versions: Version 1.0 September 27th, 2018
* Called functions: fgetc(); feof(); ungetc(); b_addc();
* Parameters: pBD: Buffer / fi: FILE
* Return value: int: -1 for errors
* Algorithm: Adds characters to the buffer until the end of file is reached or until b_addc(); returns NULL.
*/
int b_load(FILE * const fi, Buffer * const pBD) {
	char c;/*To hold the char*/

		   /*Error checking*/
	if (pBD == NULL || pBD->cb_head == NULL || fi == NULL) {
		return RT_FAIL_1;
	}

	/*Add the characters of the file until we hit end of file*/
	do {
		/*Store the char from the file*/
		c = (char)fgetc(fi);
		/* If it is end of file, break*/
		if (feof(fi)) {
			break;
		}
		/*If b_addc returns NULL, unget the char and put it back in the file*/
		if (b_addc(pBD, c) == NULL) {
			ungetc(c, fi);
			printf("The last character read from the file is: %c %d\n", c, c);
			return LOAD_FAIL;
		}
	} while (1);

	/*Return the amount of characters added to the buffer*/
	return pBD->addc_offset;
}

/* Purpose: This function check to see if the buffer is empty.
* Author: Justin Angus
* History/Versions: Version 1.0 September 27th, 2018
* Called functions: N/A
* Parameters: pBD: Buffer
* Return value: int: -1 for errors
* Algorithm: N/A
*/
int b_isempty(Buffer * const pBD) {
	/*Error checking*/
	if (pBD == NULL || pBD->cb_head == NULL) {
		return RT_FAIL_1;
	}

	if (pBD->addc_offset == 0) {
		return BUFFER_IS_EMPTY;
	}
	return BUFFER_IS_NOT_EMPTY;
}

/* Purpose: This function is used to read the contents of the buffer.
* Author: Justin Angus
* History/Versions: Version 1.0 September 27th, 2018
* Called functions: N/A
* Parameters: pBD: Buffer
* Return value: char: -2 for errors
* Algorithm: Returns the character where getc_offset as long as getc_offset has reached the end of the buffer.
* This function also uses a bitwise operation to set and reset the EOB bit
*/
char b_getc(Buffer * const pBD) {
	unsigned char c;/*To hold the char*/

		   /*Error checking*/
	if (pBD == NULL || pBD->cb_head == NULL) {
		return RT_FAIL_2;
	}

	/*If getc_offset and addc_offset are equal set eob bit to 1 and return 0*/
	if (pBD->getc_offset == pBD->addc_offset) {
		pBD->flags = pBD->flags | SET_EOB;
		return EOB_RETURN_VAL;
	}

	/*Otherwise set eob to 0, returns the char at getc_offset and increment getc_offset*/
	pBD->flags = pBD->flags & RESET_EOB;
	c = *(pBD->cb_head + pBD->getc_offset);
	++pBD->getc_offset;
	return c;
}

/* Purpose: This function check and return the value of the EOB bit
* Author: Justin Angus
* History/Versions: Version 1.0 September 27th, 2018
* Called functions: N/A
* Parameters: pBD: Buffer
* Return value: int: -1 for errors
* Algorithm: Used a bitwise operation to determine the value of EOB bit
*/
int b_eob(Buffer * const pBD) {
	short eobBit;/*Make a variable to hold value of eob bit*/

				 /*Error checking*/
	if (pBD == NULL || pBD->cb_head == NULL) {
		return RT_FAIL_1;
	}

	/*Return the value of the eob bit*/
	eobBit = pBD->flags & CHECK_EOB;
	return eobBit;
}

/* Purpose: This function is used to print the contents of the buffer.
* Author: Justin Angus
* History/Versions: Version 1.0 September 27th, 2018
* Called functions: b_getc(); b_eob();
* Parameters: pBD: Buffer
* Return value: int: -1 for errors
* Algorithm: While getc_offset is less then or equal to addc_offset the function will print the return value of
* calling b_getc(); and if the return value of b_eob is equal to one we have reached the end and we break out of
* the loop.
*/
int b_print(Buffer * const pBD) {
	char c;/*To hold the char*/

		   /*Error checking*/
	if (pBD == NULL || pBD->cb_head == NULL) {
		return RT_FAIL_1;
	}
	else if (pBD->addc_offset == 0) {
		printf("Empty buffer!\n");
		return RT_FAIL_1;
	}

	/*Print out the contents of the buffer, if the eob flag is set break out of the loop*/
	do {
		c = b_getc(pBD);
		if (b_eob(pBD) == SET_EOB_BIT) {
			break;
		}
		printf("%c", c);
	} while (1);

	/*Print a new line and return the number of characters printed*/
	printf("\n");
	return pBD->getc_offset;
}

/* Purpose: This function shrinks (or sometimes expands) the buffer.
* Author: Justin Angus
* History/Versions: Version 1.0 September 27th, 2018
* Called functions: realloc();
* Parameters: pBD: Buffer / symbol: char
* Return value: Buffer
* Algorithm: This function reallocats the buffer to addc_offset + 1 (to add the symbol). Check to see if the location
* of the buffer has moved and then assigns the cb_head to the temp buffer. Then increments the appropriate values.
*/
Buffer * b_compact(Buffer * const pBD, char symbol) {
	char *temp;/*Create a temp buffer*/
	long newSize;/*To accomadte for the overflow (bytes)*/

				 /*Error checking*/
	if (pBD == NULL || pBD->cb_head == NULL) {
		return NULL;
	}

	/*Re-set r_flag to 0*/
	pBD->flags = pBD->flags & RESET_R_FLAG;

	/*Set the new size to pBD->addc_offset plus one*/
	newSize = pBD->addc_offset * sizeof(char) + 1;

	/*Reallocate the size of the buffer to the current limit plus a space for one more character*/
	temp = realloc(pBD->cb_head, newSize);/*Mixed measurements*/
	if (temp == NULL) {
		return NULL;
	}

	/*If the location in mem has been changed by the reallocation, set r_flag bit to 1*/
	if (pBD->cb_head != temp) {
		pBD->flags = pBD->flags | SET_R_FLAG;
	}

	/*Set the head to the re-allocated storage*/
	pBD->cb_head = temp;

	/*Add the symbol to the buffer, update the capacity, increment the addc_offset, and return  pBD*/
	*(pBD->cb_head + pBD->addc_offset) = symbol;
	pBD->capacity = (short)newSize;
	++pBD->addc_offset;
	return pBD;
}

/* Purpose: This function returns the value of the rFlagBit
* Author: Justin Angus
* History/Versions: Version 1.0 September 27th, 2018
* Called functions: N/A
* Parameters: pBD: Buffer
* Return value: char: -1 for errors
* Algorithm: Used a bitwise operation to determine the value of rFlagBit
*/
char b_rflag(Buffer * const pBD) {
	char rFlagBit;/*To return the rFlag bit*/

				  /*Error checking*/
	if (pBD == NULL || pBD->cb_head == NULL) {
		return RT_FAIL_1;
	}

	/*Return the value of the eob bit*/
	return rFlagBit = pBD->flags & CHECK_R_FLAG;
}

/* Purpose: This function decrements getc_offset.
* Author: Justin Angus
* History/Versions: Version 1.0 September 27th, 2018
* Called functions: N/A
* Parameters: pBD: Buffer
* Return value: short: -1 for errors
* Algorithm: N/A
*/
short b_retract(Buffer * const pBD) {
	/*Error checking*/
	if (pBD == NULL || pBD->cb_head == NULL) {
		return RT_FAIL_1;
	}

	/*Decrement getc_offset and return it's value*/
	if (pBD->getc_offset != 0) {
		--pBD->getc_offset;
	}
	return pBD->getc_offset;
}

/* Purpose: This function sets the value of getc_offset to the value of markc_offset.
* Author: Justin Angus
* History/Versions: Version 1.0 September 27th, 2018
* Called functions: N/A
* Parameters: pBD: Buffer
* Return value: short: -1 for errors
* Algorithm: N/A
*/
short b_reset(Buffer * const pBD) {
	/*Error checking*/
	if (pBD == NULL || pBD->cb_head == NULL) {
		return RT_FAIL_1;
	}

	/*Set getc_offset to the value of markc_offset and return it's value*/
	pBD->getc_offset = pBD->markc_offset;
	return pBD->getc_offset;
}

/* Purpose: Return the value of getc_offset
* Author: Justin Angus
* History/Versions: Version 1.0 September 27th, 2018
* Called functions: N/A
* Parameters: pBD: Buffer
* Return value: short: -1 for errors
* Algorithm: N/A
*/
short b_getcoffset(Buffer * const pBD) {
	/*Error checking*/
	if (pBD == NULL || pBD->cb_head == NULL) {
		return RT_FAIL_1;
	}

	/*Return getc_offset*/
	return pBD->getc_offset;
}

/* Purpose: This function sets the value of getc_offset back to zero.
* Author: Justin Angus
* History/Versions: Version 1.0 September 27th, 2018
* Called functions: N/A
* Parameters: pBD: Buffer
* Return value: int: -1 for errors
* Algorithm: [outline the main steps (sections) only; do not include implementation
* details; for small and clear functions leave this empty]
*/
int b_rewind(Buffer * const pBD) {
	/*Error checking*/
	if (pBD == NULL || pBD->cb_head == NULL) {
		return RT_FAIL_1;
	}

	/*Set getc_offset to zero so that it can read from the start again and return zero*/
	pBD->markc_offset = 0;
	pBD->getc_offset = 0;
	return REWIND_PASS;
}

/* Purpose: This function retunrs a pointer to a location of a character in the buffer.
* Author: Justin Angus
* History/Versions: Version 1.0 September 27th, 2018
* Called functions: N/A
* Parameters: pBD: Buffer
* Return value: char* : NULL for errors
* Algorithm: N/A
*/
char * b_location(Buffer * const pBD, short loc_offset) {
	char *location;/*Return this address*/

				   /*Error checking*/
	if (pBD == NULL || pBD->cb_head == NULL) {
		return NULL;
	}
	else if (loc_offset < 0 || loc_offset > pBD->capacity) {
		return NULL;
	}

	/*Return a pointer to the loc_offset from the start of the buffer aka. cb_head*/
	location = &pBD->cb_head[loc_offset];
	return location;
}
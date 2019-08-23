/** \brief filter function helps in formatting the output.
* This function takes the unformatted output files from the simulator as 
* the input and generates formatted output for the ABP simulator and 
* all the unit tests (sender, subnet, receiver).
*
* for example (formatted output), 
* time             value  port                 component
* 00:00:10:000      5     out                  generator_con
* 00:00:20:000      1     packet_sent_out      sender1
* 00:00:20:000     11     data_out             sender1
* 00:00:23:000     11     out                  subnet1
* 00:00:33:000      1     out                  receiver1
*/

/**
* Organization    : ARSLab - Carleton University
* Original author : Cristina Ruiz Martin
* Modified by     : Manoj Goli and Revanth Sridhar
*/

#ifndef __TRANSFORM_HPP__
#define __TRANSFORM_HPP__

#include<string.h>
#include<stdio.h>


/**
* structure line_wrap declaration: As we can't pass arrays as function 
* arguments in C, we are wrapping into a structure to make single object
*/
struct line_wrap {
	char line[1000]; /**< stores the unfiltered data */
};

/**
* structure read-line is used to read each line from a file fp
* Input  : File pointer. 
* Output : The line that the file pointer is pointing.
*/
struct line_wrap read_line(FILE *fp) {
	char ch; /**< it is used to read characters in the file */
	int counter = 0; /**< counter variable initialized to 0  */
	char line[1000]; /**<  */
	struct line_wrap line2;
	
	/**
	* this loop allows us to read a particular line in the file.
	* it keeps increasing the counter value (i.e, line[counter++]) untill it 
	* finds the EOF or new line.
	*/
	while( (ch = getc(fp))!=EOF && ch!='\n' ) {
    	line[counter++] = ch;
	}

	/** 
	* If  there is no further string to read, then '\0' will be added as to say
	* its the end of that particular sub-string. 
	*/
	line[counter] = '\0';
	
	/** 
	* copies line (source) and stores in line2.line (destination) 
	*/
	strcpy(line2.line,line);
	return line2;
}

int istime(struct line_wrap line);
void filter(FILE *input, FILE *output);

#endif /** __TRANSFORM_HPP__ */
/** \brief user_filter function helps in filtering the output as per the user
* requirement. This function takes the unformatted output files from the 
* simulator as the input and generates formatted output for the ABP simulator
* and all the unit tests (sender, subnet, receiver).
*
* During the runtime of the program, the terminal will prompt the user to
* enter three inputs (i.e., start time, end time and component details)
* and these inputs will be used to print specific outputs i.e., the outputs 
* within the timeframe and components specified by the user. User can also 
* provide multiple components with a single space between them. 
*
* For example, if the user inputs start time as 00:00:00:000, end time 
* as 00:01:00:000 and component as subnet1, then the output of the program 
* will contain message details between the above timeframe and messages 
* only through subnet1.
*
* time			value	port				component
* 00:00:20:000	1		packet_sent_out		sender1
* 00:00:20:000	11		data_out			sender1
* 00:00:36:000	1		ack_received_out	sender1
* 00:00:46:000	2		packet_sent_out		sender1
* 00:00:46:000	20		data_out			sender1
*/

/**
* Organization    : ARSLab - Carleton University
* Original author : Cristina Ruiz Martin
* Modified by     : Revanth Sridhar and Manoj Goli
*/

#ifndef __USER_OUTPUT_QUERY_HPP__
#define __USER_OUTPUT_QUERY_HPP__

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include<ctype.h>
#include<iostream>
using namespace std;

/**
* structure line_wrap declaration: As we can't pass arrays as function 
* arguments in C, we are wrapping into a structure to make single object
*/
struct line_wrap_user {
	char line[1000];
};

/**
* structure read-line is used to read each line from a file fp
* Input  : File pointer. 
* Output : The line that the file pointer is pointing.
*/
struct line_wrap_user read_line_user(FILE *fp) {
	char ch; /**< it is used to read characters in the file */
	int counter = 0; /**< counter variable initialized to 0  */
	char line[1000];
	struct line_wrap_user line2;

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
	* copies line (source) and stores in line2.line (destination).
	*/
	strcpy(line2.line,line);
	return line2;

}


/**
* this structure is used to receive the time from user during runtime
*/
struct time {
	char time[13];
};

/**
* this structure is get the component details from user during runtime
*/
struct cyn {
	char components_yn[7];
};


/** 
* istime_user function tells us the type of the line i.e time or message
* if the line is Time (example, 00:00:20:000), returns True else False
*/
int istime_user(struct line_wrap_user line);

/**
* This function helps in splitting the user entered string into tokens. 
* strok/strol functions is used to perform this activity and store the 
* starttime in time_1 and endtime in time_2.
* @param struct time t1 has a start time provided by user
* @param struct time t2 has a end time provided by user
*/
int time_comp(struct time t1, struct time t20);


/**
* This function helps in filtering the output as per the user requirement. 
* @param *input points to the unfiltered input text file
* @param *output to return the filtered output 
* @param struct time start_t is used to pass the user entered start time
* @param struct time end_t is used to pass the user entered end time
* @param struct cyn c is used to pass the user entered component details
*/
void user_filter(FILE *input, struct time start_t, struct time end_t, struct cyn c, FILE *fpout);

#endif /** __USER_OUTPUT_QUERY_HPP__ */
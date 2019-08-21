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
* time           value  port              component
* 00:00:20:000    1     packet_sent_out   sender1
* 00:00:20:000   11     data_out          sender1
* 00:00:36:000    1     ack_received_out  sender1
* 00:00:46:000    2     packet_sent_out   sender1
* 00:00:46:000   20     data_out          sender1
*/

/**
* Organization    : ARSLab - Carleton University
* Original author : Cristina Ruiz Martin
* Modified by     : Revanth Sridhar and Manoj Goli
*/

#include "../include/user_output_query.hpp"

/** 
* istime_user function tells us the type of the line i.e time or message
* if the line is Time (example, 00:00:20:000), returns True else False
*/
int istime_user(struct line_wrap_user line) {
    if (strlen(line.line) == 12
        && isdigit(line.line[0])
        && isdigit(line.line[1])
        && isdigit(line.line[3])
        && isdigit(line.line[4])
        && isdigit(line.line[6])
        && isdigit(line.line[7])
        && isdigit(line.line[9])
        && isdigit(line.line[10])
        && isdigit(line.line[11])
        && line.line[2]==':'
        && line.line[5]==':'
        && line.line[8]==':') {
        return 1;
	}
    else {
        return 0;
    }
}

/**
* This function helps in splitting the user entered string into tokens. 
* strok/strol functions is used to perform this activity and store the 
* starttime in time_1 and endtime in time_2.
* @param struct time t1 has a start time provided by user
* @param struct time t2 has a end time provided by user
*/
int time_comp(struct time t1, struct time t2) {
	long time_1 = 0;
	long time_2 = 0;
	char *token_1 = strtok(t1.time, ":");
	char *temp1;
	char *temp2;
	int counter = 0;
    
    while(token_1!=NULL) {
		if (counter < 3) {
   			 time_1 = time_1*100+strtol(token_1,&temp1,10);
    	}
		else {
			time_1 = time_1*1000+strtol(token_1,&temp1,10);
    	}
		token_1 = strtok(NULL, ":");
    	counter++;
	}

	char* token_2 = strtok(t2.time, ":");
	counter=0;

	while (token_2!= NULL) {
		if (counter < 3) {
    		time_2 = time_2*100+strtol(token_2,&temp2,10);
		}
		else {
			time_2 = time_2*1000+strtol(token_2,&temp2,10);
		}
		token_2 = strtok(NULL, ":");
	    counter++;
	}


	if (time_1 == time_2) {
	    return 0;
	}
	else if (time_1 > time_2) {
	    return 1;
	}
	else {
	    return -1;
	}
}


/**
* This function helps in filtering the output as per the user requirement. 
* @param *input points to the unfiltered input text file
* @param *output to return the filtered output 
* @param struct time start_t is used to pass the user entered start time
* @param struct time end_t is used to pass the user entered end time
* @param struct cyn c is used to pass the user entered component details
*/
void user_filter(FILE *input, struct time start_t, 
				 struct time end_t, struct cyn c, FILE *fpout) {
	struct line_wrap_user l;
	struct line_wrap_user l2;
	FILE *fp; /**< intialize pointer variable for input file */
	FILE *out; /**< intialize pointer variable for output file  */
	fp = input; /**< used to store input file   */
	char ch;
	char *str;
	int counter = 0; /**< counter variable */
	char time[13]; /**< variable to strore time details */
	char port[20]; /**< variable to store port details*/
	char value[20]; /**< variable to store value details */
	char component[20]; /**< variable to store component details*/
	int flag; /**< flag variable */

	/** uses the structure read_line and stores the content in l*/
	l = read_line_user(fp);

	/** this is the header data printed in the output file 	*/
	fprintf(fpout,"time,value,port,component\n");

	/** 
	* the loop is executed until the end of file is true using the 
	* pre-defined function feof in C language
	*/
	while(!feof(fp)) {

		/**
		* loop is executed based on the return value from istime function. 
		* If return value from istime_user function is 1 (i.e., true), then we 
		* copy the time into time variable. 
		* If the return value from istime_user function is 0 (i.e., false), then 
		* loop goes to the else condition. 
		*/
		if (istime_user(l)) {
			strcpy(time,l.line);
			l = read_line_user(fp);
		}
		else {
			int counter;

			/**
			* calculates the length of a particular line and loops until there
			* is a space if it finds a space, it breaks the loop and increments
			* the counter value
			*/
			for(counter = strlen(l.line)-1;l.line[counter]!=' ';counter--) {
				continue;
			}
			counter++;
			int i;

			/**
			* this is used to capture the component details from the unfiltered 
			* output file
			*/
			for (i=0;counter!=strlen(l.line);i++) {
				component[i] = l.line[counter++];
			}

			/** null character is added to the end of the captured string */
			component[i]='\0';
			int colon_counter = 0;

			for(int i = 0;i<strlen(l.line);i++) {

				/**
				* the iterations continues further, by following below switch 
				* cases thats used perform different operations to get the 
				* desired formatted output (i.e., '['  ','  ':'  '{'  '}'  ']') 
				*/	
				switch(l.line[i]) {
					case '[':
	
					case ',':
						colon_counter = 0;
						while(l.line[i]!=':' && l.line[i]!=']' ) {
							i++;
						}
						i--;
						break;

					/** This case is used to capture the port details */
					case ':':
						if (colon_counter == 0 || colon_counter == 1) {
							colon_counter++;
							if (colon_counter == 1) {
								i--;
							}
							break;
						}
						else if(colon_counter == 2) {
							colon_counter++;
							i++;
							counter = 0;
							int j = 0;
							while(l.line[i]!=':') {
								port[counter] = l.line[i];
								counter++;
								i++;
							}
							i--;
							port[counter]='\0';
							break;
						}
						else if (colon_counter==3) {
							while(l.line[i]!='{') {
								i++;
							}
							i--;
							colon_counter=0;
							break;
						}
						else {}
						break;

					/** 
					* This case is used to capture the value details and 
					* also store all the details (i.e., time, value, port, 
					* component) into the output file.
					*/
					case '{' :
						i++;
						counter=0;
						while(l.line[i]!='}') {
							value[counter++] = l.line[i++];
						}
						value[counter] = '\0';
						i--;


						if(value[0]!='\0'&& value[0]>9) {
                            struct time present_time;
                            strcpy(present_time.time,time);
                            char components_yn[7] = "nnnnnn";

                            if(strcmp(component,"sender1") == 0) {
                                components_yn[0]='y';
                            }
                            else if(strcmp(component,"receiver1") == 0) {
                                components_yn[1]='y';
                            }
                            else if(strcmp(component,"subnet1") == 0) {
                                components_yn[2]='y';
                            }
                            else if(strcmp(component,"subnet2") == 0) {
                                components_yn[3]='y';
                            }
                            else if(strcmp(component,"generator_con") == 0) {
                                components_yn[4]='y';
                            }
                            else if(strcmp(component,"generator") == 0) {
                                components_yn[5]='y';
                            }
                            else if(strcmp(component,"generator_ack") == 0) {
 								components_yn[6]='y';
                            }
                            int index=0;
                
                            for (int a=0;a<strlen(components_yn);a++) {
                                if(components_yn[a]=='y') {
                                    index = a;
                          		    break;
                                }
                            }
                         
                            if (time_comp(present_time,start_t)>-1 
                            	&& time_comp(end_t,present_time)>-1 
                            	&& c.components_yn[index]=='y') {
                               	fprintf(fpout,"%s,%s,%s,%s\n",
                               			time,value,port,component);
                            }
						}
						break;

					case '}':
						while(l.line[i]!=']' && l.line[i]!=',') {
							i++;
						}
						i--;
						break;
		
					case ']':
						l = read_line_user(fp);
						flag=1;
						break;

					default:
						/** to print errors if any */
						printf("%c unable to print",l.line[i]); 
						break;
				}
				
				if (flag==1) {
					flag=0;
					break;
				}
			}
		}
	}
}

/*
* 
*
*/
int main_test(FILE *fp,FILE *fpout) {
	char start_time[13];
	char end_time[13];
	long start_t;
	long end_t;
	struct time t1,t2;
	char components[1000];
	char components_yn[7]="nnnnnn";
	
	printf("\nPlease enter the below details in small letters
		  to receive specific outputs");
	printf("\nEnter Start Time :");
	scanf("%s",start_time);
	
	printf("Enter End Time   :");
	scanf("%s",end_time);
	
	/** 
	* for linux fpurge(stdin) is used to erase any input or output buffered 
	* in stream. fflush(stdin)for windows
	*/
	fpurge(stdin);

	printf("Enter components seperated with a space :");
	scanf("%[^\n]",components);

	/** printf("Output files are generated in the respective folders. Please check!"); */

	strcpy(t1.time,start_time);
	strcpy(t2.time, end_time);

	char* token = strtok(components, " ");

	/** 
	* By using strcmp and strtok, we set componenets_yn to y (yes) for the 
	* components which the user had entered during runtime. 
	* Keep printing tokens while one of the delimiters present in str[]. 
	*/
	while (token != NULL) {
    	if(strcmp(token,"sender1") == 0) {
        	components_yn[0]='y';
    	}
    	else if(strcmp(token,"receiver1") == 0) {
        	components_yn[1]='y';
    	}
    	else if(strcmp(token,"subnet1") == 0) {
            components_yn[2]='y';
    	}
    	else if(strcmp(token,"subnet2") == 0) {
            components_yn[3]='y';
    	}
    	else if(strcmp(token,"generator_con") == 0) {
            components_yn[4]='y';
    	}
    	else if(strcmp(token,"generator") == 0) {
            components_yn[5]='y';
    	}
    	else if(strcmp(token,"generator_ack") == 0) {
            components_yn[6]='y';
    	}
    	token = strtok(NULL, " ");
	}

	struct cyn c;
	strcpy(c.components_yn,components_yn);
	
	if(fp == NULL || fpout == NULL) {
		printf("ERROR READING THE FILE");
	}

	/** 
	* invokes the user_filter funtion with five parameters
	* (file_input, start_time, end_time, component, file_output).
	*/
	user_filter(fp,t1,t2,c,fpout);
}
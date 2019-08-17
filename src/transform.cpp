/** \brief filter function helps in formatting the output.
* This function takes the unformatted output files from the simulator as 
* a input and generates formatted output for the ABP simulator and 
* all the unit tests (sender, subnet, receiver).
*/

/**
* Organization    : ARSLab - Carleton University
* Original author : Cristina Ruiz Martin
* Modified by     : Revanth Sridhar and Manoj Goli
*/

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
	* If  there is no further lines to read, then line[counter] is set 
	* to null. 
	*/
	line[counter] = '\0';
	
	/**
	* copies line (source) and stores in line2.line (destination).
	*/
	strcpy(line2.line,line);
	return line2;
}

/** 
* isLine function tells us the type of the line i.e time or message
* if the line is Time (example, 00:00:20:000), returns True else False
*/
int isLine(struct line_wrap line) {
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
* This function helps in formating the output in the desired format. 
* @param *input points to the unfiltered input text file
* @param *output to return the filtered output 
*/
void filter(FILE *input, FILE *output) {
	fpos_t line_pos; 
	fpos_t temp_pos; 
	struct line_wrap l; 
	struct line_wrap l2; 
	FILE *fp; /**< used to store input file name */
	FILE *out; /**< used to store the output file name */
	fp=input; /**<  */
	out=output; /**<  */
	char ch; 
	char *str;
	int counter = 0; /**< counter variable */
	char time[13]; /**< variable to strore time details */
	char port[20]; /**< variable to store port details*/
	char value[20]; /**< variable to store value details */
	char component[20]; /**< variable to store component details*/
	int flag; /**< flag variable */

	/** this is the header data printed in the output file 	*/
	fprintf(out,"time,value,port,component\n");
	
	/** l stores each line from the input file */
	l = read_line(fp);
	
	/** the loop is executed until the end of file is true */
	while (!feof(fp)) {
		
		/**
		* loop is executed based on the return value from Isline function. 
		* If return value from isLine function is 1 (i.e., true), then we 
		* copy the time into time variable. 
		* If the return value from isLine function is 0 (i.e., false), then 
		* loop goes to the else condition. 
		*/
		if (isLine(l)) {
			strcpy(time,l.line);
			l = read_line(fp);
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
				* switch case allows to perform different operations based on 
				* the input read from unfiltered file 
				* (i.e., '['  ','  ':'  '{'  '}'  ']') 
				*/	
				switch(l.line[i]) {
					case '[':
					
					case ',':
						colon_counter=0;
						while(l.line[i]!=':' && l.line[i]!=']') {
							i++;
						}
						i--;
						break;

					case ':':
						if (colon_counter==0 || colon_counter==1) {
							colon_counter++;
							if (colon_counter==1) {
								i--;
							}
							break;
						}
						else if (colon_counter==2) {
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

					case '{' :
						i++;
						counter=0;
						while(l.line[i]!='}') {
							value[counter++] = l.line[i++];
						}
						value[counter] = '\0';
						i--;	
						if(value[0]!='\0'&& value[0]>9) {
							fprintf(out,"%s,%s,%s,%s\n",time,value,port,component);
						}
						break;

					case '}':
						while(l.line[i]!=']' && l.line[i]!=',') {
							i++;
						}
						i--;
						break;
	
					case ']':
						l = read_line(fp);
						flag=1;
						break;

					default:
						printf("%c",l.line[i]); //error if prints any other
						break;
				}
			
				if (flag==1) {
					flag=0;
					break;
				}
			}
		}
	}
	fclose(out);
}
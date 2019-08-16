#include<string.h>
#include<stdio.h>

//structure declaration: As we can't pass arrays as function arguments 
//we are wrapping into a structure to make single object
struct lineWrap{
	char line[1000];
};

//To read each line from a file fp
// input : File pointer Output: The line that the file pointer is pointing.
struct lineWrap readline(FILE *fp){

char ch;
int counter = 0;
char line[1000];
struct lineWrap line2;

while( (ch = getc(fp))!=EOF && ch!='\n' ){
    line[counter++] = ch;
}

line[counter] = '\0';
strcpy(line2.line,line);
return line2;

}
//isLine function tells us the type of the line i.e time or message
// if line is Time, returns True else False
int isLine(struct lineWrap line){
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
        && line.line[8]==':')
        return 1;
    else
        return 0;
}
				
void filter(FILE *input, FILE *output){
	
	fpos_t line_pos;
	fpos_t temp_pos;

struct lineWrap l;
struct lineWrap l2;
FILE *fp;
FILE *out;
fp=input;
out=output;
char ch;
char *str;
int counter = 0;

fprintf(out,"time,value,port,component\n");

char time[13];
char text1[20];
char text2[20];
char text3[20];
int flag;
l = readline(fp);

while(!feof(fp)){
		if (isLine(l)){
				strcpy(time,l.line);
				l = readline(fp);

		}
		else{
				int counter;
				for(counter = strlen(l.line)-1;l.line[counter]!=' ';counter--)
						continue;
				counter++;
				int i;
				for (i=0;counter!=strlen(l.line);i++)
						text3[i] = l.line[counter++];
				text3[i]='\0';


				int colon_counter = 0;
				for(int i = 0;i<strlen(l.line);i++){


						switch(l.line[i]){
								case '[':
								case ',':

										colon_counter=0;
										while(l.line[i]!=':' && l.line[i]!=']' )
												i++;
										i--;
										break;

								case ':':
										if (colon_counter==0 || colon_counter==1){
												colon_counter++;
												if (colon_counter==1)
														i--;
												break;
										}
										else if(colon_counter==2){
												colon_counter++;
												i++;
												counter = 0;
												int j = 0;
												while(l.line[i]!=':'){
																text1[counter] = l.line[i];
																counter++;
																i++;
												}
												i--;
												text1[counter]='\0';
												break;
										}
										else if (colon_counter==3){
												while(l.line[i]!='{')
														i++;
												i--;
												colon_counter=0;
												break;

										}
										else{}

										break;

								case '{' :
										i++;
										counter=0;
										while(l.line[i]!='}')
												text2[counter++] = l.line[i++];
										text2[counter] = '\0';
										i--;
										if(text2[0]!='\0'&& text2[0]>9){
												fprintf(out,"%s,%s,%s,%s\n",time,text2,text1,text3);
										}

										break;
								case '}':
										while(l.line[i]!=']' && l.line[i]!=',')
												i++;
										i--;
										break;
								case ']':
										l = readline(fp);
										flag=1;
										break;

								default:
										printf("%c",l.line[i]); //error if prints any other
										break;
								}
								if (flag==1){
										flag=0;
										break;
								}
				}
		}
}

fclose(out);
}
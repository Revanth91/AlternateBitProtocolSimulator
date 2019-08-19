#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include<ctype.h>
#include<iostream>
using namespace std;
//structure declaration: As we can't pass arrays as function arguments we are wrapping into a structure to make single object
struct lineWrap1{
char line[1000];
};
//start
//To read each line from a file fp
// input : File pointer Output: The line that the file pointer is pointing.
struct lineWrap1 readline1(FILE *fp){

char ch;
int counter = 0;
char line[1000];
struct lineWrap1 line2;

while( (ch = getc(fp))!=EOF && ch!='\n' ){
    line[counter++] = ch;

}

line[counter] = '\0';
strcpy(line2.line,line);
//printf("%s\n",line2.line);
return line2;

}
//isLine1 function tells us the type of the line ie time or message
// if line is Time returns True else False
int isLine1(struct lineWrap1 line){
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
//end

struct time{
char time[13];
};

struct cyn{
char components_yn[7];
};



int time_comp(struct time t1, struct time t2){
//printf("start time_comp\n");
//printf("%s %s time func\n",t1.time, t2.time);
long time_1=0;
long time_2=0;

char* token_1 = strtok(t1.time, ":");


char *temp1;
char *temp2;
int counter=0;

while(token_1!=NULL){
if (counter<3){
    time_1 = time_1*100+strtol(token_1,&temp1,10);
    }
	else{
	time_1 = time_1*1000+strtol(token_1,&temp1,10);
    }

    token_1 = strtok(NULL, ":");
    counter++;
}
char* token_2 = strtok(t2.time, ":");
counter=0;

while (token_2!= NULL) {
	if (counter<3){
    time_2 = time_2*100+strtol(token_2,&temp2,10);
	}
	else{
	time_2 = time_2*1000+strtol(token_2,&temp2,10);
	}

    token_2 = strtok(NULL, ":");

    counter++;
}
//printf("%ld %ld %d\n",time_1,time_2,counter);

if (time_1==time_2)
    return 0;
else if (time_1>time_2)
    return 1;
else
    return -1;
}


void user_filter(FILE *input, struct time start_t,struct time end_t, struct cyn c, FILE *fpout){



struct lineWrap1 l;
struct lineWrap1 l2;
FILE *fp;
FILE *out;
fp=input;

char ch;
char *str;
int counter = 0;

fprintf(fpout,"time,value,port,component\n");

char time[13];
char text1[20];
char text2[20];
char text3[20];
int flag;
l = readline1(fp);

while(!feof(fp)){
		if (isLine1(l)){
				strcpy(time,l.line);
				l = readline1(fp);

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

                                                struct time present_time;
                                                strcpy(present_time.time,time);

                                                char components_yn[7] = "nnnnnn";

                                                if(strcmp(text3,"sender1")==0)
                                                    components_yn[0]='y';
                                                else if(strcmp(text3,"receiver1")==0)
                                                    components_yn[1]='y';
                                                else if(strcmp(text3,"subnet1")==0)
                                                        components_yn[2]='y';
                                                else if(strcmp(text3,"subnet2")==0)
                                                        components_yn[3]='y';
                                                else if(strcmp(text3,"generator_con")==0)
                                                        components_yn[4]='y';
                                                else if(strcmp(text3,"generator")==0)
                                                        components_yn[5]='y';
                                                 else if(strcmp(text3,"generator_ack")==0)
            											components_yn[6]='y';
                                                //(c.components_yn,components_yn)
                                                int index=0;
                                                for (int a=0;a<strlen(components_yn);a++){
                                                    if(components_yn[a]=='y'){
                                                        index = a;
                                                        break;
                                                    }
                                                }
                                                //printf("%d %d %d ",time_comp(present_time,start_t),time_comp(end_t,present_time),c.components_yn[index]==components_yn[index]);
                                                //printf("%s,%s,%s,%s\n",time,text2,text1,text3);
                                                //printf("%s %s\n",components_yn,c.components_yn);
                                                if (time_comp(present_time,start_t)>-1 && time_comp(end_t,present_time)>-1 && c.components_yn[index]=='y')
                                                    {
                                                    	//printf("%s %s %s times\n",present_time.time,start_t.time,end_t.time);
                                                    	//printf("%d %d ",time_comp(present_time,start_t),time_comp(end_t,present_time));
                                                    	//printf("%c %c\n",components_yn[index],c.components_yn[index]);
                                                    	fprintf(fpout,"%s,%s,%s,%s\n",time,text2,text1,text3);
                                                    	//printf("%s,%s,%s,%s\n",time,text2,text1,text3);
                                                    }
										}

										break;
								case '}':
										while(l.line[i]!=']' && l.line[i]!=',')
												i++;
										i--;
										break;
								case ']':
										l = readline1(fp);
										flag=1;
										break;

								default:
										printf("%c hello",l.line[i]); //error if prints any other
										break;
								}
								if (flag==1){
										flag=0;
										break;
								}
				}
		}
}

}



int main_test(FILE *fp,FILE *fpout){
//FILE *fp;
char start_time[13];
char end_time[13];

long start_t;
long end_t;

struct time t1,t2;



char components[1000];
char components_yn[7]="nnnnnn";

printf("Enter Starting Time:");
scanf("%s",start_time);

printf("Enter End Time:");
scanf("%s",end_time);
//fflush(stdin)//for windows
fpurge(stdin);//for linux
printf("Enter components seperated with a space:");
scanf("%[^\n]",components);

//strcpy(start_time,"00:00:10:000");
//strcpy(end_time,"00:21:48:000");
strcpy(t1.time,start_time);
strcpy(t2.time, end_time);

//strcpy(components,"sender1");

char* token = strtok(components, " ");

// Keep printing tokens while one of the
// delimiters present in str[].
while (token != NULL) {
    if(strcmp(token,"sender1")==0)
        components_yn[0]='y';
    else if(strcmp(token,"receiver1")==0)
        components_yn[1]='y';
    else if(strcmp(token,"subnet1")==0)
            components_yn[2]='y';
    else if(strcmp(token,"subnet2")==0)
            components_yn[3]='y';
    else if(strcmp(token,"generator_con")==0)
            components_yn[4]='y';
    else if(strcmp(token,"generator")==0)
            components_yn[5]='y';
    else if(strcmp(token,"generator_ack")==0)
            components_yn[6]='y';
    token = strtok(NULL, " ");
}
//printf("%s\n",components_yn);
struct cyn c;
strcpy(c.components_yn,components_yn);

//fp = fopen("data/output/abp_output.txt","r");

if(fp == NULL || fpout==NULL) {
	printf("ERROR READING THE FILE");
}
user_filter(fp,t1,t2,c,fpout);


}

//int main(){main_test();}
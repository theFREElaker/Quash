//sams quash.c as it stands
// 9/21/2015 6:45PM

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
//------------
#define BUFMAX 1024

void scrub(char *tbs)
{
	memset(tbs,'\0', sizeof(tbs));
}

void cd(char *chdircmd)//takes in a path buffer, changes directory accordingly
{
	char trustIssues[BUFMAX];
	strcpy(trustIssues,chdircmd);
	char* pathptr=strtok(trustIssues, " \n");//pathptr is "cd"
	pathptr=strtok(NULL, " \n");//pathptr is the path, finally
	
	if(pathptr==NULL){
		//change to home- implement this!
		printf("You tried to change to the home directory, but never implemented that, idiot.\n");
		return;
	}else if(strncmp(pathptr,"/",1)==0){
		//change directory easy style
		chdir(pathptr);
		return;
	}else{//change directory by appending pathptr to current directory- implement
		printf("You tried to change directories a without adding the current directory to the beginning, but never implemented that, idiot.\n");
		return;
	}
}

void plumber(/*what goes here?*/)//executes a piped double-command
{
	pid_t pid, pid2;
	int pipes[2];
	pipe(pipes);
	
	
	//first command
	if((pid=fork())<0){
		printf("ERROR FORKING IN PLUMBER() first process\n");
		exit(1);
	}else if(pid==0){
		//child process
		close(pipes[0]);
		dup2(pipes[1],1);
		//execute first command here
	}
	
	//second command
	if((pid2=fork())<0){
		printf("ERROR FORKING IN PLUMBER() second process\n");
		exit(1);
	}else if(pid2==0){
		//child process
		close(pipes[1]);
		dup2(pipes[0],0);
		//execute second command here
	}
	
	//pipes closed in original
	close(pipes[1]);
	close(pipes[0]);
	int dunn;
	while(waitpid(pid2,&dunn,0)!=pid2){};//busy wait for second process
	return;
}


int main()
{
	char cmdline[BUFMAX];
	scrub(cmdline);

	while(1){
		//read in a command
		fflush(stdout);
		read(0,cmdline,BUFMAX);
		

		//check for special io cases: <,>,or |
		bool diffin = (strpbrk(cmdline,"<")!=NULL);
		bool diffout = (strpbrk(cmdline,">")!=NULL);
		bool mario = (strpbrk(cmdline,"|")!=NULL);
		
		//if the command is exit, exit
		if((strncmp(cmdline,"exit",4)==0)||(strncmp(cmdline,"quit",4)==0)){
			printf("Bye!\n");
			return(0);//should this be exit(0)?  Does it matter? I think this is right
		}else if(strncmp(cmdline,"cd",2)==0){
			cd(cmdline);
			//if the command is cd, change directory
		}else if(diffin){
			//do stuff- redirect from some input file, I guess
		
		}else if((!mario)&&(!diffout)){
			//do stuff- there's no redirect or pipe of any kind
		}else if((!mario)&&(diffout)){
			//do stuff- redirect out, but no pipe
		
		}else if((mario)&&(!diffout)){
			//do stuff- no redirect out, but a pipe is involved
			char* jrr = strtok(cmdline, "|\n");
			char left[256];
			char right[256];
			strcpy(left,jrr);
			jrr = strtok(NULL, "|\n");
			strcpy(right, jrr);

			
			printf("\nLeft = ");
			printf(left);
			printf("\nRight = ");
			printf(right);
			printf("\n");

			scrub(left);
			scrub(right);
		}else if((mario)&&(diffout)){
			//do stuff- redirect out AND there's a pipe
		}else{
		
			printf("\nThis happened.  Probably a bug, huh?\n");
		}
	
	}
	
	return(0);
}

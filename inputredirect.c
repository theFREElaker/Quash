//sams quash.c as it stands
// 9/21/2015 6:45PM

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>

#include "Jobs.h"
//------------
#define BUFMAX 1024
#define ARGMAX 16
#define PATHMAX 768

bool backgroundCheck(char** argBunch)//checks for background char. cleans it, too.  Returns TRUE if background called
{
	//getting too old for this shit
	for(int i=0;i<ARGMAX;i++){
		if(argBunch[i]!=NULL){
			int saizu = strlen(argBunch[i]);
			if(argBunch[i][saizu-1]=='&'){
				argBunch[i][saizu-1]='\0';
				return(1);
			}
		}
	}
	
	return(0);
}

void polish(char *tbp)
{
	sscanf(tbp,"%s",tbp);
}

void scrub(char *tbs)
{
	memset(tbs,'\0', sizeof(tbs));
}

char** parser(char *worthless)//takes in a whole line, returns several lines of arguments
{
	char** tbr = (char**) malloc(sizeof(char*) * ARGMAX);
	char* tempArg;
	char* cb;
	char trustIssues[256];
	strcpy(trustIssues, worthless);
	//token time!
	tempArg = strtok(trustIssues, " \n");//tempArg ought to be first arg, now.
	for(int i=0;i<ARGMAX;i++){
		if(tempArg!=NULL){
			cb = (char*) malloc((strlen(tempArg) * sizeof(char)) + 1);//guess and check til it works
			scrub(cb);
			strcpy(cb, tempArg);
			tbr[i] = cb;
			
			cb=NULL;
			tempArg = strtok(NULL, " \n");//nextarg
		}else{
			tbr[i] = '\0';
		}
	}
	return tbr;
}

void executive(char ***args, bool background)
{
	
	pid_t pid;

	int stat;

	if ((pid = fork()) < 0) {
		printf("Forkerror\n");
		exit(1);
	} else if (pid == 0) {
		if (execvp(**args, *args) < 0) {
			// typing $PATH gets to this statement
			fprintf(stderr, "\nError executing. ERROR #%d: %s\n", errno, strerror(errno));
			std::cout << "Command: " << **args << std::endl;
			exit(1);
		}
		if (!background){
			printf("This is not BG\n");
			while (wait(&stat) != pid){};
				
		}else{
			printf("This is BG\n");
			// Job should be run in background
			//addJob(pid, //How to access command);
			printf("[1] %i\n", pid);
		}
	} 
	return;

}
void cd(char *chdircmd)//takes in a path buffer, changes directory accordingly
{
	char trustIssues[BUFMAX];
	strcpy(trustIssues,chdircmd);
	char* pathptr=strtok(trustIssues, " \n");//pathptr is "cd"
	pathptr=strtok(NULL, " \n");//pathptr is the path, finally
	
	if(pathptr==NULL){
		//change to home- implement this!
		chdir(getenv("HOME"));
		return;
	}else if(strncmp(pathptr,"/",1)==0){
		//change directory easy style
		chdir(pathptr);
		return;
	}else{//change directory by appending pathptr to current directory- implement
		char path[PATHMAX];
		getcwd(path, sizeof(path));
		strcat(path, "/");
		strcat(path, pathptr);
		chdir(path);
		return;
	}
}

void executivePlumber(char*** leftArgs,char*** rightArgs,bool leftBack,bool rightBack)//executes a piped double-command: no final output redirects
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
		if(execvp( **leftArgs,*leftArgs )<0){
			printf("ERROR EXECUTING LEFT in PLUMBER()");
			exit(1);
		}
	}else{
	//leftBack
		if(leftBack){
			printf("[1] %i\n", pid);
		}

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
		if(execvp(**rightArgs,*rightArgs)<0){
			printf("ERROR EXECUTING RIGHT in PLUMBER()\n");
			exit(1);
		}
	}else{
	//rightBack
		if(rightBack){
			printf("[1] %i\n", pid);
		}
	}
	
	//pipes closed in original
	close(pipes[1]);
	close(pipes[0]);
	int dunn;
	while(waitpid(pid2,&dunn,0)!=pid2){};//busy wait for second process
	return;
}

void executiveFalseEntrance(char*** args,bool background,char* input)
{
	pid_t pid;
	int stat;

	if ((pid = fork()) < 0) {
		printf("ERROR FORKING IN INPUTREDIRECT()\n");
		exit(1);
	} else if (pid == 0) {
	
		FILE *fp;
		fp = fopen(input, "r");
		dup2(fileno(fp), 0);
		fclose(fp);
		if (execvp(**args, *args) < 0) {
			printf("EXECUTE FAILURE IN INPUTREDIRECT\n");
			exit(1);
		}
	} else {
		if (!background){
			while (wait(&stat) != pid){};
		
		}else{
			printf("[1] %i\n", pid);
		}
	}

	return;
}

void executiveFalseExit(char ***args,bool background, char* output)
{
	pid_t pid;

	int stat;

	if ((pid = fork()) < 0) {
		printf("NO FORK IN STDOUTREDIRECT\n");
		exit(1);
	} else if (pid == 0) {
		FILE *fp;
		fp = fopen(output, "w");
		dup2(fileno(fp), 1);
		fclose(fp);
		if (execvp(**args, *args) < 0) {
			printf("NO EXECUTE STDOUTREDIRECT\n");
			exit(1);
		}
	} else {
		if (!background){
			while (wait(&stat) != pid){};
		}else{
			printf("[1] %i\n", pid);
		}
	}

	return;
}

void executivePipedFalseExit(char ***leftArgs, char ***rightArgs,bool leftBack, bool rightBack,char* output)
{
	pid_t pid, pid2;
	int stat;
	int pipes[2];

	pipe(pipes);

	if ((pid = fork()) < 0) {
		printf("NO FORK PIPE STDOUTREDIRECT\n");
		exit(1);
	} else if (pid == 0) {
		
		close(pipes[0]);
		dup2(pipes[1], 1);

		if (execvp(**leftArgs, *leftArgs) < 0) {
			printf("Bad execute process one pipe stdout redirect\n");
			exit(1);
		}
	} else {
		if (leftBack) {
			printf("[1] %i\n", pid);
		}
	}

	if ((pid2 = fork()) < 0) {
		printf("NO FORK 2 PIPE STDOUTREDIRECT\n");
		exit(1);
	} else if (pid2 == 0) {

		close(pipes[1]);
		dup2(pipes[0], 0);
		FILE *fp;
		fp = fopen(output, "w");
		dup2(fileno(fp), 1);
		fclose(fp);
		if (execvp(**rightArgs, *rightArgs) < 0) {
			printf("Bad execute process two pipe stdout redirect\n");
			exit(1);
		}

	} else {
		if (rightBack) {
			printf("[1] %i\n", pid);
		}
	}
	close(pipes[1]);
	close(pipes[0]);

	while (waitpid(pid2, &stat, 0) != pid2) {};
	return;
}


int main()
{
	char cmdline[BUFMAX];
	scrub(cmdline);

	while(1){
		//read in a command
		fflush(stdout);
		fgets(cmdline,BUFMAX,stdin);//changed to fgets
		
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
		}else if(strncmp(cmdline,"jobs",3)==0){
			// Print all background jobs
			PrintAllJobs();
		}else if(diffin){
			//do stuff- redirect from some input file, I guess
			
			
			char* jrr = strtok(cmdline, "<\n");
			char left[256];
			char right[256];
			strcpy(left,jrr);
			jrr = strtok(NULL, "<\n");
			strcpy(right, jrr);
			
			sscanf(right,"%s",right);//this works so well
			
/*			
			char** one = parser(left);
			bool bg = backgroundCheck(one);

			//execute false entrance
			executiveFalseEntrance(&one,bg,right);


			
*/
			freopen(right, "r", stdin);
			

			scrub(left);
			scrub(right);	
		}else if((!mario)&&(!diffout)){
			//do stuff- there's no redirect or pipe of any kind
			//this works
			char** someArgs = parser(cmdline);
			bool bg = backgroundCheck(someArgs);
			executive(&someArgs,bg);

		}else if((!mario)&&(diffout)){
			//do stuff- redirect out, but no pipe
			char* jrr = strtok(cmdline, ">\n");
			char left[256];
			char right[256];
			strcpy(left,jrr);
			jrr = strtok(NULL, ">\n");
			strcpy(right, jrr);

			polish(right);
			//execute!
			char** one = parser(left);
			bool bg = backgroundCheck(one);
			executiveFalseExit(&one,bg,right);


			scrub(left);
			scrub(right);		
		}else if((mario)&&(!diffout)){
			//do stuff- no redirect out, but a pipe is involved
			//this works
			char* jrr = strtok(cmdline, "|\n");
			char left[256];
			char right[256];
			strcpy(left,jrr);
			jrr = strtok(NULL, "|\n");
			strcpy(right, jrr);

			/*
			printf("\nLeft = ");
			printf(left);
			printf("\nRight = ");
			printf(right);
			printf("\n");
			*/

			char** one = parser(left);
			bool bg1 = backgroundCheck(one);

			char** two = parser(right);
			bool bg2 = backgroundCheck(two);
			executivePlumber(&one,&two,bg1,bg2);
			
			scrub(left);
			scrub(right);
		}else if((mario)&&(diffout)){
			//do stuff- redirect out AND there's a pipe
			//this is also untested, but I like it so far
			//first, treat it just like pipe above:
			char* jrr = strtok(cmdline, "|\n");
			char left[256];
			char right[256];
			strcpy(left,jrr);
			jrr = strtok(NULL, "|\n");
			strcpy(right, jrr);
			//left is left of pipe, right it right of pipe
			//now token right half for >

			char rl[256];
			char rr[256];
			jrr = strtok(right,">\n");
			strcpy(rl, jrr);
			jrr = strtok(NULL,">\n");
			strcpy(rr, jrr);
			polish(rr);

			char** one = parser(left);
			bool bg1 = backgroundCheck(one);

			char** two = parser(rl);
			bool bg2 = backgroundCheck(two);
			
			
			executivePipedFalseExit(&one,&two,bg1,bg2,rr);
			scrub(left);
			scrub(right);
			scrub(rl);
			scrub(rr);
		}else{
		
			printf("\nThis happened.  Probably a bug, huh?\n");
		}
		//cleanliness is next to godliness
		scrub(cmdline);
	}
	
	return(0);
}

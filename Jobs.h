/*
*	Author: Patrick Walter
*/

#ifndef _JOBS_H_
#define _JOBS_H_

#include <stdio.h>
#include <sys/types.h>

#define MAX_COMMAND_SIZE (200)

typedef struct __job_node_t
{
	struct __job_node_t* next_node;
	struct __job_node_t* prev_node;
	int jobID; // job identifier
	pid_t pid; // pid of child process
	char* command;
}job_node;

extern void traverseJobList(void (*func)(job_node*));
extern void killAllJobs();
extern int addJob(pid_t a_pid, char* a_cmd);
extern job_node* removeJobByPID(pid_t a_pid);
extern int getJobByJobID(int a_job_id);

//#include "Jobs.c"
#endif
// to handle pipes and redirection

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>


// function declarations

int outputRedirExec(char** producer, char* consumer);
int inputRedirExec(char** producer, char* consumer);


// function definitions


/* alternate way to handle commands when they have a pipe
   or redirection statement in them */
int pipeRedirExec(char** tokensList, int length)
{
  char **command = malloc(1024 * sizeof(char*));
  char* file = malloc(sizeof(char*));
  
  // save current position and pointer position
  int curpos = 0, pointer_pos = 0;
  
  // iterate through tokensList until we get an output redir
  while (pointer_pos < length)
    {
      //printf("%s\n",tokensList[pointer_pos]);
      
      if (strcmp(tokensList[pointer_pos],">") == 0)
        {
          while (curpos < pointer_pos)
            {
              command[curpos] = tokensList[curpos];
              curpos++;
            }
          
          strcpy(file,tokensList[pointer_pos+1]);

          outputRedirExec(command,file);
        }

      else if (strcmp(tokensList[pointer_pos],"<") == 0)
        {
          while (curpos < pointer_pos)
            {
              command[curpos] = tokensList[curpos];
              curpos++;
            }

          strcpy(file,tokensList[pointer_pos+1]);

          inputRedirExec(command,file);
        }

      pointer_pos++;
    }
  return 0;
}

  
int outputRedirExec(char** producer, char* consumer)
{
  FILE* redir_file = fopen(consumer,"w+");
  int fd = fileno(redir_file);
  
  int pid, wpid, retval = 0;
  int status;

  pid = fork();

  if(pid == 0){

    // duplicate fd with stdout
    dup2(fd,1);
    
	// this is the child process
    if(execvp(producer[0],producer) == -1)
      {
        printf("Error executing program '%s'\n",producer[0]);
        retval = 1;
      }
    exit(0);
  }

  else if (pid < 0)
    {
      // this means there is an error forking
      printf("Error forking process\n");
      retval = 1;
    }
  
  return retval;
}


// input redirection  
int inputRedirExec(char** consumer, char* producer)
{
  FILE* redir_file = fopen(producer,"r");

  if (!redir_file)
    {
      perror("Error: file not present");
      return 0;
    }

  int fd = fileno(redir_file);
  
  int pid, wpid, retval = 0;
  int status;

  pid = fork();

  if(pid == 0){

    // duplicate fd with stdout
    dup2(fd,0);
    
	// this is the child process
    if(execvp(consumer[0],consumer) == -1)
      {
        printf("Error executing program '%s'\n",consumer[0]);
        retval = 1;
      }
  }

  else if (pid < 0)
    {
      // this means there is an error forking
      printf("Error forking process\n");
      retval = 1;
    }
  
  return retval;
}


// to execute piped commands
int pipeExec()
{
  
}

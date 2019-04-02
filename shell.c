// main REPL for shell

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "commands.h"
#include "redirection.h"

//#include "mycodes.c"
#include <signal.h>


#include <sys/stat.h>
#include <dirent.h>

#include <pwd.h>
#include <grp.h>
#include <time.h>



//defining struct node
typedef struct node
{
  int data;
  struct node *ptr;
}node;
node *fhead=NULL;
node *bhead=NULL;
int length=0;


// global variable declarations
int quit = 0;
int background = 0;
int redir = 0;
int piped = 0;

// function declarations

void repl();
char* readArgs();
char** splitArgs(char* arglist);
void executeCommand(char** tokensList);
int execute(char** tokensList);
int pinfo(char** tokensList);
void pinfo_pid(int pid);
void pinfo_current(int pid);
char* intToStr(int n, char* proc);
int strToInt(char* n);
void stripBrackets(char* proc_name, char* new);
void initialise(char* s);
//int lengthOfJobList();//NEW
void addToJobList(int pid);//NEW
void jobs();//NEW
void overkill();//NEW
void fg(char **argv);
void bg (char **argv);
void kjob(char **argv);
void removeFromJobList(int pid);
int setenvcode(int argc, char **argv);
int unsetenvcode(char **argv);
void stop(char **argv); //NOT NEEDED


// driver function
int main()
{

  // run Read-Eval-Print-Loop function
  repl();
  
  return 0;
}


// to find the length of tokensList
int findLenTokensList(char** tokensList)
{
  int i;
  while(tokensList[i] != NULL)
    {
      i++;
    }

  return i;
}


// quick function to convert an integer to a string
char* intToStr(int n, char* proc)
{
  sprintf(proc, "%d", n);

  return proc;
}


// quick function to initialise a string
void initialise(char* s)
{
  int len = strlen(s);
  int i;
  for(i=0; i<len; i++)
    {
      s[i] = ' ';
    }
  return;
}


// quick function to convert a string to integer
int strToInt(char* n)
{
  return atoi(n);
}


// quick function to strip the brackets from proc_name
void stripBrackets(char* proc_name, char* new)
{
  int i;
  int len = strlen(proc_name);
  for(i=1; i < len-1; i++)
    {
      new[i-1] = proc_name[i];
    }

  return;
}


// to perform repeated Read-Eval-Print-Loop
void repl()
{

  while(1) {

    prompt();
    
    char* arglist = readArgs();

    if( strcmp(arglist,"\n") != 0) {
    
      char** tokensList = splitArgs(arglist);

      executeCommand(tokensList);
    
      if(quit == 1) {
        break;
      }
    }
  }
  
  return;
}


// function to read from stdin
char* readArgs()
{
  
  int req_space = 1;
  char* input = malloc(sizeof(char) * req_space);
  int ptr = 0;
  char temp;

  while (1) {

    temp = getchar();

    input[ptr] = temp;

    if (input[ptr] == '\n' || input[ptr] == EOF) {
      input[ptr] = '\0';
      break;
    }

    ptr++;
    req_space++;
    
    input = realloc(input, req_space);
  }
  
  return input;
}


// function to tokenise arglist
char** splitArgs(char* arglist)
{
  char **tokens = malloc(1024 * sizeof(char*));
  char *token;
  int position = 0;

  token = strtok(arglist," \n");

  while(token != NULL)
    {
      if(strcmp(token,"&") == 0)
        {
          background = 1;
        }
      else if (strcmp(token,"<") == 0)
        {
          redir = 1;
          tokens[position] = token;
          position++;
        }
      else if (strcmp(token,">") == 0)
        {
          redir = 1;
          tokens[position] = token;
          position++;
        }
      else if (strcmp(token,"|") == 0)
        {
          piped = 1;
          tokens[position] = token;
          position++;
        }
      else
        {
          tokens[position] = token;
          position++;
        }

      token = strtok(NULL, " \n");
    }

  tokens[position] = NULL;

  return tokens;
}


// to read in a commmand and call the appropriate execution function
void executeCommand(char** tokensList)
{
  if (piped == 1 || redir == 1)
    {
      pipeRedirExec(tokensList,findLenTokensList(tokensList));
    }
  else if(strcmp(tokensList[0],"cd") == 0)
    {
      cd(tokensList);
    }

  else if(strcmp(tokensList[0],"pwd") == 0)
    {
      pwd();
    }
  else if(strcmp(tokensList[0],"echo") == 0)
    {
      echo(tokensList);
    }

  else if(strcmp(tokensList[0],"ls") == 0)
    {
      ls(tokensList);
    }
  
  else if(strcmp(tokensList[0],"pinfo") == 0)
    {
      pinfo(tokensList);
    }
  else if(strcmp(tokensList[0],"quit") == 0)
    {
      exit (0); //NEW
    }
  else if(strcmp(tokensList[0],"jobs") == 0) jobs();//NEW
  else if(strcmp(tokensList[0],"overkill") ==0) overkill();
  else if (strcmp(tokensList[0],"fg")==0) fg(tokensList);
  else if (strcmp(tokensList[0],"bg")==0) bg(tokensList);
  else if (strcmp(tokensList[0],"kjob")==0) kjob(tokensList);
  else if (strcmp(tokensList[0],"stop")==0) stop(tokensList);
  else if (strcmp(tokensList[0],"setenv")==0) setenvcode(findLenTokensList(tokensList),tokensList);
  else if (strcmp(tokensList[0],"unsetenv")==0) unsetenvcode(tokensList);
  else
    {
      int status = execute(tokensList);

      if(status == 1)
        {
          printf("Error: Command '%s' not found.\n",tokensList[0]);
        }

    }

  background = 0;
  piped = 0;
  redir = 0;
  return;
}


// to execute an user-defined command
int execute(char** tokensList)
{  
  int pid, wpid, retval = 0;
  int status;

  pid = fork();

  if(pid == 0){
	// this is the child process
    if(execvp(tokensList[0],tokensList) == -1)
      {
        printf("Error executing program '%s'\n",tokensList[0]);
        retval = 1;
      }
  }
  else if (pid < 0)
    {
      // this means there is an error forking
      printf("Error forking process\n");
      retval = 1;
    }
  else if (background == 0)
    {
        {
          // parent process
          do
            {
              wpid = waitpid(pid, &status, WUNTRACED);
            }
          while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }
    }
  if (background == 1)//NEW
  {
    addToJobList(pid);
    length++;
    int wpid=waitpid(pid,&status,WNOHANG);
    
    if (wpid!=0)
    { 
      if (WIFEXITED(status)!=-1)
        printf("%s with pid %d exited normally\n",tokensList[0],pid);
      else
        printf("%s with pid %d exited abnormally\n",tokensList[0],pid);
    } 
  }
  
  return retval;
}
 


// to give process info of given process
int pinfo(char** tokensList)
{
  int len = findLenTokensList(tokensList);

  if(len == 2)
    {
      int pid = strToInt(tokensList[1]);

      // to give process info of process with given pid
      pinfo_current(pid);
    }
  else if (len == 1)
    {
      // to give process info of shell process
      pinfo_current(getpid());
    }

  return 0;
}


// to give process info of shell process
void pinfo_current(int pid)
{
  // getting process status
  char proc[4];

  strcpy(proc,intToStr(pid, proc));

  char fname[1000];

  strcpy(fname,"/proc/");
  strcat(fname,proc);
  strcat(fname,"/stat");
  
  FILE* fd = fopen(fname,"r");

  if (!fd)
    {
      printf("Error: Process Does Not Exist.\n");
    }
  else
    {
      
      printf("pid -- %d\n",pid);

      char proc_name[1000],proc_stat[4], pid_str[10], new_proc_name[1000], mem_size[1000];

      initialise(proc_name);
      initialise(proc_stat);
      initialise(pid_str);
      initialise(new_proc_name);
      initialise(mem_size);

      
      fscanf(fd, "%s %s %s",pid_str,proc_name,proc_stat);
  
      stripBrackets(proc_name, new_proc_name);
  
      printf("Process Status -- %s\n",proc_stat);

      fclose(fd);

      fd = fopen(strcat(fname,"us"), "r");

      fscanf(fd, "%s", mem_size);

      while(strcmp(mem_size,"VmSize:") != 0)
        {
          if(strcmp(mem_size,"Cpus_allowed:") != 0) {
            fscanf(fd, "%s", mem_size);
          }
          else
            {
              break;
            }
        }

      if(strcmp(mem_size,"Cpus_allowed:") != 0)
        {
          fscanf(fd, "%s", mem_size);
          printf("memory -- %s {Virtual Memory}\n",mem_size);
        }
      fclose(fd);

      printf("Executable Path -- %s\n",new_proc_name);
    }

  return;
}


/*int lengthOfJobList()//NEW
{
  int length=0;
  node*current;
  for (current=head;current!=NULL;current=current->ptr)
    length++;
  return length;
}*/
void addToJobList(int pid)//NEW
{
  node *link = (node*)malloc(sizeof(node));
  link->data=pid;
  if (length==0) fhead=link;
  else bhead->ptr=link;
  link->ptr=NULL;
  bhead=link;
}
void jobs()//NEW
{
  if (fhead==NULL) return;
  node *p=fhead;
  int i=1;
  while (p!=NULL)
  {
    char proc[4];

    strcpy(proc,intToStr(p->data, proc));

    char fname[1000];

    strcpy(fname,"/proc/");
    strcat(fname,proc);
    strcat(fname,"/stat");
    
    FILE* fd = fopen(fname,"r");
    char proc_name[1000],proc_stat[4], pid_str[10], new_proc_name[1000], mem_size[1000];
    fscanf(fd, "%s %s %s",pid_str,proc_name,proc_stat);
  
    stripBrackets(proc_name, new_proc_name);
    printf("[%d]\t",i++);
   // printf("status: %s \t",proc_stat);
    /*char *a[3];
    a[0]="ps";
    a[1]="-l";
    a[2]=NULL;
    execvp(a[0],a);*/
    int status;
    int wpid = waitpid(p->data, &status, WNOHANG);
    printf((WIFSTOPPED(status)!=0)? "Stopped\t" :"Running\t");
    //wpid = waitpid(p->data, &status, WNOHANG);
    printf(" %s [%d]\n",new_proc_name,p->data);
    /*for (i=0;;i++)
    {  if (p->arg[i]!=NULL) printf("%s \n ",p->arg[i]);
          else break;}*/
    p=p->ptr;
  } 
}
void overkill()//NEW
{
  node *current=fhead;
  int i=0;
  for (i=0;i<length;i++)
  {
    kill(current->data,SIGKILL);
    node *current2=current;
    current=current->ptr;
    free(current2);
  }
  length=0;
  fhead=NULL;
  bhead=NULL;
}
void fg(char ** argv)
{
  int i=1;
  int n=atoi(argv[1]);
  if (n>length)
  {
    printf("Job id %d does not exist\n",n);
    return;
  }
  node *current=fhead;
  node*current2;
  for (i=1;i<n;i++)
  {
    current2=current;
    current=current->ptr;
  }  
  if (current==fhead)
  {
    if (fhead->ptr==NULL) bhead=NULL;
    fhead=fhead->ptr;
  }
  else if (current=bhead)
  {
    if (fhead->ptr==NULL) fhead=NULL;
    bhead=current2;
  }
  else
    current2->ptr=current->ptr;
  length--;
/*  kill(current->data,SIGKILL);
  executeCommand(current->arg);*/
  int status;
  int wpid = waitpid(current->data, &status, WUNTRACED);
}
void removeFromJobList(int pid)
{
  node *current=fhead;
  node*current2;
  for (current=fhead;current->ptr!=NULL;current2=current,current=current->ptr)
  if (current->data==pid) break;  
  if (current==fhead)
  {
    if (fhead->ptr==NULL) bhead=NULL;
    fhead=fhead->ptr;
  }
  else if (current=bhead)
  {
    if (fhead->ptr==NULL) fhead=NULL;
    bhead=current2;
  }
  else
    current2->ptr=current->ptr;
  length--;
}
void bg(char **argv)
{
  int n=atoi(argv[1]);
  int i;
  node *current=fhead;
  for (i=1;i<n;i++)
    current=current->ptr;
  kill(current->data,SIGCONT);
}
void kjob (char **argv)
{
  int signum=atoi(argv[2]);
  int n=atoi(argv[1]);
  int i;
  if (n>length)
  {
    printf("Job number %d does not exist\n",n);
    return;
  }
  
  node *current=fhead;
  for (i=1;i<n;i++)
    current=current->ptr;
  /*int status;
  int wpid = waitpid(current->data, &status, WNOHANG);
  if (WIFSTOPPED(status)!=0)
  {
    printf("Process is stopped\n");
    return;
  }*/
//  wpid = waitpid(current->data, &status, WNOHANG);
  if (signum==9) removeFromJobList(current->data);
  kill(current->data,signum);
}
void stop(char **argv)
{
  int n=atoi(argv[1]);
  int i;
  node *current=fhead;
  for (i=1;i<n;i++)
    current=current->ptr;
  kill(current->data,SIGSTOP);
}
int setenvcode(int argc, char **argv)
{
  if (argv[1]==NULL) printf("FAIL");
  else if (argc>3) printf("FAIL");
  else
  {
    if (argv[2]!=NULL) setenv(argv[1],argv[2],1);
    else setenv (argv[1],"",1);
    //printf("%s",getenv(argv[1]));
  }
  return 0;
}
int unsetenvcode(char **argv)
{
  if (argv[1]==NULL) printf("FAIL");
  int i=1;
  while (1)
  {
    if (argv[i]!=NULL)
      unsetenv(argv[i++]);
    else break;
  }
  return 0;
}
/*void kjob(char **argv)
{
  int signum=atoi(argv[2]);
  signal(atoi(signum,SIG_DFL));
}*/




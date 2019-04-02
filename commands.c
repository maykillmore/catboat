// defining system commands

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include <pwd.h>
#include <grp.h>
#include <time.h>

#include "commands.h"


int pwd()
{
	char cwd[100000];
	printf("%s\n",getcwd(cwd,sizeof(cwd)));
	return 0;
}


int echo(char **argv)
{
  if (argv[1][0]=='$')
  {
    strcpy(argv[1],&argv[1][1]);
    printf("%s\n",getenv(argv[1]));
  }
  else
  {  
    int i=1;
    while (1)
    {
      if (argv[i]!=NULL)
        printf("%s ",argv[i++]);
      else break;
    }
    printf("\n");
  }
	return 0;
}


int cd(char **argv)
{
  if (argv[1]==NULL)
  {
  	chdir(getenv("HOME"));
  }
  else 
  {
    if (chdir(argv[1]) != 0) 
      printf("catboat: cd: %s: No such file or directory\n",argv[1]);
  }
 // char cwd[100000];
 // printf("%s\n",getcwd(cwd,sizeof(cwd)));
  return 0;
}


int ls(char ** argv)
{
  DIR *dir;
  struct dirent *file;
  struct stat st;
  int flaga=0,flagl=0;
  int exist=0,dirpos;
  char cwd[100000];
  if (argv[1]==NULL)
    dir=opendir(getcwd(cwd,sizeof(cwd)));
  else if (argv[1][0]=='-')
  {
    if (argv[1][1]=='a'||argv[1][2]=='a')
      flaga=1;
    if (argv[1][1]=='l'||argv[1][2]=='l')
      flagl=1;
    if (argv[2]!=NULL)
    {
      if (stat(argv[2],&st)==0 && S_ISDIR(st.st_mode))
      {
        dir=opendir(argv[2]);
        dirpos=2;
        exist=1;
      }
      else if (stat(argv[2],&st)==0)
      {
        if (flagl)
          {
            stat(argv[2],&st);
            printf((S_ISDIR(st.st_mode))? "d":"-");
            printf(((st.st_mode & S_IRUSR)==S_IRUSR)? "r":"-");
            printf(((st.st_mode & S_IWUSR)==S_IWUSR)? "w":"-");
            printf(((st.st_mode & S_IXUSR)==S_IXUSR)? "x":"-");
            printf(((st.st_mode & S_IRGRP)==S_IRGRP)? "r":"-");
            printf(((st.st_mode & S_IWGRP)==S_IWGRP)? "w":"-");
            printf(((st.st_mode & S_IXGRP)==S_IXGRP)? "x":"-");
            printf(((st.st_mode & S_IROTH)==S_IROTH)? "r":"-");
            printf(((st.st_mode & S_IWOTH)==S_IWOTH)? "w":"-");
            printf(((st.st_mode & S_IXOTH)==S_IXOTH)? "x":"-");
            printf(" %ld ",st.st_nlink);
            printf("%s ",getpwuid(st.st_uid)->pw_name);
            printf("%s ",getgrgid(st.st_gid)->gr_name);
            printf("%zu\t",st.st_size);
            if (st.st_size<10000) printf("\t");
            int i;
            for (i=4;i<16;printf("%c", ctime(&st.st_mtime)[i++]));
            printf("\t");
            printf("%s\n",argv[2]);}
        else
          printf("%s\n",argv[2]);
        return 0;
      }
      else
      {
        printf("ls: cannot access \'%s\': no such file or directory\n",argv[2]);
        return 0;
      }
    }
    else
      dir=opendir(getcwd(cwd,sizeof(cwd)));
  }  
  else if (argv[1][0]!='-')
  {
   // dir=opendir(argv[1]);
    if (argv[2]!=NULL && argv[2][0]=='-' && (argv[2][1]=='a'||argv[2][2]=='a'))
      flaga=1;
    if (argv[2]!=NULL && argv[2][0]=='-' && (argv[2][1]=='l'||argv[2][2]=='l'))
      flagl=1;
    if (stat(argv[1],&st)==0 && S_ISDIR(st.st_mode))
    {
      dir=opendir(argv[1]);
      dirpos=1;
      exist=1;
    }
    else if (stat(argv[1],&st)==0)
    {
      if (flagl)
        {
          stat(argv[1],&st);
          printf((S_ISDIR(st.st_mode))? "d":"-");
          printf(((st.st_mode & S_IRUSR)==S_IRUSR)? "r":"-");
          printf(((st.st_mode & S_IWUSR)==S_IWUSR)? "w":"-");
          printf(((st.st_mode & S_IXUSR)==S_IXUSR)? "x":"-");
          printf(((st.st_mode & S_IRGRP)==S_IRGRP)? "r":"-");
          printf(((st.st_mode & S_IWGRP)==S_IWGRP)? "w":"-");
          printf(((st.st_mode & S_IXGRP)==S_IXGRP)? "x":"-");
          printf(((st.st_mode & S_IROTH)==S_IROTH)? "r":"-");
          printf(((st.st_mode & S_IWOTH)==S_IWOTH)? "w":"-");
          printf(((st.st_mode & S_IXOTH)==S_IXOTH)? "x":"-");
          printf(" %ld ",st.st_nlink);
          printf("%s ",getpwuid(st.st_uid)->pw_name);
          printf("%s ",getgrgid(st.st_gid)->gr_name);
          printf("%zu\t",st.st_size);
          if (st.st_size<10000) printf("\t");
          int i;
          for (i=4;i<16;printf("%c", ctime(&st.st_mtime)[i++]));
          printf("\t");
          printf("%s\n",argv[1]);}
      else
        printf("%s\n",argv[1]);
      return 0;
    }
    else
    {
      printf("ls: cannot access \'%s\': no such file or directory\n",argv[1]);
      return 0;
    }
  }
  while ((file=readdir(dir))!=NULL)
    if (flaga)
     { 
      if (flagl)
      {
        //printf((stat(file->d_name,&st)==0 && S_ISDIR(st.st_mode))?"d":"-");
        
        if (exist)
        {
          char buf[100000];
          sprintf(buf,"%s/%s",argv[dirpos],file->d_name);
          stat(buf, &st);
        }
        else 
          stat(file->d_name,&st);
        printf((S_ISDIR(st.st_mode))? "d":"-");
        printf(((st.st_mode & S_IRUSR)==S_IRUSR)? "r":"-");
        printf(((st.st_mode & S_IWUSR)==S_IWUSR)? "w":"-");
        printf(((st.st_mode & S_IXUSR)==S_IXUSR)? "x":"-");
        printf(((st.st_mode & S_IRGRP)==S_IRGRP)? "r":"-");
        printf(((st.st_mode & S_IWGRP)==S_IWGRP)? "w":"-");
        printf(((st.st_mode & S_IXGRP)==S_IXGRP)? "x":"-");
        printf(((st.st_mode & S_IROTH)==S_IROTH)? "r":"-");
        printf(((st.st_mode & S_IWOTH)==S_IWOTH)? "w":"-");
        printf(((st.st_mode & S_IXOTH)==S_IXOTH)? "x":"-");
        printf(" %ld ",st.st_nlink);
        printf("%s ",getpwuid(st.st_uid)->pw_name);
        printf("%s ",getgrgid(st.st_gid)->gr_name);
        printf("%zu\t",st.st_size);
        if (st.st_size<10000) printf("\t");
        int i;
        for (i=4;i<16;printf("%c", ctime(&st.st_mtime)[i++]));
        printf("\t");
        printf("%s\n",file->d_name);
      }
      else
        printf("%s ",file->d_name);
    }
    else
    {
      if (file->d_name[0]!='.')
      {
        if (flagl)
        {
          if (exist)
          {
            char buf[100000];
            sprintf(buf,"%s/%s",argv[dirpos],file->d_name);
            stat(buf, &st);
          }
          else
            stat(file->d_name,&st);
          printf((S_ISDIR(st.st_mode))? "d":"-");
          printf(((st.st_mode & S_IRUSR)==S_IRUSR)? "r":"-");
          printf(((st.st_mode & S_IWUSR)==S_IWUSR)? "w":"-");
          printf(((st.st_mode & S_IXUSR)==S_IXUSR)? "x":"-");
          printf(((st.st_mode & S_IRGRP)==S_IRGRP)? "r":"-");
          printf(((st.st_mode & S_IWGRP)==S_IWGRP)? "w":"-");
          printf(((st.st_mode & S_IXGRP)==S_IXGRP)? "x":"-");
          printf(((st.st_mode & S_IROTH)==S_IROTH)? "r":"-");
          printf(((st.st_mode & S_IWOTH)==S_IWOTH)? "w":"-");
          printf(((st.st_mode & S_IXOTH)==S_IXOTH)? "x":"-");
          printf(" %ld ",st.st_nlink);
          printf("%s ",getpwuid(st.st_uid)->pw_name);
          printf("%s ",getgrgid(st.st_gid)->gr_name);
          printf("%zu\t",st.st_size);
          if (st.st_size<10000) printf("\t");
          int i;
          for (i=4;i<16;printf("%c", ctime(&st.st_mtime)[i++]));
          printf("\t");
          printf("%s\n",file->d_name);
        }
        else
          printf("%s ",file->d_name);
      }
    }
    closedir(dir);
    if (!flagl) printf("\n");
  return 0;
}


int prompt()
{
	char hostname[100000];
    printf("<");
	printf("%s@", getenv("USER"));
	gethostname(hostname,sizeof(hostname));
	printf("%s:",hostname);
	printf("~");
	char cwd[100000];
	int i=0,j,flag=0;
	int l=strlen(getcwd(cwd,sizeof(cwd)));
	for (j=0;j<l;j++)
		if (i==3 && flag==0)
		{
			printf("/%c",cwd[j]);
			flag=1;
		}
		else if (i==3)
			printf("%c",cwd[j]);
		else if (cwd[j]=='/') i++;
    printf("> ");
	return 0;
}

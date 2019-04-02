// header file for redirection commands

#ifndef redirection_h
#define redirection_h

int pipeRedirExec(char** tokensList, int length);
int outputRedirExec(char** producer, char** consumer);

#endif

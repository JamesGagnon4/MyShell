/******************************************************************************************
/* PROGRAM:     myshell.c
/* AUTHOR:   
/* DESCRIPTION: Acts as a very simple command line interpreter.  It reads commands from 
/*              standard input entered from the terminal and executes them. The shell does
/*              not include any provisions for control structures, pipes, redirection, 
/*              background processes, environmental variables, or other advanced properties
/*              of a modern shell. All commands are implemented internally and do not rely
/*              on external system programs.
/*******************************************************************************************/
#include <pwd.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#define MAX_PATH_LENGTH           256
#define MAX_BUFFER_LENGTH         256
#define MAX_FILENAME_LENGTH       256

char buffer[MAX_BUFFER_LENGTH] = {0};
char filename[MAX_FILENAME_LENGTH] = {0};
unsigned int result;

DIR *dirp;
struct dirent *d;
struct stat filebuf;
// Implements various UNIX commands using POSIX system calls
int do_cat(const char* filename);
int do_cd(char* dirname);
int do_ls(const char* dirname);
int do_mkdir(const char* dirname);
int do_pwd(void);
int do_rm(const char* filename);
int do_rmdir(const char* dirname);
int do_stat(char* filename);
int execute_command(char* buffer);
  
// Remove extraneous whitespace at the end of a command to avoid parsing problems
void strip_trailing_whitespace(char* string)
{
  int i = strnlen(string, MAX_BUFFER_LENGTH) - 1;

  while(isspace(string[i]))
    string[i--] = 0;
}

// Display a command prompt including the current working directory
void display_prompt(void)
{
  char current_dir[MAX_PATH_LENGTH];

  if (getcwd(current_dir, sizeof(current_dir)) != NULL)
    fprintf(stdout, "%s>", current_dir);
}

int main(int argc, char** argv)
{
  while (1)
    {
      display_prompt();

      // Read a line representing a command to execute from standard input into a character array
      if (fgets(buffer, MAX_BUFFER_LENGTH, stdin) != 0)     
	{
	  strip_trailing_whitespace(buffer);            // Clean up sloppy user input
	  memset(filename, 0, MAX_FILENAME_LENGTH);     //Reset filename buffer after each command execution

	  // As in most shells, "cd" and "exit" are special cases that needed to be handled separately
	  if ((sscanf(buffer, "cd %s", filename) == 1) || (!strncmp(buffer, "cd", MAX_BUFFER_LENGTH)))
	    {
	      result = do_cd(filename);
	      continue;
	    }
	  else if (!strncmp(buffer, "exit", MAX_BUFFER_LENGTH))
	    {
	      exit(0);
	    }
	  else
	    {
	      execute_command(buffer);
	    }
	}
    }
  
  return 0;
}

// changes the current working directory
int do_cd(char* dirname)
{
  struct passwd *p = getpwuid(getuid());
  int result;
  
  if (strnlen(dirname, MAX_PATH_LENGTH) == 0)
      strncpy(dirname, p->pw_dir, MAX_PATH_LENGTH);

  result = chdir(dirname);
  if (result < 0)
    fprintf(stderr, "cd: %s\n", strerror(errno));

  return result;
}

// lists the contents of a directory
int do_ls(const char* dirname)
{
if ( opendir(dirname) == 0){
	printf("Error: No such file or Directory.\n");
}
	else{
	dirp = opendir(dirname);

	while((d =readdir(dirp))!=NULL){
	printf("%s ", d->d_name);

	}}

printf("\n");
}
// outputs the contents of a single ordinary file
int do_cat(const char* filename)
{
int count = 0;
int readfd = open(filename, O_RDONLY);
if(readfd <0){
	printf("Error: Could not open %s\n",filename);
}

else{
	while((count =read(readfd, buffer, MAX_BUFFER_LENGTH)!=0))
	{
		printf("%s\n", buffer);
}	
}
close(readfd);
}

// creates a new directory 
int do_mkdir(const char* dirname)
{
if(mkdir(dirname,0777) ==-1){
	if(errno==EACCES){
	printf("Parent Directory does not allow write permission to the process.\n");
		       	}
	else if(errno==EEXIST){
	printf("Error, directory already exists.\n");
	}
	else if(errno==EFAULT){
	printf("PathnaMe points outside accessible address space.\n");
}
	else if(errno==EINVAL){
	printf("Basename of pathname is invalid\n");
	}
	else if(errno==ENAMETOOLONG){
	printf("Error: Pathname too long\n");
		}
	else{printf("Error, please try again");
}
}
else{
printf("Making directory %s with default mode 777\n",dirname);
mkdir(dirname, 0777);
}
}

// removes a directory as long as it is empty
int do_rmdir(const char* dirname)
{
if (rmdir(dirname) ==-1){
	if(errno==EACCES){
	printf("Error, access to directory not allowed\n");}
	else if(errno==EBUSY){
	printf("Error, pathname is currently in use\n");}
	else if(errno==EFAULT){
	printf("Error, pathname points outside accessible address space\n");}
	else if(errno==EINVAL){
	printf("Error, pathname has . as last component\n");}
	else if(errno==ENAMETOOLONG){
	printf("Error, pathname too long\n");}
	else if(errno==ENOTDIR){
	printf("Error, pathname is not a directory\n");}
	else if(errno==ENOTEMPTY){
	printf("Error, pathname contains illegal entries\n");}
	else if(errno==EROFS){
	printf("Error, pathname leads to a read only directory\n");}
	else{
	printf("Error, please try again\n");
}
}
else{
rmdir(dirname);
}
}

// outputs the current working directory
int do_pwd(void)
{
printf("Current Working Directiory: ");
  char current_dir[MAX_PATH_LENGTH];

  if (getcwd(current_dir, sizeof(current_dir)) != NULL)
    fprintf(stdout, "%s>\n", current_dir);

}

// removes (unlinks) a file
int do_rm(const char* filename)
{
if(unlink(filename)==-1){
	 if(errno==EACCES){
        printf("Error, access to directory not allowed\n");}
        else if(errno==EBUSY){
        printf("Error, pathname is currently in use\n");}
        else if(errno==EFAULT){
        printf("Error, pathname points outside accessible address space\n");}
        else if(errno==EIO){
        printf("Error, an I/O error has occured.\n");}
	else if(errno==EISDIR){
	printf("Error, pathname refers to a directory\n");}
        else if(errno==ENAMETOOLONG){
        printf("Error, pathname too long\n");}
        else if(errno==ENOTDIR){
        printf("Error, pathname is not a directory\n");}
        else if(errno==ENOTEMPTY){
        printf("Error, pathname contains illegal entries\n");}
        else if(errno==EROFS){
        printf("Error, pathname leads to a read only directory\n");}
	else if(errno==EPERM){
	printf("Error, requires priviliges that calling process does not have\n");}
        else{
        printf("Error, please try again\n");

}
}
else{
unlink(filename);
}
}
// outputs information about a file
int do_stat(char* filename)
{
printf("Checking stats of %s \n",filename);
if(stat(filename, &filebuf)==-1){
printf("Error checking stats of %s, File not Found \n",filename);
}
else{
printf("%s stats are:\n",filename);
printf("File Name: %s\n", filename);
printf("File Size: %jd Bytes\n",filebuf.st_size);
printf("Number of links: %d\n", filebuf.st_nlink);
printf("Inode Number: %-10d\n", filebuf.st_ino);
}
}


int execute_command(char* buffer)
 {
   if (sscanf(buffer, "cat %s", filename) == 1)
     {
       result = do_cat(filename);
       return result;
     }
   if (sscanf(buffer, "stat %s", filename) == 1)
     {
       result = do_stat(filename);
       return result;
     }
   if (sscanf(buffer, "mkdir %s", filename) == 1)
     {
       result = do_mkdir(filename);
       return result;
     }
   if (sscanf(buffer, "rmdir %s", filename) == 1)
     {
       result = do_rmdir(filename);
       return result;
     }
   if (sscanf(buffer, "rm %s", filename) == 1)
     {
       result = do_rm(filename);
       return result;
     }
 
   else if ((sscanf(buffer, "ls %s", filename) == 1) || (!strncmp(buffer, "ls", MAX_BUFFER_LENGTH)))
     {
       if (strnlen(filename, MAX_BUFFER_LENGTH) == 0)
	 sprintf(filename, ".");
       
       result = do_ls(filename);
       return result;
     }
   else if (!strncmp(buffer, "pwd", MAX_BUFFER_LENGTH))
     {
       result = do_pwd();
       return result;
     }
   else // Invalid command
     {
       if (strnlen(buffer, MAX_BUFFER_LENGTH) != 0)
	 fprintf(stderr, "myshell: %s: No such file or directory\n", buffer);
       return -1;
     }
 }

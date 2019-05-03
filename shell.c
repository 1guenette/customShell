#include "imports.h"		//.h file containing all file imports needed

const int bufferMax = 1000; //max characters to input and read



/*
function reads user input and allocates values into a double char pointer array, making it easier
to access and view argument.  Function return the number of arguments

@param *cmd = input commands
@param val = array that stores the cmd arguments 
*/
int readInput(char *cmd, char** val)
{
	char cmd2[1024];
	strcpy(cmd2, cmd);

	//Get number of strings
	char *token = strtok(cmd2," ");
	int count = 0;
	while(token != NULL)
	{
		count++;
		token = strtok(NULL," ");
	}
	
	int i=0;

	
	char *token2 = strtok(cmd," ");
	val[0] = token2;
	for ( i = 1; i < count; i++ )
	{
		val[i] = strtok(NULL," ");
	}

	return count;
}

/*
	Function checks for when background process ends, used in the parent process, waiting for a child process 
	to return.  
*/

void checkBackground()
{
	int status;
	int waitID = 1;
	while(waitID>0)
	{
		waitID = waitpid(-1, &status, WNOHANG);
		if(waitID>0)
		{
			printf("[%d] %d\n", waitID,WEXITSTATUS(status));
			waitID = 0;
		}
	}
}


int main(int argc, char** argv) {


/*check for customized prompt, uses default prompt if no input for customized input*/

char *prompt = "308sh> "; //default prompts
if(argc == 3)
{
	if(strcmp(argv[1], "-p")==0)
	{
		prompt = (char*) malloc(sizeof(argv[2])*sizeof(char));
		prompt = argv[2];
	}
}


/*
Loop that executes user commands
*/
while(1)
{
	checkBackground();
	char input[bufferMax];
	char* val[bufferMax];	
	
	//get user input
	printf("%s", prompt);
	fgets(input, bufferMax, stdin);
	
	if(strlen(input)>1)
	{		
		input[strlen(input)-1] = '\0';		//removes '/n' from input values
		int count = readInput(input, val);	//gets count and puts in "string" values into an array
		
		/*First checks if input is a builtin command and return values accordingly*/
		if(strcmp(val[0], "exit")==0)
		{
			break;
		}
		else if(strcmp(val[0], "pid")==0)	//pid command
		{
			int pidVal = getpid();
			printf("%d\n", pidVal);
		}
		else if(strcmp(val[0], "ppid")==0)	//ppid command
		{
			int ppidVal = getppid();
			printf("%d\n", ppidVal);
		}
		else if(strcmp(val[0], "pwd")==0)	//pwd command
		{
			char dir[1024];
			getcwd(dir, sizeof(dir));
			printf("%s\n", dir);
		}
		else if(strcmp(val[0], "cd")==0 )	//cd command
		{
			int ret;
			if(count == 2)			
			{
				ret = chdir(val[1]);
				if(ret)
				{
					printf("Error Invalid Address\n");
				}
			}
			else if(count == 1)
			{
				ret = chdir(getenv("HOME"));
			}
			else{
				printf("Error: Invalid use of cd command\n");
			}
		}
		else if(strcmp(val[0], "set")==0)	//set command
		{
			if(count == 3)
			{
				setenv(val[1],val[2],1);
			}
			else if(count ==2)
			{
				unsetenv(val[1]);
			}
			else
			{
				printf("Error: Invalid Input\n");
			}
		}
		else if(strcmp(val[0], "get")==0 && count==2)	//get command
		{
			char* res = getenv(val[1]);
			if(res != NULL)
			{
				printf("%s\n", res);
			}
			else
			{
				printf("Error: Environmental variable does not exist\n");
			}
		}
		else	/*command is not builtin, so try to execute script from another process*/
		{
			int pid = fork();
			
			if(pid > 0)						//If parent process
			{	usleep(500);
				printf("[%d] %s\n",pid, val[0]);
				if(strcmp(val[count-1], "&")!=0) 		//If running command in foreground, wait for child process
				{
					int status;
					waitpid(pid, &status, 0);
					if(WIFEXITED(status))	
					{
						printf("[%d] %d\n",pid,WEXITSTATUS(status));
					}
				}
				else{						//If running in background, check background status

					checkBackground();
				}

			}
			else							/*If child process, use execvp() to execute arguments, otherwise print error*/
			{
				if(strcmp(val[count-1], "&")==0)
				{
					val[count-1] = '\0';
				}
				execvp(val[0], val); 
				perror("ERROR ");		
				exit(0);
			}
			int status;
						
			
		}
		memset(val,'\0', sizeof(val));					//reset argument array to null and flush last input
		fflush( stdout );
	}
		
	}//end loop
	return 0;
}

#include<stdio.h>
#include<unistd.h>
int main(int argc,char *argv[])
{
	char *args[] = {"./sequential_min_max","100","10",NULL};
	execv(args[0],args);
	return 0;
}

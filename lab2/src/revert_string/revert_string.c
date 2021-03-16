#include "revert_string.h"
#include <string.h>
#include <stdlib.h>
void RevertString(char *str)
{
	// your code here
	int len = strlen(str);
	//char *temp = new char[len];
	char *temp = (char*)malloc(len * sizeof(char));
	for (int i = 0; i < len; i++)
	{
		temp[i] = str[len - 1 - i];
	}
	for (int i = 0; i < len; i++)
	{
		str[i] = temp[i];
	}
}



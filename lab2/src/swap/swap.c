#include "swap.h"
void Swap(char *left, char *right)
{
	// ваш код здесь
	char temp;
	temp=*left;
	*left=*right;
	*right=temp;

}

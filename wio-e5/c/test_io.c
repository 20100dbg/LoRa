#include <stdio.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
	int size = 6;
	char msg[] = "\x01\x02\x0a\x0b\xfe\xff" ;


	write(1, msg, size);

	char *msg2;
	scanf("%ms", &msg2);
	printf("%s\n", msg2);

	return 0;
}
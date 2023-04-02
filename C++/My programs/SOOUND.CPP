#include<dos.h>
#include<iostream.h>
#include<conio.h>
int main()
{
sound(440);
delay(500);
nosound();
return 0;
}
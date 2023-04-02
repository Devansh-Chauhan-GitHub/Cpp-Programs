#include<graphics.h>
#include<stdio.h>
#include<conio.h>
#include<dos.h>

void bird();
void handup();
void handdown();
int a,b,c,d,e;
int c1=20;
int c2=20;

void main()
{

int gdriver = DETECT,gmode;
initgraph(&gdriver,&gmode,"C:\\Tc\\BGI");

//setcolor(GREEN);
b=0;
c=0;
d=0;

for(a=1;a<220;a++)
{

bird();
if(a%2==0)
{
handup();
}
else
{
handdown();
}
delay(100);
cleardevice();
b=b+3;
}
getch();
}
void bird()
{
//bird
circle(50+b,100,5);
line(53+b,98,60+b,100);
line(53+b,102,60+b,100);
circle(50+b,98,1);
ellipse(35+b,100,0,360,10,5);
line(28+b,100,15+b,95);
line(28+b,100,14+b,96);
line(29+b,100,13+b,97);
line(29+b,101,12+b,98);
line(29+b,102,10+b,99);
line(29+b,103,12+b,100);

for(int j=0;j<7;j++){
if(j==0){c1=40;c2=40;}
else if(j==1){c1=-20;c2=-20;}
else if(j==2){c1=20;c2=20;}
else if(j==3){c1=70;c2=70;}
else if(j==4){c1=60;c2=100;}
else if(j==5){c1=20;c2=120;}
else if(j==6){c1=0;c2=140;}
else{c1=40;c2=60;}
circle(50+b-c1,100+c2,5);
line(53+b-c1,98+c2,60+b-c1,100+c2);
line(53+b-c1,102+c2,60+b-c1,100+c2);
circle(50+b-c1,98+c2,1);
ellipse(35+b-c1,100+c2,0,360,10,5);
line(28+b-c1,100+c2,15+b-c1,95+c2);
line(28+b-c1,100+c2,14+b-c1,96+c2);
line(29+b-c1,100+c2,13+b-c1,97+c2);
line(29+b-c1,101+c2,12+b-c1,98+c2);
line(29+b-c1,102+c2,10+b-c1,99+c2);
line(29+b-c1,103+c2,12+b-c1,100+c2);
}
}

void handdown()
{
c=b;

//handdown
ellipse(35+c,106,210,360,6,15);
line(36+c,100,30+c,110);
line(39+c,107,36+c,113);
line(36+c,107,33+c,113);

for(int j=0;j<7;j++){
if(j==0){c1=40;c2=40;}
else if(j==1){c1=-20;c2=-20;}
else if(j==2){c1=20;c2=20;}
else if(j==3){c1=70;c2=70;}
else if(j==4){c1=60;c2=100;}
else if(j==5){c1=20;c2=120;}
else if(j==6){c1=0;c2=140;}
else{c1=40;c2=60;}

ellipse(35+c-c1,106+c2,210,360,6,15);
line(36+c-c1,100+c2,30+c-c1,110+c2);
line(39+c-c1,107+c2,36+c-c1,113+c2);
line(36+c-c1,107+c2,33+c-c1,113+c2);
}
}

void handup()
{
d=b;
//handup
ellipse(35+d,94,0,150,6,15);
line(36+d,100,30+d,90);
line(36+d,87,38+d,93);
line(34+d,87,36+d,93);

for(int j=0;j<7;j++){
if(j==0){c1=40;c2=40;}
else if(j==1){c1=-20;c2=-20;}
else if(j==2){c1=20;c2=20;}
else if(j==3){c1=70;c2=70;}
else if(j==4){c1=60;c2=100;}
else if(j==5){c1=20;c2=120;}
else if(j==6){c1=0;c2=140;}
else{c1=40;c2=60;}
ellipse(35+d-c1,94+c2,0,150,6,15);
line(36+d-c1,100+c2,30+d-c1,90+c2);
line(36+d-c1,87+c2,38+d-c1,93+c2);
line(34+d-c1,87+c2,36+d-c1,93+c2);
}

}
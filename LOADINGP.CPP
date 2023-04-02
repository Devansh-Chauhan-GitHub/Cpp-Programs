#include<graphics.h>
#include<conio.h>
#include<dos.h>
#include<iostream.h>
#include<stdlib.h>
void main()
{
 clrscr();
 int x,y,i;
 int g=DETECT, d;
 initgraph(&g, &d,"C:\\TC\\BGI");
 cleardevice();
 x=getmaxx()/2;
 y=getmaxy()/2;
 settextstyle(TRIPLEX_FONT,HORIZ_DIR,3);
 setbkcolor(rand());
 setcolor(rand());
 int c=150;
for(int e=0;e<40;e++)
 {
  for(int i=50;i<100;i++)
  {
    setcolor(rand());
    settextstyle(SMALL_FONT, HORIZ_DIR, 10);
    outtextxy(x,y,"Loading...");
    settextstyle(SMALL_FONT, HORIZ_DIR, 5);
    outtextxy(x+5,y+5,"Please Wait... ");
    circle(x,y,i);
    circle(x,y,c);
    c--;
    cleardevice();
  }
  }
  setcolor(6);
  settextstyle( DEFAULT_FONT, HORIZ_DIR, 9);
  settextjustify(x,y);
  outtextxy(x-250,y-190,"WELCOME");
  delay(150);
  outtextxy(x-90,y-90,"TO");
  delay(150);
    for(int i1=0;i1<=14;i1++)
    {
    for(i1=0;i1<=14;i1++)
    {
    setcolor(rand());
    settextstyle( DEFAULT_FONT, HORIZ_DIR, 7);
  settextjustify(x,y);
outtextxy(x-280,y+30,"N.C.JINDAL");
  delay(50);
 settextstyle( DEFAULT_FONT, HORIZ_DIR, 9);
  settextjustify(x,y);
  outtextxy(x-200,y+130,"SCHOOL");
  delay(50);
  }
  }
  getch();
 closegraph();

 }
// 3dt->3db

#include <stdio.h>
#define BOOL char
#define true (0==0)
#define false (!true)

FILE* f;
FILE* g;
char b;
int typ;
float num;
unsigned numw;
BOOL min;
float mult;

void main(void){
  f=fopen("turret.3dt", "rb");
  g=fopen("turret.3do", "wb");
  typ=-1;
  while(!feof(f)){
    fread(&b, 1, 1, f);
    if(b==';'){
      while(b!=0x0d&&b!=0x0a){
	fread(&b, 1, 1, f);
      }
    }
    else if(b=='#') {
      typ=0;
      num=0;
      min=false;
      mult=1;
    }
    else if ((b>=48&&b<58)||b=='-'||b=='.'){
      if(typ==-1)typ=1;
      if(b=='-'){
	min=true;
      }else{
	if(b=='.'){
	  mult=0.1;
	}else{
	  if(mult==1){
	    num=num*10+(b-48);
	  }else{
	    num=num+(b-48)*mult;
	    mult*=0.1;
	  }
	}
      }
    }
    else {
      if(typ==0){
	    numw=(unsigned)num;
	    fwrite(&numw, 2, 1, g);
	    typ=-1;
	    num=0;
	    min=false;
	    mult=1;
      }
      if(typ==1){
	    if(min)num=-num;
	    fwrite(&num, 4, 1, g);
	    typ=-1;
	    num=0;
	    min=false;
	    mult=1;
      }
    }
  }
  fclose(g);
  fclose(f);
}
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>              
#include <sys/timeb.h>
#include <dos.h>
// directx
#include <initguid.h>
#define D3D_OVERLOADS
#include <ddraw.h>
#include <d3d.h>

// graphics
#define XRES 640
#define YRES 480
#define BITS 16
#define VIEWDIV 8
#define MAXHEIGHT 4
#define FIELDX 64//256
#define FIELDY 64//256
#define TEXX 8 // Anz der Tex's in X-Richtung
#define TEXY 8
#define TXRES 32//256 // Anz der Texel in einer Textur
#define TYRES 32//256
#define FXRES (TXRES*TEXX/FIELDX) // Anz der Texel in X-Richtung pro Feld
#define FYRES (TYRES*TEXY/FIELDY)
#define TEXSPACEOFF 7 // OFFSET der terrain-Texturen im Textur-array
#define TREESPACEOFF 5
// dx
#define STDCOMPFUNC D3DCMP_LESSEQUAL
#define ZBCLEARVALUE 1
#define ZMULT 0.5//0.5
#define ZADD 0.8
#define VIEWPORTMINZ 0.0f
#define VIEWPORTMAXZ 1.0f
// game
#define OMILS 4
#define TREES 32
#define NUMSHOTS OMILS*200 // prov.
#define OBJECTS 2
#define PARTICLES 200
#define TANKSPEED 4
#define SHOTSPEED 4
#define TURNSPEED 4
#define TURRETTURNSPEED 3
#define SHOTGRAVITY 0
#define PARTICLELIFE 16
#define EXPLFIRE 0.25
#define EXPLFULLSMOKEBEGIN 0.15
#define EXPLFULLSMOKEEND 0.4
#define EXPSIZE 5
// math
#define PI 3.141592654
// debug
#define DEBUG TRUE
// dx
#define SHOTLIGHTOFFSET 100
#define PARTLIGHTOFFSET 100+NUMSHOTS

// windows
HWND hwnd;
MSG messages;
WNDCLASS wincl; 
BOOL bQuit=FALSE;
char szClassName[ ] = "WindowsApp";
// prototypes
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

// DirectX
// defs
#define DXXRESULT(a,b,c) -((a.y-b.y)*(c.z-b.z)-(a.z-b.z)*(c.y-b.y))
#define DXYRESULT(a,b,c) -((a.z-b.z)*(c.x-b.x)-(a.x-b.x)*(c.z-b.z))
#define DXZRESULT(a,b,c) -((a.x-b.x)*(c.y-b.y)-(a.y-b.y)*(c.x-b.x))
#define DXNORM(a,b,c) dxfix(DXXRESULT(a,b,c),DXYRESULT(a,b,c),DXZRESULT(a,b,c))
//#define DXNORM(a,b,c) D3DVECTOR(DXXRESULT(a,b,c),DXYRESULT(a,b,c),DXZRESULT(a,b,c))
#define RELEASE(x) if (x) { (x)->Release(); (x)=NULL; }

// vars
LPDIRECTDRAW7 dxdd;
DDSURFACEDESC2 dxsd;
D3DVIEWPORT7 dxviewport;
LPDIRECTDRAWSURFACE7 dxprimary = NULL;
LPDIRECTDRAWSURFACE7 dxbackbuffer = NULL;
LPDIRECTDRAWSURFACE7 dxzbuffer = NULL;
LPDIRECT3D7 dxd3d = NULL;
LPDIRECT3DDEVICE7 dxd3ddevice = NULL;
LPDIRECT3DVIEWPORT3 dxd3dviewport = NULL;
DDBLTFX dxblt;
// lighting
LPDIRECT3DLIGHT dxd3dlight;
D3DLIGHT7 dxlight;
LPDIRECT3DMATERIAL3 dxd3dmat;
D3DMATERIAL7 dxmat;
D3DMATERIALHANDLE dxhmat;
// texturing
LPDIRECTDRAWSURFACE7 dxstexture[TEXSPACEOFF+TEXX*TEXY];
LPDIRECT3DTEXTURE2 dxtexture;
HDC dxtexturedc, dxtexturedc2;
// transforming
D3DMATRIX matrix;
// zbuffer
long dxzbits=0;

// types
// dx objects
struct dxtriangle {
  long num;
  D3DVERTEX* v;
  unsigned tex;
};

struct dx3dobject {
  long lists, strips, fans;
  dxtriangle *list, *strip, *fan;
} obj[OBJECTS];

struct tspeed {
  float t;// time of last frame
  float c;// speed constant
  float g;// game time
} gs;

struct tparticle {
  float x, y, z, dz;
  float t, size;
  bool used;
  bool lit;
  LPDIRECT3DLIGHT light;
} part[PARTICLES];

// game
float t;// time

// terrain
struct tterrain {
  unsigned char hmap[FIELDX][FIELDY];
  D3DVECTOR norm[FIELDX][FIELDY];
}terra;

// view
struct tview {
  float px, py;// scroll
} view;

struct mil {
  float x, y, z, a, b, c, tb, sp, da, tdx, tdy, tdz, atot, tatot, ta;
  char owner;
  int target;
  float hit;
  float lastfoot, lastshot;
  bool used;
} omil[OMILS];

struct ttree {
  float x, y, z;
  char typ;
  bool used;
} tree[TREES];

struct tshot{
  bool used;
  float x, y, z;
  float dx,dy,dz;
  float t;
  int owner;
  LPDIRECT3DLIGHT light;
} shot[NUMSHOTS];

D3DVECTOR dxfix(float x, float y, float z){
  float len=sqrt(x*x+y*y+z*z);
  D3DVECTOR v=D3DVECTOR(x/len, y/len, z/len);
  return v;
}

void status(long s){
#ifdef DEBUG
  FILE *f;
  f=fopen("status.txt", "w");
  fseek(f, 0, SEEK_SET);
  fwrite( &s, sizeof(s), 1, f);
  fclose(f);
#endif
}

void dxloadobject(dx3dobject* obj, char* fname){
  FILE *f;
  unsigned short typ;
  float scalex, scaley, scalez, fl;
  D3DVECTOR v[3];
  float tu[3], tv[3];
  obj[0].lists=obj[0].strips=obj[0].fans=0;
  obj[0].list=NULL;
  obj[0].strip=NULL;
  obj[0].fan=NULL;
  f=fopen(fname, "rb");
  fseek(f, 0, SEEK_SET);
  while(!feof(f)){
    fread(&typ, 2, 1, f);
    if(typ==0){
      fread(&scalex, 4, 1, f);
      fread(&scaley, 4, 1, f);
      fread(&scalez, 4, 1, f);
    }
    if(typ==1){
      obj[0].lists++;
      if(obj[0].list==NULL){
        obj[0].list=(dxtriangle*)malloc(obj[0].lists*sizeof(dxtriangle));
      } else {
        obj[0].list=(dxtriangle*)realloc(obj[0].list, obj[0].lists*sizeof(dxtriangle));
      }
      fread(&fl, 4, 1, f);obj[0].list[obj[0].lists-1].num=(long)fl;
      fread(&fl, 4, 1, f);obj[0].list[obj[0].lists-1].tex=(unsigned)fl;
      obj[0].list[obj[0].lists-1].v=(D3DVERTEX*)malloc(obj[0].list[obj[0].lists-1].num*sizeof(D3DVERTEX));
      for(int i=0;i<obj[0].list[obj[0].lists-1].num/3;i++){
        for(int j=0;j<3;j++){
          fread(&fl, 4, 1, f);v[j].x=scalex*fl;
          fread(&fl, 4, 1, f);v[j].y=scaley*fl;
          fread(&fl, 4, 1, f);v[j].z=scalez*fl;
          fread(&tu[j], 4, 1, f);
          fread(&tv[j], 4, 1, f);
        }
        for(int j=0;j<3;j++){
          obj[0].list[obj[0].lists-1].v[i*3+j]=D3DVERTEX(v[j], DXNORM(v[0], v[1], v[2]),tu[j], tv[j]);
        }  
      }
    }
    if(typ==2){
      obj[0].strips++;
      if(obj[0].strip==NULL){
        obj[0].strip=(dxtriangle*)malloc(obj[0].strips*sizeof(dxtriangle));
      } else {
        obj[0].strip=(dxtriangle*)realloc(obj[0].strip, obj[0].strips*sizeof(dxtriangle));
      }
      fread(&fl, 4, 1, f);obj[0].strip[obj[0].strips-1].num=(long)fl;
      fread(&fl, 4, 1, f);obj[0].strip[obj[0].strips-1].tex=(unsigned)fl;
      obj[0].strip[obj[0].strips-1].v=(D3DVERTEX*)malloc(obj[0].strip[obj[0].strips-1].num*sizeof(D3DVERTEX));
      // first 3
      for(int i=0;i<3;i++){
        fread(&fl, 4, 1, f);v[i].x=scalex*fl;
        fread(&fl, 4, 1, f);v[i].y=scaley*fl;
        fread(&fl, 4, 1, f);v[i].z=scalez*fl;
        fread(&tu[i], 4, 1, f);
        fread(&tv[i], 4, 1, f);
      }
      for(int i=0;i<3;i++){
        obj[0].strip[obj[0].strips-1].v[i]=D3DVERTEX(v[i], DXNORM(v[0], v[1], v[2]),tu[i], tv[i]);
      }  
      if(obj[0].strip[obj[0].strips-1].num>3){
        for(int i=3;i<obj[0].strip[obj[0].strips-1].num;i++){
          fread(&fl, 4, 1, f);v[(i-3)%3].x=scalex*fl;
          fread(&fl, 4, 1, f);v[(i-3)%3].y=scaley*fl;
          fread(&fl, 4, 1, f);v[(i-3)%3].z=scalez*fl;
          fread(&tu[(i-3)%3], 4, 1, f);
          fread(&tv[(i-3)%3], 4, 1, f);
          if(i%2==0){
            obj[0].strip[obj[0].strips-1].v[i]=D3DVERTEX(v[(i-3)%3], DXNORM(v[0], v[1], v[2]),tu[(i-3)%3], tv[(i-3)%3]);
          } else {
            obj[0].strip[obj[0].strips-1].v[i]=D3DVERTEX(v[(i-3)%3], DXNORM(v[1], v[0], v[2]),tu[(i-3)%3], tv[(i-3)%3]);
          }
        }
      }
    }
  }
  fclose(f);
}

void dxcleanup(void){
  if (dxd3ddevice && dxd3dviewport){
//    dxd3ddevice->DeleteViewport(dxd3dviewport);
  }
  RELEASE(dxd3dviewport);
  RELEASE(dxd3ddevice);
  RELEASE(dxd3d);
  RELEASE(dxprimary);
  RELEASE(dxdd);
}

void dxerror(void){
  dxcleanup();
  DestroyWindow(hwnd);
  exit(0);
}

void winerror(void){
  DestroyWindow(hwnd);
  exit(0);
}

int begr(int x, int ber){
  if(ber>0){
    while(x<0)x+=ber;
    while(x>=ber)x-=ber;
    return x;
  }else{
    return 0;
  }
}

void initterratex(void){
  status(500);
  ZeroMemory(&dxsd, sizeof(dxsd));
  dxsd.dwSize = sizeof(dxsd);
  dxsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
  dxsd.dwWidth = TXRES;
  dxsd.dwHeight = TYRES;
  for(int i=0;i<TEXX;i++){
    for(int j=0;j<TEXY;j++){
      status(501);
      dxsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_VIDEOMEMORY;
      if(dxdd->CreateSurface(&dxsd, &dxstexture[TEXSPACEOFF+j*TEXX+i], NULL)!=D3D_OK){
        dxsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_SYSTEMMEMORY;
        if(dxdd->CreateSurface(&dxsd, &dxstexture[TEXSPACEOFF+j*TEXX+i], NULL)!=D3D_OK) dxerror();
      }
      status(502);
      dxstexture[TEXSPACEOFF+j*8+i]->GetDC(&dxtexturedc);
      status(503);
      for(int k=0;k<TXRES;k++){
        for(int l=0;l<TYRES;l++){
          SetPixel(dxtexturedc, k, l, (256)*(128+rand()%128));
//          SetPixel(dxtexturedc, k, l, (256)*(128+(k+i*TXRES)%128));
        }
      }    
      status(504);
      dxstexture[TEXSPACEOFF+j*8+i]->ReleaseDC(dxtexturedc);
      status(505);
//      if(dxstexture[j*8+i]->QueryInterface(IID_IDirect3DTexture2, (LPVOID *)&dxtexture) != S_OK) dxerror();
//      dxtexture->GetHandle(dxd3ddevice, (DWORD*)&dxhtexture[TEXSPACEOFF+j*TEXX+i]);
//      dxtexture->Release();
    }
  }
  status(506);
  DWORD color;
  for(int k=0;k<TXRES;k++){
    for(int i=0;i<TEXX;i++){
      for(int j=0;j<TEXY;j++){
        dxstexture[TEXSPACEOFF+j*8+i]->GetDC(&dxtexturedc);
        dxstexture[TEXSPACEOFF+j*8+begr(i+1, TEXX)]->GetDC(&dxtexturedc2);
        color=GetPixel(dxtexturedc2, 0, k);
        SetPixel(dxtexturedc, TXRES-1, k, color);
        dxstexture[TEXSPACEOFF+j*8+begr(i+1, TEXX)]->ReleaseDC(dxtexturedc2);
        dxstexture[TEXSPACEOFF+begr(j+1,TEXY)*8+i]->GetDC(&dxtexturedc2);
        color=GetPixel(dxtexturedc2, k, 0);
        SetPixel(dxtexturedc, k, TYRES-1, color);
        dxstexture[TEXSPACEOFF+begr(j+1, TEXY)*8+i]->ReleaseDC(dxtexturedc2);
        dxstexture[TEXSPACEOFF+j*8+i]->ReleaseDC(dxtexturedc);
      }
    }
  }
  status(507);
}
HRESULT CALLBACK TextureSearchCallback( DDPIXELFORMAT* pddpf, VOID* param )
{
    if( pddpf->dwRGBAlphaBitMask      >= 0x1000000)
    {
        memcpy( param, pddpf, sizeof(DDPIXELFORMAT) );
        return DDENUMRET_CANCEL;
    }
    return DDENUMRET_OK;
}

void loadtex(int to, char* fname, int width, int height, bool alphamap){
  status(600);
  ZeroMemory(&dxsd, sizeof(dxsd));
  dxsd.dwSize = sizeof(dxsd);
  dxsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT/* | DDSD_CKSRCBLT*/;
/*  DDCOLORKEY ckey;
  ckey.dwColorSpaceLowValue=0x00000000;
  ckey.dwColorSpaceHighValue=0x00000000;
  dxsd.ddckCKSrcBlt = ckey;*/
  dxsd.dwWidth = width;
  dxsd.dwHeight = height;
  dxsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_VIDEOMEMORY;
  DDPIXELFORMAT dxtexpf;
  dxd3ddevice->EnumTextureFormats( TextureSearchCallback, (VOID*)&dxtexpf );
  memcpy( &dxsd.ddpfPixelFormat, &dxtexpf, sizeof(DDPIXELFORMAT) );
  FILE *bm;
  unsigned char *buf;
  buf=(unsigned char*)VirtualAlloc(NULL, width*height*3, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
  bm=fopen(fname, "rb");
  fseek(bm, -width*height*3, SEEK_END);
  fread(buf, 1, width*height*3, bm);
  fclose(bm);
  status(601);
  dxsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_VIDEOMEMORY;// | DDSCAPS_MIPMAP | DDSCAPS_COMPLEX;
  if(dxdd->CreateSurface(&dxsd, &dxstexture[to], NULL)!=D3D_OK){
    dxsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_SYSTEMMEMORY;// | DDSCAPS_MIPMAP | DDSCAPS_COMPLEX;
    if(dxdd->CreateSurface(&dxsd, &dxstexture[to], NULL)!=D3D_OK)dxerror();
  }
  status(604);
  dxsd.dwSize=sizeof(DDSURFACEDESC2);
  status(605);
  dxstexture[to]->GetSurfaceDesc( &dxsd );
  status(606);
  dxstexture[to]->Lock( NULL, &dxsd, DDLOCK_WAIT, 0 );
  status(607);
  unsigned int *texbuf=(unsigned int*)dxsd.lpSurface;
  status(608);
  if(dxsd.lpSurface==NULL){status(700);dxerror();}
  status(609);
  for(int i=0;i<height;i++){
    for(int j=0;j<width;j++){
      int alpha=buf[(i*width+j)*3]+buf[(i*width+j)*3+1]+buf[(i*width+j)*3+2];
      if(alpha<128){
        alpha=0;
      }else{
        alpha=255;
      }
//      alpha=255;// prov.
      texbuf[((height-1)-i)*width+j]=16777216*alpha+65536*buf[(i*width+j)*3+2]+256*buf[(i*width+j)*3+1]+buf[(i*width+j)*3];
    }
  }    
  status(610);
  dxstexture[to]->Unlock(0);
  status(611);
/*  dxstexture[to]->GetDC(&dxtexturedc);
  for(int i=0;i<height;i++){
    for(int j=0;j<width;j++){
      int alpha=buf[(i*width+j)*3]+buf[(i*width+j)*3+1]+buf[(i*width+j)*3+2];
      SetPixel(dxtexturedc, i, j, 16777216*0xFF+65536*buf[(i*width+j)*3]+256*buf[(i*width+j)*3+1]+buf[(i*width+j)*3+2]);
    }
  }    
  status(612);
  dxstexture[to]->ReleaseDC(dxtexturedc);*/
  status(613);
  VirtualFree(buf, width*height*3, MEM_DECOMMIT|MEM_RELEASE);
  status(614);
//  if(dxstexture2->QueryInterface(IID_IDirect3DTexture2, (LPVOID *)&dxtexture) != S_OK) dxerror();
//  dxtexture->GetHandle(dxd3ddevice, (DWORD*)&dxhtexture[to]);
//  dxtexture->Release();
}

void dxinittex(void){
  status(400);
  ZeroMemory(&dxsd, sizeof(dxsd));
  dxsd.dwSize = sizeof(dxsd);
  dxsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
  // tex.tarnfleck
  dxsd.dwWidth = 64;
  dxsd.dwHeight = 64;
  dxsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_VIDEOMEMORY;
  status(401);
  if(dxdd->CreateSurface(&dxsd, &dxstexture[0], NULL)!=D3D_OK){
    dxsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_SYSTEMMEMORY;
    dxdd->CreateSurface(&dxsd, &dxstexture[0], NULL);  
  }
  status(402);
  dxstexture[0]->GetDC(&dxtexturedc);
  status(403);
  for(int i=0;i<64;i++){
    for(int j=0;j<64;j++){
      SetPixel(dxtexturedc, i, j, (65536+256+1)*(long)(255*(sin(i/8.0)*sin(j/5.0)*0.25+0.75)*(sin(j/8.0)*cos(i/15.0)*0.25+0.75)));
    }
  }    
  status(404);
  dxstexture[0]->ReleaseDC(dxtexturedc);
  status(405);
//  if(dxstexture2->QueryInterface(IID_IDirect3DTexture2, (LPVOID *)&dxtexture) != S_OK) dxerror();
//  dxtexture->GetHandle(dxd3ddevice, (DWORD*)&dxhtexture[0]);
//  dxtexture->Release();
  status(406);
  // tex.ketten
  dxsd.dwWidth = 32;
  dxsd.dwHeight = 32;
  dxsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_VIDEOMEMORY;// | DDSCAPS_MIPMAP | DDSCAPS_COMPLEX;
  if(dxdd->CreateSurface(&dxsd, &dxstexture[1], NULL)!=D3D_OK){
    dxsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_SYSTEMMEMORY;// | DDSCAPS_MIPMAP | DDSCAPS_COMPLEX;
    dxdd->CreateSurface(&dxsd, &dxstexture[1], NULL);  
  }
  dxstexture[1]->GetDC(&dxtexturedc);
  for(int i=0;i<32;i++){
    for(int j=0;j<32;j++){
      if(i>6&&i<32-6){
        SetPixel(dxtexturedc, i, j, (65536+256+1)*(long)(255*((j/4)%2)));
      } else {
        SetPixel(dxtexturedc, i, j, (65536+256+1)*(long)(255*(((j/4)+1)%2)));
      }
    }
  }    
  dxstexture[1]->ReleaseDC(dxtexturedc);
//  if(dxstexture2->QueryInterface(IID_IDirect3DTexture2, (LPVOID *)&dxtexture) != S_OK) dxerror();
//  dxtexture->GetHandle(dxd3ddevice, (DWORD*)&dxhtexture[1]);
//  dxtexture->Release();
  status(407);
  // tex.hügras
  dxsd.dwWidth = 8;
  dxsd.dwHeight = 8;
  dxsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_VIDEOMEMORY;
  if(dxdd->CreateSurface(&dxsd, &dxstexture[2], NULL)!=D3D_OK){
    dxsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_SYSTEMMEMORY;
    dxdd->CreateSurface(&dxsd, &dxstexture[2], NULL);  
  }
  dxstexture[2]->GetDC(&dxtexturedc);
  for(int i=0;i<8;i++){
    for(int j=0;j<8;j++){
      SetPixel(dxtexturedc, i, j, (256)*(128+rand()%128));
    }
  }    
  dxstexture[2]->ReleaseDC(dxtexturedc);
//  if(dxstexture2->QueryInterface(IID_IDirect3DTexture2, (LPVOID *)&dxtexture) != S_OK) dxerror();
//  dxtexture->GetHandle(dxd3ddevice, (DWORD*)&dxhtexture[2]);
//  dxtexture->Release();
  // tex.explosion
  status(408);
  loadtex(3, "expl.bmp", 64, 64, false);
  // tex.rauch
  loadtex(4, "rauch.bmp", 64, 64, false);
  status(409);
  // tex.tree
  loadtex(TREESPACEOFF, "tree.bmp", 64, 64, true);
  // tex.bush
  loadtex(TREESPACEOFF+1, "bush.bmp", 64, 64, true);
  status(410);
  // tex.terra
  initterratex();
  status(411);
}

void setmatopacity(float diffa, float diff, float amb, float spec){
  // mat.objects
  RELEASE(dxd3dmat);
//  dxd3d->CreateMaterial(&dxd3dmat, NULL);
  memset(&dxmat, 0, sizeof(D3DMATERIAL7));
  dxmat.diffuse.r = diff;
  dxmat.diffuse.g = diff;
  dxmat.diffuse.b = diff;
  dxmat.diffuse.a = diffa;
  dxmat.ambient.r = amb;
  dxmat.ambient.g = amb;
  dxmat.ambient.b = amb;
  dxmat.ambient.a = diffa;
  dxmat.specular.r = spec;// Licht spiegeln
  dxmat.specular.g = spec;
  dxmat.specular.b = spec;
  dxmat.power = (float)0.5;
  dxd3ddevice->SetMaterial(&dxmat);
//  dxd3dmat->SetMaterial(&dxmat);
//  dxd3dmat->GetHandle(dxd3ddevice, &dxhmat);
//  dxd3ddevice->SetLightState(D3DLIGHTSTATE_MATERIAL,dxhmat);
}

void dxinitlight(){
  // material
//  dxd3d->CreateMaterial(&dxd3dmat, NULL);
  memset(&dxmat, 0, sizeof(D3DMATERIAL7));
  dxmat.diffuse.r = 1.0;
  dxmat.diffuse.g = (D3DVALUE)1.0;
  dxmat.diffuse.b = (D3DVALUE)1.0;
  dxmat.diffuse.a = (D3DVALUE)1.0;
  dxmat.ambient.r = 1.0;
  dxmat.ambient.g = (D3DVALUE)1.0;
  dxmat.ambient.b = (D3DVALUE)1.0;
  dxmat.ambient.a = (D3DVALUE)1.0;
  dxmat.specular.r = (D3DVALUE)0.3;// Licht spiegeln
  dxmat.specular.g = (D3DVALUE)0.3;
  dxmat.specular.b = (D3DVALUE)0.3;
  // mat.background
  dxd3ddevice->SetMaterial(&dxmat);
//  dxd3dmat->GetHandle(dxd3ddevice, &dxhmat);
//  dxd3dviewport->SetBackground(dxhmat);
  // mat.objects
  setmatopacity(1, 1, 1, 0.3);
  // lighting
//  dxd3d->CreateLight(&dxd3dlight, NULL);
  memset(&dxlight, 0, sizeof(D3DLIGHT7));
  dxlight.dltType = D3DLIGHT_DIRECTIONAL;
  dxlight.dcvDiffuse.r = 0;
  dxlight.dcvDiffuse.g = 0.0;
  dxlight.dcvDiffuse.b = D3DVAL(0.8);
  dxlight.dvAttenuation0 = 0.0;
  dxlight.dvAttenuation1 = 0.0;
  dxlight.dvAttenuation2 = 0.0;
  dxlight.dvPosition.x = 0.0;
  dxlight.dvPosition.y = D3DVAL(0.0);
  dxlight.dvPosition.z = D3DVAL(0);
  dxlight.dvDirection.x = 0;
  dxlight.dvDirection.y = D3DVAL(0.0);
  dxlight.dvDirection.z = D3DVAL(1);
  dxd3ddevice->SetLight(1, &dxlight);
  dxd3ddevice->LightEnable(1, TRUE);
//  dxd3dviewport->AddLight(dxd3dlight);
//  dxd3d->CreateLight(&dxd3dlight, NULL);
  dxlight.dcvDiffuse.r = 0.8;
  dxlight.dcvDiffuse.g = 0.7;
  dxlight.dcvDiffuse.b = D3DVAL(0.2);
  dxlight.dvDirection.x = 1;
  dxlight.dvDirection.y = D3DVAL(0.0);
  dxlight.dvDirection.z = D3DVAL(0.5);
  dxd3ddevice->SetLight(2, &dxlight);
  dxd3ddevice->LightEnable(2, TRUE);
//  dxd3dviewport->AddLight(dxd3dlight);
}

static HRESULT WINAPI EnumZBufferCallback( DDPIXELFORMAT* pddpf,
                                           VOID* pddpfDesired )
{
    if( pddpf->dwFlags == DDPF_ZBUFFER )
    {
        if(dxzbits<16){
          memcpy( pddpfDesired, pddpf, sizeof(DDPIXELFORMAT) );
          dxzbits=pddpf->dwZBufferBitDepth;
        }
        // Return with D3DENUMRET_CANCEL to end the search.
		return D3DENUMRET_CANCEL;
    }

    // Return with D3DENUMRET_OK to continue the search.
    return D3DENUMRET_OK;
}

void dxinit(void){
  // Init DirectX and Direct3D
  // create direct draw object on primary surface
  status(301);
  DirectDrawCreateEx( NULL, (VOID**)&dxdd, IID_IDirectDraw7, NULL );
/*  LPDIRECTDRAW dxdd1;
  if(DirectDrawCreate(NULL, &dxdd1, NULL )!=DD_OK)dxerror();
  status(302);
  // create a DX7 version of the interface
  if(dxdd1->QueryInterface(IID_IDirectDraw7, (LPVOID*)&dxdd)!=DD_OK)dxerror();
  // we're done with the DD1 interface
  RELEASE(dxdd1);*/
  status(303);
  // set the cooperative level
  if (dxdd->SetCooperativeLevel(hwnd, DDSCL_FULLSCREEN  | DDSCL_EXCLUSIVE | DDSCL_ALLOWMODEX | DDSCL_FPUSETUP)!=DD_OK) dxerror();
  status(304);
  if(dxdd->SetDisplayMode(XRES,YRES,BITS, 0, 0)!=DD_OK) dxerror();
  status(305);
  // create the primary surface
  ZeroMemory(&dxsd, sizeof(dxsd));
  dxsd.dwSize = sizeof(dxsd);
  dxsd.dwFlags = DDSD_CAPS|DDSD_BACKBUFFERCOUNT;
  dxsd.dwBackBufferCount=1;
  dxsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE|DDSCAPS_VIDEOMEMORY  |DDSCAPS_3DDEVICE |
                             DDSCAPS_FLIP | DDSCAPS_COMPLEX;
  if (dxdd->CreateSurface(&dxsd, &dxprimary, NULL )!=DD_OK){
    dxsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE|DDSCAPS_SYSTEMMEMORY;
    if (dxdd->CreateSurface(&dxsd, &dxprimary, NULL )!=DD_OK) dxerror();
  }
  status(306);
  DDSCAPS2 ddscaps = { DDSCAPS_BACKBUFFER, 0, 0, 0 };
  if( dxprimary->GetAttachedSurface( &ddscaps,&dxbackbuffer )!=DD_OK )dxerror();
  // Create backbuffer
/*  ZeroMemory(&dxsd, sizeof(dxsd));
  dxsd.dwSize = sizeof(dxsd);
  dxsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
  dxsd.dwWidth = XRES;
  dxsd.dwHeight = YRES;
  dxsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE | DDSCAPS_VIDEOMEMORY;
  status(307);
  if (dxdd->CreateSurface(&dxsd, &dxbackbuffer, NULL) != DD_OK){
    status(308);
    dxsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE | DDSCAPS_SYSTEMMEMORY;
    if (dxdd->CreateSurface(&dxsd, &dxbackbuffer, NULL) != DD_OK) dxerror();
    status(309);
  }
  status(310);
  if (dxprimary->AddAttachedSurface(dxbackbuffer) != DD_OK) dxerror();*/
  status(311);
  // get the direct3d object
  if (dxdd->QueryInterface(IID_IDirect3D7, (LPVOID*)&dxd3d)!=DD_OK) dxerror();
  status(312);
  // Create a z-buffer and attach it to the backbuffer
    DDPIXELFORMAT ddpfZBuffer;
	dxd3d->EnumZBufferFormats( IID_IDirect3DHALDevice, 
		                        EnumZBufferCallback, (VOID*)&ddpfZBuffer );
  status(313);
    if( sizeof(DDPIXELFORMAT) != ddpfZBuffer.dwSize )dxerror();
  status(314);
    dxsd.dwFlags        = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT;
    dxsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER;
	dxsd.dwWidth        = XRES;
	dxsd.dwHeight       = YRES;
    memcpy( &dxsd.ddpfPixelFormat, &ddpfZBuffer, sizeof(DDPIXELFORMAT) );
    dxsd.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
  status(315);
    if( dxdd->CreateSurface( &dxsd, &dxzbuffer, NULL )!=DD_OK  )dxerror();
  status(316);
    if( dxbackbuffer->AddAttachedSurface( dxzbuffer  )!=DD_OK )dxerror();
  status(317);
                          
/*  dxsd.ddpfPixelFormat.dwSize=sizeof(DDPIXELFORMAT);
  dxsd.ddpfPixelFormat.dwFlags=DDPF_ZBUFFER;
  dxsd.ddpfPixelFormat.dwZBufferBitDepth = 16;
  dxsd.dwFlags        = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT;
  dxsd.dwWidth        = XRES;
  dxsd.dwHeight       = YRES;
  status(312);
  dxsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER|DDSCAPS_VIDEOMEMORY;
  if( dxdd->CreateSurface( &dxsd, &dxzbuffer, NULL )!=DD_OK){
    status(313);
    dxsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER|DDSCAPS_SYSTEMMEMORY;
    if( dxdd->CreateSurface( &dxsd, &dxzbuffer, NULL )!=DD_OK)dxerror();
    status(314);
  }
  status(315);
  if( dxbackbuffer->AddAttachedSurface( dxzbuffer )!=DD_OK)dxerror();*/
/*  ZeroMemory(&dxsd, sizeof(dxsd));
  dxsd.dwSize = sizeof(dxsd);
  dxsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
  dxsd.dwWidth = XRES;
  dxsd.dwHeight = YRES;
  dxsd.ddpfPixelFormat.dwSize=sizeof(DDPIXELFORMAT);
  dxsd.ddpfPixelFormat.dwRGBBitCount = 16;
  dxsd.ddpfPixelFormat.dwZBufferBitDepth = 16;
  dxsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | DDSCAPS_VIDEOMEMORY;
  status(312);
  if (dxdd->CreateSurface(&dxsd, &dxzbuffer, NULL) != DD_OK){
    status(313);
    dxsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | DDSCAPS_SYSTEMMEMORY;
    if (dxdd->CreateSurface(&dxsd, &dxzbuffer, NULL) != DD_OK) dxerror();
    status(314);
  }  
  if (dxbackbuffer->AddAttachedSurface(dxzbuffer) != DD_OK) dxerror();
  status(315);*/
  // create the direct 3d device from the surface
//  if (dxd3d->CreateDevice(IID_IDirect3DTnLHALDevice,dxbackbuffer, &dxd3ddevice)!=DD_OK){
//    MessageBox(hwnd, "No TnL","Hardware notification",MB_OK);
    if (dxd3d->CreateDevice(IID_IDirect3DHALDevice,dxbackbuffer, &dxd3ddevice)!=DD_OK){
      MessageBox(hwnd, "Warning! You don't have a suitable 3D-accelerator card. Trying slow software mode...","Hardware warning",MB_OK);
      if (dxd3d->CreateDevice(IID_IDirect3DMMXDevice,dxbackbuffer, &dxd3ddevice)!=DD_OK){
        if (dxd3d->CreateDevice(IID_IDirect3DRGBDevice,dxbackbuffer, &dxd3ddevice)!=DD_OK){
          if (dxd3d->CreateDevice(IID_IDirect3DRampDevice,dxbackbuffer, &dxd3ddevice)!=DD_OK){
            MessageBox(hwnd, "Failed to initiate Direct3D","Hardware error",MB_OK);
            dxerror();
          }
        }
      }  
    }  
//  }
  status(318);
  // get the surface description so we know the width & height
  if (dxprimary->GetSurfaceDesc(&dxsd)!=DD_OK) dxerror();
  // create the viewport
//  if (dxd3d->CreateViewport(&dxd3dviewport, NULL)!=D3D_OK) dxerror();
  ZeroMemory(&dxviewport, sizeof(dxviewport));
//  dxviewport.dwSize = sizeof(dxviewport);
  dxviewport.dwX = 0;
  dxviewport.dwY = 0;
  dxviewport.dwWidth = XRES;
  dxviewport.dwHeight = YRES;
  dxviewport.dvMinZ = VIEWPORTMINZ;
  dxviewport.dvMaxZ = VIEWPORTMAXZ;
  if (dxd3ddevice->SetViewport(&dxviewport)!=DD_OK) dxerror();
  // lighting
  status(319);
  dxinitlight();
  // textures
  status(320);
  dxinittex();
  status(321);
  // state
//  dxd3ddevice->SetRenderState(D3DRENDERSTATE_NORMALIZENORMALS,TRUE);
//  dxd3ddevice->SetRenderState(D3DRENDERSTATE_TEXTUREADDRESS, D3DTADDRESS_CLAMP);
  dxd3ddevice->SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE,FALSE);
  dxd3ddevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);
  dxd3ddevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFG_LINEAR);
  dxd3ddevice->SetRenderState(D3DRENDERSTATE_ZENABLE,TRUE);//D3DZB_USEW
  dxd3ddevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,TRUE);
  dxd3ddevice->SetRenderState(D3DRENDERSTATE_ZFUNC,STDCOMPFUNC);
  dxd3ddevice->SetRenderState( D3DRENDERSTATE_AMBIENT, 0x40404000 );
//  dxd3ddevice->SetLightState (D3DLIGHTSTATE_AMBIENT, RGBA_MAKE(0, 0, 0, 255));
//  dxd3ddevice->SetLightState (D3DLIGHTSTATE_AMBIENT, RGBA_MAKE(80, 80, 80, 0));
//  dxd3ddevice->SetLightState (D3DLIGHTSTATE_AMBIENT, RGBA_MAKE(255, 255, 255, 255));
  dxd3ddevice->SetRenderState(D3DRENDERSTATE_LIGHTING,TRUE); 
  dxd3ddevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE,FALSE); 
//  dxd3ddevice->SetRenderState(D3DRENDERSTATE_BLENDENABLE, TRUE);
//  dxd3ddevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
//  dxd3ddevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA); 
//  dxd3ddevice->SetRenderState(D3DRENDERSTATE_EDGEANTIALIAS,TRUE);
//  dxd3ddevice->SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_WIREFRAME);
  matrix=D3DMATRIX(1,0,0,0, 0,1,0,0, 0,0,1,0.5, 0,0,0,1);
  dxd3ddevice->SetTransform(D3DTRANSFORMSTATE_PROJECTION, &matrix);
  matrix=D3DMATRIX(1,0,0,0, 0,1,0,0, 0,0,ZMULT,0, 0,0,ZADD,1);
  dxd3ddevice->MultiplyTransform(D3DTRANSFORMSTATE_PROJECTION, &matrix);
  // load objects
  status(322);
  dxloadobject(&obj[0], "tank.3do");
  dxloadobject(&obj[1], "turret.3do");
  status(323);
}

void renderobj(dx3dobject obj, float x, float y, float z, float a, float b, float c){
//  D3DVERTEX v[3];
  // transformations
  matrix=D3DMATRIX(cos(a),sin(a),0,0, -sin(a),cos(a),0,0, 0,0,1,0, x, y, z, 1);
  dxd3ddevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matrix);
  matrix=D3DMATRIX(cos(c),0,-sin(c),0, 0,1,0,0, sin(c),0,cos(c),0, 0, 0, 0, 1);
  dxd3ddevice->MultiplyTransform(D3DTRANSFORMSTATE_WORLD, &matrix);
  matrix=D3DMATRIX(1,0,0,0, 0,cos(b),-sin(b),0, 0,sin(b),cos(b),0, 0, 0, 0, 1);
  dxd3ddevice->MultiplyTransform(D3DTRANSFORMSTATE_WORLD, &matrix);
  // drawing
  if(obj.lists>0){
    for(int i=0;i<obj.lists;i++){
//      dxd3ddevice->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE, dxhtexture[obj.list[i].tex]);
      dxd3ddevice->SetTexture(0, dxstexture[obj.list[i].tex]);
      dxd3ddevice->DrawPrimitive(D3DPT_TRIANGLELIST,D3DFVF_VERTEX,&obj.list[i].v[0],obj.list[i].num,0);
    }
  }
  if(obj.strips>0){
    for(int i=0;i<obj.strips;i++){
//      dxd3ddevice->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE, dxhtexture[obj.strip[i].tex]);
      dxd3ddevice->SetTexture(0, dxstexture[obj.strip[i].tex]);
      dxd3ddevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_VERTEX,&obj.strip[i].v[0],obj.strip[i].num,0);
    }
  }
}

void rendersecobj(dx3dobject obj, float x, float y, float z, float a, float b, float c, float pa, float pb, float pc){
//  D3DVERTEX v[3];
  // transformations
  matrix=D3DMATRIX(cos(pa),sin(pa),0,0, -sin(pa),cos(pa),0,0, 0,0,1,0, x, y, z, 1);
  dxd3ddevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matrix);
  matrix=D3DMATRIX(cos(pc),0,-sin(pc),0, 0,1,0,0, sin(pc),0,cos(pc),0, 0, 0, 0, 1);
  dxd3ddevice->MultiplyTransform(D3DTRANSFORMSTATE_WORLD, &matrix);
  matrix=D3DMATRIX(1,0,0,0, 0,cos(pb),-sin(pb),0, 0,sin(pb),cos(pb),0, 0, 0, 0, 1);
  dxd3ddevice->MultiplyTransform(D3DTRANSFORMSTATE_WORLD, &matrix);
  matrix=D3DMATRIX(cos(a),sin(a),0,0, -sin(a),cos(a),0,0, 0,0,1,0, 0, 0, 0, 1);
  dxd3ddevice->MultiplyTransform(D3DTRANSFORMSTATE_WORLD, &matrix);
  matrix=D3DMATRIX(cos(c),0,-sin(c),0, 0,1,0,0, sin(c),0,cos(c),0, 0, 0, 0, 1);
  dxd3ddevice->MultiplyTransform(D3DTRANSFORMSTATE_WORLD, &matrix);
  matrix=D3DMATRIX(1,0,0,0, 0,cos(b),-sin(b),0, 0,sin(b),cos(b),0, 0, 0, 0, 1);
  dxd3ddevice->MultiplyTransform(D3DTRANSFORMSTATE_WORLD, &matrix);
  // drawing
  if(obj.lists>0){
    for(int i=0;i<obj.lists;i++){
//      dxd3ddevice->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE, dxhtexture[obj.list[i].tex]);
      dxd3ddevice->SetTexture(0, dxstexture[obj.list[i].tex]);
      dxd3ddevice->DrawPrimitive(D3DPT_TRIANGLELIST,D3DFVF_VERTEX,&obj.list[i].v[0],obj.list[i].num,0);
    }
  }
  if(obj.strips>0){
    for(int i=0;i<obj.strips;i++){
//      dxd3ddevice->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE, dxhtexture[obj.strip[i].tex]);
      dxd3ddevice->SetTexture(0, dxstexture[obj.strip[i].tex]);
      dxd3ddevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_VERTEX,&obj.strip[i].v[0],obj.strip[i].num,0);
    }
  }
}

float begrf(float x, float ber){
  if(ber>0){
    while(x<0)x+=ber;
    while(x>=ber)x-=ber;
    return x;
  }else{
    return 0;
  }
}

void renderterra(void){
  D3DVERTEX vx[4];
  int l=-15, r=16, o=22, u=-33;
  int tx, ty;
  matrix=D3DMATRIX(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
  dxd3ddevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matrix);
  for(int j=u+(int)view.py;j<o+(int)view.py;j++){
    for(int i=l+(int)view.px;i<r+(int)view.px;i++){
      tx=begr(i, FIELDX)*TEXX/FIELDX;
      ty=begr(j, FIELDY)*TEXY/FIELDY;
//      dxd3ddevice->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE, dxhtexture[TEXSPACEOFF+ty*TEXX+tx]);
      dxd3ddevice->SetTexture(0, dxstexture[TEXSPACEOFF+ty*TEXX+tx]);
      vx[0]=D3DVERTEX(D3DVECTOR(i, j, -terra.hmap[begr(i,FIELDX)][begr(j,FIELDY)]/255.0*MAXHEIGHT), terra.norm[begr(i, FIELDX)][begr(j, FIELDX)], FXRES*1.0f/TXRES*begr(i, FIELDX)-tx, FYRES*1.0f/TYRES*begr(j, FIELDY)-ty);
      vx[1]=D3DVERTEX(D3DVECTOR(i, j+1, -terra.hmap[begr(i,FIELDX)][begr(j+1,FIELDY)]/255.0*MAXHEIGHT), terra.norm[begr(i, FIELDX)][begr(j+1, FIELDX)], FXRES*1.0f/TXRES*begr(i, FIELDX)-tx, FYRES*1.0f/TYRES*begr(j, FIELDY)+FYRES*1.0f/TYRES-ty);
      vx[2]=D3DVERTEX(D3DVECTOR(i+1, j, -terra.hmap[begr(i+1,FIELDX)][begr(j,FIELDY)]/255.0*MAXHEIGHT), terra.norm[begr(i+1, FIELDX)][begr(j, FIELDX)], FXRES*1.0f/TXRES*begr(i, FIELDX)+FXRES*1.0f/TXRES-tx, FYRES*1.0f/TYRES*begr(j, FIELDY)-ty);
      vx[3]=D3DVERTEX(D3DVECTOR(i+1, j+1, -terra.hmap[begr(i+1,FIELDX)][begr(j+1,FIELDY)]/255.0*MAXHEIGHT), terra.norm[begr(i+1, FIELDX)][begr(j+1, FIELDX)], FXRES*1.0f/TXRES*begr(i, FIELDX)+FXRES*1.0f/TXRES-tx, FYRES*1.0f/TYRES*begr(j, FIELDY)+FYRES*1.0f/TYRES-ty);
      dxd3ddevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_VERTEX,&vx[0],4,0);
    }
  }  
}

void rendertrees(void){
  D3DVERTEX vx[4];
  D3DVECTOR v[4];
  dxd3ddevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);//SRCCOLOR
  dxd3ddevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);//INVSRCCOLOR
  matrix=D3DMATRIX(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
  dxd3ddevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matrix);
  //tree
  for(int i=0;i<TREES;i++){
    if(tree[i].used){
      dxd3ddevice->SetTexture(0, dxstexture[TREESPACEOFF+tree[i].typ]);
      v[0]=D3DVECTOR(-1+tree[i].x, 0.0+tree[i].y, -5+tree[i].z);
      v[1]=D3DVECTOR(1+tree[i].x, -0.0+tree[i].y, -5+tree[i].z);
      v[2]=D3DVECTOR(-1+tree[i].x, 0.0+tree[i].y, tree[i].z);
      v[3]=D3DVECTOR(1+tree[i].x, -0.0+tree[i].y, tree[i].z);
      vx[0]=D3DVERTEX(v[0], DXNORM(v[0], v[1], v[2]), 0, 0);
      vx[1]=D3DVERTEX(v[1], DXNORM(v[0], v[1], v[2]), 1, 0);
      vx[2]=D3DVERTEX(v[2], DXNORM(v[0], v[1], v[2]), 0, 1);
      vx[3]=D3DVERTEX(v[3], DXNORM(v[0], v[1], v[2]), 1, 1);
      dxd3ddevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_VERTEX,&vx[0],4,0);
    }
  }
}

void rendermil(mil milobj){
  renderobj(obj[0], milobj.x, milobj.y, milobj.z, milobj.a, milobj.b, milobj.c);
  rendersecobj(obj[1], milobj.x, milobj.y, milobj.z,milobj.ta,milobj.tb,0, milobj.a, milobj.b, milobj.c);
}

void rendershot(int n){
  D3DVERTEX vx[4];
  matrix=D3DMATRIX(cos(gs.g),sin(gs.g),0,0, -sin(gs.g),cos(gs.g),0,0, 0,0,1,0, shot[n].x, shot[n].y, shot[n].z, 1);
  dxd3ddevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matrix);
  dxd3ddevice->SetTexture(0, dxstexture[0]);
  vx[0]=D3DVERTEX(D3DVECTOR(-0.08, -0.08, 0), D3DVECTOR(0,0,-1), 0, 0);
  vx[1]=D3DVERTEX(D3DVECTOR(-0.08, 0.08, 0), D3DVECTOR(0,0,-1), 0, 1);
  vx[2]=D3DVERTEX(D3DVECTOR(0.08, -0.08, 0), D3DVECTOR(0,0,-1), 1, 0);
  vx[3]=D3DVERTEX(D3DVECTOR(0.08, 0.08, 0), D3DVECTOR(0,0,-1), 1, 1);
  dxd3ddevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_VERTEX,&vx[0],4,0);
}

float height(float x, float y){
  int xi, yi;
  xi=x<0?((int)x-1):(int)x;
  yi=y<0?((int)y-1):(int)y;
  return -(terra.hmap[begr(xi, FIELDX)][begr(yi, FIELDY)]*(1-(x-xi))*(1-(y-yi))
          +terra.hmap[begr(xi+1, FIELDX)][begr(yi, FIELDY)]*(x-xi)*(1-(y-yi))
          +terra.hmap[begr(xi, FIELDX)][begr(yi+1, FIELDY)]*(1-(x-xi))*(y-yi)
          +terra.hmap[begr(xi+1, FIELDX)][begr(yi+1, FIELDY)]*(x-xi)*(y-yi)
          )/255.0*MAXHEIGHT;
}

void renderparticle(int n){
  D3DLVERTEX vx[4];
//  D3DVERTEX vx[4];
  unsigned char opy;
  dxd3ddevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCCOLOR);//SRCALPHA
  dxd3ddevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);//ONE
/*    dxd3ddevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_ADDSMOOTH );
    dxd3ddevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    dxd3ddevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT );*/
  matrix=D3DMATRIX(1,0,0,0, 0,1,0,0, 0,0,1,0, part[n].x, part[n].y, part[n].z, 1);
  dxd3ddevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matrix);
  if(gs.g-part[n].t<PARTICLELIFE*EXPLFIRE){
    opy=(unsigned char)((1.0-(gs.g-part[n].t)/(PARTICLELIFE*EXPLFIRE))*255.0);
    dxd3ddevice->SetTexture(0, dxstexture[3]);
    vx[0]=D3DLVERTEX(D3DVECTOR(-0.65*part[n].size, -0.65*part[n].size, 0), RGBA_MAKE(opy,opy,opy,opy), RGBA_MAKE(0,0,0,0), 0, 0);
    vx[1]=D3DLVERTEX(D3DVECTOR(-0.65*part[n].size, 0.65*part[n].size, 0), RGBA_MAKE(opy,opy,opy,opy), RGBA_MAKE(0,0,0,0), 0, 1);
    vx[2]=D3DLVERTEX(D3DVECTOR(0.65*part[n].size, -0.65*part[n].size, 0), RGBA_MAKE(opy,opy,opy,opy), RGBA_MAKE(0,0,0,0), 1, 0);
    vx[3]=D3DLVERTEX(D3DVECTOR(0.65*part[n].size, 0.65*part[n].size, 0), RGBA_MAKE(opy,opy,opy,opy), RGBA_MAKE(0,0,0,0), 1, 1);
    dxd3ddevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_LVERTEX,&vx[0],4,0);
  }
  dxd3ddevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCCOLOR);//SRCCOLOR
  dxd3ddevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCCOLOR);//INVSRCCOLOR
  if(gs.g-part[n].t<PARTICLELIFE*EXPLFULLSMOKEBEGIN){
    opy=(unsigned char)(((gs.g-part[n].t)/(PARTICLELIFE*EXPLFULLSMOKEBEGIN))*255.0);
  } else {
    if(gs.g-part[n].t<PARTICLELIFE*EXPLFULLSMOKEEND){
      opy=255;
    } else {
      opy=(unsigned char)((1-(gs.g-part[n].t-PARTICLELIFE*EXPLFULLSMOKEEND)/(PARTICLELIFE*(1-EXPLFULLSMOKEEND)))*255.0);
    }
  }
  dxd3ddevice->SetTexture(0, dxstexture[4]);
  vx[0]=D3DLVERTEX(D3DVECTOR(-0.65*part[n].size, -0.65*part[n].size, 0), RGBA_MAKE(opy,opy,opy,opy), RGBA_MAKE(0,0,0,0), 0, 0);
  vx[1]=D3DLVERTEX(D3DVECTOR(-0.65*part[n].size, 0.65*part[n].size, 0), RGBA_MAKE(opy,opy,opy,opy), RGBA_MAKE(0,0,0,0), 0, 1);
  vx[2]=D3DLVERTEX(D3DVECTOR(0.65*part[n].size, -0.65*part[n].size, 0), RGBA_MAKE(opy,opy,opy,opy), RGBA_MAKE(0,0,0,0), 1, 0);
  vx[3]=D3DLVERTEX(D3DVECTOR(0.65*part[n].size, 0.65*part[n].size, 0), RGBA_MAKE(opy,opy,opy,opy), RGBA_MAKE(0,0,0,0), 1, 1);
  dxd3ddevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_LVERTEX,&vx[0],4,0);
}

void render(void){
  status(200);
  // clear zbuf, backbuf
  ZeroMemory(&dxblt, sizeof(dxblt));
  dxblt.dwSize = sizeof(dxblt);
  status(201);
  if(dxbackbuffer->Blt(NULL,NULL,NULL,/*DDBLT_WAIT|*/DDBLT_COLORFILL,&dxblt)!=DD_OK) dxerror();
  status(202);
  D3DRECT rect;
  rect.x1 = 0;rect.y1 = 0;
  rect.x2 = XRES; rect.y2 = YRES;
  status(203);
//  if(dxd3dviewport->Clear(1, &rect, D3DCLEAR_ZBUFFER)!=DD_OK) dxerror();
  if(dxd3ddevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, ZBCLEARVALUE, 0)!=DD_OK) dxerror();
  // set up camera
  status(204);
  matrix=D3DMATRIX(1,0,0,0, 0,1,0,0, 0,0,1,0, -view.px,-view.py,0,VIEWDIV);
  dxd3ddevice->SetTransform(D3DTRANSFORMSTATE_VIEW, &matrix);
  matrix=D3DMATRIX(1,0,0,0, 0,cos(PI/4),sin(PI/4),0, 0,-sin(PI/4),cos(PI/4),0, 0,0,0,1);
  dxd3ddevice->MultiplyTransform(D3DTRANSFORMSTATE_VIEW, &matrix);
/*  matrix=D3DMATRIX(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,-1,0,1);
  dxd3ddevice->MultiplyTransform(D3DTRANSFORMSTATE_VIEW, &matrix);*/
//  view.px+=0.03;
  status(205);
  // draw scene
  if (dxd3ddevice->BeginScene()!=D3D_OK) dxerror();
//  dxd3ddevice->SetRenderState(D3DRENDERSTATE_TEXTUREADDRESS, D3DTADDRESS_CLAMP);
  dxd3ddevice->SetTextureStageState( 0, D3DTSS_ADDRESS, D3DTADDRESS_CLAMP);
  renderterra();
//  dxd3ddevice->SetRenderState(D3DRENDERSTATE_TEXTUREADDRESS, D3DTADDRESS_WRAP);
  dxd3ddevice->SetTextureStageState( 0, D3DTSS_ADDRESS, D3DTADDRESS_WRAP);
  status(206);
  for(int i=0;i<OMILS;i++){
    rendermil(omil[i]);
  };
  status(208);
  for(int i=0;i<NUMSHOTS;i++){
    if(shot[i].used){
      rendershot(i);
    }
  };
  status(210);
  dxd3ddevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
  dxd3ddevice->SetRenderState( D3DRENDERSTATE_ALPHATESTENABLE, TRUE ); 
  dxd3ddevice->SetRenderState( D3DRENDERSTATE_ALPHAREF, (DWORD)0x00202020);
  dxd3ddevice->SetRenderState( D3DRENDERSTATE_ALPHAFUNC, D3DCMP_GREATEREQUAL ); 
//  dxd3ddevice->SetRenderState(D3DRENDERSTATE_COLORKEYBLENDENABLE, TRUE);
  rendertrees();
  dxd3ddevice->SetRenderState( D3DRENDERSTATE_ALPHATESTENABLE, FALSE ); 
//  dxd3ddevice->SetRenderState(D3DRENDERSTATE_COLORKEYBLENDENABLE, FALSE);
//  dxd3ddevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
  // Transparente surfaces zuletzt!
  dxd3ddevice->SetRenderState(D3DRENDERSTATE_BLENDENABLE, TRUE);
  dxd3ddevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
  dxd3ddevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,FALSE);
  dxd3ddevice->SetRenderState( D3DRENDERSTATE_LIGHTING, FALSE );
//  dxd3ddevice->SetRenderState(D3DRENDERSTATE_ZFUNC,D3DCMP_ALWAYS);
  status(211);
  for(int i=0;i<PARTICLES;i++){
    if(part[i].used&&part[i].t<gs.g){
      renderparticle(i);
    }  
  };
  status(213);
  dxd3ddevice->SetRenderState( D3DRENDERSTATE_LIGHTING, TRUE );
  dxd3ddevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,TRUE);
//  dxd3ddevice->SetRenderState(D3DRENDERSTATE_ZFUNC,STDCOMPFUNC);
  dxd3ddevice->SetRenderState(D3DRENDERSTATE_BLENDENABLE, FALSE);
  dxd3ddevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
  status(214);
  if (dxd3ddevice->EndScene()!=D3D_OK) dxerror();
  // flip buffers
  status(215);
  if(dxprimary->Flip(NULL,DDFLIP_NOVSYNC)!=DD_OK)dxerror();//DDFLIP_NOVSYNC DDFLIP_WAIT
  status(216);
}

BOOL wininit(HINSTANCE hThisInstance){
  wincl.hInstance = hThisInstance;
  wincl.lpszClassName = szClassName;
  wincl.lpfnWndProc = WindowProcedure;    
  wincl.style = CS_NOCLOSE;               
  wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
  wincl.lpszMenuName = NULL;
  wincl.cbClsExtra = 0;                   
  wincl.cbWndExtra = 0;                   
  wincl.hbrBackground = (HBRUSH) GetStockObject(LTGRAY_BRUSH);
  if(!RegisterClass(&wincl)) return FALSE;
  hwnd = CreateWindowEx(
           WS_EX_TOPMOST,
           szClassName,
           "Tank 3.0",         
           WS_POPUPWINDOW,
           0,
           0,
           XRES,
           YRES, 
           HWND_DESKTOP,  
           NULL,            
           hThisInstance,    
           NULL           
           );
  ShowWindow(hwnd, SW_SHOWMAXIMIZED);
  return true;
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message){
           case WM_DESTROY:
             PostQuitMessage(0);
           break;
           case WM_KEYDOWN:
             if(wParam==VK_ESCAPE)PostQuitMessage(0);
           break;
           case WM_KEYUP:
           break;
           default:
           return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

void terranorm(void){
  D3DVECTOR v[3];
  for(int j=0;j<FIELDY;j++){
    for(int i=0;i<FIELDX;i++){
      v[0]=D3DVECTOR(i, j, -terra.hmap[begr(i,FIELDX)][begr(j,FIELDY)]/255.0*MAXHEIGHT);
      v[1]=D3DVECTOR(i, j+1, -terra.hmap[begr(i,FIELDX)][begr(j+1,FIELDY)]/255.0*MAXHEIGHT);
      v[2]=D3DVECTOR(i+1, j, -terra.hmap[begr(i+1,FIELDX)][begr(j,FIELDY)]/255.0*MAXHEIGHT);
      terra.norm[i][j]=DXNORM(v[0], v[1], v[2]);
    }
  }
}

void terrainit(void){
  float thmap[FIELDX][FIELDY];
  float min, max;
  struct timeb t2;
  ftime(&t2);
  srand(t2.millitm+1000*t2.time);
  for(int i=0;i<FIELDX;i++){
    for(int j=0;j<FIELDY;j++){
      thmap[i][j]=begr(rand(), 10000);
    }
  }
  int anz=2;
  for(int k=0;k<anz;k++){
    for(int i=0;i<FIELDX;i++){
      for(int j=0;j<FIELDY;j++){
        thmap[i][j]=(thmap[i][j]+thmap[begr(i+1,FIELDX)][j]+thmap[i][begr(j+1,FIELDY)]+thmap[begr(i+1,FIELDX)][begr(j+1,FIELDY)]
                    +thmap[begr(i-1,FIELDX)][j]+thmap[i][begr(j-1,FIELDY)]+thmap[begr(i-1,FIELDX)][begr(j-1,FIELDY)])/7;
      }
    }  
  }
  min=thmap[0][0];
  max=thmap[0][0];
  for(int i=0;i<FIELDX;i++){
    for(int j=0;j<FIELDY;j++){
      if(thmap[i][j]>max)max=thmap[i][j];
      if(thmap[i][j]<min)min=thmap[i][j];
    }
  }  
  for(int i=0;i<FIELDX;i++){
    for(int j=0;j<FIELDY;j++){
      terra.hmap[i][j]=(unsigned char)(((thmap[i][j]-min)/(max-min))*255.0);
    }
  }
  terranorm();
}

D3DLIGHT7 partlight(int i){
  D3DLIGHT7 pdxlight;
  float intensity;
  if(gs.g-part[i].t<PARTICLELIFE*0.3){
    intensity=1.0-(gs.g-part[i].t)/(PARTICLELIFE*0.3);
  } else {
    intensity=0;
  }
  memset(&pdxlight, 0, sizeof(D3DLIGHT7));
  pdxlight.dltType = D3DLIGHT_POINT;
  pdxlight.dcvDiffuse.r = intensity;//*part[i].size/EXPSIZE;
  pdxlight.dcvDiffuse.g = intensity;//*part[i].size/EXPSIZE;
  pdxlight.dcvDiffuse.b = 0;
  pdxlight.dcvDiffuse.a = 0;//*part[i].size/EXPSIZE;
  pdxlight.dvAttenuation0 = 0.0;
  pdxlight.dvAttenuation1 = 0.0;
  pdxlight.dvAttenuation2 = 1.5;
  pdxlight.dvPosition.x = part[i].x;
  pdxlight.dvPosition.y = part[i].y;
  pdxlight.dvPosition.z = part[i].z;
  pdxlight.dvDirection.x = 0;
  pdxlight.dvDirection.y = 0;
  pdxlight.dvDirection.z = 0;
  pdxlight.dvRange = 8.0f/**intensity*/*part[i].size/EXPSIZE;
  return pdxlight;
}

D3DLIGHT7 shotlight(int i){
  D3DLIGHT7 pdxlight;
  memset(&pdxlight, 0, sizeof(D3DLIGHT7));
  pdxlight.dltType = D3DLIGHT_POINT;
  pdxlight.dcvDiffuse.r = 1;
  pdxlight.dcvDiffuse.g = 1;
  pdxlight.dcvDiffuse.b = 1;
  pdxlight.dvAttenuation0 = 0.0;
  pdxlight.dvAttenuation1 = 0.0;
  pdxlight.dvAttenuation2 = 3.0;
  pdxlight.dvPosition.x = shot[i].x;
  pdxlight.dvPosition.y = shot[i].y;
  pdxlight.dvPosition.z = shot[i].z;
  pdxlight.dvDirection.x = 0;
  pdxlight.dvDirection.y = 0;
  pdxlight.dvDirection.z = 0;
  pdxlight.dvRange = 2.0f;
  return pdxlight;
}

void shoot(int n){
  if(gs.g>omil[n].lastshot+1){
    for(int i=0;i<NUMSHOTS;i++){
      if(!shot[i].used){
        shot[i].used=true;
        shot[i].x=omil[n].x+0.999*omil[n].tdx;
        shot[i].y=omil[n].y+0.999*omil[n].tdy;
        shot[i].z=omil[n].z-0.111+0.999*omil[n].tdz;
        shot[i].dx=omil[n].tdx*2*SHOTSPEED;
        shot[i].dy=omil[n].tdy*2*SHOTSPEED;
        shot[i].dz=omil[n].tdz*2*SHOTSPEED;
        shot[i].t=gs.g;
        shot[i].owner=n;
        // lighting
//        dxd3d->CreateLight(&shot[i].light, NULL);
        dxlight=shotlight(i);
//        shot[i].light->SetLight(&dxlight);
        dxd3ddevice->SetLight(SHOTLIGHTOFFSET+i, &dxlight);
        dxd3ddevice->LightEnable(SHOTLIGHTOFFSET+i, TRUE);
//        dxd3dviewport->AddLight(shot[i].light);
        break;
      }
    }
    omil[n].lastshot=gs.g;
  }
}

void input(void){
  omil[0].sp=0;
  if(GetAsyncKeyState(0x26)!=0){omil[0].sp=1*TANKSPEED;}
  if(GetAsyncKeyState(0x28)!=0){omil[0].sp=-0.6*TANKSPEED;}
  omil[0].da=0;
  if(GetAsyncKeyState(0x25)!=0){omil[0].da=0.6*TURNSPEED;}
  if(GetAsyncKeyState(0x27)!=0){omil[0].da=-0.6*TURNSPEED;}
  if(omil[0].sp<0)omil[0].da=-omil[0].da;
  if(GetAsyncKeyState(0x20)!=0)shoot(0);
/*  if((GetAsyncKeyState(0x27)&128==128||GetAsyncKeyState(0x27)&1==1)){shoot(0);}
  if((GetAsyncKeyState(0x0D)&128==128||GetAsyncKeyState(0x0D)&1==1)){shoot(0);}*/
}

float minf(float a, float b){
  if(a<b){
    return a;
  }else{
    return b;
  }
}

void ki(int n){
  if(omil[n].target==-1){
    for(int i=0;i<OMILS;i++){
      if(i!=n&&omil[i].owner!=omil[n].owner){
        omil[n].target=i;
      }
    }
  }
  if(omil[n].target!=-1){
    float posd=100, negd=100, aot;
    if(omil[n].atot-omil[n].a>0&&omil[n].atot-omil[n].a<posd)posd=omil[n].atot-omil[n].a;
    if(omil[n].a-omil[n].atot>0&&omil[n].a-omil[n].atot<negd)negd=omil[n].a-omil[n].atot;
    if(omil[n].atot-omil[n].a+2*PI>0&&omil[n].atot-omil[n].a+2*PI<posd)posd=omil[n].atot-omil[n].a+2*PI;
    if(omil[n].a-omil[n].atot+2*PI>0&&omil[n].a-omil[n].atot+2*PI<negd)negd=omil[n].a-omil[n].atot+2*PI;
    if(omil[n].atot-omil[n].a-2*PI>0&&omil[n].atot-omil[n].a-2*PI<posd)posd=omil[n].atot-omil[n].a-2*PI;
    if(omil[n].a-omil[n].atot-2*PI>0&&omil[n].a-omil[n].atot-2*PI<negd)negd=omil[n].a-omil[n].atot-2*PI;
    omil[n].da=0;
    if(posd<negd){
      omil[n].da=minf(0.6*TURNSPEED, posd*3);
      aot=posd;
    }else{
      omil[n].da=-minf(0.6*TURNSPEED, negd*3);
      aot=-negd;
    }
    float dst=sqrt((omil[omil[n].target].y-omil[n].y)*(omil[omil[n].target].y-omil[n].y)
                  +(omil[omil[n].target].x-omil[n].x)*(omil[omil[n].target].x-omil[n].x));
    omil[n].sp=0;
    if(dst>5&&aot<PI/4){
      omil[n].sp=minf(1.0*TANKSPEED, dst-5);
    }
    if(aot<PI/8)shoot(n);
  }
}

void killshot(int n){
  shot[n].used=false;
  dxd3ddevice->LightEnable(SHOTLIGHTOFFSET+n, FALSE);
  dxd3ddevice->SetLight(SHOTLIGHTOFFSET+n, NULL);
  RELEASE(shot[n].light);
}

void speed(void){
  struct timeb t2;
  ftime(&t2);
  float f2=0.001*(1000*t2.time+t2.millitm);
  gs.c=0.9*gs.c+0.1*(f2-gs.t);
  gs.g+=gs.c;
  gs.t=f2;
}

int toint(float a){
  if(a>=0){
    return (int)a;
  }else{
    return (int)(a-1);
  }  
}

void addpixel(float x,float y){
  int i=begr(toint(x), FIELDX)*TEXX/FIELDX;
  int j=begr(toint(y), FIELDY)*TEXY/FIELDY;
  dxstexture[TEXSPACEOFF+j*8+i]->GetDC(&dxtexturedc);
  unsigned int color;
  int bb=begr(rand(),32),bg=begr(rand(),bb),br=begr(rand(),bg);
  int r, g, b;
  int tx, ty;
  tx=begr(toint(x*FXRES), TXRES);
  ty=begr(toint(y*FYRES), TYRES);
  color=GetPixel(dxtexturedc, tx, ty);
  r=((color&0xFF0000)>>16)-32;
  if(r<br)r=br;
  g=((color&0xFF00)>>8)-32;
  if(g<bg)g=bg;
  b=(color&0xFF)-32;
  if(b<bb)b=bb;
  color=0x10000*r+0x100*g+b;
  SetPixel(dxtexturedc, tx, ty, color);
  dxstexture[TEXSPACEOFF+j*8+i]->ReleaseDC(dxtexturedc);
  if(tx==0||ty==0){
    int il, jo;
    if(tx==0){il=begr(i-1, TEXX);}else{il=i;}
    if(ty==0){jo=begr(j-1, TEXY);}else{jo=j;}
    dxstexture[TEXSPACEOFF+jo*8+il]->GetDC(&dxtexturedc);
    int otx, oty;
    if(tx==0){otx=TXRES-1;}else{otx=tx;}
    if(ty==0){oty=TYRES-1;}else{oty=ty;}
    SetPixel(dxtexturedc, otx, oty, color);
    dxstexture[TEXSPACEOFF+jo*8+il]->ReleaseDC(dxtexturedc);
  }
  if(tx==TXRES-1||ty==TYRES-1){
    int il, jo;
    if(tx==TXRES-1){il=begr(i+1, TEXX);}else{il=i;}
    if(ty==TYRES-1){jo=begr(j+1, TEXY);}else{jo=j;}
    dxstexture[TEXSPACEOFF+jo*8+il]->GetDC(&dxtexturedc);
    int otx, oty;
    if(tx==TXRES-1){otx=0;}else{otx=tx;}
    if(ty==TYRES-1){oty=0;}else{oty=ty;}
    SetPixel(dxtexturedc, otx, oty, color);
    dxstexture[TEXSPACEOFF+jo*8+il]->ReleaseDC(dxtexturedc);
  }
}

void addfoot(int n){
  for(int i=-8;i<8;i+=2){
    addpixel(omil[n].x+0.45*cos(omil[n].a)+0.1*i*sin(omil[n].a), omil[n].y+0.45*sin(omil[n].a)-0.1*i*cos(omil[n].a));
    addpixel(omil[n].x-0.45*cos(omil[n].a)+0.1*i*sin(omil[n].a), omil[n].y-0.45*sin(omil[n].a)-0.1*i*cos(omil[n].a));
  }
}

void einschlag(float x, float y){
  for(int i=-4;i<=4;i++){
    for(int j=-4;j<=4;j++){
      if(sqrt(i*i+j*j)<=2){
        addpixel(x+(float)i/FXRES, y+(float)j/FYRES);
      }  
    }
  }
  for(int i=-1;i<=1;i++){
    for(int j=-1;j<=1;j++){
      if(sqrt(i*i+j*j)<=1){
        addpixel(x+(float)i/FXRES, y+(float)j/FYRES);
      }  
    }
  }
  addpixel(x, y);
/*  if(terra.hmap[begr(toint(x), FIELDX)][begr(toint(y), FIELDY)]>0){
    terra.hmap[begr(toint(x), FIELDX)][begr(toint(y), FIELDY)]--;
  }*/
}

void killparticle(int n){
  part[n].used=false;
  if(part[n].lit){
    dxd3ddevice->LightEnable(PARTLIGHTOFFSET+n, FALSE);
    dxd3ddevice->SetLight(PARTLIGHTOFFSET+n, NULL);
//    dxd3dviewport->DeleteLight(part[n].light);
    RELEASE(part[n].light);
  }
}

void newparticle(unsigned char typ, float x, float y, float z, float size, bool lit) {
  for(int i=0;i<PARTICLES;i++){
    if(!part[i].used){
      part[i].used=true;
      part[i].x=x;
      part[i].y=y;
      part[i].z=z;
      part[i].size=size;
      part[i].t=gs.g+begrf(rand()/1000.0, 0.18);
      if(typ==1){
        part[i].dz=-0.005-begrf(rand()/10000.0, 0.01);
      }
      part[i].lit=lit;
      if(lit){
        // lighting
//        dxd3d->CreateLight(&part[i].light, NULL);
        dxlight=partlight(i);
//        part[i].light->SetLight(&dxlight);
        dxd3ddevice->SetLight(PARTLIGHTOFFSET+i, &dxlight);
        dxd3ddevice->LightEnable(PARTLIGHTOFFSET+i, TRUE);
      }
      goto out;
    }
  }
  if(typ==2){
    int i=begr(rand(), PARTICLES);
    part[i].used=true;
    part[i].x=x;
    part[i].y=y;
    part[i].z=z;
    part[i].t=gs.g;
  }
  out:;
}

void explode(int n){
  newparticle(1, omil[n].x, omil[n].y, omil[n].z-0.5, EXPSIZE, true);
  for(int i=0;i<10;i++){
    newparticle(1, omil[n].x+begrf(rand()/1000.0, 1), omil[n].y+begrf(rand()/1000.0, 1), omil[n].z-0.5+begrf(rand()/1000.0, 1), EXPSIZE*0.8-begrf(rand()/1000.0, EXPSIZE*0.6), false);
  }
  do{omil[n].target=begr(rand(), OMILS);}while(omil[n].target==n);
  omil[n].lastfoot=gs.g;
  omil[n].lastshot=gs.g;
  omil[n].x=begr(rand(),20)-10;
  omil[n].y=begr(rand(),20)-10;
  omil[n].a=begrf(rand(),2*PI);
  omil[n].tb=PI/8;
  omil[n].hit=80;
}

float absf(float x){
  if(x>=0){
    return x;
  } else {
    return -x;
  }  
}

void checkhit(int n, int s){
  if(absf(omil[n].x-shot[s].x)<0.6&&absf(omil[n].y-shot[s].y)<0.6&&absf(omil[n].z-shot[s].z)<0.35&&shot[s].owner!=n){
    omil[n].hit-=10;
    killshot(s);
    newparticle(1, omil[n].x, omil[n].y, omil[n].z-0.5, 1.3-begrf(rand()/1000.0, 1.0), true);
    if(omil[n].hit<0){
      explode(n);
    }
  }
}

void calcomil(int i){
    // fahren
    omil[i].x-=sin(omil[i].a)*omil[i].sp*gs.c;
    omil[i].y+=cos(omil[i].a)*omil[i].sp*gs.c;
    // drehen
    omil[i].a+=omil[i].da*gs.c;
    // z-position
    omil[i].z=height(omil[i].x, omil[i].y)-0.35;
    // a, b
    omil[i].b=atan((height(omil[i].x+0.88*sin(omil[i].a), omil[i].y-0.88*cos(omil[i].a))-height(omil[i].x-0.88*sin(omil[i].a), omil[i].y+0.88*cos(omil[i].a)))/1.76);
    omil[i].c=atan((height(omil[i].x-0.66*cos(omil[i].a), omil[i].y-0.66*sin(omil[i].a))-height(omil[i].x+0.66*cos(omil[i].a), omil[i].y+0.66*sin(omil[i].a)))/1.32);
    // a, ta auf 2*PI begrenzen
    omil[i].a=begrf(omil[i].a, 2*PI);
    omil[i].ta=begrf(omil[i].ta, 2*PI);
    // Zielberechnungen
    status(103);
    if(omil[i].target!=-1){
      // virtuelle Position des Ziels
      float tgtx, tgty, tgtz, nx, ny, nz;
      tgtx=omil[omil[i].target].x-omil[i].x;
      tgty=omil[omil[i].target].y-omil[i].y;
      tgtz=omil[omil[i].target].z-omil[i].z;
      nx=tgtx*cos(-omil[i].a)-tgty*sin(-omil[i].a);
      ny=tgty*cos(-omil[i].a)+tgtx*sin(-omil[i].a);
      tgtx=nx;
      tgty=ny;
      nx=tgtz*sin(-omil[i].c)+tgtx*cos(-omil[i].c);
      nz=tgtz*cos(-omil[i].c)-tgtx*sin(-omil[i].c);
      tgtx=nx;
      tgtz=nz;
      ny=tgtz*sin(-omil[i].b)+tgty*cos(-omil[i].b);
      nz=tgtz*cos(-omil[i].b)-tgty*sin(-omil[i].b);
      tgty=ny;
      tgtz=nz;
      tgtx+=omil[i].x;
      tgty+=omil[i].y;
      tgtz+=omil[i].z;
//      if(i==0)newparticle(2, tgtx, tgty, tgtz);
      // Entfernung des Ziels
      float dst=sqrt((omil[i].x-tgtx)*(omil[i].x-tgtx)
                    +(omil[i].y-tgty)*(omil[i].y-tgty));
      // Richtung des Ziels (Panzer)
      if(omil[omil[i].target].y-omil[i].y!=0){
        omil[i].atot=atan((omil[i].x-omil[omil[i].target].x)/(omil[omil[i].target].y-omil[i].y));
      }else{
        omil[i].atot=atan((omil[i].x-omil[omil[i].target].x)/(1E-20));
      }
      if(omil[omil[i].target].y<omil[i].y)omil[i].atot=omil[i].atot+PI;
      // Richtung des Ziels (Turm)
      if(tgty-omil[i].y!=0){
        omil[i].tatot=atan((omil[i].x-tgtx)/(tgty-omil[i].y));
      }else{
        omil[i].tatot=atan((omil[i].x-tgtx)/(1E-20));
      }
      if(tgty<omil[i].y)omil[i].tatot=omil[i].tatot+PI;
      // Turm b-Richtung
      omil[i].tb=atan((omil[i].z-tgtz)/dst);
      // begrenzen
      if(omil[i].tb>PI/4)omil[i].tb=PI/4;
      if(omil[i].tb<-0.17)omil[i].tb=-0.17;
      // Turm a-Richtung
      float posd=100, negd=100;
      if(omil[i].tatot-(0*omil[i].a+omil[i].ta)>=0&&omil[i].tatot-(0*omil[i].a+omil[i].ta)<posd)posd=omil[i].tatot-(0*omil[i].a+omil[i].ta);
      if((0*omil[i].a+omil[i].ta)-omil[i].tatot>0&&(0*omil[i].a+omil[i].ta)-omil[i].tatot<negd)negd=(0*omil[i].a+omil[i].ta)-omil[i].tatot;
      if(omil[i].tatot-(0*omil[i].a+omil[i].ta)+2*PI>=0&&omil[i].tatot-(0*omil[i].a+omil[i].ta)+2*PI<posd)posd=omil[i].tatot-(0*omil[i].a+omil[i].ta)+2*PI;
      if((0*omil[i].a+omil[i].ta)-omil[i].tatot+2*PI>0&&(0*omil[i].a+omil[i].ta)-omil[i].tatot+2*PI<negd)negd=(0*omil[i].a+omil[i].ta)-omil[i].tatot+2*PI;
      if(omil[i].tatot-(0*omil[i].a+omil[i].ta)-2*PI>=0&&omil[i].tatot-(0*omil[i].a+omil[i].ta)-2*PI<posd)posd=omil[i].tatot-(0*omil[i].a+omil[i].ta)-2*PI;
      if((0*omil[i].a+omil[i].ta)-omil[i].tatot-2*PI>0&&(0*omil[i].a+omil[i].ta)-omil[i].tatot-2*PI<negd)negd=(0*omil[i].a+omil[i].ta)-omil[i].tatot-2*PI;
      if(omil[i].tatot-(0*omil[i].a+omil[i].ta)+4*PI>=0&&omil[i].tatot-(0*omil[i].a+omil[i].ta)+4*PI<posd)posd=omil[i].tatot-(0*omil[i].a+omil[i].ta)+4*PI;
      if((0*omil[i].a+omil[i].ta)-omil[i].tatot+4*PI>0&&(0*omil[i].a+omil[i].ta)-omil[i].tatot+4*PI<negd)negd=(0*omil[i].a+omil[i].ta)-omil[i].tatot+4*PI;
      if(omil[i].tatot-(0*omil[i].a+omil[i].ta)-4*PI>=0&&omil[i].tatot-(0*omil[i].a+omil[i].ta)-4*PI<posd)posd=omil[i].tatot-(0*omil[i].a+omil[i].ta)-4*PI;
      if((0*omil[i].a+omil[i].ta)-omil[i].tatot-4*PI>0&&(0*omil[i].a+omil[i].ta)-omil[i].tatot-4*PI<negd)negd=(0*omil[i].a+omil[i].ta)-omil[i].tatot-4*PI;
      if(posd<negd){
//        omil[i].ta+=posd;
        omil[i].ta+=minf(gs.c*minf(1.0*TURRETTURNSPEED, posd*9), posd);
      }else{
//        omil[i].ta-=negd;
        omil[i].ta-=minf(gs.c*minf(1.0*TURRETTURNSPEED, negd*9), negd);
      }
      // calculate heading of turret
      omil[i].tdx=0;
      omil[i].tdy=1;
      omil[i].tdz=0;
      ny=omil[i].tdy*cos(omil[i].tb)+omil[i].tdz*sin(omil[i].tb);
      nz=omil[i].tdz*cos(omil[i].tb)-omil[i].tdy*sin(omil[i].tb);
      omil[i].tdy=ny;
      omil[i].tdz=nz;
      nx=omil[i].tdx*cos(omil[i].ta)-omil[i].tdy*sin(omil[i].ta);
      ny=omil[i].tdy*cos(omil[i].ta)+omil[i].tdx*sin(omil[i].ta);
      omil[i].tdx=nx;
      omil[i].tdy=ny;
      ny=omil[i].tdy*cos(omil[i].b)+omil[i].tdz*sin(omil[i].b);
      nz=omil[i].tdz*cos(omil[i].b)-omil[i].tdy*sin(omil[i].b);
      omil[i].tdy=ny;
      omil[i].tdz=nz;
      nx=omil[i].tdx*cos(omil[i].c)+omil[i].tdz*sin(omil[i].c);
      nz=omil[i].tdz*cos(omil[i].c)-omil[i].tdx*sin(omil[i].c);
      omil[i].tdx=nx;
      omil[i].tdz=nz;
      nx=omil[i].tdx*cos(omil[i].a)-omil[i].tdy*sin(omil[i].a);
      ny=omil[i].tdy*cos(omil[i].a)+omil[i].tdx*sin(omil[i].a);
      omil[i].tdx=nx;
      omil[i].tdy=ny;
    }
    if(omil[i].owner!=0){
      ki(i);
    }
    if((omil[i].sp!=0||omil[i].da!=0)&&gs.g>omil[i].lastfoot+0.6/TANKSPEED){
      omil[i].lastfoot=gs.g;
      addfoot(i);
    }
}

void calccol(int i, int j){
  float xdst=omil[i].x-omil[j].x;
  float ydst=omil[i].y-omil[j].y;
  float dst=sqrt(xdst*xdst+ydst*ydst);
  if(dst<1.5){
    omil[i].x+=xdst*0.2;
    omil[i].y+=ydst*0.2;
    omil[j].x-=xdst*0.2;
    omil[j].y-=ydst*0.2;
  }
}

void calctreecol(int i, int j){
  float xdst=omil[i].x-tree[j].x;
  float ydst=omil[i].y-tree[j].y;
  float dst=sqrt(xdst*xdst+ydst*ydst);
  if(dst<0.75){
    omil[i].x+=xdst*0.2;
    omil[i].y+=ydst*0.2;
  }
}

void calculate(void){
  status(100);
  speed();
  status(101);
  for(int i=0;i<OMILS;i++){
    if(omil[i].used){
      calcomil(i);
      for(int j=0;j<OMILS;j++){
        if(omil[j].used&&i!=j){
          calccol(i, j);
        }
      }
      for(int j=0;j<TREES;j++){
        if(tree[j].used){
          calctreecol(i, j);
        }
      }
    }
  }
  status(106);
  tparticle swap;
  for(int i=0;i<PARTICLES;i++){
    if(part[i].used){
      if(gs.g-part[i].t>=PARTICLELIFE){
        status(107);
        killparticle(i);
        status(108);
      } else {
        part[i].z=part[i].z+part[i].dz;
        if(i+1<PARTICLES&&part[i+1].used){
          if(part[i].z<part[i+1].z){
            swap=part[i];
            part[i]=part[i+1];
            part[i+1]=swap;
          }
        }
        if(part[i].lit){
          // lighting
          dxlight=partlight(i);
          dxd3ddevice->SetLight(PARTLIGHTOFFSET+i, &dxlight);
//          dxd3ddevice->LightEnable(PARTLIGHTOFFSET+i, TRUE);
        }
      }
    }
  }
  status(114);
  for(int i=0;i<NUMSHOTS;i++){
    if(shot[i].used){
      shot[i].x+=shot[i].dx*gs.c;
      shot[i].y+=shot[i].dy*gs.c;
      shot[i].z+=shot[i].dz*gs.c;
      shot[i].dz+=SHOTGRAVITY*gs.c;
      if(gs.g-shot[i].t>10){
        killshot(i);
        status(116);
        continue;
      }
      if(height(shot[i].x,shot[i].y)<shot[i].z){
        einschlag(shot[i].x, shot[i].y);
        killshot(i);
        continue;
      }  
      // lighting
      dxlight=shotlight(i);
      dxd3ddevice->SetLight(SHOTLIGHTOFFSET+i, &dxlight);
//      dxd3ddevice->LightEnable(SHOTLIGHTOFFSET+i, TRUE);
      for(int j=0;j<OMILS;j++){
        if(omil[j].used)checkhit(j, i);// der shot könnte danach nicht mehr existieren
      }
    }
  }
  status(126);
}

void gameinit(void){
  // time
  gs.c=1/25;
  struct timeb t2;
  ftime(&t2);
  float f2=0.001*(1000*t2.time+t2.millitm);
  gs.t=f2;
  gs.g=0;
  for(int i=0;i<OMILS;i++){
    omil[i].owner=i==0?0:1;
    /*prov.*/
    do{omil[i].target=begr(rand(), OMILS);}while(omil[i].target==i);
    omil[i].lastfoot=gs.g;
    omil[i].lastshot=gs.g;
    omil[i].x=begr(rand(),20)-10;
    omil[i].y=begr(rand(),20)-10;
    omil[i].a=begrf(rand()*0.1,2*PI);
    omil[i].tb=PI/8;
    omil[i].hit=80;
    omil[i].used=true;
  }
}

void treeinit(void){
  for(int i=0;i<TREES;i++){
    tree[i].x=begr(rand(), 10)-5;
    tree[i].y=begr(rand(), 10)-5;
    tree[i].z=-terra.hmap[begr((int)tree[i].x,FIELDX)][begr((int)tree[i].y,FIELDY)]/255.0*MAXHEIGHT;
    if(rand()<1000){
      tree[i].typ=1;
    }else{
      tree[i].typ=0;
    }
    tree[i].used=true;
  }
}

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nFunsterStil){
  status(1);
  if(!wininit(hThisInstance))winerror();
  status(2);
  dxinit();
  status(3);
  terrainit();
  treeinit();
  status(4);
  gameinit();
  status(5);
  // render
  while ( !bQuit ) {
    if ( PeekMessage( &messages, NULL, 0, 0, PM_REMOVE ) ) {
      status(6);
      if ( messages.message == WM_QUIT ) {
        status(7);
        bQuit=TRUE;
      } else {
        status(8);
        TranslateMessage( &messages );
        DispatchMessage( &messages );
      }
    } else {
      status(9);
      input();
      status(10);
      calculate();
      status(11);
      render();
      status(12);
      t+=0.005;
    }
  }  
  status(13);
  dxcleanup();
  status(14);
  DestroyWindow(hwnd);
  status(15);
  return 0;
}


#include<graphics.h>
#include<conio.h>
#include<stdio.h>
#include<process.h>
#include<fstream.h>
#include<iomanip.h>
#include<stdlib.h>
#include<dos.h>
char ch;
class student
{
 int rollno;
 char name[50];
 int p_marks,c_marks,m_marks,e_marks,cs_marks;
 float per;
 char grade;
 int std;
 void calculate()
{
  per=(p_marks+c_marks+m_marks+e_marks+cs_marks)/5.0;
 if(per>=80)
   grade='A';
 else if(per>=60 && per<80)
  grade='B';
 else if(per>=33 && per<60)
 grade='C';
else
 grade='F';
}
 public:
	void getdata()
	{
	 textcolor(rand());
     cout<<"\n";	 cprintf("Enter The roll number of student ");
	 cin>>rollno;
     cout<<"\n\n";	 cprintf("Enter The Name of student ");
	 gets(name);
     cout<<"\n";	 cprintf("Enter The marks in physics out of 100 : ");
	 cin>>p_marks;
     cout<<"\n";	 cprintf("Enter The marks in chemistry out of 100 : ");
	 cin>>c_marks;
     cout<<"\n";	 cprintf("Enter The marks in maths out of 100 : ");
	 cin>>m_marks;
     cout<<"\n";	 cprintf("Enter The marks in english out of 100 : ");
	 cin>>e_marks;
     cout<<"\n";	 cprintf("Enter The marks in computer science out of 100 : ");
	 cin>>cs_marks;
	calculate();
	 }

	void showdata()
	{
	 textcolor(rand()+BLINK);
	cout<<"\n"; cprintf("Roll number of student : ");cout<<rollno;
	cout<<"\n"; cprintf("Name of student : ");cout<<name;
	cout<<"\n"; cprintf("Marks in Physics : ");cout<<p_marks;
	cout<<"\n"; cprintf("Marks in Chemistry : ");cout<<c_marks;
	cout<<"\n"; cprintf("Marks in Maths : ");cout<<m_marks;
	cout<<"\n"; cprintf("Marks in English : ");cout<<e_marks;
	cout<<"\n"; cprintf("Marks in Computer Science :");cout<<cs_marks;
	cout<<"\n"; cprintf("Percentage of student is  :");cout<<setprecision(2)<<per;
	cout<<"\n"; cprintf("Grade of student is :");cout<<grade;
	 }

	 void show_tabular()
	 {
	 cout<<rollno<<setw(12)<<name<<setw(10)<<p_marks<<" "<<c_marks<<" "<<m_marks<<" "<<e_marks<<" "<<cs_marks<<" "<<setprecision(3)<<per<<"   "<<grade<<endl;
	 }

  int  retrollno()
  {return rollno;}

};
 fstream fp;
 student st;
void write_student()
   {
    textcolor(rand() );
    fp.open("student.txt",ios::out|ios::app);
    st.getdata();
    fp.write((char*)&st,sizeof(student));
    fp.close();
cout<<"\n\n";    cprintf("student record Has Been Created ");
    getch();
   }
void display_all()
{
    clrscr();
    textcolor(rand() );
   cout<<"\n\n\n\t\t"; cprintf("DISPLAY ALL RECORD !!!");cout<<"\n\n";
    fp.open("student.txt",ios::in);
    while(fp.read((char*)&st,sizeof(student)))
	{
	 st.showdata();
       cout<<"\n\n";cprintf("====================================");cout<<"\n";
	 getch();
	 }
    fp.close();
    getch();
}
void display_sp(int n)
{
    textcolor(rand() );
    int flag=0;
    fp.open("student.txt",ios::in);
    while(fp.read((char*)&st,sizeof(student)))
	{
	 if(st.retrollno()==n)
		{
		 clrscr();
		 st.showdata();
		 flag=1;
		}
	}
    fp.close();
if(flag==0)
cout<<"\n\n"; cprintf("record not exist");
    getch();
}
void modify_student()
{
    textcolor(rand() );
    int no,found=0;
    clrscr();
   cout<<"\n\n\t"; cprintf("To Modify ");
   cout<<"\n\n\t"; cprintf("Please Enter The roll number of student");
    cin>>no;
    fp.open("student.txt",ios::in|ios::out);
    while(fp.read((char*)&st,sizeof(student)) && found==0)
	   {
	    if(st.retrollno()==no)
		   {
		    st.showdata();
	      cout<<"\n";	    cprintf("Please Enter The New Details of student");cout<<endl;
		    st.getdata();
		    int pos=-1*sizeof(st);
		    fp.seekp(pos,ios::cur);
		    fp.write((char*)&st,sizeof(student));
	      cout<<"\n\n\t";	    cprintf(" Record Updated");
		    found=1;
		   }
	     }
    fp.close();
    if(found==0)
  cout<<"\n\n";  cprintf(" Record Not Found ");
    getch();
}
void delete_student()
   {
    textcolor(rand() );
    int no;
    clrscr();
cout<<"\n\n\n\t";    cprintf("Delete Record");
cout<<"\n\n";    cprintf("Please Enter The roll number of student You Want To Delete");
    cin>>no;
    fp.open("student.txt",ios::in|ios::out);
    fstream fp2;
    fp2.open("Temp.dat",ios::out);
    fp.seekg(0,ios::beg);
    while(fp.read((char*)&st,sizeof(student)))
	{
	 if(st.retrollno()!=no)
		{
		 fp2.write((char*)&st,sizeof(student));
		 }
	 }
    fp2.close();
    fp.close();
    remove("student.txt");
    rename("Temp.txt","student.txt");
 cout<<"\n\n\t";   cprintf("Record Deleted ..");
    getch();
    }
    void class_result()
    {
     textcolor(rand() );
     clrscr();
     fp.open("student.txt",ios::in);
     if(!fp)
     {
       cprintf("ERROR!!! FILE COULD NOT BE OPEN");cout<<"\n\n\n";cprintf(" Go To Entry Menu to create File");
    cout<<"\n\n\n";   cprintf(" Program is closing ....");
       getch();
       exit(0);
     }

cout<<"\n\n\t\t";     cprintf("ALL STUDENTS RESULT ");cout<<"\n\n";
	  cprintf("====================================================");cout<<"\n";
	  cprintf("Roll No. Name          P  C  M  E  CS  %age Grade"); cout<<"\n";
	  cprintf("====================================================");cout<<"\n";

      while(fp.read((char*)&st,sizeof(student)))
	 {
	   st.show_tabular();
	}
     fp.close();
     getch();
}
   void result()
   {
    textcolor(rand() );
    int ans,rno;
    char ch;
    clrscr();
   cout<<"\n\n\n"; cprintf("RESULT MENU");
   cout<<"\n\n\n"; cprintf("1. Class Result");cout<<"\n\n";cprintf("2. Student Report Card");cout<<"\n\n";cprintf("3.Back to Main Menu");
   cout<<"\n\n\n"; cprintf("Enter Choice (1/2)? ");
    cin>>ans;
    switch(ans)
    {
     case 1 : class_result();break;
     case 2 : {
		do{
		clrscr();
		char ans;
	       cout<<"\n\n";cprintf("Enter Roll Number Of Student : ");
		cin>>rno;
		display_sp(rno);
	       cout<<"\n\n";cprintf("Do you want to See More Result (y/n)?");
		cin>>ans;
		}while(ans=='y'||ans=='Y');

		break;
	       }
     case 3: break;
     default:  cout<<"\a";
    }
 }
void intro()
{
 clrscr();
  textcolor(rand() );
cout<<"\n\n\n\n\n\n\t\t\t"; cprintf("STUDENT");
cout<<"\n\t\t\t"; cprintf("      REPORT CARD");
cout<<"\n\t\t\t\t\t"; cprintf("PROJECT");
cout<<"\n\n\n\t\t"; cprintf("MADE BY : DEVANSH CHAUHAN AND ABHISHEK SAXENA");
cout<<"\n\n\n\t\t"; cprintf("SCHOOL : N.C JINDAL PUBLIC SCHOOL");
cout<<"\n\n\n\t\t"; cprintf("ADDRESS : PUNJABI BAGH,NEW DELHI-110026");
 getch();

}
void entry_menu()
{
  clrscr();
  textcolor(rand() );
  char ch2;
 cout<<"\n\n\n\t";  cprintf("\n\n\n\tENTRY MENU");
 cout<<"\n\n\t";  cprintf("1.CREATE STUDENT RECORD");
 cout<<"\n\n\t";  cprintf("2.DISPLAY ALL STUDENTS RECORDS");
 cout<<"\n\n\t";  cprintf("3.SEARCH STUDENT RECORD ");
 cout<<"\n\n\t";  cprintf("4.MODIFY STUDENT RECORD");
 cout<<"\n\n\t";  cprintf("5.DELETE STUDENT RECORD");
 cout<<"\n\n\t";  cprintf("6.BACK TO MAIN MENU");
 cout<<"\n\n\t";  cprintf("Please Enter Your Choice (1-6) ");
  ch2=getche();
  switch(ch2)
  {
    case '1': clrscr();
	      write_student();
	      break;
    case '2': display_all();break;
    case '3':
	       int num;
	       clrscr();
	      cout<<"\n\n\t"; cprintf("Please Enter The roll number ");
	       cin>>num;
	       display_sp(num);
	       break;
      case '4': modify_student();break;
      case '5': delete_student();break;
      case '6': break;
      default:cout<<"\a";entry_menu();
   }
}
void loading()
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
  setcolor(2);
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
 closegraph();
}
void apass()
{
clrscr();
textcolor(rand() );
int i,flag=0;
char ap[9]="mrchauhan";
char user[9];
do{
cprintf("Enter the password : ");
for(i=0;i<=8;i++)
{
user[i]=getch();
cprintf("*");
}
for(i=0;i<=8;i++)
{if(user[i]==ap[i])
flag++;
}
if(flag==9)
{
clrscr();
textcolor(rand() );
cout<<"\n\n\n\n\n\n\n\t\t\t";
cprintf("    PASSWORD AUTHENTICATED");cout<<"\n\n\t\t\t";cprintf("     welcome sir");
delay(100);
for(i=0;i<=10;i++)
{
cprintf(".");
delay(200);
}
ch='y';
break;
}
else
{
flag=0;
cout<<"\n";
cprintf("Wrong password");
cout<<"\n";
cprintf("Do you want to continue (y/n) : ");
cin>>ch;
}
}while(ch=='y'||ch=='Y');
}
void main()
{
  char ch1;
  loading();
  intro();
  apass();
if(ch=='n'||ch=='N')
  exit(0);
  do
    {
	  clrscr();
	  textcolor(GREEN );
	 cout<<"\n\n\n\t"; cprintf("MAIN MENU");
	 cout<<"\n\n\t"; cprintf("01. RESULT MENU");
	 cout<<"\n\n\t"; cprintf("02. ENTRY/EDIT MENU");
	 cout<<"\n\n\t"; cprintf("03. EXIT");
	 cout<<"\n\n\t"; cprintf("Please Select Your Option (1-3) ");
	  ch1=getche();
	  switch(ch1)
	  {
		 case '1': clrscr();
			   result();
			   break;
		  case '2': entry_menu();
			    break;
		  case '3':exit(0);
		  default :cout<<"\a";
	}
    }while(ch!='3');
}
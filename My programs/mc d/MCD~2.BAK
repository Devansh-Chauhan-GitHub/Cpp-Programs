 #include <stdio.h>
#include <iostream.h>
#include <ctype.h>
#include <stdlib.h>
#include <conio.h>
#include <iomanip.h>
#include <string.h>
int main()
{
char *l = " ------------------------------------------------------------------------------\n";
int order = 1;
char name[30],choice;
int num1=0, num2=0, num3=0, num4=0, num5=0,num_customers;
int sentinel=0,rawr=1,ans=1;
double UnitPrice1= 204.00, UnitPrice2= 75.00,UnitPrice3= 39.00, UnitPrice4= 149.00, UnitPrice5= 25.00;
double AmountofSale1=0, AmountofSale2=0, AmountofSale3=0, AmountofSale4=0, AmountofSale5=0,total=0,pepay=999999999,tsenj=0;
system("COLOR A");
while (ans != 0)
 {

 sentinel++;
 cout<<"\t       __      __        __\n";
 cout<<"\t      /  \\    /  \\ ____ |  | ____  ____    _____   ____ \n";
 cout<<"\t      \\   \\/\\/   // __ \\|  |/ ___\\/  _ \\  /     \\ / __ \\\n";
 cout<<"\t       \\        /|  ___/|  |  \\__(  <_> )|  Y Y  \|  ___/\n";
 cout<<"\t        \\__/\\__/  \\____\\|__|\\_____>____/ |__|_|__|\\____\\\n";
 cout<<"\t   _____         ________                        __      _\n";
 cout<<"\t  /     \\   ____ \\       \\   ____   ____ _____  |  |  __| | ______\n";
 cout<<"\t /  \\ /  \\_/ ___\\ |   |   \\ /  _ \\ /    \\\\__  \\ |  | / __ |/  ___/\n";
 cout<<"\t/    Y    \\  \\___ |   |    (  <_> )   |  \\/ __ \\|  |/ /_/ |\\___ \\\n";
 cout<<"\t\\____|__  /\\_____>_________/\\____/|___|___|_____|__|\\____/\\_____/\n\n";
 cout<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
 cout<<" xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx ";
 cout<<"  ****************************************************************************  ";
 cout<<"\n\tEnter your name to continue: ";
 gets(name);
  do
  {
   system("cls");
   cout<<"\n CASHIER : ";
   puts(name);
   cout<<" Do you want to proceed to the main menu? "<<name;
   cout<<" \n Press [Y] for Yes, and [N] for No.\n\n\tEnter Choice : ";
   cin >> choice;
  } while ((choice !='Y')&&(choice !='N')&&(choice !='y')&&(choice !='n'));
  system("cls");
    switch (choice)
   case 1=='Y'|'y':
   cout<<"    _____\n";
   cout<<"   /     \\   ____   ____    __ __\n";
   cout<<"  /  \\ /  \\_/ __ \\ /     \\ |  |  |\n";
   cout<<" /    Y    \\  ___/|   |   \\|  |  |\n";
   cout<<" \\____|____/\\____>____|____|_____|\n\n";
  cout <<" _____(1) Super Burger      Php 204.00_____\n"
       <<" _____(2) Amazing Spaghetti         Php  75.00_____\n"
    <<" _____(3) French Fries of Heaven    Php  39.00_____\n"
    <<" _____(4) Spicy Chicken Wings     Php 149.00_____\n"
    <<" _____(5) Softdrinks      Php  25.00_____\n";
  while (rawr != 0)
 {
   do
   {
    cout<<" \n From the list of food, what would you like : ";
    cin>>order;
   } while ((order > 5)&&(order < 1));
   switch(order)
   {
    case 1:
		 cout<<" How many Super Burger would you like to order : ";
		 cin>>num1;
		 AmountofSale1 = UnitPrice1 * num1;
		 break;
    case 2:
		 cout<<" How many Amazing Spaghetti would you like to order : ";
     cin>>num2;
		 AmountofSale2= UnitPrice2 * num2;
		 break;
		case 3:
		 cout<<" How many French Fries of Heaven would you like to order : ";
		 cin>>num3;
		 AmountofSale3= UnitPrice3 * num3;
		 break;
		case 4:
		 cout<<" How many Spicy Chicken Wings would you like to order : ";
		 cin>>num4;
		 AmountofSale4= UnitPrice4 * num4;
		 break;
		case 5:
		 cout<<" How many Softdrinks would you like to order : ";
		 cin>>num5;
		 AmountofSale5= UnitPrice5 * num5;
		 break;
		default: cout<<" Please choose a valid item from our list\n";
   }

  cout.precision(2);
  cout<<" You have ordered:\n\n";
  cout<<l;
  cout<<" ITEM\t\t\t\tQUANTITY\tUNIT PRICE\tAMOUNT OF SALE\n";
  cout<<l;
  cout<<" Super Burger:\t\t\t"<<num1<<"\t\tPhp  "<<UnitPrice1<<"\tPhp "<<AmountofSale1<<"\n";
  cout<<" Amazing Spaghetti:\t\t"<<num2<<"\t\tPhp   "<<UnitPrice2<<"\tPhp "<<AmountofSale2<<"\n";
  cout<<" French Fries of Heaven:\t"<<num3<<"\t\tPhp   "<<UnitPrice3<<"\tPhp "<<AmountofSale3<<"\n";
  cout<<" Spicy Chicken Wings:\t\t"<<num4<<"\t\tPhp  "<<UnitPrice4<<"\tPhp "<<AmountofSale4<<"\n";
  cout<<" Softdrinks:\t\t\t"<<num5<<"\t\tPhp   "<<UnitPrice5<<"\tPhp "<<AmountofSale5<<"\n";
  total=0;
  total=total+AmountofSale1+AmountofSale2+AmountofSale3+AmountofSale4+AmountofSale5;
  cout<<l;
  cout<<"\t\t\t\t\t\tThat would be:  Php "<<total<<"\n";
  cout<<l;
  do
  {
   cout<<" Order Again [0] no [1] yes : ";
   cin >> rawr;
  } while ((rawr != 0)&&(rawr != 1));
    }
 do
 {
  system("cls");
  if (pepay < total)
  {
   cout<<"\t\t--- Insufficient Money... PAY OR DIE ??? ---\n";
  }
   cout<<"\n\tCashier : "<<name<<"\t\t\t\t\tTransaction # "<<sentinel<<"\n";
   cout<<l;
   cout<<" You have ordered:\n\n";
   cout<<" ITEM\t\t\t\tQUANTITY\tUNIT PRICE\tAMOUNT OF SALE\n";
  if (num1 != 0)
  {
   cout<<" Super Burger:\t\t\t"<<num1<<"\t\t"<<UnitPrice1<<"\t\t"<<AmountofSale1<<"\n";
  }
    if (num2 != 0)
  {
   cout<<" Amazing Spaghetti:\t\t"<<num2<<"\t\t"<<UnitPrice2<<"\t\t"<<AmountofSale2<<"\n";
  }
  if (num3 != 0)
  {
   cout<<" French Fries of Heaven:\t"<<num3<<"\t\t"<<UnitPrice3<<"\t\t"<<AmountofSale3<<"\n";
  }
  if (num4 != 0)
  {
   cout<<" Spicy Chicken Wings:\t\t"<<num4<<"\t\t"<<UnitPrice4<<"\t\t"<<AmountofSale4<<"\n";
  }
  if (num5 != 0)
  {
   cout<<" Softdrinks:\t\t\t"<<num5<<"\t\t"<<UnitPrice5<<"\t\t"<<AmountofSale5<<"\n";
  }
  cout<<l;
  cout<<"\tTotal : "<<total;
  cout<<"\n\tEnter Received Cash : ";
  cin >> pepay;
 } while (pepay < total);
   tsenj = pepay - total;
 cout<<"\tYour change is : "<<tsenj<<"\n\n\n\n\t\t";
  do
  {
   cout<<" New transaction [0] no [1] yes : ";
   cin >> ans;
  } while ((ans != 0)&&(ans != 1));
 }
 system("cls\n\n\n\t\t");
 system("PAUSE");
return 0;
}
#include<stdio.h>
#include<fstream.h>

int main()
{
  FILE *fp = fopen ("prova.txt","w");
  float num1=100;
  float num2=100;
  fprintf(fp,"%f\t%f\n",num1, num2);
  num1 += 10;
  fprintf(fp,"%f\t",num1);
  num2 += 20;
  fprintf(fp,"%f\n",num2);
  num1 += 10;
  fprintf(fp,"%f\t",num1);
  num2 += 20;
  fprintf(fp,"%f\n",num2);
  fclose(fp);
  FILE *fp1 = fopen ("prova.txt","r");
  float num3,num4;
  fscanf(fp1,"%f\t%f\n",&num3,&num4);
  cout << num3 << '\t' << num4 << endl;
  fscanf(fp1,"%f\t%f\n",&num3,&num4);
  cout << num3 << '\t' << num4 << endl;
  fscanf(fp1,"%f\t",&num3);
  fscanf(fp1,"%f\n",&num4);
  cout << num3 << '\t' << num4 << endl;
  fclose(fp1);
  return 0;
}

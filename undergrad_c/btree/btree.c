#include<stdio.h>
#include <stdlib.h>
/*���������*/
typedef struct Binarytree
{
        char a;
        struct Binarytree *lc;
        struct Binarytree *rc;
}BTN;

/*���볢�����鷽�����*/
char c[20]={'A','B','D','0','G','0','0','0','C','E','0','0','F','0','0'};
int j=0;

/*����������*/
BTN *jl()
{
     BTN *t;
     
     char ch;
     ch=c[j++];
     if(ch=='0')
         t=NULL;
     else
         {
             t=(BTN *)malloc(sizeof(BTN));
             t->a=ch;
             t->lc=jl();
             t->rc=jl();
         }
    return t;
}

/*��������������*/
void nzx(BTN *t,int n)
{
     int i;
     if(t==NULL)
     {
         return;
     }
     nzx(t->rc,n+1);
     for(i=0;i<n;i++)
       printf("   ");
     printf("%c\n",t->a);
     nzx(t->lc,n+1);
}

/*�������*/
void zx(BTN *t)
{
     if(t!=NULL)
     {
       zx(t->lc);
       printf("%3c",t->a);
       zx(t->rc);
     }
}

/*�������*/
void hx(BTN *t)
{
     if(t!=NULL)
     {
       zx(t->lc);
       zx(t->rc);
       printf("%3c",t->a);
     }
}

/*����α���*/
void acc(BTN *t)
{
     BTN *p,*q[100];
     int f,r;
     if(t==NULL)
       return;
     f=0;r=0;
     q[r++]=t;
     while(f!=r)
     {
         p=q[f++];
         printf("%3c",p->a);
         if (p->lc!=NULL)
           q[r++]=p->lc;
         if (p->rc!=NULL)
           q[r++]=p->rc;
     }
}

/*�ȽϺ���*/
int max(int a,int b)
{
    return a>=b?a:b;
} 
/*��������߶�*/
int gd(BTN *t)
{
	if (t==NULL)
		return 0;
	else
		return 1+max(gd(t->lc),gd(t->rc));
}

/*��Ҷ�ӽ�����*/
int jdgs(BTN *t)
{
    if (t==NULL)
	   return 0;
	if (t->lc==NULL&&t->lc==NULL)
	   return 1;
    return (jdgs(t->lc)+jdgs(t->rc));
}

main()
{
    BTN *t;
    int g,a;
    t=jl();
    printf("ԭʼ���вο��̲�Ϊ��\n");
    printf("���£ģ��ǣ������ãţ����ƣ���\n");
    printf("�������������Ϊ��\n");
    nzx(t,1);
    printf("\n");
    printf("�����������Ϊ��\n");
    zx(t);
    printf("\n");
    printf("�����������Ϊ��\n");
    hx(t);
    printf("\n");
    printf("����α�������Ϊ��\n");
    acc(t);
    printf("\n");
    printf("�߶�Ϊ��\n");
    a=gd(t);
    printf("%3d\n",a);
    printf("������Ϊ��\n");
    g=jdgs(t);
    printf("%3d\n",g);
    printf("\n");
    system("pause");
}

#include<stdio.h>
#include <stdlib.h>
/*定义二叉树*/
typedef struct Binarytree
{
        char a;
        struct Binarytree *lc;
        struct Binarytree *rc;
}BTN;

/*存入尝试数组方便测试*/
char c[20]={'A','B','D','0','G','0','0','0','C','E','0','0','F','0','0'};
int j=0;

/*创建二叉树*/
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

/*逆中序遍历并输出*/
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

/*中序遍历*/
void zx(BTN *t)
{
     if(t!=NULL)
     {
       zx(t->lc);
       printf("%3c",t->a);
       zx(t->rc);
     }
}

/*后序遍历*/
void hx(BTN *t)
{
     if(t!=NULL)
     {
       zx(t->lc);
       zx(t->rc);
       printf("%3c",t->a);
     }
}

/*按层次遍历*/
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

/*比较函数*/
int max(int a,int b)
{
    return a>=b?a:b;
} 
/*求二叉树高度*/
int gd(BTN *t)
{
	if (t==NULL)
		return 0;
	else
		return 1+max(gd(t->lc),gd(t->rc));
}

/*求叶子结点个数*/
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
    printf("原始序列参考教材为：\n");
    printf("ＡＢＤ０Ｇ０００ＣＥ００Ｆ００\n");
    printf("逆中序遍历树形为：\n");
    nzx(t,1);
    printf("\n");
    printf("中序遍历序列为：\n");
    zx(t);
    printf("\n");
    printf("后序遍历序列为：\n");
    hx(t);
    printf("\n");
    printf("按层次遍历序列为：\n");
    acc(t);
    printf("\n");
    printf("高度为：\n");
    a=gd(t);
    printf("%3d\n",a);
    printf("结点个数为：\n");
    g=jdgs(t);
    printf("%3d\n",g);
    printf("\n");
    system("pause");
}

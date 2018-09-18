#include<stdio.h>
#include<stdlib.h>
 //��ȡ���� ��ʼ��
typedef struct
{
    int x,y;
    int d;
}sq;
typedef struct
{
    int x,y;
}item;
    sq a[1024];//�����㹻��Ķ��з�ֹ���
    int front,rear;
    int temp[2][2];
    int maze[20][20];
    
void Read(int* a, int* b)
{
    int p,q;
    int i,j,k,f,g;
    FILE *fp;	//����һ���ļ�ָ�룬����fopen�ķ���ֵ���������Ĺر��ļ�
    char ch;	//����һ������������ļ��ĵ�һ���ַ��������������������Ҫ�������ж��ļ��ǲ���Ϊ��
    char filename[20];
	printf("\t��������������ļ���.txt��");
	scanf("\t%s",filename);
    if((fp=fopen(filename,"rt+"))==NULL)      //������ж���û���ļ�����ȥ��ȡ
    {
        printf("\t���ļ������ڣ���ʧ�ܡ�\n");
 		getch();	//����������ͽ�����һ�����ĺ���
        exit(0);
    }


    ch=fgetc(fp);   //���������ϲ鵽��һ���й��ļ��ĺ���������ֵ���ļ��ĵ�һ���ַ������ļ�Ϊ�յ�ʱ�������ص���һ������ֵ��EOF
    if(ch==EOF)		//���ļ�Ϊ�յ�ʱ��
    {
        printf("\t�ļ�Ϊ�գ������½����Թ���\n");
        exit(0);
    }
    else
    {
		rewind(fp);
    }
    for (f=0;f<20;f++)
    {
      for (g=0;g<20;g++)
      {
        maze[f][g]=1;
      }
    }
    //��ȡ����
    fscanf(fp,"%d%d\n",&p,&q);
    while(!feof(fp))
    {
    for(i=1;i<=p;i++)
    {
        for(j=1;j<=q;j++)
        {
            fscanf(fp,"%d",&maze[i][j]);
        }
    }
    fscanf(fp,"%d%d\n",&temp[0][0],&temp[0][1]);
    fscanf(fp,"%d%d\n",&temp[1][0],&temp[1][1]);
    }
    rewind(fp);
    *a=p;*b=q;                                   //C���Է��������p��qͨ��ָ�� 
    printf("��ȡ�ļ�Ϊ��\n");
    printf("%d %d\n",p,q);
    for(i=1;i<p+1;i++)
    {
        for(j=1;j<q+1;j++)
        {
            printf("%d,",maze[i][j]);
        }
        printf("\n");
    }
    printf("%d %d\n",temp[0][0],temp[0][1]);
    printf("%d %d\n",temp[1][0],temp[1][1]);
}

void printpath(sq a[],int rear,int m,int n)
{
    int i,j;
    i=rear;
    printf("����·��Ϊ��");
    do
                {
                printf("����%d,%d��",a[i].x,a[i].y);
                maze[a[i].x][a[i].y]=2;
                i=a[i].d;
                }
                while(i!=-1);
                printf("\n");
                printf("������Ϊ��\n");
                for(i=1;i<=m;i++)
                {
                  for(j=1;j<=n;j++)
                    if(maze[i][j]==2)
                      printf("* ");
                    else if(maze[i][j]==1)
                      printf("1 ");
                    else printf("0 ");
                      printf("\n");
                }                
}

int path(item move[8])
{
    int f, i, j, x, y,p,q,d;
    Read(&p,&q);
    a[0].x=temp[0][0]; //������ڵ�
    a[0].y=temp[0][1];
    a[0].d=-1;
    front=rear=0;
    maze[temp[0][0]-1][temp[0][1]-1]=-1;
    while (front<=rear)
    {
        x=a[front].x; y=a[front].y;d=a[front].d; /*�ص���һλ�ý�����һ���������̽*/
        for(f=0;f<8;f++)           /*�����з������*/
        {
            i=x+move[f].x;    /*�µ�����*/
            j=y+move[f].y;
            if (maze[i][j]==0)  /*�ж��Ƿ�ɵ���*/
            {
                rear++;
                a[rear].x=i;
                a[rear].y=j;
                a[rear].d=front;
                maze[i][j]=-1;
            }
            if(i==temp[1][0]&&j==temp[1][1])
            {
                printpath(a,rear,p,q);
                return 1;
            }
        } /* (f<8)*/
        front++;
      }  /*while */
      printf("�Թ���·\n");
      return 0;	/*�Թ���·*/
 }


main()
{
    int p,q;
    item move[8]={{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1}}; /*������������ move �ĳ�ʼ��*/
    path(move);
    system("pause");
}
//���⣺�ļ������ʽ

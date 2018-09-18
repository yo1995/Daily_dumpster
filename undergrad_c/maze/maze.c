#include<stdio.h>
#include<stdlib.h>
 //读取函数 初始化
typedef struct
{
    int x,y;
    int d;
}sq;
typedef struct
{
    int x,y;
}item;
    sq a[1024];//定义足够大的队列防止溢出
    int front,rear;
    int temp[2][2];
    int maze[20][20];
    
void Read(int* a, int* b)
{
    int p,q;
    int i,j,k,f,g;
    FILE *fp;	//定义一个文件指针，接收fopen的返回值，用于最后的关闭文件
    char ch;	//定义一个变量来存放文件的第一个字符，但在这个函数里面主要是用来判断文件是不是为空
    char filename[20];
	printf("\t请输入所保存的文件名.txt：");
	scanf("\t%s",filename);
    if((fp=fopen(filename,"rt+"))==NULL)      //这可以判断有没有文件给你去读取
    {
        printf("\t该文件不存在，打开失败。\n");
 		getch();	//按下任意键就进入下一条语句的函数
        exit(0);
    }


    ch=fgetc(fp);   //这是在网上查到的一个有关文件的函数，返回值是文件的第一个字符，当文件为空的时候，他返回的是一个特殊值：EOF
    if(ch==EOF)		//当文件为空的时候
    {
        printf("\t文件为空，请重新建立迷宫！\n");
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
    //读取部分
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
    *a=p;*b=q;                                   //C语言返回所需的p，q通过指针 
    printf("读取文件为：\n");
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
    printf("回溯路径为：");
    do
                {
                printf("←（%d,%d）",a[i].x,a[i].y);
                maze[a[i].x][a[i].y]=2;
                i=a[i].d;
                }
                while(i!=-1);
                printf("\n");
                printf("输出结果为：\n");
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
    a[0].x=temp[0][0]; //定义入口点
    a[0].y=temp[0][1];
    a[0].d=-1;
    front=rear=0;
    maze[temp[0][0]-1][temp[0][1]-1]=-1;
    while (front<=rear)
    {
        x=a[front].x; y=a[front].y;d=a[front].d; /*回到上一位置进行下一个方向的试探*/
        for(f=0;f<8;f++)           /*当还有方向可试*/
        {
            i=x+move[f].x;    /*新点坐标*/
            j=y+move[f].y;
            if (maze[i][j]==0)  /*判断是否可到达*/
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
      printf("迷宫无路\n");
      return 0;	/*迷宫无路*/
 }


main()
{
    int p,q;
    item move[8]={{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1}}; /*坐标增量数组 move 的初始化*/
    path(move);
    system("pause");
}
//问题：文件输出格式

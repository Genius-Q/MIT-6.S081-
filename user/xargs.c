//user/xargs.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

//运行指定程序，接受参数，根据参数执行进程，并替换为参数进程
void run(char* program,char** args){
	//创建子进程，在子进程中执行指定的程序
	if(fork()==0){   //通过fork（）创建子进程
		exec(program,args);	//在子进程中调用exec执行目标程序
		exit(0);		//父进程直接返回，不等待子进程结束
	}
	return ;
}
int main(int argc,char *argv[]){
	char buf[2048];			//作为输入缓冲区
	char* p=buf,* last_p=buf;	//字符指针:p指向缓冲区的位置，last_p指向参数的字符起始位置
	char* argsbuf[128];		//作为参数缓冲区
	char** args=argsbuf;		//用于存放参数，字符串指针

	//首先将Xargs的参数复制到argsbuf中
	for(int i=1;i<argc;i++){
		*args=argv[i];
		args++;
	}
	//记录当前参数位置
	char** pa=args;		//argsbuf数组记录每个参数的位置地址
	
	//从标准输入读取数据，存储在缓冲区buf中：
	while(read(0,p,1)!=0){		//持续从标准输入中读取一个字节并存放在输入缓冲区buf中去
		//使用P指针遍历缓冲区，遇到空格或换行时，将其替换为字符串结束符\0
		if(*p==' '||*p=='\n'){
			*p ='\0';

			//将参数添加到参数缓冲区argbuf中
			*(pa++)=last_p;  //将参数的位置放入缓冲区中，pa是指向参数位置的指针
			last_p=p+1;  	 //last_p指向下一个参数的位置
			
			//每当遇到换行符\n时，表示一组参数读取完毕，调用run函数执行程序，传递参数
			if(*p=='\n'){

				*pa=0;        //将最后一个参数字符串置0，终止当前参数字符串
				run(argv[1],argsbuf);	//将缓冲区存的参数取出，来进行执行，不用在意那些是命令那些是参数，对于函数来说都是参数
				pa=args;	//pa重新指向缓冲区的开始，后序重新将往后的参数转入缓冲区
			}

		}
		//继续读数据
		p++;

	}

	//如果最后一行不是空行，同样的逻辑再处理一次,那么最后一行不会执行*p=\n的条件，此时pa是指向参数的参数缓冲区的最后一个位置的参数（等待装入），但已经结束没有参数了。
	if(pa!=args){
		*p='\0';
		*(pa++)=last_p;
		*pa=0;

		run(argv[1],argsbuf);
	}

	//等待所有子进程结束
	while(wait(0)!=-1){};
	exit(0);
}

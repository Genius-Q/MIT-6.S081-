#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

//筛选质数的函数，接受一个管道作为参数
void sieve(int pleft[2]){
    //从左邻居读取整数
    int p;
    read(pleft[0],&p,sizeof(p));
    if(p==-1){
        exit(0);  //如果读取到-1，表示结束，退出进程
    }
    printf("prime %d\n",p);  //此时接收到数字肯定是质数

    //创建一个新的管道用于向子进程传递数据
    int pright[2];
    pipe(pright);

    if(fork()==0){      //右邻居（子进程）
        close(pright[1]);       //右邻居用不到这个管道的写端，关闭
        close(pleft[0]);        //右邻居用不到上一个管道的读段，关闭
        sieve(pright);          //递归调用筛选函数
    }else{
        close(pright[0]);     //当前进程用不到这个管道的读端，关闭
        //从做邻居接受数字
        int buf;
        while(read(pleft[0],&buf,sizeof(buf))&&buf!=-1){
            if(buf%p!=0){
                write(pright[1],&buf,sizeof(buf));
            }
        }
        //此时接受到了左邻居传来的-1，要给右邻居也传-1，结束右邻居进程
        buf=-1;
        write(pright[1],&buf,sizeof(buf));
        wait(0);
        exit(0);
    }
}

int main(int argc,char **argv){
    //创建初始管道
    int input_pipe[2];
    pipe(input_pipe);
    
    if(fork()==0){
        close(input_pipe[1]);//子进程（右邻居只接受）
        sieve(input_pipe);      //调用筛选函数
        exit(0);
    }else{
        close(input_pipe[0]);     //父进程（父进程只会向子进程（右邻居）写数据）
        int i;
        for(i=2;i<=35;i++){
            write(input_pipe[1],&i,sizeof(i)); //向管道写入2~35整数
        }
        //写入结束标志
        i=-1;
        write(input_pipe[1],&i,sizeof(i));
    }
    wait(0);     //等待子进程结束
    //注意这里无法等待子进程的子进程，只能直接等待子进程，无法等待间接子进程
    //在sieve（）中再各自执行wait(0),形成等待链
    exit(0);
}
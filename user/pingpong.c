#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc,char **argv){
    //发送的字节数据
    char message;
    //用于保存管道的一对文件描述符
    int p0[2];  
    int p1[2];
    pipe(p0);  //使用管道0，从父向子发送信息
    pipe(p1);  //使用管道1，从子向父发送信息
    if(fork()==0){
        close(p0[1]);//关闭p0的标准输出，只输入
        close(p1[0]);//关闭p1的标准输入，只输出
        if(read(p0[0],&message,1)==1){//如果读取到了一个字节
            if(message=='a'){
                close(p0[0]);//关闭p0的标准输入，避免阻塞
                printf("%d: received ping\n",getpid());
                //发送一个字节
                write(p1[1],"b",1);
                close(p1[1]);
            }
            exit(0);
        }

    }else{
        //向子进程发送一个信息，使用管道0
        close(p0[0]); //关闭p0的标准输入，只输出
        write(p0[1],"a",1); //向管道写入1个字节
        close(p0[1]);//关闭p0的标准输入
        close(p1[1]);//关闭p1的标准输出
        //读取接受信息
        if(read(p1[0],&message,1)==1){
            if(message=='b'){
                printf("%d: received pong\n",getpid());
                close(p1[0]);
            }
        }
        wait(0);
    }
    exit(0);
}

// int main(int argc,char **argv){
//     char message;
//     int p0[2],p1[2];
//     pipe(p0);
//     pipe(p1);

//     if(fork()==0){
//         //子进程接受信息
//         //char message;
//         read(p0[0],&message,1);
//         printf("%d: received ping\n",getpid());

//         //子进程发送信息
//         write(p1[1],".",1);
//         close(p1[1]);
//     }else{
//         //父进程发送信息
//         write(p0[1],".",1);
//         close(p0[1]);
        
//         //父进程接受信息
//         //char message;
//         read(p1[0],&message,1);
//         printf("%d: received pong\n",getpid());

//         wait(0);
//     }

//     //关闭管道的读口
//     close(p0[0]);
//     close(p1[0]);

//     exit(0);
// }
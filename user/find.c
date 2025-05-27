#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

//读取目录
//递归查找目录

//递归查找函数，查找路径为path的目录下是否有文件target
void find(char* path,char* target){
    char buf[512],*p;
    int fd;
    struct dirent de;     //目录结构体
    struct stat st;       //文件结构体
    //打开目录
    if((fd=open(path,0))<0){
        fprintf(2,"find:cannot open %s\n",path);
        return;
    }
    //获取目录状态信息
    if(fstat(fd,&st)<0){
        fprintf(2,"find:cannot stat %s\n",path);
        close(fd);
        return;
    }
    //文件，目录分别处理
    switch(st.type){
        //如果是文件，检查文件名是否与目标文件名匹配
        case T_FILE:
            //这里用来比较字符串path的末尾部分是否与target完全匹配
            if(strcmp(path+strlen(path)-strlen(target),target)==0){
                printf("%s\n",path);
            }
            break;
        //如果是目录
        case T_DIR:
        //检查路径长度是否超出缓冲区大小
            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
                printf("ls: path too long\n");
                break;
            }
            strcpy(buf, path);
            p = buf+strlen(buf);
            *p++ = '/';
            //读取目录项
            while(read(fd, &de, sizeof(de)) == sizeof(de)){
                if(de.inum == 0)
                    continue;
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                //获取目录项的状态信息
                if(stat(buf, &st) < 0){
                    printf("ls: cannot stat %s\n", buf);
                    continue;
                }
                //排除“.”和“..”目录（路径结尾为/.和/..）
                if(strcmp(buf+strlen(buf)-2,"/.")&&strcmp(buf+strlen(buf)-3,"/..")!=0){
                    find(buf,target);
                }
            }
            break;
        }
        close(fd);
}

int main(int argc,char *argv[])
{
    if(argc < 3){       //如果参数不足，退出程序
        exit(0);
    }
    char target[512];
    target[0]='/';      //为查找文件名添加/在开头
    strcpy(target+1,argv[2]);   //将目标文件名存储在target中
    find(argv[1],target);       //调用查找函数
    exit(0);
}
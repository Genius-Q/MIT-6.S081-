#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]){
    if(argc<2){
        fprintf(2,"sleep: no use sleep<argv>\n");
        //fprintf(1,"use:sleep<time>\n");
        exit(1);
    }
    sleep(atoi(argv[1]));
    fprintf(1,"argc:%d\n",argc);
    fprintf(1,"sleep:%d\n",atoi(argv[1]));
    exit(0);
}
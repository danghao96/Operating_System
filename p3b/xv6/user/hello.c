#include "types.h"
#include "user.h"

int main(int argc, char *argv[]) {
	//char *p = (char*) ((640 * 1024) - 4096);
    //printf(1, "%x\n", p);
    //*p = 'a';
    /*
    int rc = fork();
    if(rc == 0){
        printf(1, "child: %c\n", *p);
    } else if (rc > 0){
        printf(1, "parent: %c\n", *p);
    } else {
        printf(1, "Error\n");
    }
    */
    //int *p0 = 0;
    //printf(1, "%x\n", *p0);
    // *p0 = 'b';
    //printf(1, "%c\n", *p0);
    
    //int *p8k = (int*) 8192;
    //printf(1, "%x\n", *p8k);

    int a = 0;
    int b = 1;
    int c = 2;
    printf(1, "%p, %p, %p\n", &a, &b, &c);
    int *p1 = (int*) (636 * 1024);
    int *p2 = (int*) (636 * 1024 + 1);
    int *p3 = (int*) (636 * 1024 + 2);
    printf(1, "%p, %p, %p\n", p1, p2, p3);
    printf(1, "%d, %d, %d\n", p1, p2, p3);
	exit();
}
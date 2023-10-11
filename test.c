#include <stdio.h> 
#include <unistd.h>

int main(int argc, char *argv[]) {
    void *c_break = sbrk(0);
    printf("1- %p \n", c_break);
    c_break = sbrk(0);
    printf("2- %p \n", c_break);
    /* Result in different addresses because printf
    allocates for a buffer in a first place (buffer
    that will be printed on the stdout) */


    c_break = sbrk(0);
    printf("3- %p \n", c_break);

    sbrk(0x100); // Increase the allocated memory of 0x100
    c_break = sbrk(0); // See the above change
    printf("4- %p \n", c_break);
}
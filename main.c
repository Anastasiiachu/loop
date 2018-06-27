#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "loop.h";

int main(int argc, char *argv[]) {
    /*char * filename = "input.txt";
    if (argc >= 2) {
        filename = argv[1];
    }
    if (exec(filename, ""))
        return 1;
    return 0;*/
    exec("input.txt", "output.txt");
    exec("input2.txt", "output2.txt");
    exec("input3.txt", "output3.txt");
}
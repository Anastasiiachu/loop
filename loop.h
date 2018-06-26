//структура input файла
typedef struct _I_FILE {
    FILE * f;
    int lines;
    int eof;
} I_FILE;

typedef struct _O_X {
    long long * x;
    long int length;
} O_X;

int exec(char * input, char * output);
int exec_l(O_X * x, char ** lines, int l_index);

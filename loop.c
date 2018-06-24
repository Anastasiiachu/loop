#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "loop.h";




char * readln(I_FILE * file) {
    char * line = malloc(1000 * sizeof(char)); //макс длина в 1000 символов
    int length = 0;
    int new_line_terminated = 0;
    int c;
    while ((c = fgetc(file->f)) != EOF) {
        if (c == '\n') {
            new_line_terminated = 1;
            break;
        }
        line[length++] = (char)c;
    }
    if (!new_line_terminated)
        file->eof = 1;
    line[++length] = '\0';
    file->lines++;
    return line;
}
//выделение памяти под переменные
void realloc_x(O_X * x, long int index) {
    if (x->length <= index) {
        x->x = realloc(x->x, sizeof(x->x) + (sizeof(long long) * (index + 1 - x->length)));
        x->length = index + 1;
    }
}
//скипаем пробелы
int s_s(char * string, int from) {
    for (int i = from; i < strlen(string); ++i) {
        if (string[i] != ' ' && string[i] != '\t' && string[i] != '\n' && string[i] != 'v'
            && string[i] != '\f' && string[i] != '\r')
            return i;
    }
    return -1;
}

void math(O_X * x, char * _line) {
    char * line = malloc(strlen(_line) * sizeof(char));
    strcpy(line, _line);
    line += s_s(line, 0);
    if (*line != 'x')
        return;
    line++; //сдвигаем указатель
    //Читаем, с какой переменной работаем
    long int index = strtol(line, &line, 10);
    if (index >= x->length)
        realloc_x(x, index); //В любом случае выделям память на эту переменную
    line++; //Снова сдвигаем указатель
    //Проверяем знак присваивания
    if (*(line)++ != ':' || *(line)++ != '=')
        return;
    line++; //И опять сдвигаем указатель
    long long value_a;
    //Если это другая переменная
    if (*line == 'x') {
        line++;
        long int index_a = strtol(line, &line, 10);
        if (index_a >= x->length)
            value_a = 0; //Переменная не используется -> равна 0
        else
            value_a = x->x[index_a];
    //Если константа
    } else {
        value_a = strtol(line, &line, 10);
    }
    long long value_b = 0;
    int sum = 1;
    //Выражение могло закончится всего на одном слагаемом, так что проверяем наличие второго
    if (*line != '\0') {
        line++; //Да сколько можно их сдвигать?
        if (*line == '+') {
            sum = 1;
        } else if (*line == '-') {
            sum = -1;
        } else {
            return;
        }
        line+=2; //Остановись!
        if (*line == 'x') {
            line++;
            long int index_b = strtol(line, &line, 10);
            if (index_b >= x->length)
                value_b = 0; //Переменная не используется -> равна 0
            else
                value_b = x->x[index_b];
            //Если константа
        } else {
            value_b = strtol(line, &line, 10);
        }
    }
    long long result = value_a + (value_b * sum);
    if (result > 0)
        x->x[index] = result;
    else
        x->x[index] = 0;
    //free(line);
}

int loop(O_X * x, char ** lines, int l_index) {
    char * line = malloc(strlen(lines[l_index]) * sizeof(char));
    strcpy(line, lines[l_index]);
    line += s_s(line, 0);
    if (*(line)++ != 'L' || *(line)++ != 'O' || *(line)++ != 'O' || *(line)++ != 'P')
        return -1;
    line++; //again..
    long int count;
    if (*line == 'x') {
        line++;
        long int index = strtol(line, &line, 10);
        if (index >= x->length)
            count = 0; //Переменная не используется -> равна 0
        else
            count = x->x[index];
        //Если константа
    } else {
        count = strtol(line, &line, 10);
    }
    int i,j;
    j = 1;
    for (i = 0; i < count; i++) {
        int result = 0;
        j = 1;
        while (result != -1) {
            result = exec_l(x, lines, (l_index + j++));
            if (result > 0)
                j = result - l_index;
        }
    }
    return l_index + j;

}
//куда пошлем строку?
int exec_l(O_X * x, char ** lines, int l_index) {
    int i = s_s(lines[l_index], 0);
    char c = lines[l_index][i];
    if (c == 'L') {
        return loop(x, lines, l_index);
    } else if (c == 'x') {
        math(x, lines[l_index]);
        return 0;
    } else if (lines[l_index][i] == 'E' && lines[l_index][i + 1] == 'N' && lines[l_index][i + 2] == 'D')
        return -1;
}

void exec(char * input, char * output) {
    O_X x;
    x.length = 0;

    char ** lines = '\0';

    I_FILE file;
    file.f = fopen(input, "r");
    file.eof = 0;
    file.lines = 0;
    while (!file.eof) {

        lines = realloc(lines, sizeof(lines) + 1000);
        lines[file.lines] = readln(&file);
    }
    int i;
    for (i = 0; i < file.lines; i++) {
        int result = exec_l(&x, lines, i);
        if (result > 0)
            i = result;
    }
    int j = 0;
    for (j = 0; j < x.length; j++)
        printf("x%i = %i\n", j, (int)x.x[j]);


}


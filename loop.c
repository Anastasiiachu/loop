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

void big_int_free(big_int * a) {
    free(a->num);
    free(a);
}

void refit(big_int * a) {
    int n = 0;
    for (int i = a->length - 1; i >= 0; i--) {
        if (a->num[i] == 0)
            n++;
        else
            break;
    }
    if (n > 0) {
        a->length -= n;
        a->num = realloc(a->num, sizeof(int) * a->length);
    }

}

big_int * bfroml(long a) {
    big_int * c = (big_int *)malloc(sizeof(big_int));
    c->length = 19; //Длина long long
    c->num = malloc(sizeof(int) * c->length);
    for (int i = 0; i < c->length; i++)
        c->num[i] = 0;
    for (int i = 0; a > 0; i++) {
        c->num[i] = (int)(a % 10);
        a /= 10;
    }
    refit(c);
    return c;
}
long long btol(big_int * a) {
    long long result = 0;
    for (int i = a->length - 1; i >= 0; i--) {
        result *= 10;
        result += a->num[i];
    }
    return result;
}
big_int * sum (big_int * a, big_int * b) {
    if (a == 0)
        a =  bfroml(0);
    if (b == 0)
        b =  bfroml(0);
    big_int * c = (big_int *)malloc(sizeof(big_int));
    c->length = ((a->length > b->length) ? (a->length + 1) : (b->length + 1));
    c->num = malloc(sizeof(int) * c->length);
    for (int i = 0; i < c->length; i++)
        c->num[i] = 0; //начальная инициализация. Без нее какой-нибудь незначащий 0 может вдруг оказаться пятеркой
    int shift = 0;
    for (int i = 0; i < c->length; i++) {
        int d_a = ((i >= a->length) ? 0 : (a->num[i]));
        int d_b = ((i >= b->length) ? 0 : (b->num[i]));
        int sum = d_a + d_b + shift;
        if (sum > 9) {
            c->num[i] = sum % 10;
            shift = 1;
        } else {
            c->num[i] = sum;
            shift = 0;
        }
    }
    refit(c);
    return c;
}

big_int * substract(big_int * a, big_int * b) {
    if (a == 0)
        a =  bfroml(0);
    if (b == 0)
        b =  bfroml(0);
    big_int * c = (big_int *)malloc(sizeof(big_int));
    c->length = ((a->length > b->length) ? (a->length) : (b->length));
    c->num = malloc(sizeof(int) * c->length);
    for (int i = 0; i < c->length; i++)
        c->num[i] = 0;
    int shift = 0;
    for (int i = 0; i < c->length; i++) {
        int d_a = ((i >= a->length) ? 0 : (a->num[i]));
        int d_b = ((i >= b->length) ? 0 : (b->num[i]));
        int sum = d_a - d_b + shift;
        if (sum < 0 && i != (c->length - 1)) {
            c->num[i] = sum + 10;
            shift = -1;
        } else {
            c->num[i] = sum;
            shift = 0;
        }
    }
    if (c->num[c->length - 1] < 0)
        for (int i = 0; i < c->length; i++)
            c->num[i] = 0;
    refit(c);
    return c;
}

//выделение памяти под переменные
void realloc_x(O_X * x, long int index) {
    if (x->length <= index) {
        x->x = realloc(x->x, sizeof(big_int *) * (index + 1));
        long i;
        for (i = x->length; i <= index; i++)
            x->x[i] = bfroml(0);
        x->length = index + 1;
    }
}
//скипаем пробелы
int s_s(char * string, int from) {
    for (int i = from; i < strlen(string); ++i) {
        if (string[i] != ' ' && string[i] != '\t' && string[i] != '\n' && string[i] != '\v'
            && string[i] != '\f' && string[i] != '\r')
            return i;
    }
    return -1;
}
//математические операции
void math(O_X * x, char * line) {
    size_t length = strlen(line);
    line += s_s(line, 0);
    if (*line != 'x')
        return; //TODO вывести ошибку
    line++; //сдвигаем указатель
    //Читаем, с какой переменной работаем
    long int index = strtol(line, &line, 10);
    if (index >= x->length)
        realloc_x(x, index); //В любом случае выделям память на эту переменную
    line++; //Снова сдвигаем указатель
    //Проверяем знак присваивания
    if (*(line)++ != ':' || *(line)++ != '=')
        return; //TODO вывести ошибку
    line++; //И опять сдвигаем указатель
    big_int * value_a;
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
        value_a = bfroml(strtol(line, &line, 10));
    }
    big_int * value_b = 0;
    int sum_flag = 1; //по нему определяем какой знак операции
    //Выражение могло закончится всего на одном слагаемом, так что проверяем наличие второго
    if (*line != '\0' && *line != ';' && *line != '\r') {
        line++; //Да сколько можно их сдвигать?
        if (*line == '+') {
            sum_flag = 1;
        } else if (*line == '-') {
            sum_flag = -1;
        } else {
            return; //TODO вывести ошибку
        }
        line+=2; //Остановись! сразу + и пробел
        if (*line == 'x') {
            line++;
            long int index_b = strtol(line, &line, 10);
            if (index_b >= x->length)
                value_b = 0; //Переменная не используется -> равна 0
            else
                value_b = x->x[index_b];
            //Если константа
        } else {
            value_b = bfroml(strtol(line, &line, 10));
        }
    }
    if (sum_flag == 1) {
        big_int * old = x->x[index];
        x->x[index] = sum(value_a, value_b);
        //big_int_free(old);
    } else {
        big_int * old = x->x[index];
        x->x[index] = substract(value_a, value_b);
        //big_int_free(old);
    }
    line -= (length - strlen(line));
}

int loop(O_X * x, char ** lines, int l_index) {
    size_t length = strlen(lines[l_index]);
    //char * line = malloc(strlen(lines[l_index]) * sizeof(char));
    char * line = lines[l_index];
    strcpy(line, lines[l_index]);
    line += s_s(line, 0);
    //проверим слово loop
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
            count = btol(x->x[index]); //переменная была использована ранее, берем ее значение по индексу
        //Если константа (а может так нельзя)
    } else {
        //count = strtol(line, &line, 10);
        return -1;
    }
    //i- сам count, j - сдвиг строки относительно loop (l_index)
    int i,j;
    j = 1;
    for (i = 0; i < count; i++) {
        int result = 0; //ответ функции exec_l
        j = 1;
        //какие строки будут выполняться в цикле loop
        while (result != -1) {
            result = exec_l(x, lines, (l_index + j++));
            //loop в loop'е
            if (result > 0)
                j = result - l_index;
        }
    }
    line -= (length - strlen(line));
    return l_index + j; //вернет индекс строки после "end" -> передаст ей управление

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

int exec(char * input, char * output) {
    O_X x;
    x.x = malloc(sizeof(long long));
    x.length = 0;

    char ** lines = '\0';

    I_FILE file;
    if (!(file.f = fopen(input, "r"))) {
        //fclose(file.f);
        return 1;
    }
    file.eof = 0;
    file.lines = 0;
    while (!file.eof) {
        lines = realloc(lines, (file.lines + 1) * sizeof(char *));
        lines[file.lines] = readln(&file);
    }
    int i;
    for (i = 0; i < file.lines; i++) {
        int result = exec_l(&x, lines, i);
        if (result > 0)
            i = --result;
    }
    FILE * out = fopen(output, "w+");
    int j = 0;
    for (j = 0; j < x.length; j++)
        fprintf(out,"x%i = %lli\n", j, btol(x.x[j]));
    fclose(out);
    fclose(file.f);
    return 0;
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char* String_Assemble();


struct Word{
    char* word;
    int len;
    int upcase_cnt;
    int starts_and_ends_with_num;
};

struct Sentence{
    struct Word* arr;
    int word_cnt;
    int pos;
};

struct Text{
    struct Sentence* arr;
    int sentences_cnt;
};

int main() {





    return 0;
}

char* string_Assemble(){
    // Функция, которая будет собирать предложение в виде строки из массива слов структуры Sentence
    return 1;
}

void input_Text(){
    // Фукция ввода. Алгоритм работы: посимвольно считывает поток ввода, записывает слова в структуру Word и сохраняет их
    // в массив. Если встречает точку, то создает структуру Sentence, передает ей этот массив и другие аргументы.
    // Sentence сразу записывается в массив структуры Text.

}
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


char *string_Assemble();

void input_Text();

struct Word {
    char *arr;
    int len;
    int upcase_cnt;
    int starts_with_num;
    int ends_with_num;
};

struct Comma_Array {
    int *comma_positions;
    int count;

};

struct Sentence {
    struct Word *words;
    int *comma_indexes;
    int word_cnt;
    int symb_cnt;
    int pos;
};

struct Text {
    struct Sentence *arr;
    int sentences_cnt;
};
//

struct Text text;

//
int main() {
    input_Text();
    for (int i = 0; i<text.sentences_cnt; i++){
        // printf("%s", string_Assemble(text.arr[i]));
    }
    return 0;
}

char* string_Assemble(struct Sentence sentence) {
    // Функция, которая будет собирать предложение в виде строки из массива слов структуры Sentence
    char* str = calloc(sentence.symb_cnt+1, sizeof(char));
    int counter = -1;
    for (int i = 0; i<sentence.word_cnt; i++){
        struct Word word = sentence.words[i];
        for (int j = 0; j<word.len; j++){
            str[++counter] = word.arr[j];
        }
        str[++counter] = ' ';
    }
    str[++counter] = '.';
    str[++counter] = '\0';
    return str;
}

struct Word word_Initialization() {
    struct Word word;
    word.len = 0;
    word.starts_with_num = 0;
    word.ends_with_num = 0;
    word.upcase_cnt = 0;
    return word;

}

struct Sentence sentence_Initialization(int sentence_pos) {
    struct Sentence sentence;
    sentence.pos = sentence_pos;
    sentence.word_cnt = 0;
    sentence.symb_cnt = 0;
    return sentence;
}

void *memory_Allocate(void *array, int len, int size) {
    if (len == 0) array = calloc(len + 1, size);
    else array = realloc(array, (len + 1) * size);
    return array;
}

void input_Text() {
    // Фукция ввода. Алгоритм работы: посимвольно считывает поток ввода, записывает слова в структуру Word и сохраняет их
    // в массив. Если встречает точку, то создает структуру Sentence, передает ей этот массив и другие аргументы.
    // Sentence сразу записывается в массив структуры Text.
    int sentences_cnt = 0;
    struct Sentence* sentences_arr = memory_Allocate(sentences_arr, sentences_cnt, sizeof(struct Sentence));

    // Создаем структуру слова, инициализируем начальные значения
    struct Word word = word_Initialization();

    int word_cnt = 0;
    struct Word *words_arr = memory_Allocate(words_arr, word_cnt, sizeof(struct Word));

    // Создаем структуру предложения, инициализируем начальные значения
    int sentence_pos = 0;
    struct Sentence sentence = sentence_Initialization(sentence_pos);

    // Массив и переменная для запоминания местоположения запятых
    struct Comma_Array commas;
    commas.count = 0;
    commas.comma_positions = calloc(commas.count + 1, sizeof(int));
    int char_pos = 0;

    int end_sentence_flag = 0;
    char c = getchar();
    while (c != '\n') { // Главный цикл, идет до конца текста
        sentence.symb_cnt++;
        if (c == '.') {
            // Если конец предложения, то нужно еще дочитать одно слово. Поставим специальный флаг и сделаем c = ' '
            // Так как именно в блоке с пробелом происходит считывание слова
            end_sentence_flag = 1;
            c = ' ';
        }
        if (c == ',') {
            commas.comma_positions = memory_Allocate(commas.comma_positions, commas.count, sizeof(int));
            commas.comma_positions[commas.count] = char_pos;
            commas.count++;
        }

        else if (c == ' ') {
            // Здесь нужно проверять последний символ слова на соответствие цифре (ends_with_num)
            // А еще записывать слово в предложение
            if (end_sentence_flag == 2){ // убирает лишний пробел после точки
                c = getchar();
                end_sentence_flag = 0;
                continue;
            }
            else {
                if (isdigit(word.arr[word.len])) {
                    word.ends_with_num = 1;
                }

                // Записываем слово в структуру предложения
                words_arr = memory_Allocate(words_arr, word_cnt, sizeof(struct Word));
                words_arr[word_cnt] = word;
                word_cnt++;
                word = word_Initialization();
            }

            // Если была точка (конец предложения)
            if (end_sentence_flag == 1) {
                // Заполняем структуру предложения
                sentence.words = words_arr;
                sentence.word_cnt = word_cnt;
                sentence.pos = sentence_pos;
                sentence.comma_indexes = commas.comma_positions;

                // Записываем в структуру текста
                sentences_arr = memory_Allocate(sentences_arr, sentences_cnt, sizeof(struct Sentence));
                sentences_arr[sentences_cnt] = sentence;
                sentences_cnt++;

                // Обновляем переменные
                sentence = sentence_Initialization(sentences_cnt);
                end_sentence_flag = 2;
                char_pos = -1;
            }
        }

        else if (c != '.') {
            // Проверяем, на какой параметр может повлиять этот символ
            if (isdigit(c) && word.len == 0) {
                word.starts_with_num = 1;
            }
            if (isupper(c)) {
                word.upcase_cnt++;
            }

            // Записываем символ в word.arr - строку.
            word.arr = memory_Allocate(word.arr, word.len, sizeof(char));
            word.arr[word.len] = c;
            word.len++;
        }

        c = getchar();
        char_pos++;
    }

    text.sentences_cnt = sentences_cnt;
    text.arr = sentences_arr;

}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <locale.h>
#include <wchar.h>

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
    int comma_count;
    int word_cnt;
    int symb_cnt;
    int pos;
};

struct Text {
    struct Sentence *arr;
    int sentences_cnt;
};
//
typedef struct Sentence Sentence;
typedef struct Word Word;
typedef struct Comma_Array Comma_Array;

void free_sentence_memory(Sentence *sentence);

char *string_assemble(Sentence sentence);

void remove_replicated_sentences();

void *memory_allocate(void *array, int len, int size);

void remove_even_sentences();

void uppercase_sort();

void less_then_three();

void max_len_word_sort();

void input_text();

struct Text text;


int main() {
    setlocale(LC_ALL, "");
    wprintf(L"Введите текст:\n");
    input_text();
    remove_replicated_sentences();
    wprintf(L"Выберите одно из доступных действий:\n"
            "1) Удалить все четные по счету предложения в которых четное количество слов.\n"
            "2) Отсортировать все слова в предложениях по возрастанию количества букв в верхнем регистре в слове.\n"
            "3) Заменить все слова в тексте длина которых не более 3 символов на подстроку “Less Then 3”.\n"
            "4) Найти в каждом предложении строку максимальной длины, которая начинается и заканчивается цифрой. Вывести найденные подстроки по убыванию длины подстроки.\n"
            "Для выхода из программы введите любое другое значение. \n");
    int user_choice;
    scanf("%d", &user_choice);

    switch (user_choice) {
        case 1:
            remove_even_sentences();
            break;
        case 2:
            uppercase_sort();
            break;
        case 3:
            less_then_three();
            break;
        case 4:
            max_len_word_sort();
            break;
    }

    // Вывод текста после обработки
    if (user_choice != 4) {
        for (int i = 0; i < text.sentences_cnt; i++) {
            printf("%s ", string_assemble(text.arr[i]));
        }
    }

    // Очищение памяти
    for (int i = 0; i < text.sentences_cnt; i++) {
        free_sentence_memory(&text.arr[i]);
    }
    text.arr = NULL;
    return 0;
}

void free_sentence_memory(Sentence *sentence) {
    for (int i = 0; i < sentence->word_cnt; i++) {
        free(sentence->words[i].arr);
        sentence->words[i].arr = NULL;
    }
    free(sentence->comma_indexes);
    sentence->comma_indexes = NULL;
    free(sentence->words);
    sentence->words = NULL;
}

char *string_assemble(Sentence sentence) {
    // Функция, которая будет собирать предложение в виде строки из массива слов структуры Sentence
    char *str = calloc(sentence.symb_cnt + 2, sizeof(char));
    int counter = -1;
    int words_used = 0;
    int last_comma_index = -1;
    for (int i = 0; i < sentence.word_cnt; i++) {
        Word word = sentence.words[i];
        for (int j = 0; j < word.len; j++) {
            str[++counter] = word.arr[j];
        }
        // Ввожу запятую, если нужно
        for (int k = last_comma_index + 1; k < sentence.comma_count; k++) {
            if (words_used > sentence.comma_indexes[k]) break;
            if (words_used == sentence.comma_indexes[k]) {
                str[++counter] = ',';
                last_comma_index = k;
            }
        }
        str[++counter] = ' ';
        words_used++;
    }
    str[counter] = '.';
    str[++counter] = '\0';
    return str;
}

void remove_sentence(int pos) {
    // Удаляет предложение из текста
    free_sentence_memory(&text.arr[pos]);
    memmove(text.arr + pos, text.arr + pos + 1, (text.sentences_cnt - pos) * sizeof(Sentence));
    text.sentences_cnt--;

    // Сдвигает позиции всех следующих предложений
    for (int i = pos; i < text.sentences_cnt; i++) {
        text.arr[i].pos--;
    }
}

void remove_replicated_sentences() {
    // Бежит по тексту и сравнивает предложения вложенными циклами. Прежде чем начать посимвольное сравнение проверяется совпадение кол-ва символов и кол-ва слов.
    for (int i = 0; i < text.sentences_cnt; i++) {
        Sentence cur_sentence = text.arr[i];
        for (int j = i + 1; j < text.sentences_cnt; j++) {
            Sentence next_sentence = text.arr[j];
            if (cur_sentence.symb_cnt == next_sentence.symb_cnt && cur_sentence.word_cnt == next_sentence.word_cnt) {
                // Если длина и кол-во слов совпали, то идет посимвольное сравнение
                char *cur_string = string_assemble(cur_sentence);
                char *next_string = string_assemble(next_sentence);
                if (strcasecmp(cur_string, next_string) == 0) {
                    remove_sentence(next_sentence.pos);
                    j--; // Так как одно предложение удалилось, то если инкрементировать итератор, следующее за ним будет пропускаться.
                }
            }
        }
    }
}

void remove_even_sentences() {
    // Удаляет все четные по счету предложения в которых четное кол-во слов.
    int shift = 0;
    for (int i = 0; i < text.sentences_cnt; i++) {
        Sentence sentence = text.arr[i];
        if ((sentence.pos + 1 + shift) % 2 == 0 && sentence.word_cnt % 2 == 0) {
            remove_sentence(sentence.pos);
            shift++;
            i--;
        }
    }
}

int uppercase_cmp(const void *value1, const void *value2) {
    Word word1 = *(Word *) value1;
    Word word2 = *(Word *) value2;
    return (word1.upcase_cnt - word2.upcase_cnt);
}

void uppercase_sort() {
    for (int i = 0; i < text.sentences_cnt; i++) {
        qsort(text.arr[i].words, text.arr[i].word_cnt, sizeof(Word), uppercase_cmp);
    }
}

void less_then_three() {
    char less_then_three_str[12] = "Less Then 3";
    int new_len = 11;
    for (int i = 0; i < text.sentences_cnt; i++) {
        Sentence *sentence = &text.arr[i];
        for (int j = 0; j < sentence->word_cnt; j++) {
            Word *word = &sentence->words[j];
            if (word->len <= 3) {
                Word new_word;
                new_word.arr = calloc(12, sizeof(char));

                strncpy(new_word.arr, less_then_three_str, new_len);

                new_word.arr[new_len] = '\0';
                new_word.len = new_len;
                new_word.upcase_cnt = 2;
                new_word.ends_with_num = 0;
                new_word.starts_with_num = 0;

                sentence->symb_cnt += new_len - text.arr[i].words[j].len;
                free(word->arr);
                *word = new_word;
            }
        }
    }

}

int str_len_cmp(const void *value1, const void *value2) {
    return strlen(*(char **) value2) - strlen(*(char **) value1);
}

void max_len_word_sort() {
    char **strings = malloc(text.sentences_cnt * sizeof(char *));
    int processed_now = 0;
    int cur_len = 0;
    char *string = memory_allocate(string, cur_len, sizeof(char));
    // Заполняем массив строк, начинающихся и кончающихся цифрами.
    for (int i = 0; i < text.sentences_cnt; i++) {
        Sentence cur_sentence = text.arr[i];
        char *str_sentence = string_assemble(cur_sentence);
        int first_num_index = -1;
        int last_num_index = -1;
        for (int j = 0; j < cur_sentence.symb_cnt; j++) {
            if (isdigit(str_sentence[j])) {
                if (first_num_index == -1) {
                    first_num_index = j;
                } else {
                    last_num_index = j;
                }
            }
        }
        if (last_num_index == -1) {
            continue;
        } else {
            for (int k = first_num_index; k < last_num_index + 1; k++) {
                string[cur_len] = str_sentence[k];
                cur_len++;
                string = memory_allocate(string, cur_len, sizeof(char));
            }
        }
        string[cur_len] = '\0';
        strings[processed_now] = calloc(cur_len + 1, sizeof(char));
        strcpy(strings[processed_now], string);
        processed_now++;
        cur_len = 0;
    }
    // Сортировка массива
    qsort(strings, processed_now, sizeof(char *), str_len_cmp);
    for (int i = 0; i < processed_now; i++) {
        if (strlen(strings[i]) >= 1) {
            printf("%s. ", strings[i]);
        }
    }
}

Word word_initialization() {
    Word word;
    word.len = 0;
    word.starts_with_num = 0;
    word.ends_with_num = 0;
    word.upcase_cnt = 0;
    word.arr = NULL;
    return word;
}

Sentence sentence_initialization(int sentence_pos) {
    Sentence sentence;
    sentence.pos = sentence_pos;
    sentence.word_cnt = 0;
    sentence.symb_cnt = 0;
    return sentence;
}

void *memory_allocate(void *array, int len, int size) {
    if (len == 0) array = calloc(len + 2, size);
    else array = realloc(array, (len + 2) * size);
    return array;
}

void input_text() {
    // Фукция ввода. Алгоритм работы: посимвольно считывает поток ввода, записывает слова в структуру Word и сохраняет их
    // в массив. Если встречает точку, то создает структуру Sentence, передает ей этот массив и другие аргументы.
    // Sentence сразу записывается в массив предложений sentences_arr, а по окончании ввода - в структуру Text.
    int sentences_cnt = 0;
    Sentence *sentences_arr = memory_allocate(sentences_arr, sentences_cnt, sizeof(Sentence));

    // Создаем структуру слова, инициализируем начальные значения
    Word word = word_initialization();

    int word_cnt = 0;
    Word *words_arr = memory_allocate(words_arr, word_cnt, sizeof(Word));

    // Создаем структуру предложения, инициализируем начальные значения
    Sentence sentence = sentence_initialization(sentences_cnt);

    // Массив и переменная для запоминания местоположения запятых
    Comma_Array commas;
    commas.count = 0;
    commas.comma_positions = calloc(commas.count + 1, sizeof(int));

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
            // Запоминаем позицию запятой (порядковый номер слова в предложении, после которого идет запятая)
            commas.comma_positions = memory_allocate(commas.comma_positions, commas.count, sizeof(int));
            commas.comma_positions[commas.count] = word_cnt;
            commas.count++;
        } else if (c == ' ') {
            if (end_sentence_flag == 2) { // убирает лишний пробел после точки
                c = getchar();
                end_sentence_flag = 0;
                sentence.symb_cnt--;
                continue;
            } else {
                // Проверяем последний символ слова на соответствие цифре и записываем слово в массив слов.
                if (isdigit(word.arr[word.len - 1])) {
                    word.ends_with_num = 1;
                }

                // Записываем слово в массив слов
                word.arr[word.len] = '\0';
                words_arr = memory_allocate(words_arr, word_cnt, sizeof(Word));
                words_arr[word_cnt] = word;
                word_cnt++;
                word = word_initialization();
            }

            // Если была точка (конец предложения)
            if (end_sentence_flag == 1) {
                // Заполняем структуру предложения
                sentence.words = words_arr;
                sentence.word_cnt = word_cnt;
                sentence.pos = sentences_cnt;
                sentence.comma_indexes = commas.comma_positions;
                sentence.comma_count = commas.count;

                // Записываем в массив предложений
                sentences_arr = memory_allocate(sentences_arr, sentences_cnt, sizeof(Sentence));
                sentences_arr[sentences_cnt] = sentence;
                sentences_cnt++;

                // Обновляем переменные
                sentence = sentence_initialization(sentences_cnt);
                word_cnt = 0;
                words_arr = memory_allocate(words_arr, word_cnt, sizeof(Word));
                end_sentence_flag = 2;
                commas.count = 0;
                commas.comma_positions = calloc(commas.count + 1, sizeof(int));
            }
        } else if (c != '.') {
            // Проверяем, на какой параметр может повлиять этот символ
            if (isdigit(c) && word.len == 0) {
                word.starts_with_num = 1;
            }
            if (isupper(c)) {
                word.upcase_cnt++;
            }

            // Записываем символ в word.arr - строку.
            word.arr = memory_allocate(word.arr, word.len, sizeof(char));
            word.arr[word.len] = c;
            word.len++;
        }
        c = getchar();
    }

    text.sentences_cnt = sentences_cnt;
    text.arr = sentences_arr;

}

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <stdbool.h>
#include <Windows.h>
#include <stb_easy_font.h>
#include <GLFW/glfw3.h>
// Определение размера алфавита
#define ALPHABET_SIZE 26

int bh = 70; int bw = 70; int min_peres = 10; int peres = 0; int max_peres = 0; int best_score_min = 1000; int best_score_max = 0; int max_area = 0; int min_area = 100000;

/* ДЕРЕВО */
///////////////////////////////////////

// Определение структуры узла дерева
typedef struct TrieNode {
    struct TrieNode* children[ALPHABET_SIZE]; // массив указателей на потомков
    bool isEndOfWord; // флаг, указывающий, является ли данный узел концом слова
    char letter; //буква в узле
} TrieNode;

// Функция для создания узла дерева
TrieNode* getNode() {
    TrieNode* node = (TrieNode*)malloc(sizeof(TrieNode)); // выделение памяти под узел
    node->isEndOfWord = false; // инициализация флага
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        node->children[i] = NULL; // инициализация массива потомков
    }
    return node;
}

// Функция для вставки слова в дерево
void insertWord(TrieNode* root, const char* word) {
    TrieNode* current = root;
    int len = strlen(word);

    for (int i = 0; i < len; i++) {
        int index = word[i] - 'A'; // определение индекса потомка по букве
        if (current->children[index] == NULL) {
            current->children[index] = getNode(); // создание нового потомка, если он не существует
            current->children[index]->letter = word[i]; // установка буквы в узле
        }
        current = current->children[index]; // переход к следующему узлу

        if (i == len - 1) {
            current->isEndOfWord = true; // пометка последнего узла как конца слова
        }
    }
}

// Получение следующего слова из префиксного дерева
bool getNextWord(TrieNode* root, char* word) {
    static int currentChildIndex = 0;
    static int currentIndex = 0;

    if (currentIndex == 0) {
        currentChildIndex = 0;
    }

    while (currentChildIndex < ALPHABET_SIZE) {
        if (root->children[currentChildIndex] != NULL) {
            word[currentIndex] = 'A' + currentChildIndex;
            currentIndex++;
            TrieNode* nextNode = root->children[currentChildIndex];

            if (getNextWord(nextNode, word)) {
                return true;
            }
            else {
                currentChildIndex++;
                currentIndex--;
            }
        }
        else {
            currentChildIndex++;
        }
    }

    if (root->isEndOfWord && currentIndex > 0) {
        word[currentIndex] = '\0';
        currentIndex = 0;
        currentChildIndex = 0; // Обнуляем индексы для следующего слова
        return true;
    }

    return false;
}

//копирование одно дерева в другое
void copyTrie(TrieNode* source, TrieNode* destination) {
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (source->children[i] != NULL) {
            if (destination->children[i] == NULL) {
                destination->children[i] = (TrieNode*)malloc(sizeof(TrieNode)); // выделяем память для нового узла
            }
            destination->children[i]->letter = source->children[i]->letter; // копируем букву
            destination->children[i]->isEndOfWord = source->children[i]->isEndOfWord; // копируем флаг конца слова
            copyTrie(source->children[i], destination->children[i]); // рекурсивный вызов для детей
        }
    }
}
bool isEmpty(TrieNode* node) {
    // Проверяем наличие дочерних узлов данного узла
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i] != NULL) {
            return false;
        }
    }
    return true;
}
//удаление слова
void delete(TrieNode** currentNode, char* word, int level, int length) {
    if (*currentNode != NULL) {
        if (level == length) {
            if ((*currentNode)->isEndOfWord) {
                (*currentNode)->isEndOfWord = false;
                if (isEmpty(*currentNode)) {
                    free(*currentNode); // освобождаем память для текущего узла
                    *currentNode = NULL; // обнуляем указатель на текущий узел
                }
            }
        }
        else {
            int index = word[level] - 'A';
            delete(&(*currentNode)->children[index], word, level + 1, length);
            if (isEmpty(*currentNode) && !(*currentNode)->isEndOfWord) {
                free(*currentNode); // освобождаем память для текущего узла
                *currentNode = NULL; // обнуляем указатель на текущий узел
            }
        }
    }
}

///////////////////////////////////////

/* ВСПОМОГАТЕЛЬНЫЕ ДЛЯ ЛОГИКИ */
int findCrosswordArea(char** cross, int bh, int bw) {
    int startX = -1, startY = -1, endX = -1, endY = -1;
    for (int i = 0; i < bh; i++) {
        for (int j = 0; j < bw; j++) {
            if (cross[i][j] != ' ') {
                startX = endX = i;
                startY = endY = j;
                break;
            }
        }
        if (startX != -1) break;
    }
    for (int i = 0; i < bh; i++) {
        for (int j = 0; j < bw; j++) {
            if (cross[i][j] != ' ') {
                if (i < startX) startX = i;
                if (j < startY) startY = j;
                if (i > endX) endX = i;
                if (j > endY) endY = j;
            }
        }
    }
    int width = endY - startY + 1;
    int height = endX - startX + 1;
    return width * height;
}
void copy_board(char** board_copy, char** board, int bh, int bw) {
    for (int i = 0; i < bh; i++) {
        for (int j = 0; j < bw; j++) {
            board[i][j] = board_copy[i][j];
        }
    }
}
void free_board(char** board, int bh) {
    for (int i = 0; i < bh; i++) {
        free(board[i]);
    }
    free(board);
}
int found_index(char str[][60], int el_mas, int n) {
    for (int i = 0; i < n; i++) {
        if (strlen(str[i]) == el_mas) {
            return i;
        }
    }
    return -1;
}
int Check_Set(int pos, int len, int i_board, int j_board, char** board_copy1, int bh, int bw, char* str) {
    int gorizont = 1;
    int vertical = 1;
    int tek_pos_i = i_board - pos;
    int tek_pos_j = j_board - pos;
    int gperes = 0;
    int vperes = 0;

    if (j_board - pos + len + 1 < bw && tek_pos_j > 0 && i_board > 0 && (board_copy1[i_board][tek_pos_j + len] == ' ' && board_copy1[i_board][tek_pos_j - 1] == ' ')) {
        for (int i = 0; i < len && gorizont; i++) {
            if (board_copy1[i_board][tek_pos_j + i] == ' ' || board_copy1[i_board][tek_pos_j + i] == str[i]) { //если место куда хотим поставить очередную букву\
                                                                                пустое или совпадает с этой буквой
                if (board_copy1[i_board][tek_pos_j + i] == str[i]) { //если совпадает с буквой, то плюсуем пересечения
                    gperes++;
                }
                //проверка на слипание
                if (i != pos && !(board_copy1[i_board + 1][tek_pos_j + i] == ' ' && board_copy1[i_board - 1][tek_pos_j + i] == ' ')) {
                    if (board_copy1[i_board][tek_pos_j + i] == str[i]) {
                        gorizont = 1;
                    }
                    else {
                        gorizont = 0;
                        gperes = 0;
                    }
                }
            }
            else {
                gorizont = 0;
                gperes = 0;
            }
        }
    }
    else {
        gorizont = 0;
        gperes = 0;
    }
    if (i_board - pos + len + 1 < bh && tek_pos_i > 0 && tek_pos_j > 0 && (board_copy1[tek_pos_i + len][j_board] == ' ' && board_copy1[tek_pos_i - 1][j_board] == ' ')) {
        for (int i = 0; i < len && vertical; i++) {
            if (board_copy1[tek_pos_i + i][j_board] == ' ' || board_copy1[tek_pos_i + i][j_board] == str[i]) {
                if (board_copy1[tek_pos_i + i][j_board] == str[i]) {
                    vperes++;
                }
                if (i != pos && !(board_copy1[tek_pos_i + i][j_board + 1] == ' ' && board_copy1[tek_pos_i + i][j_board - 1] == ' ')) {
                    if (board_copy1[tek_pos_i + i][j_board] == str[i]) {
                        vertical = 1;
                    }
                    else {
                        vertical = 0;
                        vperes = 0;
                    }
                }
            }
            else {
                vertical = 0;
                vperes = 0;
            }
        }
    }
    else {
        vertical = 0;
        vperes = 0;
    }
    peres = gperes + vperes;
    if (vertical) {
        return 1;
    }
    if (gorizont) {
        return 0;
    }
    return -1;
}
int ziro_chek(int* len_of_words, int cnt) {
    for (int j = 0; j < cnt; j++) {
        if (len_of_words[j] != 0) {
            return 1;
        }
    }
    return 0;
}
int set_board(int words_count, int* len_of_words1, struct TrieNode* root, char** board_copy, int bh, int bw, int f) {

    int check = 0;

    struct TrieNode* root_copy = (TrieNode*)malloc(sizeof(TrieNode));
    /*

    функция которая копирует одно дерево в другое | из root в root_copy

    */
    copyTrie(root, root_copy); //предположительно она


    char** board_copy1 = malloc(bh * sizeof(char*));
    if (board_copy != NULL)
    {
        size_t i = 0;
        for (i; i < bw; i++) {
            board_copy1[i] = malloc(bw * sizeof(char));

            for (int j = 0; j < bw; j++) {
                board_copy1[i][j] = board_copy[i][j];
            }
        }
    }
    int crossing = 0;
    int flag = f;
    char tmp_word[50]; //строка с текущим словом
    int k = 0;
    if (flag == 0) {
        
        ////////////// ПОЧТИ ПОРАВИЛА ///////////////////////////////////////////////////////////////////////
        for (; k < words_count || root_copy != NULL; getNextWord(root, tmp_word)) {
            int tmp_i = 0;
            int tmp_j = 0;
            int orient = -1;

            int tmp_len = strlen(tmp_word);
            for (int j = 0; j < tmp_len; j++) {
                for (int i_board = 0; i_board < bh; i_board++) {
                    for (int j_board = 0; j_board < bw; j_board++) {
                        if (board_copy1[i_board][j_board] == tmp_word[j]) {
                            check = Check_Set(j, tmp_len, i_board, j_board, board_copy1, bh, bw, tmp_word);
                            int tekPeres = 0;
                            tekPeres = peres;
                            peres = 0;
                            if (tekPeres < min_peres && check >= 0) {
                                min_peres = tekPeres;
                                if (check == 1) {
                                    tmp_i = i_board - j;
                                    tmp_j = j_board;
                                    orient = 1;

                                }
                                else {
                                    tmp_i = i_board;
                                    tmp_j = j_board - j;
                                    orient = 0;
                                }
                            }
                        }
                    }
                }
            }
            if ((tmp_i != 0) && (tmp_j != 0)) {
                for (int l = 0; tmp_word[l] != NULL; l++) {
                    if (orient == 1) {
                        board_copy1[tmp_i + l][tmp_j] = tmp_word[l];
                    }
                    else {
                        board_copy1[tmp_i][tmp_j + l] = tmp_word[l];
                    }
                }
                delete(root_copy, tmp_word, 0, strlen(tmp_word)); //дописать удаление
                crossing += min_peres;
                min_peres = 10;
            }
            k++;
        }
        copy_board(board_copy1, board_copy, bh, bw);
    }
    ///////////////////////////////////////////////////////////////////////////////////
    else {
        for (; k<words_count|| root_copy != NULL; getNextWord(root, tmp_word)) {

            int tmp_i = 0;
            int tmp_j = 0;
            int orient = -1;

            int tmp_len = strlen(tmp_word);
            for (int j = 0; j < tmp_len; j++) {
                for (int i_board = 0; i_board < bh; i_board++) {
                    for (int j_board = 0; j_board < bw; j_board++) {
                        if (board_copy1[i_board][j_board] == tmp_word[j]) {
                            check = Check_Set(j, tmp_len, i_board, j_board, board_copy1, bh, bw, tmp_word);
                            int tekPeres = 0;
                            tekPeres = peres;
                            peres = 0;
                            if (tekPeres > max_peres && check >= 0) {
                                max_peres = tekPeres;
                                if (check == 1) {
                                    tmp_i = i_board - j;
                                    tmp_j = j_board;
                                    orient = 1;

                                }
                                else {
                                    tmp_i = i_board;
                                    tmp_j = j_board - j;
                                    orient = 0;
                                }
                            }
                        }
                    }
                }
            }
            if ((tmp_i != 0) && (tmp_j != 0)) {
                for (int l = 0; tmp_word[l] != NULL; l++) {
                    if (orient == 1) {
                        board_copy1[tmp_i + l][tmp_j] = tmp_word[l];
                    }
                    else {
                        board_copy1[tmp_i][tmp_j + l] = tmp_word[l];
                    }
                }
                delete(root_copy, tmp_word, 0, strlen(tmp_word)); //дописать удаление

                crossing += max_peres;
                max_peres = 0;
            }
        }
        copy_board(board_copy1, board_copy, bh, bw);
    }
    return crossing;
}

/* ПЛОЩАДИ И ПЕРЕСЕЧЕНИЯ */
int min_crossing = 1300;
int min_cross_max_area(int words_count, int* len_of_words1, char str1[][60], char** board, int bh, int bw) {
    int ind = 0;
    for (int k = 0; k < words_count; k++) {
        int area = 0;
        int score = 0;
        int len_of_words[197];
        char str[250][60];
        for (int i = 0; i < words_count; i++)
        {
            for (int j = 0; j < 60; j++)
            {
                str[i][j] = str1[i][j];
            }
        }
        for (int i = 0; i < words_count; i++) {
            len_of_words[i] = len_of_words1[i];
        }
        char** board_copy = malloc(bh * sizeof(char*));
        if (board != NULL)
        {
            size_t i = 0;
            for (i; i < bw; i++) {
                board_copy[i] = malloc(bw * sizeof(char));

                for (int j = 0; j < bw; j++) {
                    board_copy[i][j] = ' ';
                }
            }
        }
        int flag_orient = 1;
        for (int j = 0; j < len_of_words[ind]; j++) {
            board_copy[(bh - len_of_words[ind]) / 2 + j][bw / 2] = str[ind][j];
        }
        str[ind][0] = NULL;
        len_of_words[ind] = 0;
        score = set_board(words_count, len_of_words, str, board_copy, bh, bw, 0);
        area = findCrosswordArea(board_copy, bh, bw);
        if (score <= best_score_min && area > max_area) {
            best_score_min = score;
            max_area = area;
            ind++;
            copy_board(board_copy, board, bh, bw);
        }
        else {
            free_board(board_copy, bh);
            ind++;
        }
    }
    //return min_crossing;
}
int min_cross_min_area(int words_count, int* len_of_words1, char str1[][60], char** board, int bh, int bw) {
    int ind = 0;
    for (int k = 0; k < words_count; k++) {
        int area = 0;
        int score = 0;
        int len_of_words[197];
        char str[250][60];
        for (int i = 0; i < words_count; i++)
        {
            for (int j = 0; j < 60; j++)
            {
                str[i][j] = str1[i][j];
            }
        }
        for (int i = 0; i < words_count; i++) {
            len_of_words[i] = len_of_words1[i];
        }
        char** board_copy = malloc(bh * sizeof(char*));
        if (board != NULL)
        {
            size_t i = 0;
            for (i; i < bw; i++) {
                board_copy[i] = malloc(bw * sizeof(char));

                for (int j = 0; j < bw; j++) {
                    board_copy[i][j] = ' ';
                }
            }
        }

        int flag_orient = 1;

        for (int j = 0; j < len_of_words[ind]; j++) {
            if (flag_orient == 0) {
                board_copy[bh / 2][(bw - len_of_words[ind]) / 2 + j] = str[ind][j];
            }
            else {
                board_copy[(bh - len_of_words[ind]) / 2 + j][bw / 2] = str[ind][j];
            }
        }
        str[ind][0] = NULL;
        len_of_words[ind] = 0;
        score = set_board(words_count, len_of_words, str, board_copy, bh, bw, 0);
        area = findCrosswordArea(board_copy, bh, bw);
        if (score <= best_score_min && area < min_area) {
            if (score >= words_count / 2) {
                best_score_min = score;
                min_area = area;
                ind++;
                copy_board(board_copy, board, bh, bw);
            }
        }
        else {
            free_board(board_copy, bh);
            ind++;
        }
    }
}
int max_crossing = 0;
int max_cross_max_area(int words_count, int* len_of_words1, char str1[][60], char** board, int bh, int bw) {
    int ind = 0;
    for (int k = 0; k < words_count; k++) {
        int area = 0;
        int score = 0;
        int len_of_words[197];
        char str[250][60];
        for (int i = 0; i < words_count; i++)
        {
            for (int j = 0; j < 60; j++)
            {
                str[i][j] = str1[i][j];
            }
        }
        for (int i = 0; i < words_count; i++) {
            len_of_words[i] = len_of_words1[i];
        }
        char** board_copy = malloc(bh * sizeof(char*));
        if (board != NULL)
        {
            size_t i = 0;
            for (i; i < bw; i++) {
                board_copy[i] = malloc(bw * sizeof(char));

                for (int j = 0; j < bw; j++) {
                    board_copy[i][j] = ' ';
                }
            }
        }
        int flag_orient = 1;
        for (int j = 0; j < len_of_words[ind]; j++) {
            if (flag_orient == 0) {
                board_copy[bh / 2][(bw - len_of_words[ind]) / 2 + j] = str[ind][j];
            }
            else {
                board_copy[(bh - len_of_words[ind]) / 2 + j][bw / 2] = str[ind][j];
            }
        }
        str[ind][0] = NULL;
        len_of_words[ind] = 0;

        score = set_board(words_count, len_of_words, str, board_copy, bh, bw, 1);
        area = findCrosswordArea(board_copy, bh, bw);
        if (score >= best_score_max && area > max_area) {
            best_score_max = score;
            max_area = area;
            ind++;
            copy_board(board_copy, board, bh, bw);
        }
        else {
            free_board(board_copy, bh);
            ind++;
        }
    }
}
int max_cross_min_area(int words_count, int* len_of_words1, char str1[][60], char** board, int bh, int bw) {
    int ind = 0;
    for (int k = 0; k < words_count; k++) {
        int area = 0;
        int score = 0;
        int len_of_words[197];
        char str[250][60];
        for (int i = 0; i < words_count; i++)
        {
            for (int j = 0; j < 60; j++)
            {
                str[i][j] = str1[i][j];
            }
        }
        for (int i = 0; i < words_count; i++) {
            len_of_words[i] = len_of_words1[i];
        }
        char** board_copy = malloc(bh * sizeof(char*));
        if (board != NULL)
        {
            size_t i = 0;
            for (i; i < bw; i++) {
                board_copy[i] = malloc(bw * sizeof(char));

                for (int j = 0; j < bw; j++) {
                    board_copy[i][j] = ' ';
                }
            }
        }

        int flag_orient = 1;

        for (int j = 0; j < len_of_words[ind]; j++) {
            if (flag_orient == 0) {
                board_copy[bh / 2][(bw - len_of_words[ind]) / 2 + j] = str[ind][j];
            }
            else {
                board_copy[(bh - len_of_words[ind]) / 2 + j][bw / 2] = str[ind][j];
            }
        }
        str[ind][0] = NULL;
        len_of_words[ind] = 0;

        score = set_board(words_count, len_of_words, str, board_copy, bh, bw, 1);
        area = findCrosswordArea(board_copy, bh, bw);
        if (score >= best_score_max && area < min_area) {
            best_score_max = score;
            min_area = area;
            ind++;
            copy_board(board_copy, board, bh, bw);
        }
        else {
            free_board(board_copy, bh);
            ind++;
        }
    }
}

/* МИНИМАЛЬНЫЕ ПЕРЕСЕЧЕНИЯ */
int find_min_crossing(int words_count, int* len_of_words1, char str1[][60], char** board, int bh, int bw, int flag) {
    //int ind = 0;
    if (flag == 1) {

        min_cross_max_area(words_count, len_of_words1, str1, board, bh, bw);

    }
    else {

        min_cross_min_area(words_count, len_of_words1, str1, board, bh, bw);

    }
}
/* МАКСИМАЛЬНЫЕ ПЕРЕСЕЧЕНИЯ */
int find_max_crossing(int words_count, int* len_of_words1, char str1[][60], char** board, int bh, int bw, int flag) {
    int ind = 0;
    if (flag == 1) {

        max_cross_max_area(words_count, len_of_words1, str1, board, bh, bw);

    }
    else {

        max_cross_min_area(words_count, len_of_words1, str1, board, bh, bw);
    }
}

void print_string(float x, float y, char* text, float r, float g, float b)
{
    static char buffer[99999];
    int num_quads;

    num_quads = stb_easy_font_print(x, y, text, NULL, buffer, sizeof(buffer));

    glColor3f(r, g, b);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 16, buffer);
    glDrawArrays(GL_QUADS, 0, num_quads * 4);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void Colorize(int x, int y, char* symbol) {
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(0.88, 0.88, 0.88); glVertex2f(0, 1);
    glColor3f(0.88, 0.88, 0.88); glVertex2f(1, 1); glVertex2f(0, 0);
    glColor3f(0.88, 0.30, 0.88); glVertex2f(1, 0);
    glEnd();
    glPushMatrix();
    glTranslatef(-x * 0.09, (y + 2) * 0.09, 0);
    glScalef(0.09, -0.09, 1);
    print_string(x + 3, y - 7, symbol, 0.7, 0, 2);
    glPopMatrix();
}

void print_Board(char** board2, char** board, int bh, int bw) {
    for (int i = 0; i < bh; i++) {
        for (int j = 0; j < bw; j++) {
            board2[j][bh - i + 1] = board[i][j];
        }
    }
    glLoadIdentity();
    glScalef(3.0 / bw, 3.0 / bh, 1);
    glTranslatef(-bw * 0.7, -bh * 0.4, 0);
    for (int j = 0; j < bh; j++) {
        for (int i = 0; i < bw; i++) {
            char symbol[2];
            symbol[0] = board2[i][j];
            symbol[1] = '\0';
            glPushMatrix();
            glTranslatef(i, j, 0);

            if (board2[i][j] != ' ') {
                Colorize(i, j, symbol);
            }
            glPopMatrix();
        }
    }
}

void printWords(TrieNode* root, char* buffer, int depth) {
    if (root->isEndOfWord) {
        buffer[depth] = '\0';
        if (depth > 0) {
            printf("%s - %d\n", buffer, depth);
        }
    }
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (root->children[i] != NULL) {
            buffer[depth] = root->children[i]->letter;
            printWords(root->children[i], buffer, depth + 1);
            buffer[depth] = root->letter; // Вернуть верное значение в buffer[depth] после рекурсивного вызова
        }
    }
}

int main() {
    setlocale(0, "Russian");
    srand((unsigned)time(NULL));

    int WordsCount = 1, max_len = 0, min_len = 100000;
    char tmp[90];
    char str[250][60];
    FILE* file;
    file = fopen("cross.txt", "r");

    if (!file) {
        printf("ERROR: read from file.");
        return 0;
    }

    char c;
    while (!feof(file)) {
        c = fgetc(file);
        if (c == '\n')
            WordsCount++;
    }
    rewind(file);
    char wrd;

    /* заполнение словаря */
    TrieNode* root = getNode(); // создание корневого узла
    while (fgets(tmp, sizeof(tmp), file) != NULL) {
        int len_of_tmp = strlen(tmp);
        if (tmp[len_of_tmp - 1] == '\n') {
            tmp[len_of_tmp - 1] = '\0';
        }
        else {
            tmp[len_of_tmp] = '\0';
        }
        _strupr(tmp);
        insertWord(root, tmp); // вставка слова в префиксное дерево
    }
    /*char* buf[256];
    printWords(root, buf, 0);*/

    /* высчитывание длины каждого из слов в словаре */


    // УДАЛИТЬ /////////////////////////////////
    int* len_of_words;
    len_of_words = malloc(WordsCount * sizeof(int));
    for (int i = 0; i < WordsCount; i++) {
        len_of_words[i] = strlen(str[i]);
    }
    for (int i = 0; i < WordsCount; i++) {
        if (len_of_words[i] > max_len) {
            max_len = len_of_words[i];
        }
        if (len_of_words[i] < min_len) {
            min_len = len_of_words[i];
        }
    }
    /////////////////////////////////////////////////////

    char** board = malloc(bh * sizeof(char*));
    if (board != NULL)
    {
        size_t i = 0;
        for (i; i < bh; i++) {
            board[i] = malloc(bw * sizeof(char));

            for (int j = 0; j < bw; j++) {
                board[i][j] = ' ';
            }
        }
    }
    char** board2 = malloc(bw * sizeof(char*));
    if (board2 != NULL)
    {
        size_t i = 0;
        for (i; i < bw; i++) {
            board2[i] = malloc(bh * sizeof(char));

            for (int j = 0; j < bh; j++) {
                board2[i][j] = ' ';
            }
        }
    }
    int peres = 0;
    printf("Choose the number of interections (max/min): ");
    char inter[4];
    scanf("%s", &inter);

    printf("Choose the area of the crossword puzzle (max/min): ");
    char area[4];
    scanf("%s", &area);
    clock_t start = clock();


    if (inter[0] == 'm' && inter[1] == 'i' && inter[2] == 'n') {
        if (area[0] == 'm' && area[1] == 'a' && area[2] == 'x') {
            find_min_crossing(WordsCount, len_of_words, str, board, bh, bw, 1);
            printf("Count crossings: %d\n", best_score_min);
            printf("Puzzle size: %d\n", findCrosswordArea(board, bh, bw));
        }
        if (area[0] == 'm' && area[1] == 'i' && area[2] == 'n') {
            find_min_crossing(WordsCount, len_of_words, str, board, bh, bw, 0);
            printf("Count crossings: %d\n", best_score_min);
            printf("Puzzle size: %d\n", findCrosswordArea(board, bh, bw));
        }
    }
    if (inter[0] == 'm' && inter[1] == 'a' && inter[2] == 'x') {
        if (area[0] == 'm' && area[1] == 'a' && area[2] == 'x') {
            find_max_crossing(WordsCount, len_of_words, str, board, bh, bw, 1);
            printf("Count crossings: %d\n", best_score_max);
            printf("Puzzle size: %d\n", findCrosswordArea(board, bh, bw));
        }
        if (area[0] == 'm' && area[1] == 'i' && area[2] == 'n') {
            find_max_crossing(WordsCount, len_of_words, str, board, bh, bw, 0);
            printf("Count crossings: %d\n", best_score_max);
            printf("Puzzle size: %d\n", findCrosswordArea(board, bh, bw));
        }
    }
    clock_t end = clock();
    double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Время выполнения программы: %f секунд\n", cpu_time_used);
    GLFWwindow* window;
    if (!glfwInit())
        return -1;
    window = glfwCreateWindow(1100, 1100, "generator", NULL, NULL);
    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glClearColor(0.7f, 1.0f, 0.7f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        print_Board(board2, board, bh, bw);
        glfwSwapBuffers(window);
    }
    glfwTerminate();


    getch();
}
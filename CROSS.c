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

FILE* file;

/* ДЕРЕВО */
///////////////////////////////////////

// Структура узла бинарного дерева
typedef struct TrieNode {
    char word[20];  // Слово из словаря
    struct TrieNode* left;  // Левый потомок узла
    struct TrieNode* right;  // Правый потомок узла
    struct TrieNode* parent;  // Родитель узла
}TrieNode;

TrieNode* copyTree(TrieNode* from);

// Функция для создания нового узла
struct TrieNode* createNode(char word[]) {
    struct TrieNode* newNode = (struct TrieNode*)malloc(sizeof(struct TrieNode));
    strcpy(newNode->word, word);
    newNode->left = newNode->right = newNode->parent = NULL;
    return newNode;
}

// Функция для вставки слова в бинарное дерево
struct TrieNode* insert(struct TrieNode* node, char word[]) {
    // Если дерево пустое, вставляем новый узел
    if (node == NULL)
        return createNode(word);

    // Если слово меньше текущего узла, рекурсивно ищем место в левом поддереве
    if (strcmp(word, node->word) < 0) {
        node->left = insert(node->left, word);
        node->left->parent = node;
    }
    // Если слово больше текущего узла, рекурсивно ищем место в правом поддереве
    else if (strcmp(word, node->word) > 0) {
        node->right = insert(node->right, word);
        node->right->parent = node;
    }
    // Если слово уже существует, ничего не делаем

    return node;
}

// Функция для поиска слова в бинарном дереве
struct TrieNode* search(struct TrieNode* node, char word[]) {
    // Если дерево пустое или текущий узел содержит нужное слово
    if (node == NULL || strcmp(node->word, word) == 0)
        return node;

    // Если слово меньше текущего узла, ищем в левом поддереве
    if (strcmp(word, node->word) < 0)
        return search(node->left, word);

    // Если слово больше текущего узла, ищем в правом поддереве
    else if (strcmp(word, node->word) > 0)
        return search(node->right, word);
}

// Функция для получения следующего слова из бинарного дерева
// Функция для обхода бинарного дерева в порядке возрастания
void inorderTraversal(struct TrieNode* root) {
    if (root == NULL) {
        return;
    }
    inorderTraversal(root->left);
    printf("%s\n", root->word);
    inorderTraversal(root->right);
}
// Функция для поиска слова, следующего за указанным в бинарном дереве
char* findNextWord(TrieNode* root, const char* target) {
    TrieNode* current = root;
    TrieNode* next = NULL;

    // Находим узел с заданным словом
    while (current != NULL && strcmp(current->word, target) != 0) {
        if (strcmp(target, current->word) < 0) {
            next = current;
            current = current->left;
        }
        else {
            current = current->right;
        }
    }

    if (current == NULL) {
        return NULL;  // Слово не найдено
    }

    // Если правое поддерево существует, то следующим будет слово, наименьшее в этом поддереве
    if (current->right != NULL) {
        next = current->right;
        while (next->left != NULL) {
            next = next->left;
        }
        return next->word;
    }
    if (current->right == NULL && current->left == NULL) {
        return next->word;
    }
    if (current->right == NULL) {
        // В противном случае, следующим будет самый низкий родитель, у которого текущее слово находится в левом поддереве
        TrieNode* parent = current->parent;
        while (parent != NULL && current == parent->right) {
            current = parent;
            parent = parent->parent;
        }
        if (parent != NULL) {
            return parent->word;
        }
        else {
            return NULL;  // Это слово было последним в алфавитном порядке
        }
    }
}

TrieNode* deleteNode(TrieNode* root, char* wrd) {
    if (root == NULL) {
        return;
    }
    // Находим узел с заданным словом

    int cmp = strcmp(wrd, root->word);
    if (cmp == 0) {
        // проверка на равенство даты события, которое нужно удалить, с датой события текущего узла.Если даты равны, то удаляем событие.
        TrieNode* temp;
        if (root->left == NULL && root->right == NULL) {
            // Проверяем, является ли удаляемый узел листом (удаляем, если да)
            temp = root;
            free(root);
            root = NULL;
            return temp->parent;
        }
        else if (root->left == NULL) {
            // Если удаляемый узел имеет только правого потомка
            temp = root;
            root = root->right; //меняем указатель на корень на указатель правого потомка
            free(temp);
            return root;
        }
        else if (root->right == NULL) {
            // Если удаляемый узел имеет только левого потомка
            temp = root;
            root = root->left; //меняем указатель на корень на указатель левого потомка
            free(temp);
            return root;
        }
        else {
            // Если удаляемый узел имеет и левого, и правого потомков
            // Ищем самый правый узел в левом поддереве для замены удаляемого узла
            temp = root->left;
            while (temp->right != NULL) {
                temp = temp->right;
            }
            // Копируем данные заменяющего узла в удаляемый узел
            strcpy(root->word, temp->word);
            // удаляем самый правый узел в левом поддереве
            deleteNode(root->left, temp->word);
        }
    }
    else if (cmp < 0) {
        deleteNode(root->left, wrd);
    }
    else {
        deleteNode(root->right, wrd);
    }
}
TrieNode* copyTree(TrieNode* from) {
    if (from == NULL) {
        return NULL;
    }
    else {
        TrieNode* newTree = (TrieNode*)malloc(sizeof(TrieNode));
        if (from->word == NULL) {
            if (from->left->word != NULL) {
                newTree->left = copyTree(from->left);
            }
            else {
                newTree->right = copyTree(from->right);
            }
        }
        else {
            strcpy(newTree->word, from->word);
            if (from->parent == NULL) newTree->parent = NULL;
            newTree->left = copyTree(from->left);
            if (newTree->left != NULL) {
                newTree->left->parent = newTree;
            }
            newTree->right = copyTree(from->right);
            if (newTree->right != NULL) {
                newTree->right->parent = newTree;
            }

            return newTree;

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
int set_board(int words_count, struct TrieNode* root, char** board_copy, int bh, int bw, int f) {

    int check = 0;

    TrieNode* root_copy = NULL;
    root_copy = copyTree(root);

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
    char* tmp_word; //строка с текущим словом
    int k = 0;
    if (flag == 0) {

        ////////////// ПОЧТИ ПОРАВИЛА ///////////////////////////////////////////////////////////////////////
        for (tmp_word = root_copy->word; k < words_count; tmp_word = findNextWord(root_copy, tmp_word)) {
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
                deleteNode(root_copy, tmp_word);
                crossing += min_peres;
                min_peres = 10;
            }
            k++;
        }
        copy_board(board_copy1, board_copy, bh, bw);
    }
    ///////////////////////////////////////////////////////////////////////////////////
    else {
        for (tmp_word = root_copy->word; k < words_count; tmp_word = findNextWord(root_copy, tmp_word)) {

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
                deleteNode(root_copy, tmp_word);

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
int min_cross_max_area(int words_count, TrieNode* root, char** board, int bh, int bw) {
    int ind = 0;
    TrieNode* tmp_root = NULL;
    tmp_root = copyTree(root);
    char* tmp_word;
    tmp_word = &(tmp_root->word);
    for (int k = 0; k < words_count; k++, tmp_word = findNextWord(tmp_root, tmp_word)) {
        int area = 0;
        int score = 0;
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
        //tmp_word = findNextWord(tmp_root, tmp_word);
        for (int j = 0; j < strlen(tmp_word); j++) {
            board_copy[(bh - strlen(tmp_word)) / 2 + j][bw / 2] = tmp_word[j];
        }
        ////////////////TROUBLE/////////////////////////////////////////////////

        char wrd_for_delete[100];
        strcpy(wrd_for_delete, tmp_word);
        tmp_root = deleteNode(tmp_root, wrd_for_delete);
        strcpy(tmp_word, wrd_for_delete);

        /*РЕШИТЬ ПРОБЛЕМУ:
        слово tmp_word удаляется (так и должно быть), но мы должны взять следующее слово за текущим tmp.
        можно оставить эту же конструкцию, но перезаписать wrd_for_delete в tmp
        */

        /////////////////////////////////////////////////////////////////////////
        score = set_board(words_count, tmp_root, board_copy, bh, bw, 0);
        area = findCrosswordArea(board_copy, bh, bw);
        if (score <= best_score_min && area > max_area) {
            best_score_min = score;
            max_area = area;
            copy_board(board_copy, board, bh, bw);
        }
        else {
            free_board(board_copy, bh);
        }
    }
    //return min_crossing;
}
int min_cross_min_area(int words_count, TrieNode* root, char** board, int bh, int bw) {
    int ind = 0;

    TrieNode* tmp_root = NULL;
    tmp_root = copyTree(root);
    char* tmp_word;
    tmp_word = tmp_root->word;
    for (int k = 0; k < words_count; k++, tmp_word = findNextWord(tmp_root, tmp_word)) {
        int area = 0;
        int score = 0;
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
        for (int j = 0; j < strlen(tmp_word); j++) {
            board_copy[(bh - strlen(tmp_word)) / 2 + j][bw / 2] = tmp_word[j];
        }
        deleteNode(tmp_root, tmp_word);
        score = set_board(words_count, tmp_root, board_copy, bh, bw, 0);
        area = findCrosswordArea(board_copy, bh, bw);
        if (score <= best_score_min && area < min_area) {
            if (score >= words_count / 2) {
                best_score_min = score;
                min_area = area;
                copy_board(board_copy, board, bh, bw);
            }
        }
        else {
            free_board(board_copy, bh);
        }
    }
}
int max_crossing = 0;
int max_cross_max_area(int words_count, TrieNode* root, char** board, int bh, int bw) {
    int ind = 0;

    TrieNode* tmp_root = NULL;
    tmp_root = copyTree(root);
    char* tmp_word;
    tmp_word = tmp_root->word;
    for (int k = 0; k < words_count; k++, tmp_word = findNextWord(tmp_root, tmp_word)) {
        int area = 0;
        int score = 0;
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
        for (int j = 0; j < strlen(tmp_word); j++) {
            board_copy[(bh - strlen(tmp_word)) / 2 + j][bw / 2] = tmp_word[j];
        }
        deleteNode(tmp_root, tmp_word);
        score = set_board(words_count, tmp_root, board_copy, bh, bw, 0);
        area = findCrosswordArea(board_copy, bh, bw);
        if (score >= best_score_max && area > max_area) {
            best_score_max = score;
            max_area = area;
            copy_board(board_copy, board, bh, bw);
        }
        else {
            free_board(board_copy, bh);
        }
    }
}
int max_cross_min_area(int words_count, TrieNode* root, char** board, int bh, int bw) {
    int ind = 0;

    TrieNode* tmp_root = NULL;
    tmp_root = copyTree(root);
    char* tmp_word;
    tmp_word = tmp_root->word;
    for (int k = 0; k < words_count; k++, tmp_word = findNextWord(tmp_root, tmp_word)) {
        int area = 0;
        int score = 0;
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
        for (int j = 0; j < strlen(tmp_word); j++) {
            board_copy[(bh - strlen(tmp_word)) / 2 + j][bw / 2] = tmp_word[j];
        }
        deleteNode(tmp_root, tmp_word);
        score = set_board(words_count, tmp_root, board_copy, bh, bw, 0);
        area = findCrosswordArea(board_copy, bh, bw);
        if (score >= best_score_max && area < min_area) {
            best_score_max = score;
            min_area = area;
            copy_board(board_copy, board, bh, bw);
        }
        else {
            free_board(board_copy, bh);
        }
    }
}

/* МИНИМАЛЬНЫЕ ПЕРЕСЕЧЕНИЯ */
int find_min_crossing(int words_count, TrieNode* root, char** board, int bh, int bw, int flag) {
    //int ind = 0;
    if (flag == 1) {

        min_cross_max_area(words_count, root, board, bh, bw);

    }
    else {

        min_cross_min_area(words_count, root, board, bh, bw);

    }
}
/* МАКСИМАЛЬНЫЕ ПЕРЕСЕЧЕНИЯ */
int find_max_crossing(int words_count, TrieNode* root, char** board, int bh, int bw, int flag) {
    int ind = 0;
    if (flag == 1) {

        max_cross_max_area(words_count, root, board, bh, bw);

    }
    else {

        max_cross_min_area(words_count, root, board, bh, bw);
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


int main() {
    FILE* file;
    setlocale(0, "Russian");
    srand((unsigned)time(NULL));

    int WordsCount = 1, max_len = 0, min_len = 100000;
    char tmp[90];

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
    TrieNode* root = NULL; // создание корневого узла
    while (fgets(tmp, sizeof(tmp), file) != NULL) {
        int len_of_tmp = strlen(tmp);
        if (tmp[len_of_tmp - 1] == '\n') {
            tmp[len_of_tmp - 1] = '\0';
        }
        else {
            tmp[len_of_tmp] = '\0';
        }
        _strupr(tmp);
        root = insert(root, tmp); // вставка слова в префиксное дерево
    }
    root->parent = NULL;
    rewind(file);
    /*char* buf[256];
    printWords(root, buf, 0);*/

    /* высчитывание длины каждого из слов в словаре */

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
            find_min_crossing(WordsCount, root, board, bh, bw, 1);
            printf("Count crossings: %d\n", best_score_min);
            printf("Puzzle size: %d\n", findCrosswordArea(board, bh, bw));
        }
        if (area[0] == 'm' && area[1] == 'i' && area[2] == 'n') {
            find_min_crossing(WordsCount, root, board, bh, bw, 0);
            printf("Count crossings: %d\n", best_score_min);
            printf("Puzzle size: %d\n", findCrosswordArea(board, bh, bw));
        }
    }
    if (inter[0] == 'm' && inter[1] == 'a' && inter[2] == 'x') {
        if (area[0] == 'm' && area[1] == 'a' && area[2] == 'x') {
            find_max_crossing(WordsCount, root, board, bh, bw, 1);
            printf("Count crossings: %d\n", best_score_max);
            printf("Puzzle size: %d\n", findCrosswordArea(board, bh, bw));
        }
        if (area[0] == 'm' && area[1] == 'i' && area[2] == 'n') {
            find_max_crossing(WordsCount, root, board, bh, bw, 0);
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
/*NEGOITA Andreea-Teodora - 313CD*/
#pragma once

 typedef struct page page;
 typedef struct tab tab;
 typedef struct browser browser;
 typedef struct stack stack;
typedef struct TabList TabList;

#define MAX_URL 50
#define MAX_DESCRIPTION 100

struct page {
    int id; //ID
    char url[MAX_URL]; // URL
    char *description; //Descriere
};

struct stack {
    struct page *currentPage; // Pagina curenta
    struct stack *next; // Elementul urmator
};

struct tab {
    int id; //ID
    page *currentPage; // Pagina curenta
    struct stack *backwardStack; // Stiva BACKWARD
    struct stack *forwardStack; // Stiva FORWARD
    struct tab *next; // Elementul urmator
    struct tab *prev; // Elementul anterior
};

struct TabList {
    struct tab *sentinel;
};

struct browser {
    struct tab *current; // Tabul curent
    struct TabList list; // Lista de taburi
    int nextID; // ID-ul urmator
};



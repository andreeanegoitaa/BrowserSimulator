/*NEGOITA Andreea-Teodora - 313CD*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "structuri.h"

void FreePage(page *p) {
    if (p != NULL) {
        free(p->description);
        free(p);
    }
}

void FreeStack(stack *s) {
    while (s) {
        stack *temp = s;
        s = s->next;
        FreePage(temp->currentPage);
        free(temp);
    }
}

void FreeTab(tab *t) {
    if (t != NULL) {
        FreePage(t->currentPage);
        FreeStack(t->backwardStack);
        FreeStack(t->forwardStack);
        free(t);
    }
}

void FreeTabList(tab *sentinel) {
    tab *current = sentinel->next;

    while (current != sentinel) {
        tab *temp = current;
        current = current->next;
        FreeTab(temp);
    }

    free(sentinel);
}

void FreeBrowser(browser *b) {
    if (b != NULL) {
        FreeTabList(b->list.sentinel);
    }
}

tab* CreateTab (int id) {
    tab *newTab = ( tab *)malloc(sizeof(tab));
    if (newTab == NULL) {
        return NULL;
    }

    newTab->id = id;
    newTab->backwardStack = NULL;
    newTab->forwardStack = NULL;

    newTab->currentPage = (page *)malloc(sizeof(page));
    if (newTab->currentPage == NULL) {
        free(newTab);
        return NULL;
    }

    // Initializare pagina implicita
    newTab->currentPage->id = 0;
    strcpy(newTab->currentPage->url, "https://acs.pub.ro/");

    newTab->currentPage->description = (char *)malloc(MAX_URL * sizeof(char));
    if (newTab->currentPage->description == NULL) {
        free(newTab->currentPage);
        free(newTab);
        return NULL;
    }

    strcpy(newTab->currentPage->description, "Computer Science");

    newTab->next = NULL;
    newTab->prev = NULL;

    return newTab;
}

stack *PushPage(stack *top, page *p) {
    stack *newNode = (stack *)malloc(sizeof(stack));
    if (newNode == NULL) {
        return top;
    }

    // Fac o copie a paginii sa nu fac double free
    page *copy = malloc(sizeof(page));
    if (copy == NULL) {
        free(newNode);
        return top;
    }
    copy->id = p->id;
    strcpy(copy->url, p->url);
    copy->description = (char *)malloc(MAX_DESCRIPTION * sizeof(char));
    if (copy->description == NULL) {
        free(copy);
        free(newNode);
        return top;
    }

    strcpy(copy->description, p->description);

    newNode->currentPage = copy;
    newNode->next = top;

    return newNode;
}

page *ReadPage (FILE *fin) {
    page *p = (page *)malloc(sizeof(page));
    if (p == NULL) {
        return NULL;
    }

    // ID
    fscanf(fin, "%d", &p->id);
    fgetc(fin); // pentru '/n'

    // URL
    fgets(p->url, MAX_URL, fin);
    p->url[strcspn(p->url, "\n")] = 0;

    // DESCRIERE
    p->description = (char *)malloc(MAX_DESCRIPTION * sizeof(char));
    if (p->description == NULL) {
        free(p);
        return NULL;
    }

    fgets(p->description, MAX_DESCRIPTION, fin);

    p->description[strcspn(p->description, "\n")] = 0;

    return p;

}

stack *ReadAllPages(FILE *fin, int page_nr) {
    stack *top = NULL;

    for (int i = 0; i < page_nr; i++) {
        page *p = ReadPage(fin);
        if (p == NULL) {
            return NULL;
        }

        top = PushPage(top, p);

        free(p->description);
        free(p);
        p = NULL;
    }

    return top;
}

page *Cautare(stack *pages, int id) {
    while (pages) {
        if (pages->currentPage->id == id) {
            return pages->currentPage;
        }
        pages = pages->next;
    }

    return NULL;
}

void NEW_TAB(browser *b) {
    int id = b->nextID;
    b->nextID++;

    tab *newTab = CreateTab(id);
    if (newTab == NULL) {
        return;
    }

    tab *last = b->list.sentinel->prev;

    newTab->next = b->list.sentinel;
    newTab->prev = last;
    last->next = newTab;
    b->list.sentinel->prev = newTab;

    b->current = newTab;
}

void CLOSE(browser *b, FILE *fout) {
    if (b == NULL || b->current == NULL || b->list.sentinel == NULL) {
        return;
    }

    tab *deleteTab = b->current;
    
    // Verific daca tab-ul curent are ID 0 (nu trebuie sa fie inchis)
    if (deleteTab->id == 0) {
        fprintf(fout, "403 Forbidden\n");
        return;
    }

    // Setez tab-ul curent la cel din stanga
    b->current = deleteTab->prev;
    
    // Elimin tab-ul din lista
    deleteTab->prev->next = deleteTab->next;
    deleteTab->next->prev = deleteTab->prev;

    // Eliberez memoria pentru pagina curenta
    if (deleteTab->currentPage) {
        if (deleteTab->currentPage->description) {
            free(deleteTab->currentPage->description);
        }
        free(deleteTab->currentPage);
    }

    // Eliberez memoria stivelor
    stack *tmp;
    //BACKWARD STACK
    while (deleteTab->backwardStack) {
        tmp = deleteTab->backwardStack;
        deleteTab->backwardStack = deleteTab->backwardStack->next;

        if (tmp->currentPage) {
            if (tmp->currentPage->description) {
                free(tmp->currentPage->description);
                free(tmp->currentPage);
            }
        }

        free(tmp);
    }

    //FORWARD STACK
    while (deleteTab->forwardStack) {
        tmp = deleteTab->forwardStack;
        deleteTab->forwardStack = deleteTab->forwardStack->next;

        if (tmp->currentPage) {
            if (tmp->currentPage->description) {
                free(tmp->currentPage->description);
                free(tmp->currentPage);
            }
        }

        free(tmp);
    }

    // Eliberez memoria pentru tab-ul curent
    free(deleteTab);
}

void OPEN_ID (browser *b, int id, FILE *fout) {
    //Verific daca tab-ul curent este valid
    if (b == NULL || b->current == NULL || b->list.sentinel == NULL) {
        return;
    }
    //E valid => Parcurg lista de tab-uri (incep de la primul tab dupa sentinel)
    tab *iter = b->list.sentinel->next;
    while (iter != b->list.sentinel) {
        if (iter->id == id) {   //Am gasit tab-ul cu ID-ul dat => Il setez ca tab curent
            b->current = iter;
            return;
        }
        iter = iter->next;
    }

    // Daca nu am gasit tab-ul cu ID-ul dat, afisez mesajul de eroare
    fprintf(fout, "403 Forbidden\n");
}

void PRINT(browser *b, FILE *fout) {
    if (b == NULL || b->current == NULL || b->list.sentinel == NULL) {
        return;
    }

    tab *start = b->current;
    tab *iter = start;

    // Parcurg lista de taburi si afisez ID-urile
    do {
        if (iter != b->list.sentinel) {
        fprintf(fout, "%d ", iter->id);
        }
        iter = iter->next;
    } while (iter != start);

    fprintf(fout, "\n");

    // Afisez descrierea paginii curente (daca este o pagina curenta)
    if(b->current->currentPage && b->current->currentPage->description) {
        fprintf(fout, "%s\n", b->current->currentPage->description);
    } 
}

void NEXT (browser *b) {
    if (b == NULL || b->current == NULL || b->list.sentinel == NULL) {
        return;
    }

   tab *nextTab = b->current->next;

   if (nextTab == b->list.sentinel) {
        nextTab = nextTab->next;
   }

   b->current = nextTab;
}

void PREV (browser *b) {
    if (b == NULL || b->current == NULL || b->list.sentinel == NULL) {
        return;
    }

   tab *prevTab = b->current->prev;

   if (prevTab == b->list.sentinel) {
        prevTab = prevTab->prev;
   }

   b->current = prevTab;
}

page *CopyPage(page *src) {
    if (!src) return NULL;

    page *copy = malloc(sizeof(page));
    if (!copy) return NULL;

    copy->id = src->id;

    // Copiere URL
    strncpy(copy->url, src->url, MAX_URL);
    copy->url[MAX_URL - 1] = '\0';

    // Copiere descriere dacă există
    if (src->description != NULL) {
        copy->description = malloc(MAX_DESCRIPTION);
        if (copy->description == NULL) {
            free(copy);
            return NULL;
        }
        strncpy(copy->description, src->description, MAX_DESCRIPTION);
        copy->description[MAX_DESCRIPTION - 1] = '\0';
    } else {
        copy->description = NULL;
    }

    return copy;
}

void PAGE_ID(browser *b, stack *pages, int id, FILE *fout) {
    if (b == NULL || b->current == NULL) {
        return;
    }

    // Verific daca exista pagina cu ID-ul dat
    page *p = Cautare(pages, id);
    if (p == NULL) {
        fprintf(fout, "403 Forbidden\n");
        return;
    }

    // Salvez pagina curenta in stiva backward
    stack *back_node = (stack *)malloc(sizeof(stack));
    if (back_node == NULL) {
        return;
    }

    back_node->currentPage = CopyPage(b->current->currentPage);
    back_node->next = b->current->backwardStack;
    b->current->backwardStack = back_node;

    //Eliberez stiva forward
    stack *tmp;
    while (b->current->forwardStack) {
        tmp = b->current->forwardStack;
        b->current->forwardStack = b->current->forwardStack->next;
        FreePage(tmp->currentPage);
        free(tmp);
    }

      FreePage(b->current->currentPage);
    b->current->currentPage = CopyPage(p);
}

void BACKWARD (browser *b, FILE *fout) {
    if (b == NULL || b->current == NULL || b->current->backwardStack == NULL) {
        fprintf(fout, "403 Forbidden\n");
        return;
    }

    // Retin prima pagina din stiva backward
    stack *backwardTOP = b->current->backwardStack;

    // Pun pagina curenta in stiva forward
    stack *forwardNODE = (stack *)malloc(sizeof(stack));
    if (forwardNODE == NULL) {
        return;
    }

    forwardNODE->currentPage = CopyPage(b->current->currentPage);
    forwardNODE->next = b->current->forwardStack;
    b->current->forwardStack = forwardNODE;

    // Actualizez pagina curenta cu cea din stiva backward
    FreePage(b->current->currentPage);
    b->current->currentPage = CopyPage(backwardTOP->currentPage);

    // Actualizez stiva backward si eliberez memoria
    b->current->backwardStack = backwardTOP->next;
    FreePage(backwardTOP->currentPage);
    free(backwardTOP);
}

void FORWARD(browser *b, FILE *fout) {
    if (b == NULL || b->current == NULL || b->current->forwardStack == NULL) {
        fprintf(fout, "403 Forbidden\n");
        return;
    }

    // Retin prima pagina din stiva forward
    stack *forwardTop = b->current->forwardStack;
    
    // Salvez pagina curenta in stiva backward
    stack *backwardNode = (stack *)malloc(sizeof(stack));
    if (backwardNode == NULL) {
        return;
    }
    backwardNode->currentPage = CopyPage(b->current->currentPage);
    backwardNode->next = b->current->backwardStack;
    b->current->backwardStack = backwardNode;
    
    // Actualizez pagina curenta cu cea din stiva forward
    FreePage(b->current->currentPage);
    b->current->currentPage = CopyPage(forwardTop->currentPage);
    
    // Scoatem pagina din stiva forward si eliberam memoria
    b->current->forwardStack = forwardTop->next;
    FreePage(forwardTop->currentPage);
    free(forwardTop);
}

void PRINT_HISTORY(browser *b, int id, FILE *fout) {
    if (b == NULL || b->list.sentinel == NULL) {
        fprintf(fout, "403 Forbidden\n");
        return;
    }

    // Caut tab-ul cu ID-ul dat
    tab *targetTab = NULL;
    tab *iter = b->list.sentinel->next;
    
    while (iter != b->list.sentinel) {
        if (iter->id == id) {
            targetTab = iter;
            break;
        }
        iter = iter->next;
    }

    // Nu am gasit tab-ul => afisez mesajul de eroare
    if (targetTab == NULL) {
        fprintf(fout, "403 Forbidden\n");
        return;
    }

    // Fac o copie a stivei forward pentru a o afisa in ordinea corecta
    char forwardUrls[100][MAX_URL];
    int forwardCount = 0;
    
    // Parcurg stiva forward si salvez URL-urile (sunt invers)
    stack *currentForward = targetTab->forwardStack;
    while (currentForward != NULL) {
        strcpy(forwardUrls[forwardCount], currentForward->currentPage->url);
        forwardCount++;
        currentForward = currentForward->next;
    }
    
    // Afisez URL-urile din stiva forward (in ordinea buna)
    for (int i = forwardCount - 1; i >= 0; i--) {
        fprintf(fout, "%s\n", forwardUrls[i]);
    }
    
    // Afisez URL-ul paginii curente
    if (targetTab->currentPage != NULL) {
        fprintf(fout, "%s\n", targetTab->currentPage->url);
    }
    
    // Afisez URL-urile din stiva backward
    stack *currentBackward = targetTab->backwardStack;
    while (currentBackward != NULL) {
        fprintf(fout, "%s\n", currentBackward->currentPage->url);
        currentBackward = currentBackward->next;
    }
}

int main() 
{
    FILE *fin = fopen("tema1.in", "r");
    FILE *fout = fopen("tema1.out", "w");
    if (fin == NULL || fout == NULL) {
        return 1;
    }

    // Initializare browser
    browser b;
    b.nextID = 1;
    b.list.sentinel = malloc(sizeof(tab));
    b.list.sentinel->next = b.list.sentinel;
    b.list.sentinel->prev = b.list.sentinel;

    // Creare tab initial (ID 0)
    tab *firstTab = CreateTab(0);
    if (firstTab == NULL) {
        free(b.list.sentinel);
        fclose(fin);
        fclose(fout);
        return 1;
    }

    b.current = firstTab;
    b.list.sentinel->next = firstTab;
    b.list.sentinel->prev = firstTab;
    firstTab->next = b.list.sentinel;
    firstTab->prev = b.list.sentinel;

    // Citire pagini
    int page_nr;
    fscanf(fin, "%d", &page_nr);
    fgetc(fin); // Consumam '\n' ramas
    stack *MY_PAGES = ReadAllPages(fin, page_nr);

    // Citire operatii
    int N;
    fscanf(fin, "%d", &N);

    char operatie[MAX_URL];
    for (int i = 0; i < N; i++) {
        fscanf(fin, "%s", operatie);

        if (strcmp(operatie, "NEW_TAB") == 0) {
            NEW_TAB(&b);
        } else if (strcmp(operatie, "PRINT") == 0) {
            PRINT(&b, fout);
        } else if (strcmp(operatie, "CLOSE") == 0) {
            CLOSE(&b, fout);
        } else if (strncmp(operatie, "OPEN", 4) == 0) {
            int id;
            fscanf(fin, "%d", &id);
            OPEN_ID(&b, id, fout);
        } else if (strcmp(operatie, "NEXT") == 0) {
            NEXT(&b);
        } else if (strcmp(operatie, "PREV") == 0) {
            PREV(&b);
        } else if (strncmp(operatie, "PAGE", 4) == 0) {
            int id;
            fscanf(fin, "%d", &id);
            PAGE_ID(&b, MY_PAGES, id, fout);
        } else if (strcmp(operatie, "BACKWARD") == 0) {
            BACKWARD(&b, fout);
        } else if (strcmp(operatie, "FORWARD") == 0) {
            FORWARD(&b, fout);
        } else if (strcmp(operatie, "PRINT_HISTORY") == 0) {
            int id;
            fscanf(fin, "%d", &id);
            PRINT_HISTORY(&b, id, fout);
        }
    }

    // Eliberare memorie
    FreeStack(MY_PAGES);
    FreeBrowser(&b);

    fclose(fin);
    fclose(fout);
    return 0;
}

 Am explicat functiile mai lungi si in cod, dar nu pe toate.
 Punctajul pe checkerul local este 100/100 si 20/20 pe Valgrind.
 
 ## structuri.h
 Aici am definit toate structurile utilizate. Am folosit '#pragma once' ca sa fie procesate o singura data si am pus si 'typedef' pt fiecare structura ca sa nu repet mereu in cod 'struct ...'

 **update: am adaugat in structura browser un int id next pentru ca am vazut tarziu ca atunci cand dau close, tab-ul urmator nu are acelasi id cu tab-ul sters. Variabila retine id-ul care trebuie folosit pentru fiecare tab care urmeaza sa fie creat

 ## main-ul
 Functia main incepe prin a deschide fisierele de input si output. Creeaza un browser ca o lista dublu inlantuita circulara cu santinela si adauga un tab initial cu ID 0. Citeste toate paginile disponibile intr-o stiva, apoi citeste numarul de operatii. Pentru fiecare operatie, apeleaza functia corespunzatoare (NEW_TAB, PRINT, CLOSE, etc). La final elibereaza toata memoria alocata si inchide fisierele.

## FUNCTII SUPLIMENTARE FACUTE :
-FreePage: Elibereaza memoria alocata unei pagini.

-FreeStack: Elibereaza memoria pentru stiva de pagini (parcurge lista si elibereaza fiecare nod si pagina asociata folosing FreePage).

-FreeTab: Elibereaza memoria ocupata de un tab, inclusiv paginile curente si stivele backward si forward.

-FreeTabList: Elibereaza memoria pentru lista de tab-uri (Pornesc de la santinela si eliberez intreaga lista pana cand ajnug inapoi la santinela, facand FreeTab pt fiecare tab).

-FreeBrowser: Elibereaza memoria ocupata de structutra browser-ului (Face FreeTabList pentru fiecare tab).

-CreateTab: Creeaza un nou tab in browser cu id-ul specificat si initializeaza stivele backward si forward cu NULL. Creeaza si pagina implicita pentru tab-ul creat si seteaza NULL pentru elementul urmator si pentru cel anterior.

-PushPage: Adauga o pagina noua in varful stivei: aloca memorie pentru un nou nod, aloca si copiaza o pagina in nodul creat(sa evit double free) si returneaza noul nod.

-ReadPage: Citeste informatiile despre o pagina din fisier (am grija sa consum newline-ul ramas dupa scanf)si returneaza un pointer pentru structura alocata

-ReadAllPages: Citeste un numar page_nr de pagini si le adauga in stiva. Paginile sunt inserate in ordine inversa fata de cum apar in fisier (insereaza in varful stivei cu PushPage) si eliberz mereu pagina originala pentru a evita scurgeri de memorie

-Cautare: Cauta o pagina in stiva pe baza ID-ului

-CopyPage: Creeaza o copie a unei pagini existente: Mai inati verifica daca pagina sursa e valida, si daca este aloca memorie pentru o noua pagina si copiază id-ul, URL-ul si descrierea (daca exista) din pagina sursa. URL-ul si descrierea sunt copiate cu ajutorul functiei strncpy pentru a preveni depasirea dimensiunii maxime alocate. Daca nu exista descriere, se seteaza la NULL. La final, se returneaza copia paginii.

 ## NEW_TAB
Avand in vedere ca in main se creeaza deja tab-ul cu ID-ul 0, functia NEW_TAB ia valoarea actuala a nextID din browser, o foloseste pentru a crea un tab nou si apoi actualizeaza nextID pentru fiecare tab nou creat. Functia CreateTab este utilizata pentru a crea tab-ul, asa cum am descris anterior.

Ulterior, functia se asigura ca tab-ul nou este inserat corect in lista de taburi, imediat inainte de sentinela si dupa ultimul tab existent. In final, tab-ul nou devine tab-ul curent al browserului.

## CLOSE
Functia sterge tab-ul curent din lista, iar daca e ultimul tab ramas (tab-ul cu id 0) afiseaza eroarea. Seteaza tabul curent la tabul precedent (stanga tabului care urmeaza sa fie sters), il elimina din lista si elibereaza memoria alocata paginii curente a tab-ului si pentru stivele backward si forward, si in final elibereaza memoria si pentru tab-ul curent.

## OPEN_ID
Functia schimba tab-ul curent al browser-ului la tabul cu ID-ul specificat. Mai intai verifica daca browserul sau tab-ul curent sunt NULL. Daca da,functia se opreste si nu face nimic. Altfel, se parcurge lista de taburi, incepand de la primul tab dupa santinela,iar daca se gaseste tab-ul cu ID-ul specificat, acesta devine tab-ul curent si functia se opreste. Daca tab-ul nu este gasit, se afiseaza un mesaj de eroare "403 Forbidden".

 ## PRINT
Functia verifica initial daca lista de taburi nu este goala, apoi initializeaza doua variabile: una pentru tab-ul de start si una pentru iteratie, cu care parcurge lista. Pe parcurs, functia returneaza in fisier ID-ul fiecarui tab gasit in lista, avand grija sa nu afiseze ID-ul nodului sentinel, pentru a evita afisarea ID-ului 0 de doua ori. Dupa ce afiseaza toate ID-urile, functia printreaza descrierea paginii curente a tab-ului activ pe ultimul rand al fisierului.

## NEXT
Daca tab-ul curent are un tab succesiv (adica exista un tab in fata sa unde ma pot duce), atunci acesta devine tab-ul curent, avand grija sa nu se ajunga inapoi la nodul sentinel. Pentru a realiza acest lucru, am folosit un pointer pentru a accesa tab-ul urmator. In cazul in care urmatorul tab este sentinel, se va sari direct la primul tab valid. Altfel, tab-ul curent va fi schimbat cu succes in tab-ul urmator.

## PREV
Functia verifica intai daca exista un tab curent valid si daca lista de taburi nu este goala. Daca aceste conditii sunt indeplinite, functia cauta tab-ul anterior al tab-ului curent. Daca tab-ul anterior este chiar nodul santinela, se ajunge la ultimul tab din lista. In cele din urma, tab-ul curent este schimbat cu tab-ul anterior.

## PAGE_ID
Functia schimba pagina curenta din browser, cautand pagina in stiva de pagini. In primul rand, functia verifica daca exista un tab curent valid si daca exista o pagina cu ID-ul dat. Daca nu se gaseste pagina, se returneaza mesajul de eroare. Daca pagina este gasita, aceasta este salvata in stiva backward a tab-ului curent pentru a permite navigarea inapoi. Apoi, functia elibereaza stiva forward si actualizeaza pagina curenta cu copia paginii gasite.

## BACKWARD
Functia iti permite sa mergi inapoi in istoric, aducand pagina anterioara din stiva backward. Inainte sa faca asta, verifica daca exista un tab curent si daca stiva backward nu e goala. Daca gaseste o pagina, o salveaza temporar in stiva forward, apoi o seteaza ca pagina curenta. La final, stiva backward e actualizata si memoria eliberata. Daca nu sunt pagini in stiva backward, afiseaza "403 Forbidden".

## FORWARD
Functia te duce mai departe in istoricul de pagini. Verifica daca exista tab curent si daca stiva forward nu e goala. Daca da, salveaza pagina curenta in stiva backward, apoi seteaza ca pagina curenta cea din varful stivei forward. La final, actualizeaza stiva forward si elibereaza memoria. Daca nu ai unde sa te duci mai departe, afiseaza "403 Forbidden".

## PRINTHISTORY
Functia afiseaza istoricul de pagini pentru un tab dat dupa ID. Mai intai verifica daca browserul e valid si daca tab-ul exista. Apoi afiseaza paginile din stiva forward (in ordine normala), urmate de pagina curenta, si apoi stiva backward (in ordine inversa). Daca tab-ul nu e gasit, afiseaza "403 Forbidden". (la functia asta am explicat si in cod mai multe)

#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define INF 300000000
#define REALLOC(ptr, size) {\
    ptr = realloc(ptr, size);\
    if (!ptr) {\
	    printf("MEM_GRESKA\n");\
	    exit(1);\
    }\
}
#define MALLOC(ptr, size) {\
    ptr = malloc(size);\
    if (!ptr) {\
	    printf("MEM_GRESKA\n");\
	    exit(1);\
    }\
}
#define FOPEN(file, name, mode) {\
    file = fopen(name, mode);\
    if (!file) {\
	    printf("DAT_GRESKA\n");\
	    exit(1);\
    }\
}

typedef struct LinkedListGraph {
	int wei;
	int data;  // cvor, a = 0, b = 1, ..., z = 25, { = 26 (prazan kruzic)
	struct LinkedListGraph *next;
} Graph;

typedef struct LinkedList {
	int data;
	struct LinkedList *next;
} List;

typedef struct LinkedStack {
	int data;
	struct LinkedStack *next;
} Stack;

int minn(int a, int b) {
	return a < b ? a : b;
}

int maxx(int a, int b) {
	return a > b ? a : b;
}

void push(Stack **top, int data) {

	Stack *p = NULL;
	MALLOC(p, sizeof(Stack));
	p->data = data;
	p->next = (*top);
	*top = p;

}

int pop(Stack **top) {

	int x = (*top)->data;
	Stack *p = *top;
	*top = (*top)->next;
	free(p);
	return x;

}

int stackEmpty(Stack **top) {

	return *top == NULL;

}

int glavniMeni(int *made) {

	int stavka;
	
	while (1) {
		printf("\nIzaberite jednu od ponudjenih stavki:\n");
		printf("1. Unos grafa.\n");
		printf("2. Dodavanje operacije.\n");
		printf("3. Brisanje grafa.\n");
		printf("4. Ispis grafa.\n");
		printf("5. Kritican put.\n");
		printf("6. Tranzitivnost.\n");
		printf("7. Izlaz.\n\n");

		scanf("%d", &stavka);
		system("cls");
		if (stavka <= 0 || stavka >= 8) printf("Uneli ste nepostojucu stavku. Pokusajte ponovo.\n");
		else if (stavka >= 3 && stavka <= 6 && *made == 0) printf("Program nije u mogucnosti da uradi trazenu stavku jer ne postoji graf. Pokusajte ponovo.\n");
		else if (stavka == 3) { *made = 0; break; }
		else if (stavka == 1 && *made == 1) printf("Morate prvo izbrisati prethodni graf kako biste uneli novi. Pokusajte ponovo.\n");
		else if (stavka == 1 || stavka == 2) { *made = 1; break; }
		else if (stavka == 7 && *made == 1) printf("Treba prvo obrisati graf pre izlaska iz programa.\n");
		else break;
	}

	return stavka;

}

char* unosLinije() {

	char c, *str = NULL;
	int len = 0;

	while ((c = getchar()) != '\n') {
		REALLOC(str, ++len);
		str[len - 1] = c;
	}
	if (!len) return NULL;
	REALLOC(str, ++len);
	str[len - 1] = '\0';

	return str;
}

char *unosLinijeFajl(FILE *file) {

	char c, *str = NULL;
	int len = 0;
	while ((c = fgetc(file)) != '\n' && c != EOF) {
		REALLOC(str, ++len);
		str[len - 1] = c;
	}
	if (!len) return NULL;
	REALLOC(str, ++len);
	str[len - 1] = '\0';
	return str;

}

void dodavanjeGrane(Graph **graph, int node, int arg) {

	if (arg >= 0 && arg <= 25 && graph[arg] != NULL) {
		Graph *p = graph[arg], *q = NULL;

		if (p->next != NULL && p->next->data == '{' - 97) { // bitno kod dodavanja cvora (da se ukloni {)
			q = p->next;
			p->next = q->next;
			free(q);
			q = NULL;
		}

		while (p->next) p = p->next;
		MALLOC(q, sizeof(Graph));
		q->wei = graph[arg]->wei;
		q->data = node;
		q->next = NULL;
		p->next = q;
	}

}

int unosCvora(Graph **graph, FILE *file) {

	// unos linije
	char *str = NULL;
	if (!file) str = unosLinije();
	else str = unosLinijeFajl(file);

	if (!str) return 0;	

	// pravljenje cvora
	int node, arg1, arg2;

	node = str[0] - 97;
	MALLOC(graph[node], sizeof(Graph));
	graph[node]->wei = 0;
	graph[node]->data = node;
	graph[node]->next = NULL;

	if (strstr(str, "not")) {
		graph[node]->wei = 1;
		dodavanjeGrane(graph, node, str[8] - 97);
	}
	else {
		if (strstr(str, "and") || strstr(str, "or") || strstr(str, "xor")) graph[node]->wei = 1;
		else if (strstr(str, "+") || strstr(str, "-")) graph[node]->wei = 3;
		else if (strstr(str, "*")) graph[node]->wei = 10;
		else graph[node]->wei = 23;

		dodavanjeGrane(graph, node, str[4] - 97);
		dodavanjeGrane(graph, node, str[strlen(str) - 1] - 97);
	}

	free(str);
	return 1;

}

void prazniCvorovi(Graph **graph) {

	for (int i = 0;i < 26;i++) {
		if (!graph[i]) continue;
		if (!graph[i]->next) {
			Graph *q = NULL;
			MALLOC(q, sizeof(Graph));
			q->wei = graph[i]->wei;
			q->data = 26; // '{'-97
			q->next = NULL;
			graph[i]->next = q;
		}
	}

}

void unosGrafa(Graph **graph) {

	system("cls");
	printf("Format unosa je \"node : arg bin_op arg\" ili \"node : un_op arg\".\n");
	printf("Naziv cvora je jedno malo slovo engleskog alfabeta.\n");
	printf("Ponudjene operacije su binarneo +, -, *, /, AND, OR, XOR i unarno NOT.\n");
	printf("Obratite paznju na redosled unetih operacija tako da se ispostuju zahtevi zadatka.\n");
	printf("Unos preko komandne linije se zavrsava praznim redom.\n");
	printf("Ako zelite da unesete graf preko datoteke pritisnite '1', u suprotnom '2': ");
	int type;
	scanf("%d", &type);
	printf("\n");
	
	if (type == 1) {
		printf("Unesite ime teksutalne datoteke sa ekstenzijom iz koje zelite da program cita podatke:\n");
		const char name[30];
		scanf("%s", name);
		
		FILE *file;
		FOPEN(file, name, "r");
		while (unosCvora(graph, file));
		fclose(file);
		prazniCvorovi(graph);
		printf("\n");
	}
	else {
		char c = getchar(); // novi red
		while (unosCvora(graph, NULL));
		prazniCvorovi(graph); // { (prazni kruzici na primeru)
	}

	printf("Graf je uspesno napravljen.\n");

}

void novaOperacija(Graph **graph) {

	printf("Unesite novu operaciju:\n");
	char c = getchar(); // \n
	unosCvora(graph, NULL);
	prazniCvorovi(graph);

}

void brisanjeGrafa(Graph **graph) {

	Graph *p = NULL, *q = NULL;
	for (int i = 0;i < 26;i++) {
		p = graph[i];
		while (p) {
			q = p;
			p = p->next;
			free(q);
		}
		graph[i] = NULL;
	}
	printf("Graf je uspesno obrisan.\n");

}

void ispisGrafa(Graph **graph) {

	int type;
	printf("Ako zelite da se graf ispise u datoteku unesite '1', u suprotnom unesite '2': ");
	scanf("%d", &type);
	
	if (type == 1) {
		const char name[30];
		printf("Unesite ime tekstualne datoteke sa ekstenzijom u koju zelite da se ispise graf:\n");
		scanf("%s", name);
		FILE *file;
		FOPEN(file, name, "w");
		fprintf(file, "Graf je prikazan u sledecem formatu: (cvor, tezina operacije) -> sledbenik1 -> sledbenik2 -> ...\n");
		for (int i = 0;i < 26;i++) {
			if (!graph[i]) continue;

			fprintf(file, "(%c, %d)", i + 97, graph[i]->wei);
			for (Graph *p = graph[i]->next;p;p = p->next) fprintf(file, " --> %c", p->data != 26 ? p->data + 97 : '#');
			fprintf(file, "\n");
		}
		fclose(file);
		printf("\nGraf je uspesno ispisan u datoteku.\n");
	}
	else {
		printf("Graf je prikazan u sledecem formatu: (cvor, tezina operacije) -> sledbenik1 -> sledbenik2 -> ...\n");
		for (int i = 0;i < 26;i++) {
			if (!graph[i]) continue;

			printf("(%c, %d)", i + 97, graph[i]->wei);
			for (Graph *p = graph[i]->next;p;p = p->next) printf(" --> %c", p->data != 26 ? p->data + 97 : '#');
			printf("\n");
		}
	}

}

void kopirajGraf(Graph **tgraph, Graph **graph) {

	for (int i = 0;i < 26;i++) {
		tgraph[i] = NULL;
		if (!graph[i]) continue;

		MALLOC(tgraph[i], sizeof(Graph));
		tgraph[i]->wei= graph[i]->wei;
		tgraph[i]->data = graph[i]->data;
		tgraph[i]->next = graph[i]->next;
	}

}

void brojPrethodnika(int *pret, Graph **graph) { // O(e)

	for (int i = 0;i < 26;i++) pret[i] = 0;
	for (int i = 0;i < 26;i++) {
		if (!graph[i]) continue;
		for (Graph *p = graph[i]->next;p != NULL && p->data != '{' - 97;p = p->next) pret[p->data]++;
	}
	for (int i = 0;i < 26;i++) if (!pret[i] && !graph[i]) pret[i] = -1;

}

List* makeListNode(int i) {

	List *temp = NULL;
	MALLOC(temp, sizeof(List));
	temp->data = i;
	temp->next = NULL;
	return temp;

}

List* bezPrethodnika(int *pret) {

	List *head = NULL, *tail = NULL;
	for (int i = 0;i < 27;i++) {
		if (!pret[i]) {
			List *temp = makeListNode(i);
			if (!head) head = temp;
			else tail->next = temp;
			tail = temp;
		}
	}
	return head;

}

void insertAfter(List *p, int info) {

	List *q = makeListNode(info);
	q->next = p->next;
	p->next = q;

}

void topoloskoSortiranje(int *T, int *cnt, Graph **graph) {

	// broj prethodnika za svaki cvor
	int pret[27];
	brojPrethodnika(&pret, graph);

	// lista u kojoj su oni bez prethodnika
	List *head = bezPrethodnika(&pret);

	// topolosko sortiranje
	int cvor;
	*cnt = 0;
	List *temp = NULL;
	while (head) {
		cvor = head->data;
		T[(*cnt)++] = cvor;

		for (Graph *p = graph[cvor]->next;p != NULL && p->data != '{' - 97;p = p->next) {
			pret[p->data]--;
			if (!pret[p->data]) insertAfter(head, p->data);
		}

		temp = head;
		head = head->next;
		free(temp);
	}
	T[(*cnt)++] = '{' - 97;

}

void kriticniPut(Graph **graph) {

	// topolosko soritranje
	int T[27], n;
	for (int i = 0;i < 27;i++) T[i] = -1;
	topoloskoSortiranje(&T, &n, graph);

	// racunanje EST, LST, L
	int EST[27], LST[27], L[27], node;
	for (int i = 0;i < 27;i++) {
		EST[i] = 0;
		LST[i] = INF;
	}
	for (int i = 1;i < n;i++) {
		node = T[i];
		for (int j = 0;j < n;j++) {
			if (i == j || graph[T[j]] == NULL) continue;
			for (Graph *p = graph[T[j]]->next;p;p = p->next) {
				if (node == p->data) {
					EST[node] = maxx(EST[node], EST[T[j]] + p->wei);
				}
			}
		}
	}
	LST[T[n - 1]] = EST[T[n - 1]];
	for (int i = n - 2;i >= 0;i--) {
		node = T[i];
		for (Graph *p = graph[node];p;p = p->next) {
			LST[node] = minn(LST[node], LST[p->data] - p->wei);
		}
	}
	for (int i = 0;i < 27;i++) L[i] = LST[i] - EST[i];

	// ispis
	printf("* Duzina kriticnog puta iznosi: %d\n* Cvorovi na kriticnom putu su:", LST[T[n-1]]);
	for (int i = 0;i < n - 1;i++) if (!L[T[i]]) printf(" %c", T[i] + 97);
	printf("\n");

	// razliciti moguci rasporedi #1
	printf("* U nastavku je prikaz razlicitog moguceg rasporeda za svaku operaciju.\n");
	printf("  Karakter '|' oznacava trenutak u kojem je moguce zapoceti operaciju,\n  dok karatker '_' oznacava trenutak u kojem se operacija ne moze zapoceti.\n");
	printf("  Za svaki cvor postoji onoliko trenutaka koliko iznosi duzina kriticnog puta.\n");
	printf("  Broj u zagradi oznacava trajanje operacije (njena tezina).\n");
	printf("  (Za operacije koje imaju samo jedan karakter '|' znaci da su na kriticnom putu)\n\n");

	for (int i = 0;i < n - 1;i++) {
		printf("  %c :", T[i] + 97);
		for (int j = 0;j < LST[T[n - 1]];j++) {
			if (j < EST[T[i]] || j > LST[T[i]]) printf(" _");
			else printf(" |");
		}
		printf(" (%d)\n", graph[T[i]]->wei);
	}
	printf("\n");

	// razlicit moguci rasporedi #2
	int opcija;
	printf("Pritisnite opciju '1' ako zelite da se ispise drugi prikaz razlicitih mogucih rasporeda, u suprotnom pritisnite '2': ");
	scanf("%d", &opcija);

	if (opcija == 1) {
		printf("\n* Broj levo od ':' predstavlja trenutak,\n  dok se desno od ':' nalaze operacije koje su u mogucnosti da zapocnu u tom trenutku.\n\n");
		for (int i = 0;i < LST[T[n - 1]];i++) {
			printf("%3d :", i);
			for (int j = 0;j < n;j++) {
				if (i >= EST[T[j]] && i <= LST[T[j]]) printf(" %c", T[j] + 97);
			}
			printf("\n");
		}
	}

}

void tranzitivnost(Graph **graph) {

	char c;
	while (1) {
		printf("Postojece operacije su:");
		for (int i = 0;i < 26;i++) if (graph[i]) printf(" %c", i + 97);
		printf("\nUnesite operaciju ciju tranzitivnost zelite da nadjete: ");
		c = getchar(); // novi red
		c = getchar();
		if (c < 97 || c > 122 || !graph[c - 97]) {
			system("cls");
			printf("Uneli ste nepostojecu operaciju. Pokusajte ponovo.\n");
		}
		else break;
	}

	int node = c - 97, visit[27];
	int oldNode = node;
	
	for (int i = 0;i < 27;i++) visit[i] = 0;
	for (Graph *p = graph[node]->next;p;p = p->next) visit[p->data] = -1; // incidentni cvorovi

	// visit = -1 (neposecen incidentni cvor), visit = 0 (neposecen cvor), visit = 1 (posecen incidentni cvor), visit = 2 (posecen cvor), visit = 3 (tranzitivni cvor)
	Stack *top = NULL;
	push(&top, node);
	while (!stackEmpty(&top)) {
		node = pop(&top);
		if (visit[node] <= 0) {
			visit[node] += 2;
			for (Graph *p = graph[node]->next;p != NULL && p->data != '{' - 97;p = p->next) {
				if (visit[p->data] <= 0) push(&top, p->data);
				else if (visit[p->data] == 1) visit[p->data] += 2;
			}
		}
	}

	node = oldNode;
	int cnt = 0;
	for (Graph *p = graph[node]->next;p;p = p->next) if (visit[p->data] == 3) cnt++;
	if (!cnt) printf("Zadati cvor ne utice indirektno ni na jednu operaciju.\n");
	else {
		printf("Operacija %c utice indirektno na sledece operacije:", node + 97);
		for (Graph *p = graph[node]->next;p;p = p->next) if (visit[p->data] == 3) printf(" %c", p->data + 97);
		printf("\n");
	}

}

int main() {

	int tipUnosa, made = 0;
	Graph *graph[27];
	for (int i = 0;i < 27;i++) graph[i] = NULL;

	printf("Dobrodosli!\n");
	while (1) {
		int stavka = glavniMeni(&made);

		if (stavka == 1) unosGrafa(&graph);
		else if (stavka == 2) novaOperacija(&graph);
		else if (stavka == 3) brisanjeGrafa(&graph);
  		else if (stavka == 4) ispisGrafa(&graph);
		else if (stavka == 5) kriticniPut(&graph);
		else if (stavka == 6) tranzitivnost(&graph);
		else break;
	}
	system("cls");
	printf("Dovidjenja!\n");
	
	return 0;
}

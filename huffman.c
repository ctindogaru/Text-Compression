#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>


// structura folosita pentru a determina frecventa fiecarei litere in sir, care are pe langa campurile frecventa si litera inca 2 campuri de care ma voi folosi ca sa pastrez codificarea Huffman a literei respective si nr_biti care imi arata cati din cei 8 biti ai unsigned char-ului apartin codificarii
typedef struct frecventa_aparitiei{
	float probabilitate;
	unsigned char litera;
	int cod;
	int nr_biti;
}frecventa_aparitiei;

//structura Huffman din enuntul problemei
typedef struct TagHuffmanNode {
	unsigned char value;
	int16_t	left;
	int16_t right;
} __attribute__ ((__packed__)) TagHuffmanNode;

//structura care are pentru inceput rolul de coada, dupa care devine arborele propriu-zis
typedef struct nod {
	unsigned char litera; //litera nodului, va fi 0 in caz ca nu exista
	float probabilitate; // frecventa literei in text
	unsigned char index; // va fi 0 daca nodul e in stanga si 1 daca nodul e in dreapta
	int cod; // codificarea Huffman a literei
	int nr_biti; // cati biti din cei 8 ai unsigned charului are codificarea Huffman
	int pus_in_vector; // pozitia fiecarui nod in arbore
	struct nod *right; //folosit pentru arbore
	struct nod *left; //folosit pentru arbore
	struct nod *next; //folosit pentru coada
}nod;

typedef struct arbore {
	unsigned char litera;
	unsigned char index;
	int pus_in_vector;
	unsigned char vazut;
	struct arbore *left;
	struct arbore *right;
}arbore;

//frecventa literelor in text
void frecventa(unsigned char *text, frecventa_aparitiei sir[], int *lungime) {
 	int i, j, k;
 	int n = strlen(text);
 	//printf("%s", text);
	sir[0].probabilitate = 1.0/n;
	sir[0].litera = text[0];
	*lungime = 1;
	for(i = 1; i < n; i++) {
		k = 0;
		for(j = 0 ; j < *lungime; j++) {
			if(sir[j].litera == text[i]) {
				k = 1;
				sir[j].probabilitate = sir[j].probabilitate + 1.0/n;
				break;
			}
		}
		if(k == 0) {
			sir[*lungime].probabilitate = 1.0/n;
			sir[*lungime].litera = text[i];
			*lungime = *lungime + 1;
		}
	}
}

//algoritmul de sortare a literelor in coada, in functie de frecventa lor de aparitie; am folosit bubblesort pentru ca alte sortari nu-mi pastrau ordinea din vector
void bubblesort(frecventa_aparitiei *sir, int lungime) {
	int i, j, flag = 1;
	float aux_prob;
    unsigned char aux_litera;

    for(i = 0; (i < lungime) && flag; i++) {
      	flag = 0;
      	for (j = 0; j < lungime - 1; j++)
      	{
               if (sir[j+1].probabilitate < sir[j].probabilitate)
               { 
                    aux_prob = sir[j].probabilitate;
                    sir[j].probabilitate = sir[j+1].probabilitate;
                    sir[j+1].probabilitate = aux_prob;
               		aux_litera = sir[j].litera;
               		sir[j].litera = sir[j+1].litera;
               		sir[j+1].litera = aux_litera;
               		flag = 1;
               }
        }
    }
}

//determina numarul de noduri
int count(nod *cap)
{
  	int c = 1;
  	if (cap == NULL)
    	return 0;
  	else
  	{
	    c += count(cap->left);
	    c += count(cap->right);
  	}
	return c;
}

//imi adauga la sfarsitul cozii un nod nou
nod* adauga_coada(nod *cap, unsigned char litera, float probabilitate) {
	nod *nou, *cap_aux = cap;
	nou = malloc(sizeof(nod));
	nou->probabilitate = probabilitate;
	nou->litera = litera;
	nou->right = NULL;
	nou->left = NULL;
	nou->next = NULL;

	if(cap == NULL) { 
		cap = nou;
		return cap;
	}
	else {
		while(cap->next != NULL)
			cap = cap->next;
		cap->next = nou;
	}
	cap = cap_aux;
	return cap;
}

//imi adauga la inceputul cozii un nod nou
nod* adauga_coada_inceput(nod *cap, unsigned char litera, float probabilitate) {
	nod *nou = malloc(sizeof(nod));
	nou->probabilitate = probabilitate;
	nou->litera = litera;
	nou->right = NULL;
	nou->left = NULL;
	nou->next = cap;
	return nou;
}

//adaug un nod nou in coada, pe care il voi pozitiona in functie de frecventa adunata a primelor 2 noduri din coada
nod * adauga_coada_crescator(nod *cap, nod *aux1, nod *aux2) {
	if(cap == NULL) {
		nod *nou = malloc(sizeof(nod));
		nou->probabilitate = aux1->probabilitate + aux2->probabilitate;
		nou->litera = -1;
		nou->left = aux1;
		nou->right = aux2;
		nou->next = NULL;
		cap = nou;
		return cap;
	}

	if(cap->probabilitate == (aux1->probabilitate + aux2->probabilitate)) {
		cap = adauga_coada_inceput(cap, -1, aux1->probabilitate + aux2->probabilitate);
		cap->left = aux1;
		cap->right = aux2;
		return cap;
	}

	nod *nou = malloc(sizeof(nod));
	nod *cap_aux = cap;
	nou->probabilitate = aux1->probabilitate + aux2->probabilitate;
	nou->litera = -1;
	nou->left = aux1;
	nou->right = aux2;

	while(cap->next != NULL && cap->next->probabilitate < nou->probabilitate)
		cap = cap->next;
	nod *urmatorul = cap->next;
	cap->next = nou;
	nou->next = urmatorul;
	cap = cap_aux;
	return cap;
}

//sterge primul nod din coada
nod * sterge_coada(nod *cap) {
	if(cap == NULL)
		return cap;
	cap = cap->next;
	return cap;
}

//printeaza coada
void printare_coada(nod *cap) {
	if(cap == NULL)
		printf("Coada este vida!\n");
	else {
		while(cap->next != NULL) {
			printf("%c %f\n", cap->litera, cap->probabilitate);
			cap = cap->next;
		}
		printf("%c %f\n", cap->litera, cap->probabilitate);
	}
}

//printeaza arborele
void afisare_arbore(arbore *cap) {
	printf("%c %d %d\n", cap->litera, cap->pus_in_vector, cap->index);
	if(cap->left != NULL) {
		//printf("stang");
		afisare_arbore(cap->left);
	}
	if(cap->right != NULL) {
		//printf("drept");
		afisare_arbore(cap->right);
	}
}

void printare_arbore_coada(nod *cap) {
	printf("%c %f index:%d biti:%d cod:%d pozitie:%d\n", cap->litera, cap->probabilitate, cap->index, cap->nr_biti, cap->cod, cap->pus_in_vector);
	if(cap->left != NULL) {
		//printf("stang");
		printare_arbore_coada(cap->left);
	}
	if(cap->right != NULL) {
		//printf("drept");
		printare_arbore_coada(cap->right);
	}
}

// in campul index al nodului voi pune 0 daca acesta este pozitionat in stanga, respectiv 1 daca se afla in dreapta
void adaugare_index(nod *cap) {
	if(cap->left != NULL) {
		cap->left->index = 0;
		cap->left->cod = 0;
		cap->left->nr_biti = 0;
		adaugare_index(cap->left);
	}
	if(cap->right != NULL) {
		cap->right->index = 1;
		cap->right->cod = 0;
		cap->left->nr_biti = 0;
		adaugare_index(cap->right);
	}
}

// pentru decomprimare
void pune_index(arbore *cap) {
	if(cap->left != NULL) {
		cap->left->index = 0;
		pune_index(cap->left);
	}
	if(cap->right != NULL) {
		cap->right->index = 1;
		pune_index(cap->right);
	}
}

// aflu codurile si nr_biti ale fiecarui nod
void coduri(nod *cap, int lungime, int prev_cod) {
	if(cap->left != NULL) {
		cap->left->cod = prev_cod<<1;
		cap->left->nr_biti = 0;
		cap->left->nr_biti = cap->left->nr_biti + lungime;
		coduri(cap->left, lungime + 1, cap->left->cod);
	}
	if(cap->right != NULL) {
		cap->right->cod = prev_cod<<1;
		cap->right->cod = 1 | cap->right->cod;
		cap->right->nr_biti = 0;
		cap->right->nr_biti = cap->right->nr_biti + lungime;
		coduri(cap->right, lungime + 1, cap->right->cod);
	}
}


//completez campurile cod si nr_biti ale vectorului sir, in functie de campurile pe care le-am completat anterior in fiecare nod
void completeaza(nod *cap, frecventa_aparitiei *sir) {
	if(cap->left != NULL && cap->left->litera == sir->litera) {
		sir->cod = cap->left->cod;
		sir->nr_biti = cap->left->nr_biti;
		return;
	}
	if(cap->right != NULL && cap->right->litera == sir->litera) {
		sir->cod = cap->right->cod;
		sir->nr_biti = cap->right->nr_biti;
		return;
	}
	if(cap->left != NULL)
		completeaza(cap->left, sir);
	if(cap->right != NULL)
		completeaza(cap->right, sir);

}

//inaltimea arborelui
int maxHeight(nod *cap) {
  	if (cap == NULL) 
  		return 0;

 	int left_height = maxHeight(cap->left);
  	int right_height = maxHeight(cap->right);

  	if(left_height > right_height)
  		return left_height + 1;
  	else return right_height + 1;
}

int pozitie = 0;


void fiecare_nivel(nod *cap, int level) {
	if (cap == NULL) 
		return;
	if (level == 1) {
		cap->pus_in_vector = 0;
		if(cap->pus_in_vector == 0) {
			cap->pus_in_vector = pozitie;
			pozitie ++;
			//return;
		}
	}
	else {
		fiecare_nivel(cap->left, level-1);
		fiecare_nivel(cap->right, level-1);
	}
}

// functie folosita pentru a determina pozitia fiecarui nod in arbore (o apeleaza pe cea de sus)
void niveluri(nod *cap) {
	int height = maxHeight(cap);
	int level;
	for ( level = 1; level <= height; level++)
		fiecare_nivel(cap, level);
}

int poz = 0;

// crearea unei structuri TagHuffmanNode
void adaugare_huffman(nod *cap, TagHuffmanNode *huffman, int i) {
	if(cap == NULL)
		return;
	if(cap->pus_in_vector != i) {
		adaugare_huffman(cap->left, huffman, i);
		adaugare_huffman(cap->right, huffman, i);
	}
	else {
		if(cap->left == NULL || cap->right == NULL) {
			huffman->left = -1;
			huffman->right = -1;
			huffman->value = cap->litera;
		}
		else {
			huffman->left = cap->left->pus_in_vector;
			huffman->right = cap->right->pus_in_vector;
			huffman->value = 0;
		}
	}
	
}

// printarea vectorului de structuri TagHuffmanNode
void print_vector_huffman(TagHuffmanNode *huffman, int nr_noduri) {
	int i;
	for(i = 0; i < nr_noduri; i++)
		printf("%c %d %d\n", huffman[i].value, huffman[i].left, huffman[i].right);
} 

void inserare(arbore **cap, int i, unsigned char litera) {
	arbore *temp = NULL;
	if(!(*cap))
	  {
	    temp = malloc(sizeof(arbore));
	    temp->vazut = 1;
	    temp->left = NULL;
	    temp->right = NULL;
	    temp->pus_in_vector = i;
	    temp->litera = litera;
	    *cap = temp;
	    return;
	  }

	if(i < (*cap)->pus_in_vector)
	   inserare(&(*cap)->left, i, litera);
	else if(i > (*cap)->pus_in_vector)
	   inserare(&(*cap)->right, i, litera);
}

// cauta o frunza, urmand drumul indicat de indice; cand o va gasi inseamna ca a gasit codificarea pentru litera respectiva si va afisa litera
arbore *cauta_index(arbore *cap, unsigned char indice, FILE *file2) {
	if(cap->left == NULL) {
		//printf("%c", cap->litera);
		fprintf(file2, "%c", cap->litera);
		//fprintf(file2, "%d ", cap->litera);
		return NULL;
	}
	//printf("%d %d\n", indice, cap->left->index);
	if(cap->left->index == indice)
		return cap->left;
	else if(cap->right->index == indice)
		return cap->right;
}

arbore *initializare_arbore(arbore *cap) {
	cap = malloc(sizeof(arbore));
	cap->litera = -1;
	cap->pus_in_vector = 0;
	return cap;
}

void *cauta(arbore **aux, arbore *cap, int i) {
	if(cap->pus_in_vector == i) {
		(*aux) = cap;
		return;
	}
	else if(cap->left != NULL) {
			cauta(&(*aux), cap->left, i);
			cauta(&(*aux), cap->right, i);
		}
}

void adaugare(arbore **cap, unsigned char value, int16_t left, int16_t right) {
	(*cap)->litera = value;
	if(left != -1) {
		arbore *aux1, *aux2;
		aux1 = malloc(sizeof(arbore));
		aux2 = malloc(sizeof(arbore));

		aux1->pus_in_vector = left;
		aux1->left = NULL;
		aux1->right = NULL;

		aux2->pus_in_vector = right;
		aux2->left = NULL;
		aux2->right = NULL;

		(*cap)->left = aux1;
		(*cap)->right = aux2;

	}
}


int main(int argc, char *argv[])
{
	FILE *file1, *file2;


	if(strcmp(argv[1], "-c") == 0) {
		file1 = fopen(argv[2], "r");
		file2 = fopen(argv[3], "wb");
		int i, j, lungime;
		nod *cap = NULL, *nou;
		frecventa_aparitiei *sir;
		sir = malloc(256 * sizeof(frecventa_aparitiei));
		unsigned char *text; //textul citit din fisier
		int ch, lung_text = 0;
		text = malloc(1000 * sizeof(unsigned char));
		while((ch = fgetc(file1)) != EOF) {
			if(lung_text != 0 && lung_text%1000 == 0)
				text = realloc(text, 2 * lung_text * sizeof(unsigned char));
			text[lung_text] = ch;
			lung_text++;
		}
		//n = n-1;
		//printf("%d\n\n\n\n", n-1);
		text[lung_text] = '\0';
		/* for(i = 0; i < lung_text; i++)
			printf("%d ", text[i]); */

			int k;
		 	//int n = strlen(text);
		 	//printf("%s", text);
			sir[0].probabilitate = 1.0/lung_text;
			sir[0].litera = text[0];
			lungime = 1;
			for(i = 1; i < lung_text; i++) {
				k = 0;
				for(j = 0 ; j < lungime; j++) {
					if(sir[j].litera == text[i]) {
						k = 1;
						sir[j].probabilitate = sir[j].probabilitate + 1.0/lung_text;
						break;
					}
				}
				if(k == 0) {
					sir[lungime].probabilitate = 1.0/lung_text;
					sir[lungime].litera = text[i];
					lungime = lungime + 1;
				}
			}
		//frecventa(text, sir, &lungime); // pune in sir literele distincte si frecventa lor in text
		bubblesort(sir, lungime); // sorteaza sirul
		//printf("%d lungime\n", lungime);

		for(i = 0; i < lungime; i++)
			cap = adauga_coada(cap, sir[i].litera, sir[i].probabilitate); // adauga sirul in coada
		//formez arborele
		 while(cap->next != NULL) {
			nod *aux1 = cap, *aux2 = cap->next;
			cap = sterge_coada(cap);
			cap = sterge_coada(cap);
			cap = adauga_coada_crescator(cap, aux1, aux2);
		}

		cap->index = 0;
		cap->nr_biti = 0;
		cap->cod = 0;
		cap->pus_in_vector = 0;
		adaugare_index(cap); // adauga index; adaugare de 0 daca e in stanga, sau 1 daca e in dreapta + initializarea lungimii codului cu 0 + initializarea codului fiecarui nod cu 0
		coduri(cap, 1, 0); // adauga cod si nr_biti
		for(i = 0; i < lungime; i++) // pentru fiecare litera distincta din sir se completeaza nr_biti + codul
			completeaza(cap, &sir[i]);
		niveluri(cap); // adauga pozitia nodurilor in vector
		//printare_arbore_coada(cap);

		uint32_t n = lung_text;
		uint16_t nr_noduri = count(cap);
		fwrite(&n, sizeof(uint32_t), 1, file2);
		fwrite(&nr_noduri, sizeof(uint16_t), 1, file2);
		//printf("%d\n%d\n", n, nr_noduri);


		TagHuffmanNode *huffman;
		huffman = malloc(nr_noduri * sizeof(TagHuffmanNode));
		for(i = 0; i < nr_noduri; i++)
			adaugare_huffman(cap, &huffman[i], i);
		for(i = 0; i < nr_noduri; i++)
			fwrite(&huffman[i], sizeof(TagHuffmanNode), 1, file2);
		//print_vector_huffman(huffman, nr_noduri);
		//for(i = 0; i < lungime; i++)
		//	printf("%c %d %d\n", sir[i].litera, sir[i].cod, sir[i].nr_biti);

		int c = 0;
		int biti = 0;
		k = 0;
		for(i = 0; i < n; i++)
			for(j = 0; j < lungime; j++) 
				if(text[i] == sir[j].litera) {
					biti = sir[j].nr_biti;
					while(biti != 0) {
						int aux = 1 << (biti-1);
						if(aux & sir[j].cod) {
							c = c<<1;
							c = c | 1;
						}
						else {
							c = c<<1;
						}
						biti--;
						k++;
						if(k == 8) {
							//printf("%d ", c);
							fwrite(&c, sizeof(unsigned char), 1, file2);
							k = 0;
							c = 0;
						}
					}
				}
		if(k != 0) {
			c = c<<(8-k);
			//printf("%d", c);
			//printf("\n"); 
			fwrite(&c, sizeof(unsigned char), 1, file2);
		}

		/*for(i = 0; i < n; i++) {
			printf("%d ", text[i]);
		}
		printf("\n%d\n", n);*/
	}

	else if(strcmp(argv[1], "-d") == 0) {
		file1 = fopen(argv[2], "rb");
		file2 = fopen(argv[3], "w");
		int i, j;


		uint32_t n;
		fread(&n, sizeof(uint32_t), 1, file1);
		uint16_t nr_noduri;
		fread(&nr_noduri, sizeof(uint16_t), 1, file1);


		TagHuffmanNode *huffman;
		huffman = malloc(nr_noduri * sizeof(TagHuffmanNode));
		for(i = 0; i < nr_noduri; i++)
			fread(&huffman[i], sizeof(TagHuffmanNode), 1, file1);


		unsigned char *vector;
		vector = malloc(1000 * sizeof(unsigned char));
		unsigned char c;
		int k = 0;
		while(fread(&c, sizeof(unsigned char), 1, file1) == 1) {
			if(k != 0 && k%100 == 0)
				vector = realloc(vector, 2 * k);
			vector[k] = c;
			k++;
		}
		

		/*printf("%d %d\n", n, nr_noduri);
		print_vector_huffman(huffman, nr_noduri);
		for(i = 0; i < k; i++)
			printf("%d ", vector[i]);
		printf("\n"); */


		// creez arborele huffman
		arbore *cap = NULL;
		cap = initializare_arbore(cap);
		for(i = 0; i < nr_noduri; i++) {
			arbore *aux = cap;
			cauta(&aux, cap, i);
			adaugare(&aux, huffman[i].value, huffman[i].left, huffman[i].right);
		}
		pune_index(cap);

		arbore *auxiliar = cap;
		unsigned char v[8];
		int lungime = 0;
		for(i = 0; i < k; i++) {
			for(j = 7; j >= 0; j--) {
				v[7-j] = 1<<j & vector[i];
				if(v[7-j])
					v[7-j] = 1;
				else v[7-j] = 0;
			}
			for(j = 0; j < 8; j++) {
					auxiliar = cauta_index(auxiliar, v[j], file2);
					if(auxiliar == NULL) {
						auxiliar = cap;
						j = j-1;
						lungime++;
						if(lungime == n)
							break;
					}
			}
		}
	}
	return 0;
}

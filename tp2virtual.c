#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
/*essas variaveis globais compoem as estatisticas
pedidas no enunciado do trabalho*/
int lidas = 0;
int escritas = 0;
int totalUsed = 0;
int faults = 0;

typedef struct Page_t {
	unsigned addr;
	struct Page_t *next;	
    struct Page_t *prev;
    int ref;
}Page_t;

Page_t *first, *last, *current;

void AddNew(unsigned addr){
    Page_t *latest = (Page_t*)malloc(sizeof(Page_t));
    latest->addr = addr;
    latest->next = NULL;
    latest->prev = NULL;
    latest->ref = 1;
    if(totalUsed == 0){
        first = latest;
        first->next = last;
        current = first;
    }
    else{
        if(last == NULL){
            last = latest;
            last->prev = first;
            first->next = latest;
            last->next = first;
            first->prev = last;
        }
        else{
            last->next = latest;
            latest->prev = last;
            latest->next = first;
            last = latest;
            first->prev = last;
            last->next = first;
        }
        
    }
    totalUsed++;
    escritas++;
}
void FIFO(unsigned addr){
    //criar a pagina que vai subtituir a selecionada
    Page_t *new = (Page_t*)malloc(sizeof(Page_t));
    new->addr = addr;
    new->next = NULL;
    new->prev = NULL;
    //retiramos a primeira pagina da fila
    first = first->next;
    //colocamos a nova pagina no final
    last->next = new;
    new->prev = last;
    new->next = first;
    last = new;
    first->prev = last;

    escritas++;
}

void lruListModifier(Page_t *pag_addr){
    Page_t *aux = first;
    while (aux->next != NULL || aux->next != first)
    {
        if(aux->next == pag_addr){
            aux->next = pag_addr->next;
        }
        if(aux->prev == pag_addr){
            aux->prev = pag_addr->prev;
            break;
        }
    }
    pag_addr->prev = last;
    pag_addr->next = NULL;
    last->next = pag_addr;
    last = pag_addr;
}

void LRU(unsigned addr){
    //cria pagina com o novo end
    Page_t *new = (Page_t*)malloc(sizeof(Page_t));
    new->addr = addr;
    new->next = NULL;
    new->prev = NULL;
    //retira a pagina mais antiga
    first = first->next;
    first->prev = NULL;
    //coloca a pagina nova no topo da fila
    last->next = new;
    new->prev = last;
    last = new;

    escritas++;
}

void secondChance(unsigned addr){
    Page_t *new = current;
    //Page_t *prox = new->next;
    while(current->ref == 1){
        //se o que esta sendo olhado pelo ponteiro for 1 da a segunda
        //chance e troca para 0 
        if(new->ref == 1){
            new->ref = 0;
        }
        //se for 0, tira ele da fila e coloca o outro no lugar.
        //na proxima vez que trocar de pagina, comeca nesse novo (current)
        else{
            new->addr = addr;
            new->ref = 1;
            current = new;
            break;
        }
        // if(prox->ref == 0){
        //     prox->addr = addr;
        //     prox->ref = 1; 
        //     current = prox;
        //     break;
        // }
        new = new->next;
        current = new;
    }
    if(current->ref == 0){
        printf("não há página a ser substituida sc");
    }

}

void randomAlg(unsigned addr, int totalPages){
    int pag_del = rand() % totalPages;
    pag_del = pag_del-1;
    Page_t *new = first;
    int i = 0;
    while(i < pag_del){
        new = new->next;
        i++;
    }
    new->addr = addr;
}

void writeAddress(unsigned addr, char *sub, int totalPages){
    if(strcmp(sub, "lru") == 0){
        LRU(addr);
    }
    else if(strcmp(sub, "fifo") == 0){
        FIFO(addr);
    }
    else if(strcmp(sub, "2a") == 0){
        secondChance(addr);
    }
    else if(strcmp(sub, "random") == 0){
        randomAlg(addr, totalPages);
    }
}

Page_t* AlreadyWritten(unsigned address){
    Page_t *aux = first;
    while (aux != NULL && aux != first)
    {
        if(aux->addr == address){
            return aux;
        }
        aux = aux->next;
    }
    return NULL;
}

int main(int argc, char* argv[]){
    FILE *file; 
    unsigned addr;
    char rw;
    // char *sub = argv[1];
    // file = fopen(argv[2], "r");
    // int pag = atoi(argv[3]);
    // int mem = atoi(argv[4]); 
    char *sub = "random";
    file = fopen("matriz.log", "r");
    int pag = 4;
    int mem = 123;
    int totalPages = mem/pag;
    first = (Page_t*)malloc(sizeof(Page_t));
    last = (Page_t*)malloc(sizeof(Page_t));
    first->next = NULL;
    first->prev = NULL;
    last->next = NULL;
    last->prev = NULL;
    while(fscanf(file,"%x %c",&addr,&rw) != -1){
        //printf("%x",addr);
        Page_t* pag_addr = AlreadyWritten(addr);
        if(rw == 'W' || rw == 'w'){
            if(pag_addr == NULL){
                if(totalUsed < totalPages){
                    AddNew(addr);
                }
                else{
			        writeAddress(addr, sub, totalPages);
                }faults++;
            }
            /*caso a pagina a ser escrita ja tiver na lista de paginas
            e o algoritmo usado for LRU, sera necessário colocar a pagina no topo
            da pilha(modificacao na lista anterior) já que ela foi novamente
            referenciada
            */
            else if(strcmp(sub, "lru") == 0){
                lruListModifier(pag_addr);
            }
		}
        else if(rw == 'R' || rw == 'r'){
            if(pag_addr == NULL){
                if(totalUsed < totalPages){
                    AddNew(addr);
                }else{
                    writeAddress(addr, sub, totalPages);
                }faults++;
            }
            else if(strcmp(sub, "lru") == 0){
                lruListModifier(pag_addr);
            }
            lidas++;
        }
    }
    fclose(file);
}
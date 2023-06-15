#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
/*essas variaveis globais compoem as estatisticas
pedidas no enunciado do trabalho*/
int lidas = 0;
int escritas = 0;
int totalUsed = 0;
int faults = 0;

typedef struct Page {
	unsigned addr;
	struct Page_t *next;	
    struct Page_t *prev;
}Page_t;

Page_t *first, *last;

void AddNew(unsigned addr){
    Page_t *current = (Page_t*)malloc(sizeof(Page_t));
    current->addr = addr;
    current->next = NULL;
    current->prev = NULL;
    if(totalUsed == 0){
        first = current;
        last = current->next;
    }
    else{
        last->next = current;
        current->prev = last;
        last = current;
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
    first->prev = NULL;
    //colocamos a nova pagina no final
    last->next = new;
    new->prev = last;
    last = new;

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

void LRU(addr){
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

void writeAddress(unsigned addr, char *sub, int totalPages){
    if(strcmp(sub, "lru") == 0){
        LRU(addr);
    }
    else if(strcmp(sub, "2a") == 0){
        FIFO(addr);
    }
    else if(strcmp(sub, "random") == 0){
        secondChance(addr);
    }
    else if(strcmp(sub, "fifo") == 0){
        randomAlg(addr);
    }
}

Page_t* AlreadyWritten(unsigned address){
    Page_t *aux = first;
    while (aux->next != NULL || aux->next != first)
    {
        if(aux->addr == address){
            return aux;
        }
    }
    return NULL;
}

int main(int argc, char* argv[]){
    FILE *file; 
    unsigned addr;
    char rw;
    char *sub = argv[1];
    file = fopen(argv[2], "r");
    int pag = atoi(argv[3]);
    int mem = atoi(argv[4]); 
    int totalPages = mem/pag;
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
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
    latest->next = latest;
    latest->prev = latest;
    latest->ref = 1;
    if(totalUsed == 0){
        first = latest;
        last = latest;
        current = first;
    }
    else{
        if(totalUsed == 1){
            last = latest;
            last->next = first;
            last->prev = first;
            first->next = last;
            first->prev = last;
            current = first;
        }
        else{
            Page_t *aux = last;
            aux->next = latest;
            latest->prev = aux;
            last = latest;
            last->next = first;
            first->prev = last;
            current = first;
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

void lruListModifier(unsigned addr){
    //pag_addr = pagina do endereco lido que ja estava na lista
    Page_t *aux = first;
    //procura a pagina na lista e tira ela de la, arrumando os ponteiros
    //de que ta antes dela e de quem ta depois
    for(int i = 0; i < totalUsed; i++)
    {
        if(aux->addr == addr){
            break;
        }
        aux = aux->next;
    }

    if(aux->addr != last->addr){
        if(aux->addr == first->addr){
            first = first->next;
        }
        else{
            //coloca ela no final da fila
            aux->prev->next = aux->next;
            aux->next->prev = aux->prev;
            
        }
        aux->prev = last;
        aux->next = first;
        last->next = aux;
        last = aux;
        first->prev = last;
    }

    
}

void LRU(unsigned addr){
    //cria pagina com o novo end
    Page_t *new = (Page_t*)malloc(sizeof(Page_t));
    Page_t *aux = (Page_t*)malloc(sizeof(Page_t));
    new->addr = addr;
    //retira a pagina mais antiga
    first = first->next;
    
    //coloca a pagina nova no topo da fila
    last->next = new;
    new->prev = last;
    new->next = first;
    last = new;

    first->prev = last;

    escritas++;
}

void secondChance(unsigned addr){
    Page_t *new = current;
    int found = 1;
    while(found){
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
            current = new->next;
            found = 0;
            break;
        }
        new = new->next;

        current = new;
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
    for(int i = 0; i < totalUsed; i++)
    {
        if(aux->addr == address){
            aux->ref = 1;
            current = aux->next;
            return aux;
        }
        aux = aux->next;
    }
    return NULL;
}

void printList(){
    Page_t *aux = first;
    for(int i = 0; i < totalUsed; i++){
        printf("%X      %x       %x     %d\n", aux->prev->addr, aux->addr, aux->next->addr, aux->ref);
        aux = aux->next;
    }
    printf("\n ------------------------------------\n");
}

int findS(int pag){
    int s = 0;
    int temp = pag * 1024;
    while(temp > 1){
        temp = temp >> 1;
        s++;
    }
    return s;
}

int main(int argc, char* argv[]){
    FILE *file; 
    unsigned addr;
    char rw;
    // char *sub = argv[1];
    // file = fopen(argv[2], "r");
    // int pag = atoi(argv[3]);
    // int mem = atoi(argv[4]); 
    char *sub = "lru";
    file = fopen("teste2a.log", "r");
    int pag = 4;
    int mem = 12;
    int s = findS(pag);
    int totalPages = mem/pag;
    first = (Page_t*)malloc(sizeof(Page_t));
    last = (Page_t*)malloc(sizeof(Page_t));
    first = NULL;
    last = NULL;
    while(fscanf(file,"%x %c",&addr,&rw) != -1){
        //printf("%x",addr);
        addr = addr >> s;
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
                lruListModifier(addr);
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
                lruListModifier(addr);
            }
            lidas++;
        }
        printList();
    }
    fclose(file);
}
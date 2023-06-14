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
}Page_t;

Page_t *first, *last;

void AddNew(unsigned addr){
    Page_t *current = (Page_t*)malloc(sizeof(Page_t));
    current->addr = addr;
    current->next = NULL;
    if(totalUsed == 0){
        first = current;
        last = current->next;
    }
    else{
        last->next = current;
        last = current;
    }
    totalUsed++;
    escritas++;
}

void writeAddress(addr, sub, totalPages){
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

Page_t* AlreadyWritten(address){
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
        if(rw == 'W' || rw == 'w'){
            if(AlreadyWritten(addr) == NULL){
                if(totalUsed < totalPages){
                    AddNew(addr);
                }
                else{
			        writeAddress(addr, sub, totalPages);
                }faults++;
            }
		}
        else if(rw == 'R' || rw == 'r'){
            if(AlreadyWritten(addr) == NULL){
                if(totalUsed < totalPages){
                    AddNew(addr);
                }else{
                    writeAddress(addr, sub, totalPages);
                }faults++;
            }
            lidas++;
        }
    }
    fclose(file);
}
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

typedef struct Page {
	unsigned addr;
	Page *next;	
}Page;

Page *first, *last;
Page array[] =

void AddNew(unsigned addr){
    Page *new = (Page*)malloc(sizeof(Page));
    new->addr = addr;
    new->next = NULL;

    if(totalUsed == 0){
        first = new;
        last = new;
    }
    else{
        last->next = new;
        last = new;
    }
    totalUsed++;
    escritas++;
}

void writeAddress(addr, sub, totalPages){
    if(totalUsed < totalPages){
        AddNew(addr);
    }else{
        if(strcmp(sub, "lru") == 0){

        }
        else if(strcmp(sub, "2a") == 0){
            
        }
        else if(strcmp(sub, "random") == 0){

        }
        else if(strcmp(sub, "fifo") == 0){
            
        }
    }
}

int main(int argc, char* argv[]){
    FILE *file; 
    unsigned addr;
    char rw;
    char *sub = argv[1];
    file = fopen(argv[2], "r");
    int pag = argv[3];
    int mem = argv[4]; 
    int totalPages = mem/pag;
    while(fscanf(file,"%x %c",&addr,&rw) != -1){
        //printf("%x",addr);
        if(rw == 'W' || rw == 'w'){
			writeAddress(addr, sub, totalPages);
		}
        else if(rw == 'R' || rw == 'r'){
            if(!AddresExists(addr)){
                writeAddress(addr, sub, totalPages);
            }
            lidas++;
        }
    }
    fclose(file);
}
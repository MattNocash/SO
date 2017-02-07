#include <stdio.h>
#include <stdlib.h>
#include "listx.h"
#include "mikabooq.h"
#include "const.h"

//Allocazione statica delle MAXPROC strutture in memoria.
struct pcb_t pcb_mem[MAXPROC];

typedef struct pcb_t * ptr_pcb_t;

//Puntatore alla lista libera.
ptr_pcb_t ptr_pcblistfree_head;

//Puntatore al processo "root".
ptr_pcb_t root;

//Definizione della funzione proc_init
struct pcb_t *proc_init(void){
	short int i;

	//Collego il puntatore al primo elemento della lista libera.
	ptr_pcblistfree_head = &pcb_mem[1];

	//Collego i puntatori p_siblings formando la lista libera.
	for(i = 1; i < MAXPROC-2; i++){
		pcb_mem[i]->p_siblings.next = &pcb_mem[i+1];
		pcb_mem[i+1]->p_siblings.prev = &pcb_mem[i];
	}

	// Restituisco il puntatore al processo root.
	return &pcb_mem[0];
}



-------------- MAIN ---------------
int main{
ptr_pcb_t root = proc_init();


}
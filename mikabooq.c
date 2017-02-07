#include <stdio.h>
#include <stdlib.h>
#include "listx.h"
#include "mikabooq.h"
#include "const.h"


struct pcb_t pcb_mem[MAXPROC];
typedef struct pcb_t * ptr_pcb_t;
ptr_pcb_t ptr_pcblistfree_head;
//Definizione della funzione proc_init
struct pcb_t *proc_init(void){
	ptr_pcb_t tmpnext, tmpprev;
	ptr_pcblistfree_head = pcb_mem;
	tmp = ptr_pcblistfree_head;
	pcb_mem[0]->p_siblings.next = &pcb_mem[1];
	pcb_mem[1]->p_siblings.prev = &pcb_mem[0];


}

int main{
struct pcb_t * root = proc_init();


}
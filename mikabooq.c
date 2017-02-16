#include <stdio.h>
#include <stdlib.h>
#include "listx.h"
#include "mikabooq.h"
#include "const.h"

//Allocazione statica delle MAXPROC, MAXTHREAD e MAXMSG strutture in memoria.
struct pcb_t pcb_mem[MAXPROC];
struct tcb_t tcb_mem[MAXTHREAD];
struct msg_t msg_mem[MAXMSG];

// Head lista libera.
struct list_head listfree_head;

//Puntatore al processo "root".
struct pcb_t * root;

//Definizione della funzione proc_init
struct pcb_t *proc_init(void){
	short int i;
	//Inizializzo la testa della coda dei processo liberi, poi utilizzata come lista.
	INIT_LIST_HEAD(listfree_head);
	//Collego i puntatori p_siblings formando la lista libera.
	for(i = 1, i < MAXPROC, i++){
		//Setto a NULL i puntatori p_parent.
		INIT_LIST_HEAD(&(pcb_mem[i].p_siblings));
		INIT_LIST_HEAD(&(pcb_mem[i].p_children));
		INIT_LIST_HEAD(&(pcb_mem[i].p_threads));
		pcb_mem[i].p_parent = NULL;
		if(i > 0)
			list_add_tail(&(pcb_mem[i].p_siblings), &listfree_head);
	}
	// Restituisco il puntatore al processo root.
	return &pcb_mem[0];
}

struct pcb_t *proc_alloc(struct pcb_t *p_parent){
	struct pcb_t * new = NULL;
	struct list_head * curr; 
	if(p_parent != NULL){
		// Prendo dalla lista il primo processo libero.
		curr = list_next(&listfree_head);
		//Se la lista dei processi liberi non è vuota passo eseguo le operazioni.
		if(new != NULL){
			//Cancello dalla lista libera il processo.
			list_del(curr);
			//new = container_of(curr, struct pcb_t, p_siblings);
			// Lo aggiungo alla lista p_children del processo puntato da p_parent.
			list_add(curr, &(p_parent->p_children));
			//Ottengo il puntatore al nuovo processo attivo.
			new = container_of(curr, struct pcb_t, p_siblings);
			//Assegno a p_parent del nuovo processo il processo padre.
			new.p_parent = p_parent;
		}
		// Ritorno il puntatore al nuovo processo.
		return new;
	} else {
		return NULL;
	}
}

int proc_delete(struct pcb_t *oldproc){
	if(!list_empty(&(oldproc->p_children))){
		return(-1);
	} else if(!list_empty(&(oldproc->p_threads))){
		return(-1);
	} else {
		// Stacco i collegamenti ai fratelli
		list_del(&(oldproc->p_siblings));
		// Stacco il puntatore al padre.
		oldproc->p_parent = NULL;
		//Aggiungo il processo alla lista libra.
		list_add_tail(oldproc, &listfree_head);
		return 0;
	}
}

struct pcb_t *proc_firstchild(struct pcb_t *proc){
	return proc->p_children.next;
}

struct tcb_t *proc_firstthread(struct pcb_t *proc){
	return proc->p_threads.next;
}

//---------------threads-----------------------------

void thread_init(void){
	
}



//-------------- MAIN ---------------
int main{
ptr_pcb_t root = proc_init();


}
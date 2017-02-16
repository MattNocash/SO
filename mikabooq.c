#include "mikabooq.h"
#include "const.h"
#include <stdint.h>

//List heads, 3 sounds like free
struct list_head pcb_3;
struct list_head tcb_3;
struct list_head msg_3;

/************************************** PROC MGMT ************************/

/* initialize the data structure */
/* the return value is the address of the root process */
struct pcb_t *proc_init(void)
{
	static struct pcb_t pcb_array[MAXPROC]; //Static array of pcbs containing MAXPROC elements
	
	INIT_LIST_HEAD(&(pcb_3)); //Initialize pcb_3 head
	
	struct pcb_t *root = &(pcb_array[0]); //Assign first element of pcb_3 to root
	
	for(int i = 1; i < MAXPROC; i++) //Iterate for MAXPROC-1
	{
		struct pcb_t *new_pcb = &pcb_array[i]; //
		list_add(&(new_pcb -> p_siblings), &(pcb_3));
	}
	
	root -> p_parent = NULL; //Root has no parent
	INIT_LIST_HEAD(&(root -> p_children)); //Initialize root children
	INIT_LIST_HEAD(&(root -> p_threads)); //Initialize root threads
	
	return root; 
}

/* alloc a new empty pcb (as a child of p_parent) */
/* p_parent cannot be NULL */
struct pcb_t *proc_alloc(struct pcb_t *p_parent)
{
	
}

/* delete a process (properly updating the process tree links) */
/* this function must fail if the process has threads or children. */
/* return value: 0 in case of success, -1 otherwise */
int proc_delete(struct pcb_t *oldproc)
{
	
}

/* return the pointer to the first child (NULL if the process has no children) */
struct pcb_t *proc_firstchild(struct pcb_t *proc)
{
	
}

/* return the pointer to the first thread (NULL if the process has no threads) */
struct tcb_t *proc_firstthread(struct pcb_t *proc)
{
	
}

/****************************************** THREAD ALLOCATION ****************/

/* initialize the data structure */
void thread_init(void)
{
	
}

/* alloc a new tcb (as a thread of process) */
/* return -1 if process == NULL or mo more available tcb-s.
	 return 0 (success) otherwise */
struct tcb_t *thread_alloc(struct pcb_t *process)
{
	
}

/* Deallocate a tcb (unregistering it from the list of threads of
	 its process) */
/* it fails if the message queue is not empty (returning -1) */
int thread_free(struct tcb_t *oldthread)
{
	
}

/*************************** THREAD QUEUE ************************/

/* add a tcb to the scheduling queue */
void thread_enqueue(struct tcb_t *new, struct list_head *queue)
{
	
}

/* return the head element of a scheduling queue.
	 (this function does not dequeues the element)
	 return NULL if the list is empty */
struct tcb_t *thread_qhead(struct list_head *queue)
{
	
}

/* get the first element of a scheduling queue.
	 return NULL if the list is empty */
struct tcb_t *thread_dequeue(struct list_head *queue)
{
	
}

/*************************** MSG QUEUE ************************/

/* initialize the data structure */
/* the return value is the address of the root process */
void msgq_init(void)
{
	
}

/* add a message to a message queue. */
/* msgq_add fails (returning -1) if no more msgq elements are available */
int msgq_add(struct tcb_t *sender, struct tcb_t *destination, uintptr_t value)
{
	
}

/* retrieve a message from a message queue */
/* -> if sender == NULL: get a message from any sender
	 -> if sender != NULL && *sender == NULL: get a message from any sender and store
	 the address of the sending tcb in *sender
	 -> if sender != NULL && *sender != NULL: get a message sent by *sender */
/* return -1 if there are no messages in the queue matching the request.
	 return 0 and store the message payload in *value otherwise. */
int msgq_get(struct tcb_t **sender, struct tcb_t *destination, uintptr_t *value)
{
	
}















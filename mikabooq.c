#include "mikabooq.h"
#include "const.h"

//Free list heads, 3 sounds like free
struct list_head pcb_3;
struct list_head tcb_3;
struct list_head msg_3;

/************************************** PROC MGMT ************************/

/* initialize the data structure */
/* the return value is the address of the root process */
struct pcb_t *proc_init(void)
{
	static struct pcb_t pcb_array[MAXPROC]; //Static array of pcbs containing MAXPROC elements
	
	INIT_LIST_HEAD(&pcb_3); //Initialize pcb_3 head
	
	struct pcb_t *root = &pcb_array[0]; //Assign first element of free list to root
	root->p_parent = NULL; //Root process has no parent
	INIT_LIST_HEAD(&root->p_threads); //Initialize root threads
	INIT_LIST_HEAD(&root->p_children); //Initialize root children

	for(int i = 1; i < MAXPROC; i++) //Iterate for MAXPROC-1 times
	{
		struct pcb_t *new_pcb = &pcb_array[i]; // Pointer to working element of array
		list_add(&new_pcb->p_siblings, &pcb_3); //Use p_siblings to manage pcb free list
	}
	
	return root; // Return pointer to root process
}

/* alloc a new empty pcb (as a child of p_parent) */
/* p_parent cannot be NULL */
struct pcb_t *proc_alloc(struct pcb_t *p_parent)
{
	if(p_parent == NULL) return NULL; //Error case 1: a process cannot have no parent
	if(list_empty(&pcb_3)) return NULL; //Error case 2: no free process in list
	
	struct pcb_t *alloc_pcb = container_of(&pcb_3->next, struct pcb_t, p_siblings); //Create pointer to first unused item in free list

	list_del(&pcb_3->next); //Remove pcb from free list

	alloc_pcb->p_parent=p_parent; //Assign p_parent to pcb
	INIT_LIST_HEAD(&alloc_pcb->p_threads); //Initialize pcb threads
	INIT_LIST_HEAD(&alloc_pcb->p_children); //Initialize pcb children
	list_add(&alloc_pcb->p_siblings, &p_parent->p_children); //Assign pcb to parent's children

	return alloc_pcb; //Return pointer to created pcb
}

/* delete a process (properly updating the process tree links) */
/* this function must fail if the process has threads or children. */
/* return value: 0 in case of success, -1 otherwise */
int proc_delete(struct pcb_t *oldproc)
{
	if(!(list_empty(&oldproc->p_children))) return -1; //Error 1: cannot delete process if 
	if(!(list_empty(&oldproc->p_threads))) return -1; //it has active threads or children

	list_del(&oldproc->p_siblings); //Remove oldproc from parent`s children list
	oldproc->p_parent = NULL; //Remove reference to parent
	
	list_add(&oldproc->p_siblings, &pcb_3); //Add oldproc back to free list

	return 0;
}

/* return the pointer to the first child (NULL if the process has no children) */
struct pcb_t *proc_firstchild(struct pcb_t *proc)
{
	if(list_empty(&proc->p_children) return NULL; //Error 1: no children returns NULL
	
	return container_of(&(proc->p_children)->next, struct pcb_t, p_siblings); //Return pointer to process' first child
}

/* return the pointer to the first thread (NULL if the process has no threads) */
struct tcb_t *proc_firstthread(struct pcb_t *proc)
{
	if(list_empty(&proc->p_threads)) return NULL; //Error 1: no threads returns NULL
	
	return container_of(&(proc->p_threads)->next, struct tcb_t, t_next); //Return pointer to process' first thread
}

/****************************************** THREAD ALLOCATION ****************/

/* initialize the data structure */
void thread_init(void)
{
	static struct tcb_t tcb_array[MAXTHREAD]; //Declare static array of tcb of size MAXTHREAD

	INIT_LIST_HEAD(&tcb_3); //Initialize threads free list
	
	for(int i = 0; i<MAXTHREAD; i++) //Iterate for MAXTHREAD times
	{
		struct tcb_t *new_tcb = &tcb_array[i]; //Pointer to working element of array
		list_add(&new_tcb->t_next, &tcb_3); //Use t_next to manage tcb free list
	}
}

/* alloc a new tcb (as a thread of process) */
/* return -1 if process == NULL or mo more available tcb-s.
	 return 0 (success) otherwise */
struct tcb_t *thread_alloc(struct pcb_t *process)
{
	if(process == NULL) return -1; //Error 1: process cannot be NULL


	return 0; //Return 0 for success
}

/* Deallocate a tcb (unregistering it from the list of threads of
	 its process) */
/* it fails if the message queue is not empty (returning -1) */
int thread_free(struct tcb_t *oldthread)
{
	if(!(list_empty(&(oldthread->t_msgq))) return -1; //Error 1: message queue not empty


}

/*************************** THREAD QUEUE ************************/

/* add a tcb to the scheduling queue */
void thread_enqueue(struct tcb_t *new, struct list_head *queue)
{
	list_add(&(new->t_sched), &queue);
}

/* return the head element of a scheduling queue.
	 (this function does not dequeues the element)
	 return NULL if the list is empty */
struct tcb_t *thread_qhead(struct list_head *queue)
{
	if(list_empty(&queue)) return NULL; //Error 1: queue is empty

	return &queue->t_sched;
}

/* get the first element of a scheduling queue.
	 return NULL if the list is empty */
struct tcb_t *thread_dequeue(struct list_head *queue)
{
	if(list_empty(&queue)) return NULL; //Error 1: queue is empty

	
}

/*************************** MSG QUEUE ************************/

/* initialize the data structure */
/* the return value is the address of the root process */
void msgq_init(void)
{
	static struct msg_t msg_array[MAXMSG]; //Declare static array of msg of size MAXMSG

	INIT_LIST_HEAD(&msg_3); //Initialize messages free list
	
	for(int i = 0; i<MAXMSG; i++) //Iterate for MAXTHREAD times
	{
		struct msg_t *new_msg = &msg_array[i]; //Pointer to working element of array
		list_add(&(msg_tcb->m_next), &msg_3); //Use m_next to manage msg free list
	}

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















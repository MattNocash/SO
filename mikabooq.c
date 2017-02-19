/*****************************************************************************
 * mikabooq.c Year 2017 v.1.0 February 19, 2017                              *
 * Copyright 2017                                                            *
 *  Andrea Aldrovandi, Matteo Cassano, Marcello Mazzoleni, Soukaina Harrati   *
 *                                                                           *
 * This file is part of MiKABoO.                                             *
 *                                                                           *
 * MiKABoO is free software; you can redistribute it and/or modify it under  *
 * the terms of the GNU General Public License as published by the Free      *
 * Software Foundation; either version 2 of the License, or (at your option) *
 * any later version.                                                        *
 * This program is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General *
 * Public License for more details.                                          *
 * You should have received a copy of the GNU General Public License along   *
 * with this program; if not, write to the Free Software Foundation, Inc.,   *
 * 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.                  *
 *****************************************************************************/

#include "mikabooq.h"
#include "const.h"
#include <stdint.h>

//Free list heads, 3 sounds like free
struct list_head pcb_3;
struct list_head tcb_3;
struct list_head msg_3;

static struct pcb_t pcb_array[MAXPROC]; //Static array of pcbs containing MAXPROC elements
static struct tcb_t tcb_array[MAXTHREAD]; //Declare static array of tcb of size MAXTHREAD
static struct msg_t msg_array[MAXMSG]; //Declare static array of msg of size MAXMSG

/************************************** PROC MGMT ************************/

/* initialize the data structure */
/* the return value is the address of the root process */
struct pcb_t *proc_init(void)
{
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
	if(process == NULL) return NULL; //Error 1: process cannot be NULL
	if(list_empty(&tcb_3)) return NULL; //Free threads list is empty

	struct tcb_t * alloc_tcb = container_of(&tcb_3->next, struct tcb_t, t_next);

	list_del(&tcb_3->next); // Remove the new tcb from the free threads list.

	alloc_tcb->t_pcb = process; //Link thread to process

	INIT_LIST_HEAD(&t_next); //Initialize tcb thread list
	INIT_LIST_HEAD(&t_sched); //Initialize tcb schedule list
	INIT_LIST_HEAD(&t_msgq); //Initialize tcb message queue list

	list_add(&alloc_tcb->t_next, &process->p_threads); //Add tcb to process' threads list

	return alloc_tcb; //Return reference to the new thread for success
}

/* Deallocate a tcb (unregistering it from the list of threads of
	 its process) */
/* it fails if the message queue is not empty (returning -1) */
int thread_free(struct tcb_t *oldthread)
{
	if(!(list_empty(&(oldthread->t_msgq))) return -1; //Error 1: message queue not empty

	list_del(&oldthread->t_next); //Remove thread from list

	list_add(&(oldthread->t_next), &tcb_3); //Put thread back on free list

	return 0; //Return 0 for success

}

/*************************** THREAD QUEUE ************************/

/* add a tcb to the scheduling queue */
void thread_enqueue(struct tcb_t *new, struct list_head *queue)
{
	list_add(&new->t_sched, &queue); //Add tcb to queue
}

/* return the head element of a scheduling queue.
	 (this function does not dequeues the element)
	 return NULL if the list is empty */
struct tcb_t *thread_qhead(struct list_head *queue)
{
	if(list_empty(&queue)) return NULL; //Error 1: queue is empty

	struct tcb_t *thread_c = container_of(&queue, struct tcb_t, t_sched); //Find pointer to tcb struct from t_sched field


	struct list_head *temp_list = thread_c->t_pcb->p_threads.next; //Find first element of thread's process' queue
	return container_of(temp_list, struct tcb_t, t_next); //Find pointer to tcb struct from t_next
}

/* dequeue element from queue and return it
	 return NULL if the list is empty */
struct tcb_t *thread_dequeue(struct list_head *queue)
{
	if(list_empty(&queue)) return NULL; //Error 1: queue is empty

	list_del(&queue); 

	return container_of(&queue, struct tcb_t, t_sched);
	
}

/*************************** MSG QUEUE ************************/

/* initialize the data structure */
void msgq_init(void)
{
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
	if(list_empty(&msg_3)) return -1; //Error 1: free list of messages empty
	if(sender == NULL) return -1; //Error 2: No sender specified
	if(destination == NULL) return -1; //Error 3: no destination specified

	struct msg_t *new_msg = container_of(&msg_3->next, struct msg_t, m_next); //Find pointer to first element of free list

	list_del(&msg_3->next); //Remove element from free list

	new_msg->m_sender = sender; //Assign sender
	new_msg->m_value = value; //Assign value

	list_add(&new_msg->m_next, &destination->t_msgq); //Add message to thread message queue

	return 0; //Return 0 for success
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
	struct msgq_t *new_msg; //Working variable

	if(sender == NULL) //Case 1
	{
		if(list_empty(&destination->t_msgq)) return -1; //No messages in queue

		new_msg = container_of(&destination->t_msgq->next, struct msg_t,m_next); //Assign first pending message of thread destination

		*value = new_msg->m_value; //Store message value

		list_del(&new_msg->m_next); //Delete message from thread queue
		list_add(&new_msg->m_next, &msg_3); //Add message to free list

		return 0; //Return 0 for case 1 success
	} else if(sender != NULL && *sender == NULL) { //Case 2
		if(list_empty(&destination->t_msgq)) return -1; //No messages in queue

		new_msg = container_of(&destination->t_msgq->next, struct msg_t,m_next); //Assign first pending message of thread destination
		
		*value = new_msg->m_value; //Store message value
		*sender = &new_msg->m_sender; //Store sender tcb address

		list_del(&new_msg->m_next); //Delete message from thread queue
		list_add(&new_msg->m_next, &msg_3); //Add message to free list

		return 0; //Return 0 for case 2 success
	} else if(sender != NULL && *sender != NULL) { //Case 3
		if(list_empty(&destination->t_msgq)) return -1; //No messages in queue

		int match = 0; //Checking variable
		list_for_each_entry(new_msg, &destination->t_msgq, m_next) //Iterate for every element in queue
		{
			if(new_msg->m_sender == *sender) //If sender is the same
			{
				match = 1; //Match!
				break; //Stop iterating
			}
		}

		if(match) //If message is found
		{
			*value = new_msg->m_value; //Store message value

			list_del(&new_msg->m_next); //Delete message from thread queue
			list_add(&new_msg->m_next, &msg_3); //Add message to free list
		
			return 0; //Return 0 for case 3 success
		}
	}
	
	return -1; //Error: No message matches any cases
}


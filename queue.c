
/* queue.c --- 
 * 
 * 
 * Author: Dave S. Lacroix
 * Created: Tue Oct 16 15:32:09 2018 (-0400)
 * Version: 
 * 
 * Description: This is the implementation of my queue.h 
 * 
 */

#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>


//Define what a queue node is
typedef struct queue_node {
	void *info_ptr;
	struct queue_node *next_ptr;
} queue_node_t;


//Define my queue
typedef struct queue {
	queue_node_t *front;
	queue_node_t *rear;
} queue_tt;
	


//Creating an empty queue
queue_t* qopen(void){
	queue_tt *qp;
	//If malloc is successful in allocating space proceed to create an empty queue
	if((qp=(queue_tt*)malloc(sizeof(queue_tt)))!=NULL) { 				
		qp->front = NULL;
		qp->rear = NULL;
		return (queue_tt*)qp;
	}
	else //If malloc is unsuccessful return NULL
		return NULL;
}


//Putting an element at the end of the queue
int32_t qput(queue_t *qp, void *elementp) {
	queue_node_t *new_rear;

	//If we are able to create a new node, go ahead and do all of this.
	if((new_rear=(queue_node_t*)malloc(sizeof(queue_node_t)))!=NULL) { 
		queue_tt *new_queue = (queue_tt*)malloc(sizeof(queue_tt));
		new_queue = (queue_tt*)qp; 
		new_rear->info_ptr=elementp; //You are replacing whatever information is in the new_rear object with what is passed in. You are setting the infromation equal to the element. 
		new_rear->next_ptr=NULL; //If we are making new rear the last node in the list than we want whatever the next points to to be the null.

		if(new_queue->front == NULL) { //If the front of the queue is equal to NULL than we set both the front and the rear to whatever the new rear is.
			new_queue->front = new_rear;
			new_queue->rear = new_rear;
		}
		else {
			new_queue->rear->next_ptr=new_rear; //Take the next pointer of the current rear and make it point to the new rear.
			new_queue->rear=new_rear; //Make the rear equal to the new rear
		}
		return 0;
	}
	else
		return -1;
}



//Deallocating a Queue and freeing everything in it
void qclose(queue_t *qp) {
	queue_tt *new_queue = ((queue_tt*)qp);
	queue_node_t *curr;
	//This is a pointer that makes the current node point to the front of our new queue structure. I wanted to have a current that will close each of the nodes
	//and then close the queue itself. 
	curr = new_queue->front; 
	while(curr != NULL) { //This is saying whilst what the front of the queue is not equal to NULL make the front of the new queue point to the next of the current. 
		new_queue->front = curr->next_ptr;
		free(curr->info_ptr);//Here you are accessing the info_ptr and freeing it which would free that object.
	  free(curr); //Here you are freeing the node
		curr->next_ptr = new_queue->front; //Although you freed the data that existed in the current node, you set it's next equal to the front of the queue. 
	}
	free(new_queue);
	}

 
//Get the first element from the queue, removing it from the queue
void* qget(queue_t *qp){
	queue_tt *new_queue;
	new_queue=((queue_tt*)qp);
	queue_node_t *first_node = new_queue->front;

	if(first_node == NULL) {
		return NULL;
	}
	new_queue->front = first_node->next_ptr;
	void *first_Element = first_node->info_ptr;
	free(first_node);
	return first_Element;
}



//This is just going to be a simple print function
void qapply(queue_t *qp, void (*fn)(void* elementp)) {
	queue_tt *new_queue = (queue_tt*)qp;
	queue_node_t *p; //This allows me to traverse my LinkedList

	for(p=new_queue->front; p != NULL; p = p->next_ptr) {
		//This will allow me to do my function call to all of the elements in my list
		fn(((void*)p->info_ptr));
	}
}



/* search a queue using a supplied boolean function                             
 * skeyp -- a key to search for                                                 
 * searchfn -- a function applied to every element of the queue                 
 *          -- elementp - a pointer to an element                               
 *          -- keyp - the key being searched for (i.e. will be                  
 *             set to skey at each step of the search                           
 *          -- returns TRUE or FALSE as defined in bool.h                       
 * returns a pointer to an element, or NULL if not found
 */                        
void* qsearch(queue_t *qp, 
							bool (*searchfn)(void* elementp,const void* keyp),
							const void* skeyp) {
	queue_tt *new_queue = (queue_tt*)qp;
	queue_node_t *searched_node;
	bool Alive; //This will be a true or false statement saying whether or not I was able to find what I was looking for.
	//This is saying if the front of the queue is empty, we have nothing to search for therefore return NULL.
	if (new_queue->front = NULL) {
		return NULL;
	}
	searched_node = new_queue->front;
	while (searched_node != NULL) {
		Alive = searchfn(((void*)searched_node->info_ptr), ((void*)skeyp));
		//While our Boolean Statement is False, so we have not found what we are looking for, go tot he next node in the queue
		if(!Alive)
			searched_node=searched_node->next_ptr;
		//If we have found what we are looking for, then return the information from that node
		else
			return(void*)searched_node->info_ptr;
	}
	//Since this is a pointer to void, once this program is done, we do not want to return anything.
	return NULL;	
}



/* search a queue using a supplied boolean function (as in qsearch),
 * removes the element from the queue and returns a pointer to it or
 * NULL if not found
 */
void* qremove(queue_t *qp,
							bool (*searchfn)(void* elementp,const void* keyp),
							const void* skeyp) {
	//When removing we need to take into account if we remove from the
	//front of the queue or anywhere else. It is not explicit
	//here so we must test for both cases. The logic of this code is
	//utilizing the search boolean function and seeing if what we are looking for is either
	//in the front or at the end of the list and then we remove it.

	//I have to search for the actual node and the previous node that I am searching for 
	queue_node_t *searched_node;
	queue_node_t *previous_node;

	//I also have to make a new queue but right when I do, I have to set that previous_node equal to NULL because I don't want the previous to have a value in it.
	queue_tt *new_queue = (queue_tt*)qp;
	previous_node = NULL;

	//This checks to see if the pointer to the front of the queue is equal to NULL. If it is, then you return NULL.  
	if(new_queue->front == NULL) {
		return NULL;
	}

	//Just like we did in the search function, we make the node we are searching for point to the front of the new queue. This essentially allows us
	//to make make the first node vanish 
	searched_node = new_queue->front;
	
	if(searchfn(((void*)searched_node->info_ptr), ((void*)skeyp))) {
		new_queue->front=new_queue->front->next_ptr;
		skey = searched_node->info_ptr;
		free(searched_node);
		return (void*)searched_node->info_ptr;
	}

	//Now we have to check and see that we haven't found what we wanted and that we are no longer at the front
	while(searchfn(((void*)searched_node->info_ptr), ((void*)skeyp)) && searched_node != NULL) {
		previous_node=searched_node;
		searched_node = searched_node->next_ptr;
	}


	//After checking for seeing if what the user is looking for is
	//either in the front of the list or anywhere in the list, you will
	//want to return NULL because in this case, the information that the
	//user is looking for is not in the queue.
	if(temp == NULL)
		return NULL;
	

	skey = searched_node->info_ptr;
	previous_node->next_ptr = searched_node->next_ptr;
	void* new_element;
	new_element = searched_node->info_ptr;
	free(searched_node);
	return new_element;
	
}

/* concatenatenates elements of q2 into q1
 * q2 is dealocated, closed, and unusable upon completion 
 */
void qconcat(queue_t *q1p, queue_t *q2p) {

}


//
// Created by fuhchang on 10/7/15.
//
#include "tldlist.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "date.h"
typedef struct tldlist{
	TLDNode *root;
	Date *begin;
    Date *end;
	int count;
	int size;
} TLDList;
typedef struct tldnode{
	char *currentNode;
	TLDNode *leftChild;
	TLDNode *rightChild;
	//number of child
	long count;
} TLDNode;
typedef struct tlditerator{
	TLDList *tld;
	int i;
	int size;
	TLDNode *next;
} TLDIterator;

TLDNode *addTldNode(TLDList *tld, char *currentNode, TLDNode *node);

int getNodeHeight(TLDNode *node);

int getNodeDiff(TLDNode *node);

TLDNode *balance(TLDNode *node);

TLDNode *rightRightRotation(TLDNode *parentNode);
TLDNode *rightLeftRotation(TLDNode *parentNode);
TLDNode *leftLeftRotation(TLDNode *parentNode);
TLDNode *leftRightRotation(TLDNode *parentNode);

void iteradd(TLDIterator *iter, TLDNode *node, int *i);
TLDNode *newtldnode(char *currentNode);
/*
 * tldlist_create generates a list structure for storing counts against
 * top level domains (TLDs)
 *
 * creates a TLDList that is constrained to the `begin' and `end' Date's
 * returns a pointer to the list if successful, NULL if not
 */
TLDList *tldlist_create(Date *begin, Date *end){
	
}

/*
 * tldlist_destroy destroys the list structure in `tld'
 *
 * all heap allocated storage associated with the list is returned to the heap
 */
void tldlist_destroy(TLDList *tld){
	free(tld);
}

/*
 * tldlist_add adds the TLD contained in `hostname' to the tldlist if
 * `d' falls in the begin and end dates associated with the list;
 * returns 1 if the entry was counted, 0 if not
 */
int tldlist_add(TLDList *tld, char *hostname, Date *d){
	
}

TLDNode *addTldNode(TLDList *tld, char *currentNode, TLDNode *node){

}
/*
 * tldlist_count returns the number of successful tldlist_add() calls since
 * the creation of the TLDList
 */
long tldlist_count(TLDList *tld){
	return tld->count;
}


/*
 * tldlist_iter_create creates an iterator over the TLDList; returns a pointer
 * to the iterator if successful, NULL if not
 */
TLDIterator *tldlist_iter_create(TLDList *tld){
	 
}

void iteradd(TLDIterator *iter, TLDNode *node, int *i){

}
/*
 * tldlist_iter_next returns the next element in the list; returns a pointer
 * to the TLDNode if successful, NULL if no more elements to return
 */
TLDNode *tldlist_iter_next(TLDIterator *iter){

}


/*
 * tldlist_iter_destroy destroys the iterator specified by `iter'
 */
void tldlist_iter_destroy(TLDIterator *iter){
	free(iter);
}


/*
 * tldnode_tldname returns the tld associated with the TLDNode
 */
char *tldnode_tldname(TLDNode *node){
	return node->currentNode;
}

/*
 * tldnode_count returns the number of times that a log entry for the
 * corresponding tld was added to the list
 */
long tldnode_count(TLDNode *node){
	return node->count;
}

TLDNode *balance(TLDNode *node){
	int diff;
	diff = getNodeDiff(node);
	if(diff > 1){
		if(getNodeDiff(node->leftChild) > 0){
			node = leftLeftRotation(node);
		}else{
			node = leftRightRotation(node);
		}
	}else if(diff < -1){
		if(getNodeDiff(node->rightChild)>0){
			node = rightLeftRotation(node);
		}else{
			node = rightRightRotation(node);
		}
	}
	return node;
}

int getNodeHeight(TLDNode *node){
	int height =0;
	int leftT;
	int rightT;
	int last;
	if(node != NULL){
		leftT = getNodeHeight(node->leftChild);
		rightT = getNodeHeight(node->rightChild);
		if(leftT > rightT){
			last = leftT;
		}else{
			last = rightT;
		}
		height = last+1;
	}
	return height;
}

int getNodeDiff(TLDNode *node){
	int diff;
	int leftT;
	int rightT;

	leftT = getNodeHeight(node->leftChild);
	rightT = getNodeHeight(node->rightChild);
	diff = leftT - rightT;
	return diff;

}

TLDNode *rightRightRotation(TLDNode *parentNode){
	TLDNode *temp;
	temp = parentNode->rightChild;
	temp->leftChild = parentNode;
	return temp;
}
TLDNode *rightLeftRotation(TLDNode *parentNode){
	TLDNode *temp;
	temp = parentNode->rightChild;
	parentNode->rightChild = rightRightRotation(temp);
	return rightRightRotation(parentNode);
}
TLDNode *leftLeftRotation(TLDNode *parentNode){
	TLDNode *temp;
	temp = parentNode->leftChild;
	temp->rightChild = parentNode;
	return temp;
}
TLDNode *leftRightRotation(TLDNode *parentNode){
	TLDNode *temp;
	temp = parentNode->leftChild;
	parentNode->leftChild = leftLeftRotation(temp);
	return leftLeftRotation(parentNode);
}
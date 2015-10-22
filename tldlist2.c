#include "tldlist.h"
#include "date.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


struct tldlist{
	TLDNode *mainnode;
	Date *begindate;
	Date *enddate;
	char *tldname;
	long totalsize;
};
struct tldnode{
	TLDNode *leftchild;
	TLDNode *rightchild;
	char *tldstr;
	long numberoftimes;
};
struct tlditerator{
	TLDList *tld;

	int i;
	long size;

	TLDNode **nextnode;


};
static void placenodeiniterator(TLDNode *node, TLDIterator *tlditer, int *i);
static TLDNode *rrr(TLDNode *node);
static TLDNode *llr(TLDNode *node);
static TLDNode *lrr(TLDNode *node);
static TLDNode *rlr(TLDNode *node);
static int getbalancefactor(TLDNode *node);
static int getheight(TLDNode *node);
static TLDNode *balancetree(TLDNode *node);
static TLDNode *tldNodeCreate(TLDList *tld, char *tldname, TLDNode *nodecreate);
static void moveintoNodes(char *tldname, TLDNode *node, TLDList *tld);
/*
 * tldlist_create generates a list structure for storing 
s against
 * top level domains (TLD
 *
 * creates a TLDList that is constrained to the `begin' and `end' Date's
 * returns a pointer to the list if successful, NULL if not
 */
TLDList *tldlist_create(Date *begin, Date *end){
	TLDList *createdlist = (TLDList *)malloc(sizeof(TLDList));
	if(createdlist != NULL){
		createdlist->mainnode = NULL;
		createdlist->totalsize = 0;
		createdlist->begindate = begin;
		createdlist->enddate = end;
	}
	else{
		tldlist_destroy(createdlist);
	}
	return createdlist;

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
static void moveintoNodes(char *tldname, TLDNode *node, TLDList *tld){
	if(node != NULL){
		if(strcmp(tldname, node->tldstr) > 0 ) {
			TLDNode *randomnode = tldNodeCreate(tld, tldname, node->rightchild);
			printf("%s:%s\n", randomnode->tldstr, tldname);
			node->rightchild = randomnode;
			//printf("%s:%s\n", node->rightchild->tldstr, tldname);
			balancetree(node);
		}
		else if(strcmp(tldname, node->tldstr) < 0){
			TLDNode *randomnode = tldNodeCreate(tld, tldname, node->leftchild);
			printf("%s:%s\n", randomnode->tldstr, tldname);
			node->leftchild = randomnode;
			//printf("%s:%s\n", node->leftchild->tldstr, tldname);
			balancetree(node);
		}
		else{
			free(tldname);
			node->numberoftimes++;
		}
	}
}
int tldlist_add(TLDList *tld, char *hostname, Date *d){

	if(date_compare(tld->begindate, d) > 0 || date_compare(tld->enddate, d) < 0){
		//printf("%s\n", hostname);
		return 0;
	}


	int i, count=0, counter=0;
	for(i = ((strlen(hostname))); i >=0 ; i--){
		if(hostname[i] == '.'){
			count = i;
			break;
		}
		else{
			counter++;
		}
	}
	int lengthofstring = strlen(hostname+count+1);
	char *tlddomainname = (char *)malloc(sizeof(lengthofstring+1));
	tlddomainname = hostname+count+1;
	//printf("%s\n", tlddomainname);
	if(tld->mainnode == NULL){
		tld->mainnode = tldNodeCreate(tld, tlddomainname, tld->mainnode);
	}
	else{
		moveintoNodes(tlddomainname, tld->mainnode, tld);
	}


	//printf("%s\n", tld->mainnode->tldstr);
	return 1;

}
static TLDNode *tldNodeCreate(TLDList *tld, char *tldname, TLDNode *nodecreate){

	if(nodecreate == NULL) {
		TLDNode *node = (TLDNode *) malloc(sizeof(TLDNode));
		node->tldstr = tldname;
		node->leftchild = NULL;
		node->rightchild = NULL;
		node->numberoftimes = 1;
		tld->totalsize++;
		nodecreate = node;
		//printf("%s\n", nodecreate->tldstr);
		return nodecreate;
	}
	return nodecreate;
}
/*void iteraddr(TLDIterator *iter, TLDNode *node, int *i){

	
	 if (node->leftchild){
        	iteraddr(iter, node->leftchild, i);
	 }
    
    	*(iter->nextnode + (*i)++) = node;
    
	if (node->rightchild){
	        iteraddr(iter, node->rightchild, i);
	}
	

}*/
/*
 * tldlist_count returns the number of successful tldlist_add() calls since
 * the creation of the TLDList
 */
long tldlist_count(TLDList *tld){
	return tld->totalsize;
}

/*
 * tldlist_iter_create creates an iterator over the TLDList; returns a pointer
 * to the iterator if successful, NULL if not
 */
TLDIterator *tldlist_iter_create(TLDList *tld){

	TLDIterator *tlditer = (TLDIterator *)malloc(sizeof(TLDIterator));
	if(tlditer != NULL){
		tlditer->tld = tld;
		tlditer->size = tld->totalsize;
		tlditer->i = 0;
		tlditer->nextnode = (TLDNode **)malloc(sizeof(TLDNode)* tlditer->size);
		int i=0;
		//int size = (int)tlditer->size;
		if(tlditer->nextnode == NULL){
			tldlist_iter_destroy(tlditer);
			return NULL;
		}
		placenodeiniterator(tld->mainnode, tlditer, &i);

		return tlditer;
	}
	else{
		free(tlditer);
		return NULL;
	}
}

static void placenodeiniterator(TLDNode *node, TLDIterator *tlditer,int *count1){
	if(node != NULL){
		if(node->rightchild){
			placenodeiniterator(node->rightchild, tlditer, count1);
		}
		*(tlditer->nextnode + (*count1)++) = node;

		if(node->leftchild){
			//count1--;
			placenodeiniterator(node->leftchild, tlditer, count1);
		}
	}
}
/*
 * tldlist_iter_next returns the next element in the list; returns a pointer
 * to the TLDNode if successful, NULL if no more elements to return
 */
TLDNode *tldlist_iter_next(TLDIterator *iter){

	if (iter->i == iter->size){
		return NULL;
	}
	return *(iter->nextnode + iter->i++);

}

/*
 * tldlist_iter_destroy destroys the iterator specified by `iter'
 */
void tldlist_iter_destroy(TLDIterator *iter){
	free(iter->nextnode);
	free(iter);
}

/*
 * tldnode_tldname returns the tld associated with the TLDNode
 */
char *tldnode_tldname(TLDNode *node){
	//printf("%s\n", node->tldstr);
	return node->tldstr;
}

/*
 * tldnode_count returns the number of times that a log entry for the
 * corresponding tld was added to the list
 */
long tldnode_count(TLDNode *node){
	printf("%d\n", (int)node->numberoftimes);
	return node->numberoftimes;
}


static int getheight(TLDNode *node){
	int height, left, right, prev;
	height = 0;
	if(node != NULL){
		left = getheight(node->leftchild);
		right = getheight(node->rightchild);
		if(left > right){
			prev = left;
		}
		else{
			prev = right;
		}
		height =prev++;
	}
	//printf("%d\n", height);
	return height;
}
static int getbalancefactor(TLDNode *node){
	int bf, left, right;

	left = getheight(node->leftchild);
	right = getheight(node->rightchild);
	bf = left-right;
	return bf;
}
static TLDNode *llr(TLDNode *node){
	TLDNode *tempnode;

	tempnode = node->leftchild;

	node->leftchild = tempnode->rightchild;

	tempnode->rightchild = node;

	return tempnode;
}
static TLDNode *rrr(TLDNode *node){
	TLDNode *tempnode;

	tempnode = node->rightchild;

	node->rightchild = tempnode->leftchild;

	tempnode->leftchild = node;

	return tempnode;
}
static TLDNode *lrr(TLDNode *node){
	TLDNode *tempnode;

	tempnode = node->leftchild;


	node->leftchild = rrr(tempnode);


	return llr(node);
}
static TLDNode *rlr(TLDNode *node){
	TLDNode *tempnode;
	tempnode = node->rightchild;

	node->rightchild = llr(tempnode);

	return rrr(node);
}
static TLDNode *balancetree(TLDNode *node){

	int balancefactor;
	balancefactor = getbalancefactor(node);
	if (balancefactor >= 2)
	{
		if (getbalancefactor(node->leftchild) >0)
		{
			node = llr(node);
		}
		else
		{
			node = lrr(node);
		}
	}
	else if (balancefactor <= -2)
	{
		if (getbalancefactor(node->rightchild) > 0)
		{
			node = rlr(node);
		}
		else
		{
			node = rrr(node);
		}
	}
	return node;
}


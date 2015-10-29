#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tldlist.h"

static TLDNode *insert_node(TLDList *tld, TLDNode *node, char *tldDomain);
static void inorder_iter_push(TLDNode *node, TLDIterator *iter, int *i);
static int getheight(TLDNode *node);
static TLDNode *LL(TLDNode *node);
static TLDNode *RR(TLDNode *node);
static TLDNode *LR(TLDNode *node);
static TLDNode *RL(TLDNode *node);
static int checkDiff(TLDNode *node);
static TLDNode *balance(TLDNode *node);

struct tldlist {
	//declare variables for tldlist
	int numOfNodes;
	Date *beg;
	Date *end;
	int size;
	TLDNode *root;
};

struct tldnode {
	//declare variables for tldnode
	long frequency;
	int height;
	TLDNode *left;
	TLDNode *right;
	char *tldDomain;
};

struct tlditerator {
	//declare variables for tlditerator
	TLDNode **nodes;
	long size;
	long index;
	long point;
};

TLDList *tldlist_create(Date *begin, Date *end) {
	TLDList *dList=(TLDList *)malloc(sizeof(TLDList));
	if (dList==NULL){
		tldlist_destroy(dList);
	}else{
		//creates a TLDList for `begin' and `end' date when date value is not NULL
		dList->beg = begin;
		dList->end = end;
		dList->root = NULL;
	}
	return dList;
}

//destroys the list structure in `tld'
void tldlist_destroy(TLDList *tld) {
	free(tld);
}


static TLDNode *balance(TLDNode *node) {
	int nodeDiff;
	nodeDiff=checkDiff(node);
	if (nodeDiff>1){
		int checkLeft=checkDiff(node->left);
		if (checkLeft>0){
			node=LL(node);
		}else{
			node=LR(node);
		}
	}else if(nodeDiff<-1){
		int checkRight=checkDiff(node->right);
		if (checkRight>0){
			node=RL(node);
		}else{
			node=RR(node);
		}
	};
	return node;
}

//if value is not NULL, compare size of domain to determine the position (left/right)
static TLDNode *insert_node(TLDList *tld, TLDNode *node, char *tldDomain) {
	if (node==NULL){
		TLDNode *n=(TLDNode *)malloc(sizeof(TLDNode));
		if (n!=NULL){
			n->tldDomain=tldDomain;
			n->left=NULL;
			n->right=NULL;
			n->height=1;
			tld->size++;
			n->frequency=1;
			node=n;
		}
	}else{
		int cmp=strcmp(tldDomain, node->tldDomain);
		if (cmp<0){
			node->left=insert_node(tld, node->left, tldDomain);
		}else if(cmp>0){
			node->right=insert_node(tld, node->right, tldDomain);
		}else if(cmp==0){
			free(tldDomain);
			node->frequency++;
		}
	}
	node=balance(node);
	return node;
}

int tldlist_add(TLDList *tld, char *hostname, Date *d){
	//create node with 'hostname' to the tldlist when 'd' falls in the begin and end date

	if ((date_compare(tld->beg, d)<=0)&&(date_compare(tld->end, d)>=0)){
		//char *tldToken=strtok(strrchr(hostname, '.'), ".");
		int i;
		int len = strlen(hostname);
		i = len;
		int dotCount;
		while(i > 0){
			if(hostname[i] == '.'){
				dotCount = i;
				break;
			}
			i--;
		}
		char *tempChar = hostname+dotCount+1;
		int charLen = strlen(tempChar);
		char *tldname=(char *)malloc(charLen+1);
		if(tldname == NULL){
			free(tldname);
			return NULL;
		}
		tldname[charLen]='\0';
		for(i=0;i<charLen;i++){
			tldname[i] = tempChar[i];
		}
		tld->root =insert_node(tld, tld->root, tldname);
		tld->numOfNodes++;
		return 1;
	}
	return 0;
}

//returns the num of successful tldlist_add() calls since the creation of the TLDList
long tldlist_count(TLDList *tld){
	return tld->numOfNodes;
}

static void inorder_iter_push(TLDNode *node, TLDIterator *iter, int *i){
	if (node->left){
		inorder_iter_push(node->left, iter, i);
	}
	iter->nodes[iter->point++]=node;

	if (node->right){
		inorder_iter_push(node->right, iter, i);
	}
}

TLDIterator *tldlist_iter_create(TLDList *tld){
	TLDIterator *iter=(TLDIterator *)malloc(sizeof(TLDIterator));
	if (iter==NULL){
		free(iter);
		return NULL;
	}
	//creates an iterator over the TLDList when value is not NULL
	TLDNode **nodeArr=(TLDNode **)malloc(sizeof(TLDNode*)*tld->size);
	iter->nodes=nodeArr;
	iter->size=tld->size;
	iter->index=0;
	if (nodeArr==NULL){
		tldlist_iter_destroy(iter);
		return NULL;
	}
	int i=0;
	inorder_iter_push(tld->root, iter, &i);
	return iter;
}

/*
 * tldlist_iter_next returns the next element in the list; returns a pointer
 * to the TLDNode if successful, NULL if no more elements to return
 */
TLDNode *tldlist_iter_next(TLDIterator *iter){
	if (iter->index<iter->size){
		return iter->nodes[iter->index++];
	}
	return NULL;
}

/*
 * tldlist_iter_destroy destroys the iterator specified by `iter'
 */
void tldlist_iter_destroy(TLDIterator *iter){
	if (iter!=NULL){
		free(iter->nodes);
		free(iter);
	}
}

//returns the tld associated with the TLDNode
char *tldnode_tldname(TLDNode *node){
	return node->tldDomain;
}

//return the number of occurrence
long tldnode_count(TLDNode *node){
	return node->frequency;
}

static int checkDiff(TLDNode *node){
	int nodeDiff;
	int left;
	int right;
	left=getheight(node->left);
	right=getheight(node->right);
	nodeDiff=left-right;
	return nodeDiff;
}

//left left situation
static TLDNode *LL(TLDNode *node){
	TLDNode *temp;
	temp=node->left;
	//perform rotation
	node->left=temp->right;
	temp->right=node;
	node=temp;
	return node;
}

//right right situation
static TLDNode *RR(TLDNode *node){
	TLDNode *temp;
	temp=node->right;
	//perform rotation
	printf("%s\n", temp->tldDomain);
	node->right = temp->left;
	temp->left=node;
	node=temp;
	return node;
}

//left right situation
static TLDNode *LR(TLDNode *node){
	TLDNode *temp;
	temp=node->left;
	//perform rotation
	node->left=RR(temp);
	return LR(node);
}

//right left situation
static TLDNode *RL(TLDNode *node){
	TLDNode *temp;
	temp=node->right;
	//perform rotation
	node->right=LL(temp);
	return RR(node);
}

//return the height of current node when value is not NULL
static int getheight(TLDNode *node){
	int height=0;
	int left;
	int right;
	int last;
	if (node!=NULL){
		left=getheight(node->left);
		right=getheight(node->right);
		if (right>left){
			last=right;
		}else{
			last=left;
		}
		height=1+last;
	}else{
		return 0;
	}
	return height;
}
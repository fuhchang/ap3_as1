#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tldlist.h"

//static void insert_node(TLDNode **node,char *tldDomain);
static TLDNode *insert_node(TLDList *tld,TLDNode *node, char *tldDomain);
static long recursive_tldcount(TLDNode *n);
static long recursive_nodecount(TLDNode *n);
static void inorder_iter_push(TLDNode *node,TLDIterator *iter);
static void postorder_delete(TLDNode *node);
static int getheight(TLDNode *node);
//static int max(int l, int r);
static TLDNode *LL(TLDNode *node);
static TLDNode *RR(TLDNode *node);
static TLDNode *LR(TLDNode *node);
static TLDNode *RL(TLDNode *node);
static int checkDiff(TLDNode *node);
static TLDNode *balance(TLDNode *node);
struct tldlist{
	Date* beg;
	Date* end;
	int size;
	TLDNode* root; //the first node of the tree
};

struct tldnode{
	long frequency; //frequency
	int height;
	TLDNode* left; //left node of the tree
	TLDNode* right; //right node of the tree
	char *tldDomain; //used to store the last 3 char in the domain
};

struct tlditerator{
	TLDNode** nodes; //the position of the next node

	long size;
	long index;
};

/*
 * tldlist_create generates a list structure for storing counts against
 * top level domains (TLDs)
 *
 * creates a TLDList that is constrained to the `begin' and `end' Date's
 * returns a pointer to the list if successful, NULL if not
 */
TLDList *tldlist_create(Date *begin, Date *end){
	TLDList *dList=(TLDList *)malloc(sizeof(TLDList));
	if(dList==NULL){
		tldlist_destroy(dList);
	}else{
		dList->beg=date_duplicate(begin);
		dList->end=date_duplicate(end);
		dList->root=NULL;
	}return dList;
}

/*
 * tldlist_add adds the TLD contained in `hostname' to the tldlist if
 * `d' falls in the begin and end dates associated with the list;
 * returns 1 if the entry was counted, 0 if not
 */
int tldlist_add(TLDList *tld, char *hostname, Date *d){
	if((date_compare(tld->beg,d)<=0)&&(date_compare(tld->end,d)>=0)){
		//if date fall between or equals the begin and end
		//delimiting hostname and get the top level domain
		char* delimiter=".";
		char* tldDomain=NULL;
		char* tldToken=strtok(hostname, delimiter);
		while(tldToken!=NULL){
			tldDomain=tldToken;
			tldToken = strtok(NULL, delimiter);
		}
		//convert tldDomain to lowercase
		//comparision of signed int n unsigned long

		int i;
		for (i=0;i<(signed int)strlen(tldDomain);i++){
			tldDomain[i]=tolower((signed char)tldDomain[i]);
		}
		int len = strlen(tldDomain);
		char *tldTemp = (char *)malloc(len+1);
		tldTemp[len] = '\0';
		for(i=0;i<len;i++){
			tldTemp[i] = tldDomain[i];
		}
		//start to insert tldDomain to binary tree
		tld->root = insert_node(tld,tld->root,tldTemp);
		return 1;
	}else{
		return 0;
	}
}
static TLDNode *insert_node(TLDList *tld,TLDNode *node, char *tldDomain){
	if(node == NULL){
		TLDNode *node = (TLDNode *)malloc(sizeof(TLDNode));
		if(node != NULL){
			node->tldDomain = tldDomain;
			node->left = NULL;
			node->right = NULL;
			node->height = 1;
			tld->root = node;
			tld->size++;
		}else{
			free(node);
		}
		return node;
	}else{
		int cmp = strcmp(tldDomain, node->tldDomain);
		if(cmp < 0){
			node->left = insert_node(tld,node->left,tldDomain);
			node = balance(node);

		}else if(cmp > 0 ){
			node->right = insert_node(tld,node->right,tldDomain);
			node = balance(node);
		}
	}
		return node;
}

//left left situation
static TLDNode *LL(TLDNode *node){
	TLDNode *temp;
	temp=node->left;
	node->left=temp->right;
	temp->right= node;
	//node->height=max(height(node->left),height(node->right)) + 1;
	//temp->height=max(height(temp->left),node->height) + 1;
	node=temp;
	return node;
}
//right right situation
static TLDNode *RR(TLDNode *node){
	TLDNode *temp;
	temp= node->right;
	node->right=temp->left;
	temp->left= node;
	//node->height=max(height(node->left),height(node->right)) + 1;
	//temp->height=max(height(temp->right),node->height) + 1;
	node=temp;
	return node;
}
//left right situation
static TLDNode *LR(TLDNode *node){
	RR(node->left);
	return LL(node);
}
//right left situation
static TLDNode *RL(TLDNode *node){
	LL(node->right);
	return RR(node);
}
//return current node's height if not null, else return -1
static int getheight(TLDNode *node){
	int height =0;
	int left;
	int right;
	int last;
	if(node != NULL){
		left = getheight(node->left);
		right = getheight(node->right);
		if(right > left){
			last = right;
		}else{
			last = left;
		}
		height = 1+last;
	}else{
		return 0;
	}

	return height;
}
//compare the height between the left and right child and return the max
/*
static int max(int l,int r){
	return l > r ? l: r;
}
 */
//sums the frequency of every nodes in the tree, by using a recursive function
long tldlist_count(TLDList *tld){
	return recursive_tldcount(tld->root);
}
static long recursive_tldcount(TLDNode *n){
	if(n==NULL){
		return(0);
	}else{
		if(n->left==NULL&&n->right==NULL)
			return(n->frequency);
		else
			return(n->frequency+(recursive_tldcount(n->left)+recursive_tldcount(n->right)));
	}
}
//total counts of nodes in the tree
static long recursive_nodecount(TLDNode *n){
	if(n==NULL){
		return(0);
	}else{
		if(n->left==NULL&&n->right==NULL)
			return(1);
		else
			return(1+(recursive_tldcount(n->left)+recursive_tldcount(n->right)));
	}
}

/*
 * tldlist_iter_create creates an iterator over the TLDList; returns a pointer
 * to the iterator if successful, NULL if not
 */
//create a iter and a tldnode node array
TLDIterator *tldlist_iter_create(TLDList *tld){
	TLDIterator *iter=(TLDIterator *)malloc(sizeof(TLDIterator));
	if(iter!=NULL){
		TLDNode **nodeArr=(TLDNode **)malloc(recursive_nodecount(tld->root)*sizeof(TLDNode *));
		if(nodeArr==NULL){
			free(iter);
			iter=NULL;
		}else{
			iter->nodes=nodeArr;
			iter->size=0;
			iter->index=0;
			//pushing of nodes into the array
			inorder_iter_push(tld->root,iter);
		}
	}
	return iter;
}
//inorder traverse recursively and then push left, follow by parent then the right // check method 
static void inorder_iter_push(TLDNode *node,TLDIterator *iter){
	if(node==NULL){
		postorder_delete(node);
	}else{
		inorder_iter_push(node->left,iter);
		iter->nodes[iter->size++]= node;
		inorder_iter_push(node->right,iter);
	}
}

/*
 * tldlist_iter_next returns the next element in the list; returns a pointer
 * to the TLDNode if successful, NULL if no more elements to return
 */
//return node if there is one else return NULL
TLDNode *tldlist_iter_next(TLDIterator *iter){
	if(iter->index<iter->size){
		return iter->nodes[iter->index++];
	}
	return NULL;
}

/*
 * tldlist_destroy destroys the list structure in `tld'
 *
 * all heap allocated storage associated with the list is returned to the heap
 */
//delete the tree using recursive function, then the dates and lastly the tldlist
void tldlist_destroy(TLDList *tld){
	if(tld!=NULL){
		postorder_delete(tld->root);
		date_destroy(tld->beg);
		date_destroy(tld->end);
		free(tld);
	}
}
//recursively delete the left,right child then lastly parent node
static void postorder_delete(TLDNode *node){
	if(node!=NULL){
		postorder_delete(node->left);
		postorder_delete(node->right);
		free(node);
	}
}

/*
 * tldlist_iter_destroy destroys the iterator specified by `iter'
 */
//free the nodearray then free the iterator itself
void tldlist_iter_destroy(TLDIterator *iter){
	if(iter!=NULL){
		free(iter->nodes);
		free(iter);
	}
}

/*
 * tldnode_tldname returns the tld associated with the TLDNode
 */
//return node -> top level domain
char *tldnode_tldname(TLDNode *node){
	return node->tldDomain;
}

/*
 * tldnode_count returns the number of times that a log entry for the
 * corresponding tld was added to the list
 */
//return node -> top level domain count
long tldnode_count(TLDNode *node){
	return node->frequency;
}


static TLDNode *balance(TLDNode *node){
	int nodeDiff;
	nodeDiff = checkDiff(node);
	if(nodeDiff >1){
		int checkLeft = checkDiff(node->left);
		if(checkLeft > 0){
			node = LL(node);
		}else{
			node = LR(node);
		}
	}else if(nodeDiff < -1){
		int checkRight = checkDiff(node->right);
		if(checkRight > 0){
			node = RL(node);
		}else{
			node = RR(node);
		}
	};

	return node;
}

static int checkDiff(TLDNode *node){
	int nodeDiff;
	int left;
	int right;
	left = getheight(node->left);
	right = getheight(node->right);
	nodeDiff = left - right;

	return nodeDiff;
}
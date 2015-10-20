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
	TLDNode *leftChild;
	TLDNode *rightChild;
	//number of child
	char *tldstr;
	long count;
} TLDNode;
typedef struct TLDiterator{
	TLDList *tld;
	int i;
	int size;
	TLDNode **next;
} TLDiterator;
TLDNode *node = NULL;

 int getNodeHeight(TLDNode *node);

 int getNodeDiff(TLDNode *node);

TLDNode *balance(TLDNode *node);

TLDNode *rightRightRotation(TLDNode *Node);
TLDNode *rightLeftRotation(TLDNode *Node);
TLDNode *leftLeftRotation(TLDNode *Node);
TLDNode *leftRightRotation(TLDNode *Node);
TLDNode *addtldenode(TLDList *tld, char *tldstr, TLDNode *node);
TLDNode *create_tldnode(char *tldstr);
void TLDiteraddr(TLDiterator *TLDiter, TLDNode *node, int *i);

/*
 * tldlist_create generates a list structure for storing counts against
 * top level domains (TLDs)
 *
 * creates a TLDList that is constrained to the `begin' and `end' Date's
 * returns a pointer to the list if successful, NULL if not
 */
TLDList *tldlist_create(Date *begin, Date *end){
	TLDList *tld = (TLDList *)malloc(sizeof(TLDList));
	if(tld == NULL){
		return NULL;
	}
	tld->begin = begin;
	tld->end = end;

	tld->root = NULL;
	tld->count =0;
	tld->size =0;
	return tld;
}

/*
 * tldlist_add adds the TLD contained in `hostname' to the tldlist if
 * `d' falls in the begin and end dates associated with the list;
 * returns 1 if the entry was counted, 0 if not
 */
int tldlist_add(TLDList *tld, char *hostname, Date *d){
	if(date_compare(tld->begin, d) >0 || date_compare(tld->end, d) <0){
		return 0;
	}
	 char *tld_start = strrchr(hostname, '.') + 1;
	 printf("%s\n", tld_start);
    int tld_len = strlen(tld_start);
    char *tldstr = (char *)malloc(sizeof(char) * (tld_len + 1));
    tldstr[tld_len] = '\0'; // make sure there is a null end
    strcpy(tldstr, tld_start);

    tld->root = addtldenode(tld, tldstr, tld->root);
    tld->count++;
    return 1;
}

TLDNode *addtldenode(TLDList *tld, char *tldstr, TLDNode *node){
	if(node == NULL){
		node = create_tldnode(tldstr);
		tld->root = node;
		tld->size++;
	}else if(strcmp(tldstr, node->tldstr) >0){
		node->rightChild = addtldenode(tld, tldstr, node);
		node = balance(node);
	}else if(strcmp(tldstr, node->tldstr) < 0){
		node->leftChild = addtldenode(tld, tldstr, node);
		node = balance(node);
	}else{
		free(tldstr);
		node->count++;
	}
	return node;
}

/*
 * tldlist_destroy destroys the list structure in `tld'
 *
 * all heap allocated storage associated with the list is returned to the heap
 */
void tldlist_destroy(TLDList *tld){
	free(tld);
}


TLDNode *create_tldnode(char *tldstr){
	node = (TLDNode *)malloc(sizeof(TLDNode));
	if(node == NULL){
		free(node);
		return NULL;
	}
	node->tldstr = tldstr;
	node->leftChild = NULL;
	node->rightChild = NULL;
	node->count =1;

	return node;
}
/*
 * tldlist_count returns the number of successful tldlist_add() calls since
 * the creation of the TLDList
 */
long tldlist_count(TLDList *tld){
	return tld->count;
}

/*
 * tldlist_TLDiter_create creates an TLDiterator over the TLDList; returns a pointer
 * to the TLDiterator if successful, NULL if not
 */
TLDiterator *tldlist_TLDiter_create(TLDList *tld){
	TLDiterator *TLDiter = (TLDiterator *)malloc(sizeof(TLDiterator));
    
    if (TLDiter == NULL)
    {
        free(TLDiter);
        return NULL;
    }else {
        TLDiter->tld = tld;
        TLDiter->size = tld->size;
        TLDiter->i = 0;
        
        TLDiter->next = (TLDNode **)malloc(sizeof(TLDNode *) * TLDiter->size);
        if (TLDiter->next == NULL)
        {
            tldlist_TLDiter_destroy(TLDiter);
            return NULL;
        }
        
        int i = 0;
        TLDiteraddr(TLDiter, TLDiter->tld->root, &i);
        return TLDiter;
    }
}
void TLDiteraddr(TLDiterator *TLDiter, TLDNode *node, int *i)
{
    
    if (node->leftChild)
        TLDiteraddr(TLDiter, node->leftChild, i);
    
    *(TLDiter->next + (*i)++) = node;
    
    if (node->rightchild)
        TLDiteraddr(TLDiter, node->rightchild, i);
}
/*
 * tldlist_TLDiter_next returns the next element in the list; returns a pointer
 * to the TLDNode if successful, NULL if no more elements to return
 */
TLDNode *tldlist_TLDiter_next(TLDiterator *TLDiter){
	if (TLDiter->i == TLDiter->size)
        return NULL;
    
    return *(TLDiter->next + TLDiter->i++);
}

/*
 * tldlist_TLDiter_destroy destroys the TLDiterator specified by `TLDiter'
 */
void tldlist_TLDiter_destroy(TLDiterator *TLDiter){
	int i;
	for(i=0; i<TLDiter->size;i++){
		free(TLDiter->next[i]->node);
		free(TLDiter->next[i]);
	}
	free(TLDiter->next);
	free(TLDiter);
}

/*
 * tldnode_tldname returns the tld associated with the TLDNode
 */
char *tldnode_tldname(TLDNode *node){
	return node->tldstr;
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

TLDNode *rightRightRotation(TLDNode *Node){
	TLDNode *nodeTemp;
	nodeTemp = Node->rightChild;
	nodeTemp->leftChild = Node;
	return nodeTemp;
}
TLDNode *rightLeftRotation(TLDNode *Node){
	TLDNode *nodeTemp;
	nodeTemp = Node->rightChild;
	Node->rightChild = rightRightRotation(nodeTemp);
	return rightRightRotation(Node);
}
TLDNode *leftLeftRotation(TLDNode *Node){
	TLDNode *nodeTemp;
	nodeTemp = Node->leftChild;
	nodeTemp->rightChild = Node;
	return nodeTemp;
}
TLDNode *leftRightRotation(TLDNode *Node){
	TLDNode *nodeTemp;
	nodeTemp = Node->leftChild;
	Node->leftChild = leftLeftRotation(nodeTemp);
	return leftLeftRotation(Node);
}

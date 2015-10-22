#include <stdio.h>
#include "tldlist.h"
#include <stdlib.h>
#include "date.h"
#include <string.h>
#include <ctype.h>

static TLDNode *addnode(TLDList *tld, char *tldnodestr, TLDNode *node);
static TLDNode *newnode(char *tldnodestr);
static void additer(TLDIterator *iter, TLDNode *node, int *i);

static int getheight(TLDNode *node);
static int getDiff(TLDNode *node);
static TLDNode *rightRightRotation(TLDNode *node);
static TLDNode *rightLeftRotation(TLDNode *node);
static TLDNode *leftLeftRotation(TLDNode *node);
static TLDNode *leftRightRotation(TLDNode *node);
static TLDNode *balance(TLDNode *node);
char *tldstr = NULL;
TLDNode *node = NULL;

typedef struct tldlist {
    TLDNode *root;
    Date *begin;
    Date *end;
    long count;
    long size;
}TLDList;


typedef struct tldnode {
    TLDNode *leftChild;
    TLDNode *rightChild;
    
    char *tldnodestr;
    long count;
}TLDNode;


typedef struct tlditerator {
    TLDList *tld;
    
    int i;
    long size;
    TLDNode **next;
}TLDIterator;

/*
 * tldlist_create generates a list structure for storing counts against
 * top level domains (TLDs)
 *
 * creates a TLDList that is constrained to the `begin' and `end' Date's
 * returns a pointer to the list if successful, NULL if not
 */
TLDList *tldlist_create(Date *begin, Date *end)
{
    TLDList *tldlist = malloc(sizeof(TLDList));
    if (tldlist != NULL)
    {
        tldlist->count = 0;
        tldlist->size = 0;
        tldlist->root = NULL;
        tldlist->begin = begin;
        tldlist->end = end;
    }
    
    return tldlist;
    
}
/*
 * tldlist_add adds the TLD contained in `hostname' to the tldlist if
 * `d' falls in the begin and end dates associated with the list;
 * returns 1 if the entry was counted, 0 if not
 */
int tldlist_add(TLDList *tld, char *hostname, Date *d) {
    // check if it's within the tld dates
    if (date_compare(tld->begin, d) > 0 ||date_compare(tld->end, d) < 0)
        return 0;
    int i , counter=0;
    int len = strlen(hostname);
    for(i=len;i>0;i--){
        if(hostname[i] == '.'){
            counter=i;
            break;
        }
    }
    char *tempTLDnode = hostname+counter+1;
    int hostlen = strlen(tempTLDnode);
    tldstr = (char *)malloc((hostlen + 1));
    tldstr[hostlen] = '\0'; // make sure there is a null end
    
    for(i=0;i<hostlen;i++){
        tldstr[i] = tempTLDnode[i];
    }
    tld->root = addnode(tld, tldstr, tld->root);
    tld->count++;
    return 1;
}

TLDNode *addnode(TLDList *tld, char *tldnodestr, TLDNode *node) {
    
    if (node == NULL)
    {
        node = newnode(tldnodestr);
        tld->root = node;
        tld->size++;
        return node;
    }
    else if (strcmp(tldnodestr, node->tldnodestr) > 0)
    {
        node->rightChild = addnode(tld, tldnodestr, node->rightChild);
        node = balance(node);
    }
    else if (strcmp(tldnodestr, node->tldnodestr) < 0)
    {
        node->leftChild = addnode(tld, tldnodestr, node->leftChild);
        node = balance(node);
    }
    else
    {
        free(tldnodestr);
        node->count++;
    }
        
    return node;
}

/*
 * tldlist_count returns the number of successful tldlist_add() calls since
 * the creation of the TLDList
 */
long tldlist_count(TLDList *tld)
{
    return tld->count;
}
/*
 * tldlist_iter_create creates an iterator over the TLDList; returns a pointer
 * to the iterator if successful, NULL if not
 */
TLDIterator *tldlist_iter_create(TLDList *tld) {
    TLDIterator *iter = (TLDIterator *)malloc(sizeof(TLDIterator));
    
    if (iter != NULL)
    {
        iter->tld = tld;
        iter->size = tld->size;
        iter->i = 0;
        //memory leak location
        iter->next = (TLDNode **)malloc(sizeof(TLDNode *) * iter->size);
        if (iter->next == NULL)
        {   
            tldlist_iter_destroy(iter);
            return NULL;
        }else{
               int i = 0;
                additer(iter, iter->tld->root, &i);
                return iter;
        }
    }
    else {
        free(iter);
        return NULL;
    }
}

void additer(TLDIterator *iter, TLDNode *node, int *i)
{
    
    if (node->leftChild){
        additer(iter, node->leftChild, i);
    }
    
    *(iter->next + (*i)++) = node;
    
    if (node->rightChild){
        additer(iter, node->rightChild, i);
    }
}
/*
 * tldlist_iter_next returns the next element in the list; returns a pointer
 * to the TLDNode if successful, NULL if no more elements to return
 */
TLDNode *tldlist_iter_next(TLDIterator *iter)
{
    if (iter->i == iter->size)
        return NULL;
    
    return *(iter->next + iter->i++);
}

/*
 * tldnode_tldname returns the tld associated with the TLDNode
 */
char *tldnode_tldname(TLDNode *node)
{
    return node->tldnodestr;
}
/*
 * tldnode_count returns the number of times that a log entry for the
 * corresponding tld was added to the list
 */
long tldnode_count(TLDNode *node)
{
    return node->count;
}

TLDNode *newnode(char *tldnodestr)
{
    node = (TLDNode *)malloc(sizeof(TLDNode));
    if (node != NULL)
    {
        node->tldnodestr = tldnodestr;
        node->leftChild = NULL;
        node->rightChild = NULL;
        node->count = 1;
            
        return node;
    }  
    else
    {
      free(node);
      return NULL;
    }
}
/*
 * tldlist_destroy destroys the list structure in `tld'
 *
 * all heap allocated storage associated with the list is returned to the heap
 */
void tldlist_destroy(TLDList *tld)
{   
    free(tld);   
}

/*
 * tldlist_iter_destroy destroys the iterator specified by `iter'
 */
void tldlist_iter_destroy(TLDIterator *iter) {

int i;
for(i=0;i < iter->size; i++){
    free(iter->next[i]->tldnodestr);
    free(iter->next[i]);
}
    free(iter->next);
    free(iter);
}

int getheight(TLDNode *node)
{
    int ht = 0;
    int leftTree;
    int rightTree;
    int last;
    if(node == NULL){
        return 0;
    }else{
        leftTree = getheight(node->leftChild);
        rightTree = getheight(node->rightChild);
        if (leftTree > rightTree)
        {
            last = leftTree;
        }
        else
        {
            last = rightTree;
        }
        ht= 1+last;
    }
    return ht;
}

int getDiff(TLDNode *node)
{
    int diff;
    int leftTree;
    int rightTree;
    
    leftTree = getheight(node->leftChild);
    rightTree = getheight(node->rightChild);
    diff = leftTree - rightTree;
    
    return diff;
}

TLDNode *rightRightRotation(TLDNode *node)
{
    TLDNode *tempTLDnode;
    tempTLDnode = node->rightChild;
    node->rightChild = tempTLDnode->leftChild;
    tempTLDnode->leftChild = node;
    return tempTLDnode;
}

TLDNode *rightLeftRotation(TLDNode *node)
{
    TLDNode *tempTLDnode;
    tempTLDnode =node->rightChild;
    node->rightChild = leftLeftRotation(tempTLDnode);
    return rightRightRotation(node);
}

TLDNode *leftLeftRotation(TLDNode *node)
{
    TLDNode *tempTLDnode;
    tempTLDnode = node->leftChild;
    node->leftChild = tempTLDnode->rightChild;
    tempTLDnode->rightChild = node;
    return tempTLDnode;
}

TLDNode *leftRightRotation(TLDNode *node)
{
    TLDNode *tempTLDnode;
    tempTLDnode = node->leftChild;
    node->leftChild = rightRightRotation(tempTLDnode);
    return leftLeftRotation(node);
}

TLDNode *balance(TLDNode *node)
{
    int diff;
    diff = getDiff(node);
    if (diff > 1)
    {
        if (getDiff(node->leftChild) > 0)
        {
            node = leftLeftRotation(node);
        }
        else
        {
            node = leftRightRotation(node);
        }
    }
    else if (diff < -1)
        {
            if (getDiff(node->rightChild) > 0)
            {
                node = rightLeftRotation(node);
            }
            else
            {
                node = rightRightRotation(node);
            }
        }
    return node;
}
/*
 * Loi Fuh Chang
 * STI NO: 15AGC041S
 * UOG NO: 2228133L
 * I referenced from the following website:
 * http://video.mit.edu/watch/introduction-to-algorithms-lecture-6-avl-trees-avl-sort-14062/
 * http://anoopsmohan.blogspot.sg/2011/11/avl-tree-implementation-in-c.html
 * lecture Note Abstract Data Types in C
 */
#include <stdio.h>
#include "tldlist.h"
#include <stdlib.h>
#include "date.h"
#include <string.h>
#include <ctype.h>

static TLDNode *addnode(TLDList *tld, char *tldnodestr, TLDNode *node);
static void iter_adder(TLDIterator *iter, TLDNode *node, int *i);
static int getheight(TLDNode *node);
static int getDiff(TLDNode *node);
static TLDNode *RRRotation(TLDNode *node);
static TLDNode *RLRotation(TLDNode *node);
static TLDNode *LLRotation(TLDNode *node);
static TLDNode *LRRotation(TLDNode *node);
static TLDNode *balance(TLDNode *node);
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
    char *tldstr = (char *)malloc((hostlen + 1));
    tldstr[hostlen] = '\0'; // make sure there is a null end

    for(i=0;i<hostlen;i++){
        tldstr[i] = tempTLDnode[i];
    }
    tld->root = addnode(tld, tldstr, tld->root);
    tld->count++;
    return 1;
}
/*
 * By using the taken in value of TLDList, char tldnodestr and TLDNode node to check if the node exists
 * if not create a new know and add the detail to the node.
 * else check the value of child and add it into the child node
 * then call the balance method to rebalanced the tree.
 * finally return node
 */
static TLDNode *addnode(TLDList *tld, char *tldnodestr, TLDNode *node) {

    if(node == NULL)
    {
        TLDNode *node = (TLDNode *)malloc(sizeof(TLDNode));
        if(node != NULL)
        {
            node->tldnodestr = tldnodestr;
            node->leftChild = NULL;
            node->rightChild = NULL;
            node->count = 1;
            tld->root = node;
            tld->size++;
        }
        else {
            free(node);
        }
        return node;
    }else{
        int cmp = strcmp(tldnodestr, node->tldnodestr);
        if(cmp < 0)
        {
            node->leftChild = addnode(tld, tldnodestr, node->leftChild);
            node = balance(node);
        }
        else if(cmp > 0)
        {
            node->rightChild = addnode(tld, tldnodestr, node->rightChild);
            node = balance(node);
        }
        else
        {
            free(tldnodestr);
            node->count++;
        }
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
            iter_adder(iter, iter->tld->root, &i);
            return iter;
        }
    }
    else {
        free(iter);
        return NULL;
    }
}
/*
 * by using the taken in value of iterator , tldnode and i(for counter)
 * using as a recursive method to add the node into the iterator
 */
static void iter_adder(TLDIterator *iter, TLDNode *node, int *i)
{

    if (node->leftChild){
        iter_adder(iter, node->leftChild, i);
    }

    *(iter->next + (*i)++) = node;

    if (node->rightChild){
        iter_adder(iter, node->rightChild, i);
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
/*
 * take in TLDNode and check through the height of the node like its has more left or right child
 * and return the total of the child + itself as the current height
 */
static int getheight(TLDNode *node)
{
    int total = 1;
    if(node == NULL){
        return 0;
    }else{
        if (getheight(node->leftChild) > getheight(node->rightChild))
        {
            total = total + getheight(node->leftChild);
        }
        else
        {
            total = total + getheight(node->rightChild);
        }
    }
    return total;
}
/*
 * get the integer different of the current node's left and right child and return the value
 */
static int getDiff(TLDNode *node)
{
    return getheight(node->leftChild) - getheight(node->rightChild);
}
/*
 * To rebalanced by changing the node of the tree from right to left by creating an temp node to store and switch the position
 * and return the node
 */
static TLDNode *RRRotation(TLDNode *node)
{
    TLDNode *tempTLDnode;
    tempTLDnode = node->rightChild;
    node->rightChild = tempTLDnode->leftChild;
    tempTLDnode->leftChild = node;
    return tempTLDnode;
}
/*
 * To rebalanced by changing the node of the tree using create an new temp node and use LLRotation method and RRRotation
 */
static TLDNode *RLRotation(TLDNode *node)
{
    TLDNode *tempTLDnode;
    tempTLDnode =node->rightChild;
    node->rightChild = LLRotation(tempTLDnode);
    return RRRotation(node);
}
/*
 * To rebalanced by changing the node of the tree from left to right by creating an temp node to store and switch the position
 * and return the node
 */
static TLDNode *LLRotation(TLDNode *node)
{
    TLDNode *tempTLDnode;
    tempTLDnode = node->leftChild;
    node->leftChild = tempTLDnode->rightChild;
    tempTLDnode->rightChild = node;
    return tempTLDnode;
}
/*
 * To rebalanced by changing the node of the tree using create an new temp node and use RRRotation method and LLRotation
 */
static TLDNode *LRRotation(TLDNode *node)
{
    TLDNode *tempTLDnode;
    tempTLDnode = node->leftChild;
    node->leftChild = RRRotation(tempTLDnode);
    return LLRotation(node);
}
/*
 * take in the node and check thought to see if the tree need to be balanced and by using the rotation method
 */
static TLDNode *balance(TLDNode *node)
{
    int diff;
    diff = getDiff(node);
    if (diff > 1)
    {
        if (getDiff(node->leftChild) > 0)
        {
            node = LLRotation(node);
        }
        else
        {
            node = LRRotation(node);
        }
    }
    else if (diff < -1)
    {
        if (getDiff(node->rightChild) > 0)
        {
            node = RLRotation(node);
        }
        else
        {
            node = RRRotation(node);
        }
    }
    return node;
}
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "tldlist.h"
#include "date.h"


static TLDNode *addnode(TLDList *tld, char *tldnodestr, TLDNode *node);
static TLDNode *newnode(char *tldnodestr);
static void addIter(TLDIterator *iter, TLDNode *node, int *i);

static int getheight(TLDNode *node);
static int getDiff(TLDNode *node);
static TLDNode *rightRightRotation(TLDNode *node);
static TLDNode *rightLeftRotation(TLDNode *node);
static TLDNode *leftLeftRotation(TLDNode *node);
static TLDNode *leftRightRotation(TLDNode *node);
static TLDNode *balance(TLDNode *node);


typedef struct tldlist
{
    TLDNode *root;
    Date *begin;
    Date *end;

    long count;
    long size;
}TLDList;

typedef struct tldnode
{
    TLDNode *leftChild;
    TLDNode *rightChild;

    char *tldnodestr;
    long count;
}TLDNode;

typedef struct tlditerator
{
    TLDList *tld;
    TLDNode **next;

    int i;
    long size;
}TLDIterator;

static TLDNode *newnode(char *tldnodestr);

TLDList *tldlist_create(Date *begin, Date *end)
{
    TLDList *tldlist = malloc(sizeof(TLDList));
    if(tldlist != NULL)
    {
        tldlist->count = 0;
        tldlist->size = 0;
        tldlist->root = NULL;
        tldlist->begin = begin;
        tldlist->end = end;
    }
    return tldlist;
}

int tldlist_add(TLDList *tld, char *hostname, Date *d)
{
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
    char *tldTemp = hostname+counter+1;
    int hostlen = strlen(tldTemp);
    char *tldstr = (char *)malloc((hostlen + 1));
    tldstr[hostlen] = '\0'; // make sure there is a null end
    
    for(i=0;i<hostlen;i++){
        tldstr[i] = tldTemp[i];
    }
    tld->root = addnode(tld, tldstr, tld->root);
    tld->count++;
    return 1;
}

//add tldnode
static TLDNode *addnode(TLDList *tld, char *tldnodestr, TLDNode *node)
{
    if(node == NULL)
    {
        node = newnode(tldnodestr);
        tld->root = node;
        tld->size++;

        return node;
    }
    else if(strcmp(tldnodestr, node->tldnodestr) < 0)
    {
        node->leftChild = addnode(tld, tldnodestr, node->leftChild);
        node = balance(node);
    }
    else if(strcmp(tldnodestr, node->tldnodestr) > 0)
    {
        node->rightChild = addnode(tld, tldnodestr, node->rightChild);
        node = balance(node);
    }
    else
    {
        free(tldnodestr);
        node->count++;
    }

    return node;
}

long tldlist_count(TLDList *tld)
{
    return tld->count;
}

TLDIterator *tldlist_iter_create(TLDList *tld)
{
    TLDIterator *iter = (TLDIterator *)malloc(sizeof(TLDIterator));

    if(iter == NULL)
    {
        free(iter);
        return NULL;
    }
    else
    {
        iter->tld = tld;
        iter->size = tld->size;
        iter->i = 0;

        iter->next = (TLDNode **)malloc(sizeof(TLDNode *) *iter->size);
        if(iter->next == NULL)
        {
            tldlist_iter_destroy(iter);
            return NULL;
        }

        int i = 0;
        addIter(iter, iter->tld->root, &i);

        return iter;        
    }
}

static void addIter(TLDIterator *iter, TLDNode *node, int *i)
{
    if(node->leftChild)
        addIter(iter, node->leftChild, i);

    *(iter->next + (*i)++) = node;

    if(node->rightChild)
        addIter(iter, node->rightChild, i);
}

TLDNode *tldlist_iter_next(TLDIterator *iter)
{
    if(iter->i == iter->size)
    {
        return NULL;
    }
    return *(iter->next + iter->i++);
}

void tldlist_iter_destroy(TLDIterator *iter)
{
    int i;
    for(i=0;i<iter->size;i++){
        free(iter->next[i]->tldnodestr);
        free(iter->next[i]);
    }
        free(iter->next);
        free(iter);
}

char *tldnode_tldname(TLDNode *node)
{
    return node->tldnodestr;
}

long tldnode_count(TLDNode *node)
{
    return node->count;
}

//static newtldnode
static TLDNode *newnode(char *tldnodestr)
{
    TLDNode *node = (TLDNode *)malloc(sizeof(TLDNode));
    if(node != NULL)
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

void tldlist_destroy(TLDList *tld)
{
    free(tld);
}
static int getheight(TLDNode *node)
{
    int height = 0;
    int leftTree;
    int rightTree;
    int lastChild;
    if(node == NULL){
        return 0;
    }else{
        leftTree = getheight(node->leftChild);
        rightTree = getheight(node->rightChild);
        if(rightTree > leftTree){
            lastChild = rightTree;
        }else{
            lastChild = leftTree;
        }

        height= 1+lastChild;
    }
    return height;
}

static int getDiff(TLDNode *node)
{
    int diff;
    int leftTree;
    int rightTree;
    
    leftTree = getheight(node->leftChild);
    rightTree = getheight(node->rightChild);
    diff = leftTree - rightTree;
    
    return diff;
}

static TLDNode *rightRightRotation(TLDNode *node)
{
    TLDNode *tldTemp;
    tldTemp = node->rightChild;
    node->rightChild = tldTemp->leftChild;
    tldTemp->leftChild = node;
    return tldTemp;
}

static TLDNode *rightLeftRotation(TLDNode *node)
{
    TLDNode *tldTemp;
    tldTemp =node->rightChild;
    node->rightChild = leftLeftRotation(tldTemp);
    return rightRightRotation(node);
}

static TLDNode *leftLeftRotation(TLDNode *node)
{
    TLDNode *tldTemp;
    tldTemp = node->leftChild;
    node->leftChild = tldTemp->rightChild;
    tldTemp->rightChild = node;
    return tldTemp;
}

static TLDNode *leftRightRotation(TLDNode *node)
{
    TLDNode *tldTemp;
    tldTemp = node->leftChild;
    node->leftChild = rightRightRotation(tldTemp);
    return leftLeftRotation(node);
}

static TLDNode *balance(TLDNode *node)
{
    int diff;
    diff = getDiff(node);
    if (diff > 1) {
        if (getDiff(node->leftChild) > 0){
            node = leftLeftRotation(node);
        }else{
            node = leftRightRotation(node);
        }
    }else if (diff < -1)
        {
            if (getDiff(node->rightChild) > 0) {
                node = rightLeftRotation(node);
            }else{
                node = rightRightRotation(node);
            }
        }
    return node;
}
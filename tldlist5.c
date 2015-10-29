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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "tldlist.h"
#include "date.h"

static TLDNode *addnode(TLDList *tld, char *tldnodestr, TLDNode *node);
static TLDNode * makeNode(char * hostname);
static TLDNode * findDeepestNode(TLDNode * node);
static int getheight(TLDNode *node);
static int getDiff(TLDNode *node);
static TLDNode *rightRightRotation(TLDNode *node);
static TLDNode *rightLeftRotation(TLDNode *node);
static TLDNode *leftLeftRotation(TLDNode *node);
static TLDNode *leftRightRotation(TLDNode *node);
static TLDNode *balance(TLDNode *node);


struct tldlist {
    TLDNode *root;
    Date *begin;
    Date *end;
    long count;
    long size;
};


struct tldnode {
    TLDNode * parent, * left, * right;
    char * content;
    long count, height;
};

struct tlditerator {
    TLDList * tld;
    long size;
    TLDNode * nodes;
};

/*
 * tldlist_create generates a list structure for storing counts against
 * top level domains (TLDs)
 *
 * creates a TLDList that is constrained to the `begin' and `end' Date's
 * returns a pointer to the list if successful, NULL if not
 */

TLDList *tldlist_create(Date *begin, Date *end) {
    TLDList *tldlist = malloc(sizeof(TLDList));
    if (tldlist != NULL)
    {
        tldlist->count = 0;
        tldlist->size = 0;
        tldlist->root = NULL;
        tldlist->begin = date_duplicate(begin);
        tldlist->end = date_duplicate(end);
    }

    return tldlist;

}

//destroy everything in tldlist.
void tldlist_destroy(TLDList *tld) {

    TLDIterator *it = tldlist_iter_create(tld);
    TLDNode *node;

    while ((node = tldlist_iter_next(it)) != NULL) {
        free(node->content);
        free(node);
    }

    date_destroy(tld->begin);
    date_destroy(tld->end);
    free(tld);
    tldlist_iter_destroy(it);


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

static TLDNode *addnode(TLDList *tld, char *hostname, TLDNode *node) {

    if(node == NULL)
    {
        TLDNode *node = (TLDNode *)malloc(sizeof(TLDNode));
        if(node != NULL)
        {
            node->parent = NULL;
            node->content = hostname;
            node->left = NULL;
            node->right = NULL;
            node->count = 1;
            tld->root = node;
            tld->size++;
        }
        else {
            free(node);
        }
        return node;
    }else{
        int cmp = strcmp(hostname, node->content);
        if(cmp < 0)
        {
            if (node->left != NULL) //turn left
                return addnode(tld, hostname, node->left); //recursive
            else {
                //add node if blank
                TLDNode *n = makeNode(hostname);
                n->parent = node;
                node->left = n;

                balance(node->left); //create the balance
            }
        }
        else if(cmp > 0)
        {
            if (node->right != NULL)
                return addnode(tld, hostname, node->right);
            else {
                TLDNode *n = makeNode(hostname);
                n->parent = node;
                node->right = n;

                balance(node->right);
            }
        }
        else
        {
            free(hostname);
            node->count++;
        }
    }

    return node;
}
//make the node
static TLDNode * makeNode(char *hostname) {
    TLDNode * cNode = (TLDNode *) malloc (sizeof(TLDNode) + 1);

    if (cNode==NULL)
        return NULL;
    cNode->parent = NULL;
    cNode->content = hostname;
    cNode->left = NULL;
    cNode->right = NULL;
    cNode->height = 1;
    cNode->count = 1;


    return cNode;
}

//return the total size of tldlist
long tldlist_count(TLDList *tld) {
    return tld->size;
}


TLDIterator *tldlist_iter_create(TLDList *tld) {

    TLDIterator *iter = (TLDIterator *)malloc(sizeof(TLDIterator));
    //if memory allocation fail
    if (iter == NULL)
        return NULL;

    //set the deepest node to be the start of the iterator list.
    iter->nodes = findDeepestNode(tld->root);
    iter->size = 0;

    return iter;
}


//function to look for the deepest node in the list.
static TLDNode *findDeepestNode(TLDNode *node) {

    TLDNode * test = node;

    //check if node is empty
    if (test != NULL) {
        // recurse to find which node is the last node in the tree.
        if (((test = findDeepestNode(node->left)) != NULL) || (test = findDeepestNode(node->right)) != NULL) {
            //return the found node
            return test;
        } else {
            return node;
        }
    } else {

        return node;
    }


}


//get the next node in the list
TLDNode *tldlist_iter_next(TLDIterator *iter) {
    TLDNode * get = iter->nodes;

    //if it is the last node.
    if (get == NULL)
        return NULL;

    //if it is the root
    if(iter->nodes->parent == NULL) {
        iter->nodes = NULL;
        return get;
    }

    //check for the deepest node and get the next node.
    if(iter->nodes->parent->right != iter->nodes && iter->nodes->parent->right != NULL)
        iter->nodes = findDeepestNode(iter->nodes->parent->right);
    else
        iter->nodes = iter->nodes->parent;

    return get;
}



//destroy the iterator list
void tldlist_iter_destroy(TLDIterator *iter) {
    free(iter->nodes);
    free(iter);
}

//return the content of the node.
char *tldnode_tldname(TLDNode *node) {
    return node->content;
}

//return the amount of node being counted
long tldnode_count(TLDNode *node) {
    return node->count;
}

static int getheight(TLDNode *node)
{
    int ht = 0;
    int leftTree;
    int rightTree;
    int last;
    if(node == NULL){
        return 0;
    }else{
        leftTree = getheight(node->left);
        rightTree = getheight(node->right);
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

static int getDiff(TLDNode *node)
{
    int diff;
    int leftTree;
    int rightTree;

    leftTree = getheight(node->left);
    rightTree = getheight(node->right);
    diff = leftTree - rightTree;

    return diff;
}

static TLDNode *rightRightRotation(TLDNode *node)
{

    TLDNode *tempTLDnode;
    tempTLDnode = node->right;
    node->right = tempTLDnode->left;
    tempTLDnode->left = node;
    return tempTLDnode;
}

static TLDNode *rightLeftRotation(TLDNode *node)
{
    TLDNode *tempTLDnode;
    tempTLDnode =node->right;
    node->right = leftLeftRotation(tempTLDnode);
    return rightRightRotation(node);
}

static TLDNode *leftLeftRotation(TLDNode *node)
{
    TLDNode *tempTLDnode;
    tempTLDnode = node->left;
    node->left = tempTLDnode->right;
    tempTLDnode->right = node;
    return tempTLDnode;
}

static TLDNode *leftRightRotation(TLDNode *node)
{
    TLDNode *tempTLDnode;
    tempTLDnode = node->left;
    node->left = rightRightRotation(tempTLDnode);
    return leftLeftRotation(node);
}

static TLDNode *balance(TLDNode *node)
{
    int diff;
    diff = getDiff(node);
    if (diff > 1)
    {
        if (getDiff(node->left) > 0)
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
        if (getDiff(node->right) > 0)
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
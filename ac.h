#ifndef _AC_H
#define _AC_H

#include "avltree.h"

int MAX_WORD_LEN = 100;
int const MAX_WORD_COUNT = 1280000;
int const MAX_BUFFER_SIZE = 1048576;

typedef struct treenode
{
    struct avl_node avl;
    char ch;
    int word_id;
    struct treenode *parent;
    struct avl_tree children;
    // fail link
    struct treenode *fail;
} TREE_NODE, *TREE_PTR;



#endif
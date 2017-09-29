#ifndef _AC_H
#define _AC_H

int MAX_WORD_LEN = 100;
int const MAX_WORD_COUNT = 1280000;
int const MAX_BUFFER_SIZE = 1048576;

typedef struct treenode
{
    char ch;
    int word_id;
    struct treenode *child, *sibling, *parent;
    // fail link
    struct treenode *fail;
    // dictionary suffix link
    struct treenode *dict;
} TREE_NODE, *TREE_PTR;



#endif

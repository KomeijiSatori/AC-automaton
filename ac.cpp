#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <queue>
#include "ac.h"
using namespace std;

char *word[MAX_WORD_COUNT];


void init()
{
    int i;
    for (i = 0; i < MAX_WORD_COUNT; i++)
    {
        word[i] = NULL;
    }
}


TREE_PTR alloc_tree_node()
{
    TREE_PTR new_node = (TREE_PTR)malloc(sizeof(TREE_NODE));
    if (new_node == NULL)
    {
        puts("Alloc error!");
        exit(2);
    }
    new_node->word_id = -1;
    new_node->child = NULL;
    new_node->sibling = NULL;
    new_node->parent = NULL;
    new_node->fail = NULL;
    return new_node;
}


TREE_PTR build_trie_tree(char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        exit(1);
    }
    TREE_PTR root = alloc_tree_node();

    char cur_word[MAX_WORD_LEN];
    int word_id = 1;

    while (fscanf(fp, "%s", cur_word) != EOF)
    {
        TREE_PTR cur = root;
        int ind;
        for (ind = 0; cur_word[ind] != '\0'; ind++)
        {
            char cur_ch = cur_word[ind];
            // if the current node has no children
            if (cur->child == NULL)
            {
                TREE_PTR cur_node = alloc_tree_node();
                cur_node->ch = cur_ch;
                cur_node->parent = cur;
                cur->child = cur_node;
                cur = cur_node;
            }
            else
            {
                TREE_PTR sibling = cur->child;
                while (sibling != NULL && sibling->ch != cur_ch)
                {
                    sibling = sibling->sibling;
                }
                // if not find the same character
                if (sibling == NULL)
                {
                    TREE_PTR follow_sibling = cur->child->sibling;
                    TREE_PTR cur_node = alloc_tree_node();
                    cur_node->ch = cur_ch;
                    cur_node->sibling = follow_sibling;
                    cur_node->parent = cur;
                    cur->child->sibling = cur_node;
                    cur = cur_node;
                }
                else
                {
                    cur = sibling;
                }
            }
        }
        // deal with word end
        cur->word_id = word_id;
        word[word_id] = (char *)malloc(sizeof(char) * (ind + 1));
        strcpy(word[word_id], cur_word);
        word_id++;
    }
    fclose(fp);
    return root;
}


void build_fail_ptr(TREE_PTR root)
{
    if (root == NULL)
    {
        return;
    }
    queue<TREE_PTR> nodes;
    TREE_PTR root_child = root->child;
    while (root_child != NULL)
    {
        nodes.push(root_child);
        root_child = root_child->sibling;
    }

    while (!nodes.empty())
    {
        TREE_PTR cur_node = nodes.front();
        nodes.pop();
        TREE_PTR cur_child = cur_node->child;
        while (cur_child != NULL)
        {
            nodes.push(cur_child);
            cur_child = cur_child->sibling;
        }

        TREE_PTR cur_parent = cur_node->parent;
        TREE_PTR parent_fail = cur_parent->fail;
        // whether cur_node has found the fail link
        int find = 0;
        while (parent_fail != NULL && !find)
        {
            // the parent's fail node's child
            TREE_PTR fail_child = parent_fail->child;
            while (fail_child != NULL)
            {
                if (fail_child->ch == cur_node->ch)
                {
                    find = 1;
                    cur_node->fail = fail_child;
                    break;
                }
                else
                {
                    fail_child = fail_child->sibling;
                }
            }
            // if not found, refer to the fail link of parent_fail
            if (!find)
            {
                parent_fail = parent_fail->fail;
            }
        }
        // if reach the root
        if (!find)
        {
            cur_node->fail = root;
        }
    }
}


// output the result in filename
void proceed(TREE_PTR root, char *string_file_name, char *output_file_name)
{
    FILE *in = fopen(string_file_name, "rb");
    FILE *out = fopen(output_file_name, "w");
    if (in == NULL || out == NULL || root == NULL)
    {
        exit(3);
    }

    TREE_PTR cur_node = root;
    char buffer[MAX_BUFFER_SIZE];
    int iter = 0;
    while (!feof(in))
    {
        int read_count = (int)fread(buffer, sizeof(buffer[0]), MAX_BUFFER_SIZE, in);
        int i;
        for (i = 0; i < read_count; i++)
        {
            char cur_ch = buffer[i];
            int find = 0;

            // while root not fail
            while (cur_node != NULL && !find)
            {
                // search all children to find if ch matches
                TREE_PTR cur_child = cur_node->child;
                while (cur_child != NULL)
                {
                    if (cur_child->ch == cur_ch)
                    {
                        cur_node = cur_child;
                        find = 1;
                        break;
                    }
                    cur_child = cur_child->sibling;
                }
                if (!find)
                {
                    // jump to fail
                    cur_node = cur_node->fail;
                }
            }

            // if not found the ch in root
            if (!find)
            {
                cur_node = root;
            }
            else
            {
				// judge the string and its substrings could be a word
				TREE_PTR cur_dict = cur_node;
                while (cur_dict != root)
                {
                    int word_id = cur_dict->word_id;
					if (word_id != -1)
					{
						long long word_len = (long long)strlen(word[word_id]);
		                long long offset = (long long)iter * (long long)MAX_BUFFER_SIZE + (long long)i - word_len + 1;
		                fprintf(out, "%s %lld\n", word[word_id], offset);
					}
					cur_dict = cur_dict->fail;
                }
            }
        }
        // then add iteration
        iter++;
    }
    fclose(in);
    fclose(out);
}


void free_tree(TREE_PTR root)
{
    if (root != NULL)
    {
        free_tree(root->child);
        free_tree(root->sibling);
        free(root);
    }
}


void free_word()
{
    int i;
    for (i = 0; i < MAX_WORD_COUNT; i++)
    {
        free(word[i]);
    }
}


int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        puts("usage: ./ac [string-file] [pattern-file] [result-file]");
        return 1;
    }

    init();
    char *string_file_name = argv[1], *pattern_file_name = argv[2], *result_file_name = argv[3];
    TREE_PTR root = build_trie_tree(pattern_file_name);
    build_fail_ptr(root);
    proceed(root, string_file_name, result_file_name);
    free_tree(root);
    root = NULL;
    free_word();
    return 0;
}


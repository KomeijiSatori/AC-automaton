#include <ctime>
#include <cstring>
#include <fstream>
#include <queue>
#include <string>
#include <sstream>

#include "avltree.h"
#include "ac.h"
using namespace std;

string word[MAX_WORD_COUNT];


int cmp_func(struct avl_node *a, struct avl_node *b, void *aux)
{
    TREE_PTR aa, bb;
    aa = _get_entry(a, TREE_NODE, avl);
    bb = _get_entry(b, TREE_NODE, avl);

    if (aa->ch < bb->ch)
    {
        return -1;
    }
    else if (aa->ch > bb->ch)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


TREE_PTR alloc_tree_node()
{
    TREE_PTR new_node = new TREE_NODE;
    new_node->word_id = -1;
    new_node->parent = nullptr;
    new_node->fail = nullptr;
    avl_init(&(new_node->children), nullptr);
    return new_node;
}


TREE_PTR build_trie_tree(char *filename)
{
    ifstream in(filename);
    if (!in.is_open())
    {
        exit(1);
    }
    TREE_PTR root = alloc_tree_node();

    char cur_word[MAX_WORD_LEN];
    int word_id = 1;

    while (in >> cur_word)
    {
        TREE_PTR cur = root;
        int ind;
        for (ind = 0; cur_word[ind] != '\0'; ind++)
        {
            char cur_ch = cur_word[ind];
            TREE_NODE query;
            query.ch = cur_ch;
            struct avl_node *res = avl_search(&(cur->children), &query.avl, cmp_func);
            // if has no such ch in children
            if (res == nullptr)
            {
                TREE_PTR cur_node = alloc_tree_node();
                cur_node->ch = cur_ch;
                cur_node->parent = cur;
                avl_insert(&(cur->children), &(cur_node->avl), cmp_func);
                cur = cur_node;
            }
            else
            {
                cur = _get_entry(res, TREE_NODE, avl);
            }
        }
        // deal with word end
        cur->word_id = word_id;

        ostringstream os;
        os << cur_word;
        word[word_id] = os.str();
        word_id++;
    }
    in.close();
    return root;
}


void build_fail_ptr(TREE_PTR root)
{
    if (root == nullptr)
    {
        return;
    }
    queue<TREE_PTR> nodes;
    nodes.push(root);

    while (!nodes.empty())
    {
        TREE_PTR cur_node = nodes.front();
        nodes.pop();
        // add all children into the nodes
        struct avl_node *first_child = (cur_node->children).root;
        // if the node has children
        if (first_child != nullptr)
        {
            queue<struct avl_node *> children;
            children.push(first_child);
            while (!children.empty())
            {
                struct avl_node *cur_child = children.front();
                children.pop();
                nodes.push(_get_entry(cur_child, TREE_NODE, avl));

                if (cur_child->left != nullptr)
                {
                    children.push(cur_child->left);
                }
                if (cur_child->right != nullptr)
                {
                    children.push(cur_child->right);
                }
            }
        }

        if (cur_node == root)
        {
            continue;
        }

        TREE_PTR cur_parent = cur_node->parent;
        TREE_PTR parent_fail = cur_parent->fail;
        // whether cur_node has found the fail link
        struct avl_node *res = nullptr;
        TREE_NODE query;
        query.ch = cur_node->ch;
        while (parent_fail != nullptr)
        {
            // the parent's fail node's child
            res = avl_search(&(parent_fail->children), &query.avl, cmp_func);
            // if not found, refer to the fail link of parent_fail
            if (res == nullptr)
            {
                parent_fail = parent_fail->fail;
            }
            else
            {
                cur_node->fail = _get_entry(res, TREE_NODE, avl);
                break;
            }
        }
        // if reach the root
        if (res == nullptr)
        {
            cur_node->fail = root;
        }
    }
}


// proceed to recognize
void proceed(TREE_PTR root, char *string_file_name, char *result_file_name)
{
    ifstream in(string_file_name, ios::in | ios::binary);
    ofstream out(result_file_name);
    if (root == nullptr || !in.is_open())
    {
        exit(3);
    }

    TREE_PTR cur_node = root;
    char buffer[MAX_BUFFER_SIZE];
    int iter = 0;
    while (!in.eof())
    {
        in.read(buffer, MAX_BUFFER_SIZE);
        int read_count = (int)in.gcount();
        int i;
        for (i = 0; i < read_count; i++)
        {
            char cur_ch = buffer[i];
            TREE_NODE query;
            query.ch = cur_ch;
            struct avl_node *res = nullptr;

            // while root not fail
            while (cur_node != nullptr)
            {
                // search all children to find if ch matches
                res = avl_search(&(cur_node->children), &query.avl, cmp_func);
                // if not find, jump to fail
                if (res == nullptr)
                {
                    cur_node = cur_node->fail;
                }
                else
                {
                    cur_node = _get_entry(res, TREE_NODE, avl);
                    break;
                }
            }

            // if not found the ch in root
            if (res == nullptr)
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
						long long word_len = (long long)word[word_id].length();
		                long long offset = (long long)iter * (long long)MAX_BUFFER_SIZE + (long long)i - word_len + 1;
                        out << word[word_id] << " " << offset << "\n";
                    }
					cur_dict = cur_dict->fail;
                }
            }
        }
        // then add iteration
        iter++;
    }
    in.close();
    out.close();
}


void free_tree(TREE_PTR root)
{
    queue<TREE_PTR> nodes;
    nodes.push(root);
    while (!nodes.empty())
    {
        TREE_PTR cur_node = nodes.front();
        nodes.pop();
        struct avl_node *first_child = (cur_node->children).root;

        if (first_child != nullptr)
        {
            queue<struct avl_node *> children;
            children.push(first_child);
            while (!children.empty())
            {
                struct avl_node *cur_child = children.front();
                children.pop();
                nodes.push(_get_entry(cur_child, TREE_NODE, avl));

                if (cur_child->left != nullptr)
                {
                    children.push(cur_child->left);
                }
                if (cur_child->right != nullptr)
                {
                    children.push(cur_child->right);
                }
            }
        }
        free(cur_node);
    }
}


int main(int argc, char *argv[])
{
    clock_t start_t, end_t;
    start_t = clock();
    if (argc != 4)
    {
        puts("usage: ./ac [string-file] [pattern-file] [result-file]");
        return 1;
    }

    char *string_file_name = argv[1], *pattern_file_name = argv[2], *result_file_name = argv[3];

    TREE_PTR root = build_trie_tree(pattern_file_name);
    build_fail_ptr(root);
    proceed(root, string_file_name, result_file_name);
    free_tree(root);
    root = nullptr;
    end_t = clock();
    double total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    printf("Total time: %lf\n", total_t);
    return 0;
}

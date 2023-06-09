#include <string.h>
#include "trie.h"
#include "codeparser.h"

static void initNode(TreeNode *node)
{
    node->childNum = 0;
    int i = 0;
    for (i = 0; i < 52; i++) {
        node->child[i] = NULL;
        node->cnt[i] = 0;
    }
}

static int charToIndex(char ch)
{
    if (ch >= 'a' && ch <= 'z')
        return ch - 'a';
    else
        return 26 + ch - 'A';
}

static char indexToChar(int index)
{
    if (index >= 0 && index <= 25)
        return 'a' + index;
    else
        return 'A' + index - 26;
}


void traverseTree(TreeNode *root)
{
    if (root->childNum == 0) return;
    int i = 0;
    for (i = 0; i < 52; i++) {
        TreeNode *child = root->child[i];
        if (child == NULL) continue;
        traverseTree(child);
    }
}

void initTrie(Trie *tree)
{
    tree->root = (TreeNode *)malloc(sizeof(TreeNode));
    initNode(tree->root);
    int i;
    for (i = 0; i < 32; i++)
        addStringToTrie(tree->root, KeyWord[i]);
}

void addStringToTrie(TreeNode *root, char *str)
{
    if (root == NULL || str == NULL) return;
    if (*str <= 0) return;
    char ch = *str;
    int index = 0;
    index = charToIndex(ch);
    if (root->child[index] == NULL) {
        TreeNode *newNode = (TreeNode *)malloc(sizeof(TreeNode));
        initNode(newNode);
        root->child[index] = newNode;
        root->childNum++;
        if (*(str + 1) == '\0')
            root->cnt[index]++;
        else
            addStringToTrie(newNode, str + 1);
    } else {
        if (*(str + 1) == '\0')
            root->cnt[index]++;
        else
            addStringToTrie(root->child[index], str + 1);
    }
}

int deleteStringInTrie(TreeNode *root, char *str)
{
    if (root == NULL || str == NULL) return 0;
    if (*str < 0) return 0;
    char ch = *str;
    int index = 0;
    if (ch == '\0') {
        if (root->childNum == 0) {
            return 2; //This node is (leaf && endpoint && delete)
        }
        return 3; //This node is endpoint
    }
    index = charToIndex(ch);
    TreeNode *child = root->child[index];
    if (child != NULL) {
        int isDeleteChild = deleteStringInTrie(child, str + 1);
        if (isDeleteChild == 1 && root->cnt[index] == 0) {
            /*
                Child is a leaf but not endpoint, needs to be deleted
            */
            free(child);
            root->child[index] = NULL;
            root->childNum--;
        } else if (isDeleteChild == 2) {
            root->cnt[index]--;
            if (root->cnt[index] == 0) {
                free(child);
                root->childNum--;
                root->child[index] = NULL;
            }
        } else if (isDeleteChild == 3)
            root->cnt[index]--;
        if (root->childNum == 0) return 1; //This node is leaf
        return 0;
    }
    return 0; //NOP
}

TreeNode *searchString(TreeNode *root, char *str)
{
    if (root == NULL || str == NULL) return NULL;
    if (*str <= 0) return NULL;
    char ch = *str;
    int index = 0;
    index = charToIndex(ch);
    if (root->child[index] == NULL)
        return NULL;
    else {
        str++;
        if (*str == '\0')
            return root->child[index];
        else
            return searchString(root->child[index], str);
    }
}

/*
    addIndex is the index of the prefix string in <textList>
*/
static void getAllString(TextList *textList, TreeNode *root, char prefix[])
{
    if (root->childNum == 0) return;
    int i = 0;
    for (i = 0; i < 52; i++) {
        if (root->child[i] == NULL) continue;
        char *str = (char *)malloc(sizeof(char) * MAX_WORD_SIZE);
        *str = '\0';
        prefix[strlen(prefix)] = indexToChar(i);
        prefix[strlen(prefix)] = '\0';
        strcpy(str, prefix);
        if (root->cnt[i] >= 1) addNodeToTail(textList, str);
        TreeNode *child = root->child[i];
        getAllString(textList, child, prefix);
        prefix[strlen(prefix) - 1] = '\0';
    }
}

TextList *matchPrefix(TreeNode *root, char *str)
{
    TreeNode *startNode = searchString(root, str);
    if (startNode == NULL) return NULL;
    if (startNode->childNum == 0) return NULL;
    TextList *textList = (TextList *)malloc(sizeof(TextList));
    initList(textList);
    char prefix[MAX_WORD_SIZE] = "";
    getAllString(textList, startNode, prefix);
    return textList;
}


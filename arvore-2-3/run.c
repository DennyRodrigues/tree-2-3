#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>

#define MAX_WORD_LENGTH 100
#define INITIAL_LINES_CAPACITY 10

// Structure to hold dynamic array of line numbers
typedef struct
{
  int *lines;
  int size;
  int capacity;
} LineArray;

// Node structure
typedef struct Node
{
  char *leftKey;
  char *rightKey;
  LineArray leftLines;
  LineArray rightLines;
  struct Node *left;
  struct Node *middle;
  struct Node *right;
} Node;

// Tree structure
typedef struct
{
  Node *root;
  char **words;  // Array to store all words
  int wordCount; // Total number of words
  int wordCapacity;
} TTTree;

// Function prototypes
LineArray createLineArray();
void addLine(LineArray *arr, int line);
void freeLineArray(LineArray *arr);
Node *createNode(const char *x);
bool isLeafNode(Node *x);
Node *add(Node *x, Node *n);
Node *insert(TTTree *tree, const char *key, int line, Node *root, int *distWord);
bool searchTreeHelper(Node *x, const char *value, LineArray *lines);
void printLines(FILE *out, LineArray *lines);
void printNode(FILE *out, Node *x);
void printTreeHelper(FILE *out, Node *x);
int findHeight(Node *x);
void freeNode(Node *node);
void freeTree(TTTree *tree);

// Initialize tree
TTTree *createTree()
{
  TTTree *tree = (TTTree *)malloc(sizeof(TTTree));
  tree->root = NULL;
  tree->words = (char **)malloc(sizeof(char *) * 1000); // Initial capacity
  tree->wordCount = 0;
  tree->wordCapacity = 1000;
  return tree;
}

// Create and initialize line array
LineArray createLineArray()
{
  LineArray arr;
  arr.lines = (int *)malloc(sizeof(int) * INITIAL_LINES_CAPACITY);
  arr.size = 0;
  arr.capacity = INITIAL_LINES_CAPACITY;
  return arr;
}

// Add line to line array
void addLine(LineArray *arr, int line)
{
  if (arr->size >= arr->capacity)
  {
    arr->capacity *= 2;
    arr->lines = (int *)realloc(arr->lines, sizeof(int) * arr->capacity);
  }
  arr->lines[arr->size++] = line;
}

// Create new node
Node *createNode(const char *x)
{
  Node *t = (Node *)malloc(sizeof(Node));
  t->leftKey = strdup(x);
  t->rightKey = NULL;
  t->leftLines = createLineArray();
  t->rightLines = createLineArray();
  t->left = NULL;
  t->middle = NULL;
  t->right = NULL;
  return t;
}

// Check if node is leaf
bool isLeafNode(Node *x)
{
  return (x->left == NULL && x->middle == NULL && x->right == NULL);
}

// Add node to existing node
Node *add(Node *x, Node *n)
{
  if (x->rightKey == NULL)
  {
    if (strcmp(x->leftKey, n->leftKey) < 0)
    {
      x->rightKey = strdup(n->leftKey);
      x->rightLines = n->leftLines;
      x->middle = n->left;
      x->right = n->middle;
    }
    else
    {
      x->rightKey = strdup(x->leftKey);
      x->rightLines = x->leftLines;
      x->leftKey = strdup(n->leftKey);
      x->leftLines = n->leftLines;
      x->right = x->middle;
      x->middle = n->middle;
      x->left = n->left;
    }
    free(n->leftKey);
    free(n);
    return x;
  }

  // Add to left
  if (strcmp(x->leftKey, n->leftKey) >= 0)
  {
    Node *newNode = createNode(x->leftKey);
    newNode->leftLines = x->leftLines;
    newNode->left = n;
    newNode->middle = x;
    x->left = x->middle;
    x->middle = x->right;
    x->right = NULL;
    free(x->leftKey);
    x->leftKey = strdup(x->rightKey);
    x->leftLines = x->rightLines;
    free(x->rightKey);
    x->rightKey = NULL;
    return newNode;
  }
  // Add to center
  else if (strcmp(x->rightKey, n->leftKey) >= 0)
  {
    Node *newNode = createNode(x->rightKey);
    newNode->leftLines = x->rightLines;
    newNode->left = n->middle;
    newNode->middle = x->right;
    x->middle = n->left;
    n->middle = newNode;
    n->left = x;
    free(x->rightKey);
    x->rightKey = NULL;
    x->right = NULL;
    return n;
  }
  // Add to right
  else
  {
    Node *newNode = createNode(x->rightKey);
    newNode->leftLines = x->rightLines;
    newNode->left = x;
    newNode->middle = n;
    free(x->rightKey);
    x->rightKey = NULL;
    x->right = NULL;
    return newNode;
  }
}

// Insert key into tree
Node *insert(TTTree *tree, const char *key, int line, Node *root, int *distWord)
{
  if (root == NULL)
  {
    Node *newNode = createNode(key);
    addLine(&newNode->leftLines, line);
    (*distWord)++;
    return newNode;
  }

  if ((root->leftKey && strcmp(key, root->leftKey) == 0) ||
      (root->rightKey && strcmp(key, root->rightKey) == 0))
  {
    if (root->leftKey && strcmp(key, root->leftKey) == 0)
      addLine(&root->leftLines, line);
    else
      addLine(&root->rightLines, line);
    return root;
  }

  if (isLeafNode(root))
  {
    Node *newNode = createNode(key);
    addLine(&newNode->leftLines, line);
    Node *finalNode = add(root, newNode);
    (*distWord)++;
    return finalNode;
  }

  // Insert recursively
  if (strcmp(key, root->leftKey) < 0)
  {
    Node *newNode = insert(tree, key, line, root->left, distWord);
    if (newNode == root->left)
      return root;
    else
      return add(root, newNode);
  }
  else if (root->rightKey == NULL || strcmp(key, root->rightKey) < 0)
  {
    Node *newNode = insert(tree, key, line, root->middle, distWord);
    if (newNode == root->middle)
      return root;
    else
      return add(root, newNode);
  }
  else
  {
    Node *newNode = insert(tree, key, line, root->right, distWord);
    if (newNode == root->right)
      return root;
    else
      return add(root, newNode);
  }
}

// Build tree from file
void buildTree(TTTree *tree, FILE *input)
{
  printf("-----------------------------------------------------\n");
  printf("[MSG] BUILDING 2-3 TREE...\n");

  int line = 1, numWords = 0, distWords = 0;
  char buffer[1024];
  char word[MAX_WORD_LENGTH];
  clock_t startTime = clock();

  while (fgets(buffer, sizeof(buffer), input))
  {
    char *token = strtok(buffer, " \n");
    while (token != NULL)
    {
      strncpy(word, token, MAX_WORD_LENGTH - 1);
      word[MAX_WORD_LENGTH - 1] = '\0';

      // Trim punctuation
      int len = strlen(word);
      while (len > 0 && !isalnum(word[len - 1]))
      {
        word[len - 1] = '\0';
        len--;
      }

      if (len > 0)
      {
        if (tree->wordCount >= tree->wordCapacity)
        {
          tree->wordCapacity *= 2;
          tree->words = realloc(tree->words, sizeof(char *) * tree->wordCapacity);
        }
        tree->words[tree->wordCount] = strdup(word);
        tree->wordCount++;
        tree->root = insert(tree, word, line, tree->root, &distWords);
        numWords++;
      }
      token = strtok(NULL, " \n");
    }
    line++;
  }

  double totalTime = (double)(clock() - startTime) / CLOCKS_PER_SEC;
  int treeHeight = findHeight(tree->root);

  printf("=====================================================\n");
  printf("- Built Tree results (2-3 Tree)\n");
  printf("=====================================================\n");
  printf("Total number of words: %d\n", numWords);
  printf("Total number of distinct words: %d\n", distWords);
  printf("Total time spent building index: %f\n", totalTime);
  printf("Height of 2-3 Tree is: %d\n", treeHeight);
}

// Search helper
bool searchTreeHelper(Node *x, const char *value, LineArray *lines)
{
  if (x == NULL)
  {
    return false;
  }

  if (x->leftKey && strcmp(value, x->leftKey) == 0)
  {
    *lines = x->leftLines;
    return true;
  }

  if (x->rightKey && strcmp(value, x->rightKey) == 0)
  {
    *lines = x->rightLines;
    return true;
  }

  if (isLeafNode(x))
    return false;

  if (x->rightKey != NULL)
  {
    if (strcmp(value, x->leftKey) < 0)
    {
      return searchTreeHelper(x->left, value, lines);
    }
    else if (strcmp(value, x->leftKey) > 0 && strcmp(value, x->rightKey) < 0)
    {
      return searchTreeHelper(x->middle, value, lines);
    }
    else
    {
      return searchTreeHelper(x->right, value, lines);
    }
  }
  else
  {
    if (strcmp(value, x->leftKey) < 0)
      return searchTreeHelper(x->left, value, lines);
    else
      return searchTreeHelper(x->middle, value, lines);
  }
}

// Print lines
void printLines(FILE *out, LineArray *lines)
{
  if (lines->size > 0)
  {
    fprintf(out, " %d", lines->lines[0]);
    for (int i = 1; i < lines->size; i++)
    {
      fprintf(out, ", %d", lines->lines[i]);
    }
  }
}

// Print node
void printNode(FILE *out, Node *x)
{
  if (x != NULL)
  {
    if (x->rightKey == NULL)
    {
      fprintf(out, "%-30s", x->leftKey);
      printLines(out, &x->leftLines);
      fprintf(out, "\n");
    }
    else
    {
      fprintf(out, "%-30s", x->leftKey);
      printLines(out, &x->leftLines);
      fprintf(out, "\n");
      fprintf(out, "%-30s", x->rightKey);
      printLines(out, &x->rightLines);
      fprintf(out, "\n");
    }
  }
}

// Print tree helper
void printTreeHelper(FILE *out, Node *x)
{
  if (x != NULL)
  {
    if (isLeafNode(x))
    {
      printNode(out, x);
    }
    else
    {
      printTreeHelper(out, x->left);
      fprintf(out, "%-30s", x->leftKey);
      printLines(out, &x->leftLines);
      fprintf(out, "\n");
      if (x->middle)
      {
        printTreeHelper(out, x->middle);
        if (x->rightKey != NULL)
        {
          fprintf(out, "%-30s", x->rightKey);
          printLines(out, &x->rightLines);
          fprintf(out, "\n");
        }
      }
      printTreeHelper(out, x->right);
    }
  }
}

// Find height
int findHeight(Node *x)
{
  if (x == NULL)
    return 0;
  else
  {
    int leftHeight = findHeight(x->left);
    int rightHeight = findHeight(x->right);
    int middleHeight = findHeight(x->middle);

    int maxHeight = leftHeight;
    if (rightHeight > maxHeight)
      maxHeight = rightHeight;
    if (middleHeight > maxHeight)
      maxHeight = middleHeight;

    return maxHeight + 1;
  }
}

// Free line array
void freeLineArray(LineArray *arr)
{
  free(arr->lines);
  arr->lines = NULL;
  arr->size = 0;
  arr->capacity = 0;
}

// Free node
void freeNode(Node *node)
{
  if (node != NULL)
  {
    free(node->leftKey);
    if (node->rightKey)
      free(node->rightKey);
    freeLineArray(&node->leftLines);
    freeLineArray(&node->rightLines);
    freeNode(node->left);
    freeNode(node->middle);
    freeNode(node->right);
    free(node);
  }
}

// Free tree
void freeTree(TTTree *tree)
{
  if (tree != NULL)
  {
    for (int i = 0; i < tree->wordCount; i++)
    {
      free(tree->words[i]);
    }
    free(tree->words);
    freeNode(tree->root);
    free(tree);
  }
}

void printSpaces(int level) {
    for (int i = 0; i < level * 4; i++) {
        printf(" ");
    }
}

void printVisualNode(Node* node, int level) {
    printSpaces(level);
    printf("└─ %s", node->leftKey);
    if (node->leftLines.size > 0) {
        printf(" (lines:");
        for (int i = 0; i < node->leftLines.size; i++) {
            printf(" %d", node->leftLines.lines[i]);
        }
        printf(")");
    }
    printf("\n");

    if (node->rightKey != NULL) {
        printSpaces(level);
        printf("└─ %s", node->rightKey);
        if (node->rightLines.size > 0) {
            printf(" (lines:");
            for (int i = 0; i < node->rightLines.size; i++) {
                printf(" %d", node->rightLines.lines[i]);
            }
            printf(")");
        }
        printf("\n");
    }
}

void printVisualTree(Node* root, int level) {
    if (root != NULL) {
        printVisualNode(root, level);
        
        if (!isLeafNode(root)) {
            if (root->left) printVisualTree(root->left, level + 1);
            if (root->middle) printVisualTree(root->middle, level + 1);
            if (root->right) printVisualTree(root->right, level + 1);
        }
    }
}
int main(int argc, char *argv[])
{
  // Create test file
  FILE *testFile = fopen("input.txt", "w");
  if (testFile != NULL)
  {
    fprintf(testFile, "The quick brown fox\n");
    fprintf(testFile, "jumps over the lazy\n");
    fprintf(testFile, "dog while the cat sleeps\n");
    fclose(testFile);
  }
  else
  {
    printf("Error creating test file!\n");
    return 1;
  }

  // Create and build tree
  TTTree *tree = createTree();
  FILE *input = fopen("input.txt", "r");

  if (input != NULL)
  {
    buildTree(tree, input);
    fclose(input);

    printf("\n=== Tree Visualization ===\n\n");
    printVisualTree(tree->root, 0);

    printf("\n=== Tree Statistics ===\n");
    printf("Total words: %d\n", tree->wordCount);
    printf("Tree height: %d\n", findHeight(tree->root));

    // Example search
    printf("\n=== Search Example ===\n");
    const char *searchWord = "fox";
    LineArray searchResult;
    searchResult.lines = NULL;
    searchResult.size = 0;
    searchResult.capacity = 0;

    if (searchTreeHelper(tree->root, searchWord, &searchResult))
    {
      printf("Found '%s' in lines:", searchWord);
      for (int i = 0; i < searchResult.size; i++)
      {
        printf(" %d", searchResult.lines[i]);
      }
      printf("\n");
    }
    else
    {
      printf("Word '%s' not found\n", searchWord);
    }

    freeTree(tree);
  }
  else
  {
    printf("Error opening input file!\n");
    return 1;
  }

  return 0;
}

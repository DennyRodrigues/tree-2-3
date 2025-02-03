#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>

#define MAX_WORD_LENGTH 100
#define INITIAL_LINES_CAPACITY 10
#define MAX_WIDTH 126

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
void printTreeHelper(FILE *out, Node *x);
int findHeight(Node *x);
void freeNode(Node *node);
void freeTree(TTTree *tree);
void printTree(Node *root);


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
  // strcmp compara duas palavras (X e N). Se forem iguais, retorna 0. Se forem diferentes, retorna um valor positivo se X for maior que N (primeiro caractere diferente tem ASCII maior) ou negativo se X for menor que N (primeiro caractere diferente tem ASCII menor).

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
    printTree(tree->root);

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

// Print spaces for alignment
void printSpaces(int count)
{
  for (int i = 0; i < count; i++)
  {
    printf(" ");
  }
}

// Print a single node
void printNode(Node *node)
{
  if (node == NULL)
    return;
  if (node->rightKey == NULL)
    printf("[%s]", node->leftKey);
  else
    printf("[%s|%s]", node->leftKey, node->rightKey);
}

// Calculate the position adjustment based on node type and position
int getPositionAdjustment(Node *parent, Node *current)
{
  if (parent == NULL)
    return 0;

  // For two-node parent
  if (parent->rightKey == NULL)
  {
    if (current == parent->left)
      return -1;
    if (current == parent->middle)
      return 1;
  }

  return 0;
}

void printTreeLevel(Node *root, Node *parent, int level, int currentLevel, int spacing)
{
  if (root == NULL)
    return;

  if (level == currentLevel)
  {
    int adjustment = getPositionAdjustment(parent, root);
    printSpaces(spacing);
    printNode(root);
    return;
  }

  if (!isLeafNode(root))
  {
    int childSpacing = spacing;

    // Adjust spacing based on whether it's a 2-node or 3-node
    if (root->rightKey == NULL)
    {
      // For 2-node
      printTreeLevel(root->left, root, level, currentLevel + 1, childSpacing);
      printTreeLevel(root->middle, root, level, currentLevel + 1, childSpacing);
    }
    else
    {
      // For 3-node
      printTreeLevel(root->left, root, level, currentLevel + 1, childSpacing);
      printTreeLevel(root->middle, root, level, currentLevel + 1, childSpacing);
      printTreeLevel(root->right, root, level, currentLevel + 1, childSpacing);
    }
  }
}

void printTree(Node *root)
{
  printf("\n=== 2-3 Tree Visualization ===\n\n");

  int height = findHeight(root);
  int initialSpacing = MAX_WIDTH / 2;

  for (int i = 0; i < height; i++)
  {
    printTreeLevel(root, NULL, i, 0, initialSpacing);
    printf("\n\n");
    initialSpacing /= 2;
  }
}

// Updated getUserInput function with better error handling
int getUserInput(TTTree *tree)
{
  int opcao;
  char letra[100];

  printf("\nEscolha o que você deseja fazer com a árvore:\n");
  printf("1 para inserir\n");
  printf("2 para deletar algum elemento\n");
  printf("Opção: ");

  if (scanf("%d", &opcao) != 1)
  {
    while (getchar() != '\n')
      ; // Clear input buffer
    printf("Entrada inválida!\n");
    return 0;
  }

  if (opcao == 1)
  {
    printf("Entre alguma palavra para adicionar na árvore: ");
    if (scanf("%99s", letra) != 1)
    {
      printf("Erro na leitura da palavra!\n");
      return 0;
    }

    // Insert word
    if (tree->wordCount >= tree->wordCapacity)
    {
      int newCapacity = tree->wordCapacity * 2;
      char **newWords = realloc(tree->words, sizeof(char *) * newCapacity);
      if (!newWords)
      {
        printf("Erro de alocação de memória!\n");
        return 0;
      }
      tree->words = newWords;
      tree->wordCapacity = newCapacity;
    }

    int distWords = 0;
    tree->words[tree->wordCount] = strdup(letra);
    tree->root = insert(tree, letra, 1, tree->root, &distWords);
    tree->wordCount++;

    printf("Palavra '%s' inserida com sucesso!\n", letra);
  }
  else if (opcao == 2)
  {
    printf("Entre qual elemento você deseja deletar: ");
    printf("Feature ainda não implementada");
    if (scanf("%99s", letra) != 1)
    {
      printf("Erro na leitura da palavra!\n");
      return 0;
    }
  }
  return 0;
}

int main(int argc, char *argv[])
{
  TTTree *tree = createTree();
  FILE *input = fopen("input.txt", "r");

  if (input != NULL)
  {
    buildTree(tree, input);
    fclose(input);

    // Print tree with improved visualization
    printTree(tree->root);

    while (1)
    {
      getUserInput(tree);

      printTree(tree->root);
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

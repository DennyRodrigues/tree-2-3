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
  char *chaveNaEsquerda;
  char *chaveNaDireita;
  LineArray leftLines;
  LineArray rightLines;
  struct Node *ponteiroDaEsquerda;
  struct Node *ponteiroDoMeio;
  struct Node *ponteiroDaDireita;
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
void freeLineArray(LineArray *arr);
Node *createNode(const char *x);
bool isLeafNode(Node *x);
Node *adicionar(Node *x, Node *n);
Node *inserirNaArvore(TTTree *tree, const char *key, int line, Node *root, int *distWord);
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

// Create new node
Node *createNode(const char *x)
{
  Node *t = (Node *)malloc(sizeof(Node));
  t->chaveNaEsquerda = strdup(x);
  t->chaveNaDireita = NULL;
  t->leftLines = createLineArray();
  t->rightLines = createLineArray();
  t->ponteiroDaEsquerda = NULL;
  t->ponteiroDoMeio = NULL;
  t->ponteiroDaDireita = NULL;
  return t;
}

// Check if node is leaf
bool isLeafNode(Node *x)
{
  return (x->ponteiroDaEsquerda == NULL && x->ponteiroDoMeio == NULL && x->ponteiroDaDireita == NULL);
}

// Add node to existing node
Node *adicionar(Node *x, Node *n)
{
  // strcmp compara duas palavras (X e N). Se forem iguais, retorna 0. Se forem diferentes, retorna um valor positivo se X for maior que N (primeiro caractere diferente tem ASCII maior) ou negativo se X for menor que N (primeiro caractere diferente tem ASCII menor).

  if (x->chaveNaDireita == NULL)
  {
    if (strcmp(x->chaveNaEsquerda, n->chaveNaEsquerda) < 0)
    {
      x->chaveNaDireita = strdup(n->chaveNaEsquerda);
      x->rightLines = n->leftLines;
      x->ponteiroDoMeio = n->ponteiroDaEsquerda;
      x->ponteiroDaDireita = n->ponteiroDoMeio;
    }
    else
    {
      x->chaveNaDireita = strdup(x->chaveNaEsquerda);
      x->rightLines = x->leftLines;
      x->chaveNaEsquerda = strdup(n->chaveNaEsquerda);
      x->leftLines = n->leftLines;
      x->ponteiroDaDireita = x->ponteiroDoMeio;
      x->ponteiroDoMeio = n->ponteiroDoMeio;
      x->ponteiroDaEsquerda = n->ponteiroDaEsquerda;
    }
    free(n->chaveNaEsquerda);
    free(n);
    return x;
  }

  // Add to left
  if (strcmp(x->chaveNaEsquerda, n->chaveNaEsquerda) >= 0)
  {
    Node *newNode = createNode(x->chaveNaEsquerda);
    newNode->leftLines = x->leftLines;
    newNode->ponteiroDaEsquerda = n;
    newNode->ponteiroDoMeio = x;
    x->ponteiroDaEsquerda = x->ponteiroDoMeio;
    x->ponteiroDoMeio = x->ponteiroDaDireita;
    x->ponteiroDaDireita = NULL;
    free(x->chaveNaEsquerda);
    x->chaveNaEsquerda = strdup(x->chaveNaDireita);
    x->leftLines = x->rightLines;
    free(x->chaveNaDireita);
    x->chaveNaDireita = NULL;
    return newNode;
  }
  // Add to center
  else if (strcmp(x->chaveNaDireita, n->chaveNaEsquerda) >= 0)
  {
    Node *newNode = createNode(x->chaveNaDireita);
    newNode->leftLines = x->rightLines;
    newNode->ponteiroDaEsquerda = n->ponteiroDoMeio;
    newNode->ponteiroDoMeio = x->ponteiroDaDireita;
    x->ponteiroDoMeio = n->ponteiroDaEsquerda;
    n->ponteiroDoMeio = newNode;
    n->ponteiroDaEsquerda = x;
    free(x->chaveNaDireita);
    x->chaveNaDireita = NULL;
    x->ponteiroDaDireita = NULL;
    return n;
  }
  // Add to right
  else
  {
    Node *newNode = createNode(x->chaveNaDireita);
    newNode->leftLines = x->rightLines;
    newNode->ponteiroDaEsquerda = x;
    newNode->ponteiroDoMeio = n;
    free(x->chaveNaDireita);
    x->chaveNaDireita = NULL;
    x->ponteiroDaDireita = NULL;
    return newNode;
  }
}

// Insert key into tree
Node *inserirNaArvore(TTTree *tree, const char *key, int line, Node *root, int *distWord)
{
  if (root == NULL)
  {
    Node *newNode = createNode(key);
    (*distWord)++;
    return newNode;
  }

  if ((root->chaveNaEsquerda && strcmp(key, root->chaveNaEsquerda) == 0) ||
      (root->chaveNaDireita && strcmp(key, root->chaveNaDireita) == 0))
  {
    return root;
  }

  if (isLeafNode(root))
  {
    Node *newNode = createNode(key);
    Node *finalNode = adicionar(root, newNode);
    (*distWord)++;
    return finalNode;
  }

  // Insert recursively
  if (strcmp(key, root->chaveNaEsquerda) < 0)
  {
    Node *newNode = inserirNaArvore(tree, key, line, root->ponteiroDaEsquerda, distWord);
    if (newNode == root->ponteiroDaEsquerda)
      return root;
    else
      return adicionar(root, newNode);
  }
  else if (root->chaveNaDireita == NULL || strcmp(key, root->chaveNaDireita) < 0)
  {
    Node *newNode = inserirNaArvore(tree, key, line, root->ponteiroDoMeio, distWord);
    if (newNode == root->ponteiroDoMeio)
      return root;
    else
      return adicionar(root, newNode);
  }
  else
  {
    Node *newNode = inserirNaArvore(tree, key, line, root->ponteiroDaDireita, distWord);
    if (newNode == root->ponteiroDaDireita)
      return root;
    else
      return adicionar(root, newNode);
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
        tree->root = inserirNaArvore(tree, word, line, tree->root, &distWords);
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

  if (x->chaveNaEsquerda && strcmp(value, x->chaveNaEsquerda) == 0)
  {
    *lines = x->leftLines;
    return true;
  }

  if (x->chaveNaDireita && strcmp(value, x->chaveNaDireita) == 0)
  {
    *lines = x->rightLines;
    return true;
  }

  if (isLeafNode(x))
    return false;

  if (x->chaveNaDireita != NULL)
  {
    if (strcmp(value, x->chaveNaEsquerda) < 0)
    {
      return searchTreeHelper(x->ponteiroDaEsquerda, value, lines);
    }
    else if (strcmp(value, x->chaveNaEsquerda) > 0 && strcmp(value, x->chaveNaDireita) < 0)
    {
      return searchTreeHelper(x->ponteiroDoMeio, value, lines);
    }
    else
    {
      return searchTreeHelper(x->ponteiroDaDireita, value, lines);
    }
  }
  else
  {
    if (strcmp(value, x->chaveNaEsquerda) < 0)
      return searchTreeHelper(x->ponteiroDaEsquerda, value, lines);
    else
      return searchTreeHelper(x->ponteiroDoMeio, value, lines);
  }
}

// Find height
int findHeight(Node *x)
{
  if (x == NULL)
    return 0;
  else
  {
    int leftHeight = findHeight(x->ponteiroDaEsquerda);
    int rightHeight = findHeight(x->ponteiroDaDireita);
    int middleHeight = findHeight(x->ponteiroDoMeio);

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
    free(node->chaveNaEsquerda);
    if (node->chaveNaDireita)
      free(node->chaveNaDireita);
    freeLineArray(&node->leftLines);
    freeLineArray(&node->rightLines);
    freeNode(node->ponteiroDaEsquerda);
    freeNode(node->ponteiroDoMeio);
    freeNode(node->ponteiroDaDireita);
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
  if (node->chaveNaDireita == NULL)
    printf("[%s]", node->chaveNaEsquerda);
  else
    printf("[%s|%s]", node->chaveNaEsquerda, node->chaveNaDireita);
}

// Calculate the position adjustment based on node type and position
int getPositionAdjustment(Node *parent, Node *current)
{
  if (parent == NULL)
    return 0;

  // For two-node parent
  if (parent->chaveNaDireita == NULL)
  {
    if (current == parent->ponteiroDaEsquerda)
      return -1;
    if (current == parent->ponteiroDoMeio)
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
    if (root->chaveNaDireita == NULL)
    {
      // For 2-node
      printTreeLevel(root->ponteiroDaEsquerda, root, level, currentLevel + 1, childSpacing);
      printTreeLevel(root->ponteiroDoMeio, root, level, currentLevel + 1, childSpacing);
    }
    else
    {
      // For 3-node
      printTreeLevel(root->ponteiroDaEsquerda, root, level, currentLevel + 1, childSpacing);
      printTreeLevel(root->ponteiroDoMeio, root, level, currentLevel + 1, childSpacing);
      printTreeLevel(root->ponteiroDaDireita, root, level, currentLevel + 1, childSpacing);
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

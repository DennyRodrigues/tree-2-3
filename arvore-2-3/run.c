#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>

#define MAX_WORD_LENGTH 100
#define MAX_WIDTH 126

// Node structure
typedef struct Node
{
  char *chaveNaEsquerda;
  char *chaveNaDireita;
  struct Node *ponteiroDaEsquerda;
  struct Node *ponteiroDoMeio;
  struct Node *ponteiroDaDireita;
  struct Node *ponteiroPai;
} Node;

// Arvore structure
typedef struct
{
  Node *raiz;
  char **palavras; // Array para armazenar palavras
  int quantidadePalavras; 
  int capacidadePalavras;
} Arvore;

Node *CriarNovoNode(const char *x);
bool verificaSeNodeEhFolha(Node *x);
Node *adicionarNode(Node *x, Node *n);
Node *inserirNaArvore(Arvore *arvore, const char *key, Node *raiz);
bool buscarNaArvore(Node *x, const char *value);
int calcularAltura(Node *x);
void freeNode(Node *node);
void freeArvore(Arvore *arvore);
void imprimirArvore(Node *raiz);
Node *deletar(Arvore *arvore, const char *chave, Node *raiz);

// Initialize arvore
Arvore *CriarArvore()
{
  Arvore *arvore = (Arvore *)malloc(sizeof(Arvore));
  arvore->raiz = NULL;
  arvore->palavras = (char **)malloc(sizeof(char *) * 1000); // Initial capacity
  arvore->quantidadePalavras = 0;
  arvore->capacidadePalavras = 1000;
  return arvore;
}

// Cria um novo Node que não contém nhum filho no momento
Node *CriarNovoNode(const char *x)
{
  Node *t = (Node *)malloc(sizeof(Node));
  t->chaveNaEsquerda = strdup(x);
  t->chaveNaDireita = NULL;
  t->ponteiroDaEsquerda = NULL;
  t->ponteiroDoMeio = NULL;
  t->ponteiroDaDireita = NULL;
  return t;
}

// Verifica se o Node não contém nenhum filho(está na ponta da arvoré)
bool verificaSeNodeEhFolha(Node *x)
{
  return (x->ponteiroDaEsquerda == NULL && x->ponteiroDoMeio == NULL && x->ponteiroDaDireita == NULL);
}

// Add node to existing node
Node *adicionarNode(Node *x, Node *n)
{
  // strcmp compara duas palavras (X e N). Se forem iguais, retorna 0. Se forem diferentes, retorna um valor positivo se X for maior que N (o primeiro caractere diferente tem ASCII maior) ou negativo se X for menor que N (primeiro caractere diferente tem ASCII menor).
  // Resumindo: Ajuda a organizar palavras de acordo com o ASCII, permitindo organizar as palavras em ordem alfabética sem precisar criar loops para checkar cada letra da palavra.
  if (x->chaveNaDireita == NULL)
  {
    if (strcmp(x->chaveNaEsquerda, n->chaveNaEsquerda) < 0)
    {
      x->chaveNaDireita = strdup(n->chaveNaEsquerda);
      x->ponteiroDoMeio = n->ponteiroDaEsquerda;
      x->ponteiroDaDireita = n->ponteiroDoMeio;
      // Atualiza os pais dos filhos
      if (n->ponteiroDaEsquerda)
        n->ponteiroDaEsquerda->ponteiroPai = x;
      if (n->ponteiroDoMeio)
        n->ponteiroDoMeio->ponteiroPai = x;
    }
    else
    {
      x->chaveNaDireita = strdup(x->chaveNaEsquerda);
      x->chaveNaEsquerda = strdup(n->chaveNaEsquerda);
      x->ponteiroDaDireita = x->ponteiroDoMeio;
      x->ponteiroDoMeio = n->ponteiroDoMeio;
      x->ponteiroDaEsquerda = n->ponteiroDaEsquerda;
      // Atualiza os pais dos filhos
      if (n->ponteiroDaEsquerda)
        n->ponteiroDaEsquerda->ponteiroPai = x;
      if (n->ponteiroDoMeio)
        n->ponteiroDoMeio->ponteiroPai = x;
    }
    free(n->chaveNaEsquerda);
    free(n);
    return x;
  }

  // Add to left
  if (strcmp(x->chaveNaEsquerda, n->chaveNaEsquerda) >= 0)
  {
    Node *newNode = CriarNovoNode(x->chaveNaEsquerda);
    newNode->ponteiroDaEsquerda = n;
    newNode->ponteiroDoMeio = x;
    // Atualiza os pais
    n->ponteiroPai = newNode;
    x->ponteiroPai = newNode;
    x->ponteiroDaEsquerda = x->ponteiroDoMeio;
    x->ponteiroDoMeio = x->ponteiroDaDireita;
    x->ponteiroDaDireita = NULL;
    free(x->chaveNaEsquerda);
    x->chaveNaEsquerda = strdup(x->chaveNaDireita);
    free(x->chaveNaDireita);
    x->chaveNaDireita = NULL;
    return newNode;
  }
  // Add to center
  else if (strcmp(x->chaveNaDireita, n->chaveNaEsquerda) >= 0)
  {
    Node *newNode = CriarNovoNode(x->chaveNaDireita);
    newNode->ponteiroDaEsquerda = n->ponteiroDoMeio;
    newNode->ponteiroDoMeio = x->ponteiroDaDireita;
    // Atualiza os pais
    if (n->ponteiroDoMeio)
      n->ponteiroDoMeio->ponteiroPai = newNode;
    if (x->ponteiroDaDireita)
      x->ponteiroDaDireita->ponteiroPai = newNode;
    x->ponteiroDoMeio = n->ponteiroDaEsquerda;
    if (n->ponteiroDaEsquerda)
      n->ponteiroDaEsquerda->ponteiroPai = x;
    n->ponteiroDoMeio = newNode;
    newNode->ponteiroPai = n;
    n->ponteiroDaEsquerda = x;
    x->ponteiroPai = n;
    free(x->chaveNaDireita);
    x->chaveNaDireita = NULL;
    x->ponteiroDaDireita = NULL;
    return n;
  }
  // Add to right
  else
  {
    Node *newNode = CriarNovoNode(x->chaveNaDireita);
    newNode->ponteiroDaEsquerda = x;
    newNode->ponteiroDoMeio = n;
    // Atualiza os pais
    x->ponteiroPai = newNode;
    n->ponteiroPai = newNode;
    free(x->chaveNaDireita);
    x->chaveNaDireita = NULL;
    x->ponteiroDaDireita = NULL;
    return newNode;
  }
}

Node *inserirNaArvore(Arvore *arvore, const char *key, Node *raiz)
{

  // Cria o primeiro Node
  if (raiz == NULL)
  {
    Node *newNode = CriarNovoNode(key);
    newNode->ponteiroPai = NULL; // Raiz não tem ponteiroPai
    return newNode;
  }

  // Caso o valor que está inserido é o mesmo que ja existe no node Raiz
  if ((raiz->chaveNaEsquerda && strcmp(key, raiz->chaveNaEsquerda) == 0) ||
      (raiz->chaveNaDireita && strcmp(key, raiz->chaveNaDireita) == 0))
  {
    return raiz;
  }

  if (verificaSeNodeEhFolha(raiz))
  {
    Node *newNode = CriarNovoNode(key);
    newNode->ponteiroPai = raiz; // Define o ponteiroPai do novo nó
    Node *finalNode = adicionarNode(raiz, newNode);
    if (finalNode != raiz)
    {
      finalNode->ponteiroPai = NULL; // Se criou um novo nó raiz
    }
    return finalNode;
  }

  // Insert recursively
  if (strcmp(key, raiz->chaveNaEsquerda) < 0)
  {
    Node *newNode = inserirNaArvore(arvore, key, raiz->ponteiroDaEsquerda);
    if (newNode == raiz->ponteiroDaEsquerda)
      return raiz;
    else
    {
      Node *result = adicionarNode(raiz, newNode);
      if (result != raiz)
        result->ponteiroPai = NULL; // Se criou novo nó raiz
      return result;
    }
  }
  else if (raiz->chaveNaDireita == NULL || strcmp(key, raiz->chaveNaDireita) < 0)
  {
    Node *newNode = inserirNaArvore(arvore, key, raiz->ponteiroDoMeio);
    if (newNode == raiz->ponteiroDoMeio)
      return raiz;
    else
    {
      Node *result = adicionarNode(raiz, newNode);
      if (result != raiz)
        result->ponteiroPai = NULL;
      return result;
    }
  }
  else
  {
    Node *newNode = inserirNaArvore(arvore, key, raiz->ponteiroDaDireita);
    if (newNode == raiz->ponteiroDaDireita)
      return raiz;
    else
    {
      Node *result = adicionarNode(raiz, newNode);
      if (result != raiz)
        result->ponteiroPai = NULL;
      return result;
    }
  }
}

// Build arvore from file
void buildArvore(Arvore *arvore, FILE *input)
{
  printf("-----------------------------------------------------\n");
  printf("[MSG] BUILDING 2-3 TREE...\n");

  char buffer[1024];
  char word[MAX_WORD_LENGTH];
  clock_t startTime = clock();

  while (fgets(buffer, sizeof(buffer), input))
  {
    imprimirArvore(arvore->raiz);

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
        if (arvore->quantidadePalavras >= arvore->capacidadePalavras)
        {
          arvore->capacidadePalavras *= 2;
          arvore->palavras = realloc(arvore->palavras, sizeof(char *) * arvore->capacidadePalavras);
        }
        arvore->palavras[arvore->quantidadePalavras] = strdup(word);
        arvore->quantidadePalavras++;
        arvore->raiz = inserirNaArvore(arvore, word, arvore->raiz);
      }
      token = strtok(NULL, " \n");
    };
  }

  double totalTime = (double)(clock() - startTime) / CLOCKS_PER_SEC;
  int arvoreHeight = calcularAltura(arvore->raiz);

  printf("=====================================================\n");
  printf("- Built Arvore results (2-3 Arvore)\n");
  printf("=====================================================\n");
  printf("Total time spent building index: %f\n", totalTime);
  printf("Height of 2-3 Arvore is: %d\n", arvoreHeight);
}

// Search helper
bool buscarNaArvore(Node *x, const char *value)
{
  if (x == NULL)
  {
    return false;
  }

  if (verificaSeNodeEhFolha(x))
    return false;

  if (x->chaveNaDireita != NULL)
  {
    if (strcmp(value, x->chaveNaEsquerda) < 0)
    {
      return buscarNaArvore(x->ponteiroDaEsquerda, value);
    }
    else if (strcmp(value, x->chaveNaEsquerda) > 0 && strcmp(value, x->chaveNaDireita) < 0)
    {
      return buscarNaArvore(x->ponteiroDoMeio, value);
    }
    else
    {
      return buscarNaArvore(x->ponteiroDaDireita, value);
    }
  }
  else
  {
    if (strcmp(value, x->chaveNaEsquerda) < 0)
      return buscarNaArvore(x->ponteiroDaEsquerda, value);
    else
      return buscarNaArvore(x->ponteiroDoMeio, value);
  }
}

// Find height
int calcularAltura(Node *x)
{
  if (x == NULL) return 0;
  else
  {
    int leftHeight = calcularAltura(x->ponteiroDaEsquerda);
    int rightHeight = calcularAltura(x->ponteiroDaDireita);
    int middleHeight = calcularAltura(x->ponteiroDoMeio);

    int maxHeight = leftHeight;
    if (rightHeight > maxHeight)
      maxHeight = rightHeight;
    if (middleHeight > maxHeight)
      maxHeight = middleHeight;

    return maxHeight + 1;
  }
}

// Free node
void freeNode(Node *node)
{
  if (node != NULL)
  {
    free(node->chaveNaEsquerda);
    if (node->chaveNaDireita)
      free(node->chaveNaDireita);
    freeNode(node->ponteiroDaEsquerda);
    freeNode(node->ponteiroDoMeio);
    freeNode(node->ponteiroDaDireita);
    free(node);
  }
}

// Free arvore
void freeArvore(Arvore *arvore)
{
  if (arvore != NULL)
  {
    for (int i = 0; i < arvore->quantidadePalavras; i++)
    {
      free(arvore->palavras[i]);
    }
    free(arvore->palavras);
    freeNode(arvore->raiz);
    free(arvore);
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
  {
    printf("[%s]", node->chaveNaEsquerda);
    // if (node->ponteiroPai)
    // {
    //   printf("ponteiroPai[%s]", node->ponteiroPai->chaveNaEsquerda);
    // }
  }
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

void printArvoreLevel(Node *raiz, Node *parent, int level, int currentLevel, int spacing)
{
  if (raiz == NULL)
    return;

  if (level == currentLevel)
  {
    int adjustment = getPositionAdjustment(parent, raiz);
    printSpaces(spacing);
    printNode(raiz);
    return;
  }

  if (!verificaSeNodeEhFolha(raiz))
  {
    int childSpacing = spacing;

    // Adjust spacing based on whether it's a 2-node or 3-node
    if (raiz->chaveNaDireita == NULL)
    {
      // For 2-node
      printArvoreLevel(raiz->ponteiroDaEsquerda, raiz, level, currentLevel + 1, childSpacing);
      printArvoreLevel(raiz->ponteiroDoMeio, raiz, level, currentLevel + 1, childSpacing);
    }
    else
    {
      // For 3-node
      printArvoreLevel(raiz->ponteiroDaEsquerda, raiz, level, currentLevel + 1, childSpacing);
      printArvoreLevel(raiz->ponteiroDoMeio, raiz, level, currentLevel + 1, childSpacing);
      printArvoreLevel(raiz->ponteiroDaDireita, raiz, level, currentLevel + 1, childSpacing);
    }
  }
}

void imprimirArvore(Node *raiz)
{
  printf("\n=== 2-3 Arvore Visualization ===\n\n");

  int height = calcularAltura(raiz);
  int initialSpacing = MAX_WIDTH / 2;

  for (int i = 0; i < height; i++)
  {
    printArvoreLevel(raiz, NULL, i, 0, initialSpacing);
    printf("\n\n");
    initialSpacing /= 2;
  }
}

Node *reconstruirArvore(Arvore *arvore, const char *palavraRemovida)
{
  // Criar uma nova árvore temporária
  Node *novaRaiz = NULL;

  // Reconstrói a árvore com todas as palavras exceto a removida
  for (int i = 0; i < arvore->quantidadePalavras; i++)
  {
    // Pula a palavra que deve ser removida
    if (strcmp(arvore->palavras[i], palavraRemovida) == 0)
    {
      continue;
    }
    novaRaiz = inserirNaArvore(arvore, arvore->palavras[i], novaRaiz);
  }

  return novaRaiz;
}

int obterEntradaUsuario(Arvore *arvore)
{
  int opcao;
  char palavra[100];

  printf("\nEscolha o que você deseja fazer com a árvore:\n");
  printf("1 para inserir\n");
  printf("2 para deletar\n");
  printf("Opção: ");

  if (scanf("%d", &opcao) != 1)
  {
    while (getchar() != '\n')
      ; // Limpa buffer
    printf("Entrada inválida!\n");
    return 0;
  }

  if (opcao == 1)
  {
    printf("Digite a palavra para adicionar na árvore: ");
    if (scanf("%99s", palavra) != 1)
    {
      printf("Erro na leitura da palavra!\n");
      return 0;
    }

    // Verifica se precisa aumentar o array de palavras
    if (arvore->quantidadePalavras >= arvore->capacidadePalavras)
    {
      int novaCapacidade = arvore->capacidadePalavras * 2;
      char **novasPalavras = realloc(arvore->palavras,
                                     sizeof(char *) * novaCapacidade);

      if (!novasPalavras)
      {
        printf("Erro de alocação de memória!\n");
        return 0;
      }
      arvore->palavras = novasPalavras;
      arvore->capacidadePalavras = novaCapacidade;
    }

    // Insere a palavra
    arvore->palavras[arvore->quantidadePalavras] = strdup(palavra);
    arvore->raiz = inserirNaArvore(arvore, palavra, arvore->raiz);
    arvore->quantidadePalavras++;

    printf("Palavra '%s' inserida com sucesso!\n", palavra);
  }
  else if (opcao == 2)
  {
    printf("Digite a palavra que deseja deletar: ");
    if (scanf("%99s", palavra) != 1)
    {
      printf("Erro na leitura da palavra!\n");
      return 0;
    }

    // Verifica se a palavra existe e a remove do array
    bool palavraEncontrada = false;
    for (int i = 0; i < arvore->quantidadePalavras; i++)
    {
      if (strcmp(arvore->palavras[i], palavra) == 0)
      {
        palavraEncontrada = true;
        free(arvore->palavras[i]);

        // Move todas as palavras uma posição para trás
        for (int j = i; j < arvore->quantidadePalavras - 1; j++)
        {
          arvore->palavras[j] = arvore->palavras[j + 1];
        }
        arvore->quantidadePalavras--;
        break;
      }
    }

    if (!palavraEncontrada)
    {
      printf("Palavra '%s' não encontrada na árvore!\n", palavra);
      return 0;
    }

    // Libera a árvore atual e reconstrói
    freeNode(arvore->raiz);
    arvore->raiz = reconstruirArvore(arvore, palavra);
    printf("Palavra '%s' deletada com sucesso!\n", palavra);
  }
  else
  {
    printf("Opção inválida!\n");
  }

  return 0;
}

int main(int argc, char *argv[])
{
  Arvore *arvore = CriarArvore();
  FILE *input = fopen("input.txt", "r");

  if (input != NULL)
  {
    buildArvore(arvore, input);
    fclose(input);

    // Print arvore with improved visualization
    imprimirArvore(arvore->raiz);

    while (1)
    {
      obterEntradaUsuario(arvore);

      imprimirArvore(arvore->raiz);
    }

    freeArvore(arvore);
  }
  else
  {
    printf("Error opening input file!\n");
    return 1;
  }
  return 0;
}

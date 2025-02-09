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

#define MAX_HEIGHT 10
#define LARGURA_TELA 80
#define ALTURA_TELA 20

typedef struct
{
  char **tela;
  int altura;
  int largura;
} TelaVisualizacao;

// Função auxiliar para calcular o centro de um nó
int calcularCentroNo(Node *node)
{
  if (node->chaveNaDireita)
  {
    return 5; // [A|B] = 5 caracteres
  }
  return 3; // [A] = 3 caracteres
}

void desenharNo(TelaVisualizacao *tela, Node *node, int linha, int coluna)
{
  if (!node)
    return;

  char nodeStr[10];
  if (node->chaveNaDireita)
  {
    sprintf(nodeStr, "[%s|%s]", node->chaveNaEsquerda, node->chaveNaDireita);
  }
  else
  {
    sprintf(nodeStr, "[%s]", node->chaveNaEsquerda);
  }

  // Centraliza o nó na posição
  int len = strlen(nodeStr);
  int start = coluna - len / 2;
  for (int i = 0; i < len; i++)
  {
    if (start + i >= 0 && start + i < tela->largura)
    {
      tela->tela[linha][start + i] = nodeStr[i];
    }
  }
}

void desenharConexoes(TelaVisualizacao *tela, int linhaPai, int colunaPai,
                      int linhaFilho, int colunaFilho, bool ehMeio, bool ehNoPaiDuplo)
{
  if (ehMeio)
  {
    // Centraliza a linha vertical baseado no tipo do nó pai
    int ajuste = ehNoPaiDuplo ? 0 : 1;
    for (int i = 1; i < linhaFilho - linhaPai; i++)
    {
      tela->tela[linhaPai + i][colunaPai + ajuste] = '|';
    }
  }
  else
  {
    int distancia = abs(colunaFilho - colunaPai);
    int direcao = (colunaFilho < colunaPai) ? -1 : 1;
    // Sempre usa '/' para esquerda e '\' para direita, independente do tipo de nó
    char simbolo = (direcao < 0) ? '/' : '\\';

    // Ajusta o ponto inicial para nós simples
    if (!ehNoPaiDuplo)
    {
      colunaPai += (direcao < 0) ? -1 : 1;
    }

    for (int i = 1; i < linhaFilho - linhaPai; i++)
    {
      int col = colunaPai + (direcao * i * distancia) / (linhaFilho - linhaPai);
      if (col >= 0 && col < tela->largura)
      {
        tela->tela[linhaPai + i][col] = simbolo;
      }
    }
  }
}

void desenharArvoreRecursivo(TelaVisualizacao *tela, Node *node, int nivel, int coluna)
{
  if (!node)
    return;

  int linha = nivel * 3;
  bool ehNoDuplo = node->chaveNaDireita != NULL;
  desenharNo(tela, node, linha, coluna);

  if (!verificaSeNodeEhFolha(node))
  {
    int espacamento = 20 / (nivel + 1);

    // Ajusta espaçamento baseado no tipo do nó
    if (!ehNoDuplo)
    {
      espacamento = (espacamento * 4) / 5;
    }

    int colunaEsq = coluna - espacamento;
    int colunaDir = coluna + espacamento;

    // Desenha filho esquerdo
    desenharArvoreRecursivo(tela, node->ponteiroDaEsquerda, nivel + 1, colunaEsq);
    desenharConexoes(tela, linha, coluna, linha + 3, colunaEsq, false, ehNoDuplo);

    if (ehNoDuplo)
    {
      // Para nó duplo [A|B]:
      // - ponteiroDoMeio liga com a primeira chave
      // - ponteiroDaDireita liga com a segunda chave
      desenharArvoreRecursivo(tela, node->ponteiroDoMeio, nivel + 1, coluna);
      desenharConexoes(tela, linha, coluna, linha + 3, coluna, true, ehNoDuplo);

      desenharArvoreRecursivo(tela, node->ponteiroDaDireita, nivel + 1, colunaDir);
      desenharConexoes(tela, linha, coluna, linha + 3, colunaDir, false, ehNoDuplo);
    }
    else
    {
      // Para nó simples [A]:
      // - ponteiroDoMeio é tratado como filho direito
      // - ponteiroDaDireita não é usado
      desenharArvoreRecursivo(tela, node->ponteiroDoMeio, nivel + 1, colunaDir);
      desenharConexoes(tela, linha, coluna, linha + 3, colunaDir, false, ehNoDuplo);
    }
  }
}

void imprimirArvore(Node *raiz)
{
  if (!raiz)
  {
    printf("\n=== Árvore 2-3 Vazia ===\n");
    return;
  }

  TelaVisualizacao *tela = malloc(sizeof(TelaVisualizacao));
  tela->altura = ALTURA_TELA;
  tela->largura = LARGURA_TELA;

  // Inicializa a tela com espaços
  tela->tela = malloc(ALTURA_TELA * sizeof(char *));
  for (int i = 0; i < ALTURA_TELA; i++)
  {
    tela->tela[i] = malloc(LARGURA_TELA * sizeof(char));
    memset(tela->tela[i], ' ', LARGURA_TELA);
  }

  printf("\n=== Árvore 2-3 ===\n\n");

  // Começa o desenho no centro da tela
  desenharArvoreRecursivo(tela, raiz, 0, LARGURA_TELA / 2);

  // Imprime a visualização
  for (int i = 0; i < tela->altura; i++)
  {
    bool linhaVazia = true;
    for (int j = 0; j < tela->largura; j++)
    {
      if (tela->tela[i][j] != ' ')
      {
        linhaVazia = false;
        break;
      }
    }
    if (linhaVazia)
      continue;

    for (int j = 0; j < tela->largura; j++)
    {
      printf("%c", tela->tela[i][j]);
    }
    printf("\n");
  }

  // Libera a memória
  for (int i = 0; i < tela->altura; i++)
  {
    free(tela->tela[i]);
  }
  free(tela->tela);
  free(tela);
}

// Por enquanto, o delete exclui toda a árvore e cria uma nova com os valores antigos MENOS o valor que é para ser deletado.
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

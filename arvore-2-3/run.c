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

// Adiciona um novo nó a um nó existente, reorganizando as chaves e ponteiros conforme necessário
Node *adicionarNode(Node *noAtual, Node *novoNo)
{
  // strcmp compara duas palavras (X e N). Se forem iguais, retorna 0. Se forem diferentes, retorna um valor positivo se X for maior que N (o primeiro caractere diferente tem ASCII maior) ou negativo se X for menor que N (primeiro caractere diferente tem ASCII menor).
  // Resumindo: Ajuda a organizar palavras de acordo com o ASCII, permitindo organizar as palavras em ordem alfabética sem precisar criar loops para checkar cada letra da palavra.

  // Adiciona um novo nó a um nó existente, reorganizando as chaves e ponteiros conforme necessário
  // Se o nó x não tem chave na direita, podemos adicionar nele
  // Essa parte do código tem DUAS funções importantes:
  // Adicionar uma segunda chave em um nó que só tem uma chave
  // é aqui que os nós "soltos"(que subiram de níveis inferiores)se encaixam na árvore principal
  if (noAtual->chaveNaDireita == NULL)
  {
    // Se a chave do nó atual é menor que a nova chave, adiciona à direita
    if (strcmp(noAtual->chaveNaEsquerda, novoNo->chaveNaEsquerda) < 0)
    {
      noAtual->chaveNaDireita = strdup(novoNo->chaveNaEsquerda);
      // Reorganiza os ponteiros
      noAtual->ponteiroDoMeio = novoNo->ponteiroDaEsquerda;
      noAtual->ponteiroDaDireita = novoNo->ponteiroDoMeio;
      // Atualiza os ponteiros pai dos filhos
      if (novoNo->ponteiroDaEsquerda)
        novoNo->ponteiroDaEsquerda->ponteiroPai = noAtual;
      if (novoNo->ponteiroDoMeio)
        novoNo->ponteiroDoMeio->ponteiroPai = noAtual;
    }
    // Se a chave do nó atual é maior, move ela para a direita e coloca a nova chave na esquerda
    else
    {
      noAtual->chaveNaDireita = strdup(noAtual->chaveNaEsquerda);
      noAtual->chaveNaEsquerda = strdup(novoNo->chaveNaEsquerda);
      // Reorganiza os ponteiros
      noAtual->ponteiroDaDireita = noAtual->ponteiroDoMeio;
      noAtual->ponteiroDoMeio = novoNo->ponteiroDoMeio;
      noAtual->ponteiroDaEsquerda = novoNo->ponteiroDaEsquerda;
      // Atualiza os ponteiros pai
      if (novoNo->ponteiroDaEsquerda)
        novoNo->ponteiroDaEsquerda->ponteiroPai = noAtual;
      if (novoNo->ponteiroDoMeio)
        novoNo->ponteiroDoMeio->ponteiroPai = noAtual;
    }
    // Libera o nó temporário e retorna o nó modificado
    free(novoNo->chaveNaEsquerda);
    free(novoNo);
    return noAtual;
  }

  // Se o nó já tem duas chaves, precisamos dividir
  // Caso 1: Adiciona à esquerda quando a nova chave é menor que a chave da esquerda
  if (strcmp(noAtual->chaveNaEsquerda, novoNo->chaveNaEsquerda) >= 0)
  {
    Node *newNode = CriarNovoNode(noAtual->chaveNaEsquerda);
    newNode->ponteiroDaEsquerda = novoNo;
    newNode->ponteiroDoMeio = noAtual;
    // Atualiza os ponteiros pai
    novoNo->ponteiroPai = newNode;
    noAtual->ponteiroPai = newNode;
    // Reorganiza o nó atual
    noAtual->ponteiroDaEsquerda = noAtual->ponteiroDoMeio;
    noAtual->ponteiroDoMeio = noAtual->ponteiroDaDireita;
    noAtual->ponteiroDaDireita = NULL;
    free(noAtual->chaveNaEsquerda);
    noAtual->chaveNaEsquerda = strdup(noAtual->chaveNaDireita);
    free(noAtual->chaveNaDireita);
    noAtual->chaveNaDireita = NULL;
    return newNode;
  }
  // Caso 2: Adiciona no meio quando a nova chave está entre as duas chaves existentes
  else if (strcmp(noAtual->chaveNaDireita, novoNo->chaveNaEsquerda) >= 0)
  {
    Node *newNode = CriarNovoNode(noAtual->chaveNaDireita);
    newNode->ponteiroDaEsquerda = novoNo->ponteiroDoMeio;
    newNode->ponteiroDoMeio = noAtual->ponteiroDaDireita;
    // Atualiza os pais
    if (novoNo->ponteiroDoMeio)
      novoNo->ponteiroDoMeio->ponteiroPai = newNode;
    if (noAtual->ponteiroDaDireita)
      noAtual->ponteiroDaDireita->ponteiroPai = newNode;
    noAtual->ponteiroDoMeio = novoNo->ponteiroDaEsquerda;
    if (novoNo->ponteiroDaEsquerda)
      novoNo->ponteiroDaEsquerda->ponteiroPai = noAtual;
    novoNo->ponteiroDoMeio = newNode;
    newNode->ponteiroPai = novoNo;
    novoNo->ponteiroDaEsquerda = noAtual;
    noAtual->ponteiroPai = novoNo;
    free(noAtual->chaveNaDireita);
    noAtual->chaveNaDireita = NULL;
    noAtual->ponteiroDaDireita = NULL;
    return novoNo;
  }
  // Caso 3: Adiciona à direita quando a nova chave é maior que ambas as chaves
  else
  {
    Node *newNode = CriarNovoNode(noAtual->chaveNaDireita);
    newNode->ponteiroDaEsquerda = noAtual;
    newNode->ponteiroDoMeio = novoNo;
    // Atualiza os pais
    noAtual->ponteiroPai = newNode;
    novoNo->ponteiroPai = newNode;
    free(noAtual->chaveNaDireita);
    noAtual->chaveNaDireita = NULL;
    noAtual->ponteiroDaDireita = NULL;
    return newNode;
  }
}
// Função principal de inserção que mantém as propriedades da árvore 2-3
Node *inserirNaArvore(Arvore *arvore, const char *key, Node *raiz)
{
  // Caso base: árvore vazia
  if (raiz == NULL)
  {
    Node *newNode = CriarNovoNode(key);
    newNode->ponteiroPai = NULL;
    return newNode;
  }

  // Evita duplicatas
  if ((raiz->chaveNaEsquerda && strcmp(key, raiz->chaveNaEsquerda) == 0) ||
      (raiz->chaveNaDireita && strcmp(key, raiz->chaveNaDireita) == 0))
  {
    return raiz;
  }

  // CASO 3: INSERÇÃO EM NÓ FOLHA
  // Se chegamos em um nó folha, podemos tentar inserir diretamente nele
  // Caso o nó folha esteja cheio, ocorrerá split através do adicionarNode
  if (verificaSeNodeEhFolha(raiz))
  {
    Node *newNode = CriarNovoNode(key);
    newNode->ponteiroPai = raiz;
    Node *finalNode = adicionarNode(raiz, newNode);
    if (finalNode != raiz)
    {
      finalNode->ponteiroPai = NULL; // Novo nó se tornou raiz
    }
    return finalNode;
  }

  // CASO 4: NAVEGAÇÃO RECURSIVA
  // Não é folha, então precisamos navegar até encontrar onde inserir

  // CASO 4.1: NAVEGAÇÃO PARA ESQUERDA
  // Se a chave é menor que a chave da esquerda, desce pela subárvore esquerda
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

  // CASO 4.2: NAVEGAÇÃO PARA O MEIO
  // Se o nó é simples OU a chave é menor que a chave da direita
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
  // CASO 4.3: NAVEGAÇÃO PARA DIREITA
  // Se o nó é composto (tem duas chaves) e a chave é maior que ambas
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

// Função auxiliar para exibir o percurso ao usar buscarNaArvore
void exibirPercursoDaArvore(Node *noAtual)
{
  if (noAtual == NULL)
  {
    return;
  }

  // Imprime o nó atual
  if (noAtual->chaveNaDireita != NULL)
  {
    printf(" -> [%s|%s] ", noAtual->chaveNaEsquerda, noAtual->chaveNaDireita);
  }
  else
  {
    printf(" -> [%s] ", noAtual->chaveNaEsquerda);
  }
}

// Search helper
bool buscarNaArvore(Node *noAtual, const char *value)
{
  exibirPercursoDaArvore(noAtual);

  // Caso essa função seja chamada em um nó NULL, Isso significa que o elemento que está sendo procurado não existe.
  if (noAtual == NULL)
  {
    return false;
  }

  // Primeiro verifica se o valor está dentro do nó atual, compara com o valor da esquerda e da direita
  if (strcmp(value, noAtual->chaveNaEsquerda) == 0 ||
      (noAtual->chaveNaDireita && strcmp(value, noAtual->chaveNaDireita) == 0))
  {
    return true;
  }

  // Caso a função chegue na folhas da árvore, não vai mais ter para onde a função ir, então retornar falso indicando que o elemento não foi achado.
  if (verificaSeNodeEhFolha(noAtual))
    return false;

  // Caso a àrvore tenha dois elementos e três filhos.
  if (noAtual->chaveNaDireita != NULL)
  {
    if (strcmp(value, noAtual->chaveNaEsquerda) < 0)
    {
      return buscarNaArvore(noAtual->ponteiroDaEsquerda, value);
    }
    else if (strcmp(value, noAtual->chaveNaEsquerda) > 0 && strcmp(value, noAtual->chaveNaDireita) < 0)
    {
      return buscarNaArvore(noAtual->ponteiroDoMeio, value);
    }
    else
    {
      return buscarNaArvore(noAtual->ponteiroDaDireita, value);
    }
  }
  // Caso a àrvore tenha apenas um valor e dois filhos
  else
  {
    if (strcmp(value, noAtual->chaveNaEsquerda) < 0)
      return buscarNaArvore(noAtual->ponteiroDaEsquerda, value);
    else
      return buscarNaArvore(noAtual->ponteiroDoMeio, value);
  }
}

// Pré-ordem: visita a raiz, depois subárvore esquerda, meio e direita
void percorrerPreOrdem(Node *noAtual)
{
  if (noAtual == NULL)
    return;

  // Visita o nó atual (imprime as chaves)
  printf("[%s", noAtual->chaveNaEsquerda);
  if (noAtual->chaveNaDireita)
    printf("|%s", noAtual->chaveNaDireita);
  printf("] ");

  // Percorre os filhos da esquerda para direita
  percorrerPreOrdem(noAtual->ponteiroDaEsquerda);
  percorrerPreOrdem(noAtual->ponteiroDoMeio);
  if (noAtual->chaveNaDireita) // Se tem duas chaves, tem terceiro filho
    percorrerPreOrdem(noAtual->ponteiroDaDireita);
}

// Em-ordem: visita subárvore esquerda, primeira chave, subárvore meio,
// segunda chave (se existir), subárvore direita (se existir)
void percorrerEmOrdem(Node *noAtual)
{
  if (noAtual == NULL)
    return;

  // Visita subárvore esquerda
  percorrerEmOrdem(noAtual->ponteiroDaEsquerda);

  // Visita primeira chave
  printf("%s ", noAtual->chaveNaEsquerda);

  // Visita subárvore do meio
  percorrerEmOrdem(noAtual->ponteiroDoMeio);

  // Se tiver segunda chave, visita ela e subárvore direita
  if (noAtual->chaveNaDireita)
  {
    printf("%s ", noAtual->chaveNaDireita);
    percorrerEmOrdem(noAtual->ponteiroDaDireita);
  }
}

// Pós-ordem: visita todas as subárvores primeiro, depois a raiz
void percorrerPosOrdem(Node *noAtual)
{
  if (noAtual == NULL)
    return;

  // Percorre todos os filhos primeiro
  percorrerPosOrdem(noAtual->ponteiroDaEsquerda);
  percorrerPosOrdem(noAtual->ponteiroDoMeio);
  if (noAtual->chaveNaDireita)
    percorrerPosOrdem(noAtual->ponteiroDaDireita);

  // Depois visita o nó atual
  printf("[%s", noAtual->chaveNaEsquerda);
  if (noAtual->chaveNaDireita)
    printf("|%s", noAtual->chaveNaDireita);
  printf("] ");
}

// Find height
int calcularAltura(Node *x)
{
  if (x == NULL)
    return 0;
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
  printf("2 para procurar algum elemento\n");
  printf("3 para deletar\n");
  printf("4 para percorrer a árvore\n");
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

    // Insere a palavra
    arvore->raiz = inserirNaArvore(arvore, palavra, arvore->raiz);

    printf("Palavra '%s' inserida com sucesso!\n", palavra);
  }
  else if (opcao == 2)

  {

    printf("Digite a palavra que deseja procurar: ");
    if (scanf("%99s", palavra) != 1)
    {
      printf("Erro na leitura da palavra!\n");
      return 0;
    }

    bool palavraEncontrada = buscarNaArvore(arvore->raiz, palavra);
    if (!palavraEncontrada)
    {
      printf("\n Palavra '%s' não encontrada na árvore!\n", palavra);
      return 0;
    }
    else
    {
      printf("\n Palavra '%s' Encontrada! na árvore!\n", palavra);
      return 0;
    }
  }
  else if (opcao == 3)
  {
    printf("Digite a palavra que deseja deletar: ");
    if (scanf("%99s", palavra) != 1)
    {
      printf("Erro na leitura da palavra!\n");
      return 0;
    }

    // Verifica se a palavra existe e a remove do array
    bool palavraEncontrada = buscarNaArvore(arvore->raiz, palavra);
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
  else if (opcao == 4)
  {
    printf("\nEscolha o tipo de percurso:\n");
    printf("1 - Pré-ordem\n");
    printf("2 - Em-ordem\n");
    printf("3 - Pós-ordem\n");
    printf("Opção: ");

    int tipoPercurso;
    if (scanf("%d", &tipoPercurso) != 1)
    {
      while (getchar() != '\n')
        ;
      printf("Entrada inválida!\n");
      return 0;
    }

    printf("\nPercurso: ");
    switch (tipoPercurso)
    {
    case 1:
      printf("Pré-ordem: ");
      percorrerPreOrdem(arvore->raiz);
      break;
    case 2:
      printf("Em-ordem: ");
      percorrerEmOrdem(arvore->raiz);
      break;
    case 3:
      printf("Pós-ordem: ");
      percorrerPosOrdem(arvore->raiz);
      break;
    default:
      printf("Opção inválida!\n");
    }
    printf("\n");
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

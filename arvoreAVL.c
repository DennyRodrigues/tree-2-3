// Programa em C para implementar a Árvore AVL com interação do usuário
#include <stdio.h>
#include <stdlib.h>

// Estrutura do nó da Árvore AVL
struct No
{
  int chave;
  struct No *esquerda;
  struct No *direita;
  int altura;
};

// Função para obter a altura do nó
int obterAltura(struct No *n)
{
  if (n == NULL)
    return 0;
  return n->altura;
}
struct No *remover(struct No *raiz, int chave);
struct No *noValorMinimo(struct No *no);

// Função para criar um novo nó
struct No *criarNo(int chave)
{
  struct No *no = (struct No *)malloc(sizeof(struct No));
  no->chave = chave;
  no->esquerda = NULL;
  no->direita = NULL;
  no->altura = 1;
  return no;
}

// Função utilitária para obter o máximo entre dois números inteiros
int max(int a, int b) { return (a > b) ? a : b; }

// Função para obter o fator de balanceamento de um nó
int obterFatorBalanceamento(struct No *n)
{
  if (n == NULL)
    return 0;
  return obterAltura(n->esquerda) - obterAltura(n->direita);
}

// Função de rotação à direita
struct No *rotacaoDireita(struct No *y)
{
  struct No *x = y->esquerda;
  struct No *T2 = x->direita;
  x->direita = y;
  y->esquerda = T2;
  y->altura = max(obterAltura(y->esquerda), obterAltura(y->direita)) + 1;
  x->altura = max(obterAltura(x->esquerda), obterAltura(x->direita)) + 1;
  return x;
}

// Função de rotação à esquerda
struct No *rotacaoEsquerda(struct No *x)
{
  struct No *y = x->direita;
  struct No *T2 = y->esquerda;
  y->esquerda = x;
  x->direita = T2;
  x->altura = max(obterAltura(x->esquerda), obterAltura(x->direita)) + 1;
  y->altura = max(obterAltura(y->esquerda), obterAltura(y->direita)) + 1;
  return y;
}

// Função para inserir uma chave na Árvore AVL
struct No *inserir(struct No *no, int chave)
{
  if (no == NULL)
    return criarNo(chave);
  if (chave < no->chave)
    no->esquerda = inserir(no->esquerda, chave);
  else if (chave > no->chave)
    no->direita = inserir(no->direita, chave);
  else
    return no;
  no->altura = 1 + max(obterAltura(no->esquerda), obterAltura(no->direita));
  int balanceamento = obterFatorBalanceamento(no);
  if (balanceamento > 1 && chave < no->esquerda->chave)
    return rotacaoDireita(no);
  if (balanceamento < -1 && chave > no->direita->chave)
    return rotacaoEsquerda(no);
  if (balanceamento > 1 && chave > no->esquerda->chave)
  {
    no->esquerda = rotacaoEsquerda(no->esquerda);
    return rotacaoDireita(no);
  }
  if (balanceamento < -1 && chave < no->direita->chave)
  {
    no->direita = rotacaoDireita(no->direita);
    return rotacaoEsquerda(no);
  }
  return no;
}

// Função para buscar um nó na árvore
struct No *buscar(struct No *raiz, int chave)
{
  if (raiz == NULL || raiz->chave == chave)
    return raiz;
  if (chave < raiz->chave)
    return buscar(raiz->esquerda, chave);
  return buscar(raiz->direita, chave);
}

// Função para percorrer a árvore em pré-ordem
void preOrdem(struct No *raiz)
{
  if (raiz != NULL)
  {
    printf("%d ", raiz->chave); // Visita a raiz primeiro
    preOrdem(raiz->esquerda);   // Visita a subárvore esquerda
    preOrdem(raiz->direita);    // Visita a subárvore direita
  }
}

// Função para percorrer a árvore em ordem
void emOrdem(struct No *raiz)
{
  if (raiz != NULL)
  {
    emOrdem(raiz->esquerda);    // Visita a subárvore esquerda
    printf("%d ", raiz->chave); // Visita a raiz
    emOrdem(raiz->direita);     // Visita a subárvore direita
  }
}

// Função para percorrer a árvore em pós-ordem
void posOrdem(struct No *raiz)
{
  if (raiz != NULL)
  {
    posOrdem(raiz->esquerda);   // Visita a subárvore esquerda
    posOrdem(raiz->direita);    // Visita a subárvore direita
    printf("%d ", raiz->chave); // Visita a raiz por último
  }
}

// Função para exibir a arvoré, ela é exibida "Deitada"
void exibirArvore(struct No *raiz, int espaco)
{
  // Espaçamento base entre níveis
  int espacoIncremento = 10;

  if (raiz == NULL)
    return;

  // Aumenta o espaçamento entre níveis
  espaco += espacoIncremento;

  // Processa o filho direito primeiro (estará no topo)
  exibirArvore(raiz->direita, espaco);

  // Imprime o nó atual
  printf("\n");
  for (int i = espacoIncremento; i < espaco; i++)
    printf(" ");
  printf("%d [%d]\n", raiz->chave, obterFatorBalanceamento(raiz));

  // Processa o filho esquerdo
  exibirArvore(raiz->esquerda, espaco);
}

// Função para encontrar o nó com valor mínimo
struct No *noValorMinimo(struct No *no)
{
  struct No *atual = no;

  // Navega para encontrar o nó mais à esquerda (menor valor)
  while (atual->esquerda != NULL)
    atual = atual->esquerda;

  return atual;
}

// Função para remover um nó com chave específica da árvore AVL
int removerNo(struct No **raizPtr, int chave)
{
  // Primeiro verifica se o valor existe na árvore
  if (buscar(*raizPtr, chave) == NULL)
    return 0; // Valor não encontrado

  // Se o valor existe, realiza a remoção
  *raizPtr = remover(*raizPtr, chave);

  return 1; // Valor encontrado e removido
}


// Esta é a função interna que realiza a remoção real
struct No *remover(struct No *raiz, int chave)
{
  // Passo 1: Realizar a remoção padrão de BST
  if (raiz == NULL)
    return raiz;

  // Se a chave for menor que a chave da raiz, está na subárvore esquerda
  if (chave < raiz->chave)
    raiz->esquerda = remover(raiz->esquerda, chave);

  // Se a chave for maior que a chave da raiz, está na subárvore direita
  else if (chave > raiz->chave)
    raiz->direita = remover(raiz->direita, chave);

  // Se a chave é igual à chave da raiz, este é o nó a ser removido
  else
  {
    // Caso 1: Nó com um filho ou sem filhos
    if ((raiz->esquerda == NULL) || (raiz->direita == NULL))
    {
      struct No *temp = raiz->esquerda ? raiz->esquerda : raiz->direita;

      // Caso sem filhos
      if (temp == NULL)
      {
        temp = raiz;
        raiz = NULL;
      }
      // Caso com um filho
      else
        *raiz = *temp; // Copia o conteúdo do filho não-nulo

      free(temp);
    }
    // Caso 2: Nó com dois filhos
    else
    {
      // Encontrar o sucessor in-order (menor nó na subárvore direita)
      struct No *temp = noValorMinimo(raiz->direita);

      // Copiar a chave do sucessor para este nó
      raiz->chave = temp->chave;

      // Remover o sucessor
      raiz->direita = remover(raiz->direita, temp->chave);
    }
  }

  // Se a árvore tinha apenas um nó, retorna
  if (raiz == NULL)
    return raiz;

  // Passo 2: Atualizar a altura do nó atual
  raiz->altura = 1 + max(obterAltura(raiz->esquerda), obterAltura(raiz->direita));

  // Passo 3: Verificar o fator de balanceamento deste nó para ver se ficou desbalanceado
  int balanceamento = obterFatorBalanceamento(raiz);

  // Caso Esquerda-Esquerda
  if (balanceamento > 1 && obterFatorBalanceamento(raiz->esquerda) >= 0)
    return rotacaoDireita(raiz);

  // Caso Esquerda-Direita
  if (balanceamento > 1 && obterFatorBalanceamento(raiz->esquerda) < 0)
  {
    raiz->esquerda = rotacaoEsquerda(raiz->esquerda);
    return rotacaoDireita(raiz);
  }

  // Caso Direita-Direita
  if (balanceamento < -1 && obterFatorBalanceamento(raiz->direita) <= 0)
    return rotacaoEsquerda(raiz);

  // Caso Direita-Esquerda
  if (balanceamento < -1 && obterFatorBalanceamento(raiz->direita) > 0)
  {
    raiz->direita = rotacaoDireita(raiz->direita);
    return rotacaoEsquerda(raiz);
  }

  return raiz;
}

int main()
{
  struct No *raiz = NULL;
  int opcao, chave;
  raiz = inserir(raiz, 1);
  raiz = inserir(raiz, 2);
  raiz = inserir(raiz, 4);
  raiz = inserir(raiz, 5);
  raiz = inserir(raiz, 6);
  raiz = inserir(raiz, 3);

  do
  {
    printf("\nMenu:\n");
    printf("1. Inserir\n");
    printf("2. Remover\n");
    printf("3. Buscar\n");
    printf("4. Exibir pré-ordem\n");
    printf("5. Exibir em ordem\n");
    printf("6. Exibir pós-ordem\n");
    printf("7. Exibir árvore\n");
    printf("8. Sair\n");
    printf("Escolha uma opcao: ");
    scanf("%d", &opcao);
    switch (opcao)
    {
    case 1:
      printf("Digite o valor a ser inserido: ");
      scanf("%d", &chave);
      raiz = inserir(raiz, chave);
      break;
    case 2:
      printf("Digite o valor a ser removido: ");
      scanf("%d", &chave);
      raiz = remover(raiz, chave);
      printf("Valor removido (se existir).\n");
      break;
    case 3:
      printf("Digite o valor a ser buscado: ");
      scanf("%d", &chave);
      if (buscar(raiz, chave) != NULL)
        printf("Valor encontrado!\n");
      else
        printf("Valor nao encontrado.\n");
      break;
    case 4:
      printf("Percurso pré-ordem: ");
      preOrdem(raiz);
      printf("\n");
      break;
    case 5:
      printf("Percurso em ordem: ");
      emOrdem(raiz);
      printf("\n");
      break;
    case 6:
      printf("Percurso pós-ordem: ");
      posOrdem(raiz);
      printf("\n");
      break;
    case 7:
      printf("\nEstrutura da árvore AVL:\n");
      exibirArvore(raiz, 0);
      break;
    case 8:
      printf("Saindo...\n");
      break;
    default:
      printf("Opcao invalida!\n");
    }
  } while (opcao != 8);
  return 0;
}

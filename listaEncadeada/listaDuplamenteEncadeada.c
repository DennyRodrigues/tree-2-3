#include <stdio.h>
#include <stdlib.h>

// nó para lista duplamente encadeada
typedef struct node {
    int valor;
    struct node *anterior;
    struct node *proximo;
} node;

// struct de lista para facilitar acesso a cabeça e à cauda
typedef struct list {
    node *cabeca;
    node *cauda;
}list;


void init_list(list *mylist);

void inserir_inicio(list *myList, int value);

void inserir_meio(list *myList, int value, int ant);

void inserir_fim(list *myList, int value);

node* buscar(list *myList, int value);

void remover_inicio(list *myList, int value);

void remover_final(list *myList, int value);

void remover_elemento(list *myList, int value);

void mostrar_crescente(list *myList);

void mostrar_decrescente(list *myList);

void liberar_memoria(list *myList);

int main(void)
{

    // Criar lista e alocar memória
    list *lista_numeros = (list *)malloc(sizeof(list));
    if (lista_numeros == NULL)
    {
        fprintf(stderr, "Erro: falha na alocação de memória.\n");
        return 1;
    }

    init_list(lista_numeros);

    // Variáveis para controle do menu
    int opcao = -1;
    int numero, elemento_ref;

    // Loop principal do programa
    while (opcao != 0)
    {
        // Exibir menu de opções
        printf("\n===== GERENCIADOR DE LISTA =====\n");
        printf("[1] Inserir no início\n");
        printf("[2] Inserir no fim\n");
        printf("[3] Inserir após elemento específico\n");
        printf("[4] Eliminar elemento\n");
        printf("[5] Exibir em ordem crescente\n");
        printf("[6] Exibir em ordem decrescente\n");
        printf("[7] Localizar elemento\n");
        printf("[0] Encerrar programa\n");
        printf("\nDigite sua escolha: ");
        scanf("%d", &opcao);

        // Processar escolha do usuário
        switch (opcao)
        {
        case 1:
            printf("Informe o número para inserir no início: ");
            scanf("%d", &numero);
            inserir_inicio(lista_numeros, numero);
            break;

        case 2:
            printf("Informe o número para inserir no fim: ");
            scanf("%d", &numero);
            inserir_fim(lista_numeros, numero);
            break;

        case 3:
            printf("Informe o número a ser inserido: ");
            scanf("%d", &numero);
            printf("Informe o elemento de referência: ");
            scanf("%d", &elemento_ref);
            inserir_meio(lista_numeros, numero, elemento_ref);
            break;

        case 4:
            printf("Informe o número a ser eliminado: ");
            scanf("%d", &numero);
            remover_elemento(lista_numeros, numero);
            break;

        case 5:
            printf("\nConteúdo da lista (crescente): ");
            mostrar_crescente(lista_numeros);
            break;

        case 6:
            printf("\nConteúdo da lista (decrescente): ");
            mostrar_decrescente(lista_numeros);
            break;

        case 7:
            printf("Informe o número a ser localizado: ");
            scanf("%d", &numero);
            node *elemento = buscar(lista_numeros, numero);
            if (elemento != NULL)
            {
                printf("✓ Elemento %d encontrado na lista!\n", elemento->valor);
            }
            else
            {
                printf("✗ Elemento %d não existe na lista.\n", numero);
            }
            break;

        case 0:
            printf("\nFinalizando programa...\n");
            break;

        default:
            printf("\n[ERRO] Opção inválida! Tente novamente.\n");
        }
    }

    // Liberar memória antes de encerrar
    liberar_memoria(lista_numeros);
    printf("Memória liberada com sucesso!\n");

    return 0;
}

// inicializa a lista
void init_list(list *mylist)
{
    mylist->cabeca = NULL;
    mylist->cauda = NULL;
}

// adiciona valor ao inicio da lista
void inserir_inicio(list *myList, int value)
{
    // Aloca memória para o novo nó
    node *novo_elemento = (node *)malloc(sizeof(node));

    if (novo_elemento != NULL)
    {
        // Configura o novo elemento
        novo_elemento->valor = value;
        novo_elemento->proximo = myList->cabeca;
        novo_elemento->anterior = NULL;

        // Ajusta as referências da lista
        if (myList->cabeca != NULL)
        {
            // Se já existem elementos, atualiza a referência anterior da cabeça atual
            myList->cabeca->anterior = novo_elemento;
        }
        else
        {
            // Se a lista estava vazia, o novo elemento também é a cauda
            myList->cauda = novo_elemento;
        }

        // Atualiza a cabeça da lista para o novo elemento
        myList->cabeca = novo_elemento;
    }
    else
    {
        fprintf(stderr, "[ERRO] Falha na alocação de memória!\n");
    }
}

void inserir_fim(list *myList, int value)
{
    // Aloca memória para o novo elemento
    node *novo_elemento = (node *)malloc(sizeof(node));

    if (novo_elemento != NULL)
    {
        // Configura o novo elemento
        novo_elemento->valor = value;
        novo_elemento->proximo = NULL; // Último elemento não tem próximo

        // Verifica se a lista está vazia
        if (myList->cabeca == NULL)
        {
            // Caso especial: lista vazia
            novo_elemento->anterior = NULL;
            myList->cabeca = novo_elemento;
        }
        else
        {
            // Adiciona após o último elemento atual
            myList->cauda->proximo = novo_elemento;
            novo_elemento->anterior = myList->cauda;
        }

        // Atualiza a cauda da lista
        myList->cauda = novo_elemento;
    }
    else
    {
        fprintf(stderr, "[ERRO] Não foi possível alocar memória para o novo elemento!\n");
    }
}

node *buscar(list *myList, int value)
{
    // Verifica se a lista está vazia
    if (myList->cabeca == NULL)
    {
        printf("\n[AVISO] Lista vazia - busca não realizada!\n");
        return NULL;
    }

    // Percorre a lista buscando o elemento
    node *atual = myList->cabeca;

    while (atual != NULL)
    {
        // Verifica se o elemento atual contém o valor procurado
        if (atual->valor == value)
        {
            return atual; // Elemento encontrado
        }

        // Avança para o próximo elemento
        atual = atual->proximo;
    }

    // Elemento não encontrado na lista
    return NULL;
}
// adiciona depois do numero "ant" informado
void inserir_meio(list *myList, int value, int ant)
{   
    // 1. busca o valor do nó com valor ant
    node *novoAnterior = buscar(myList, ant);

    // 1.1 se ele existe
    if (novoAnterior)
    {
        if (novoAnterior == myList->cabeca)
        {
            inserir_inicio(myList, value);
        }
        else if (novoAnterior == myList->cauda)
        {
            inserir_fim(myList, value);
        }
        else
        {
            node *novo = malloc(sizeof(node));
            if (novo)
            {   
                // 2.3 configurar novo node
                novo->valor = value;
                novo->proximo = novoAnterior->proximo;
                novo->anterior = novoAnterior;
                // 2.4 nó proximo->anterior agora aponta o novo node
                novoAnterior->proximo->anterior = novo;
                // 2.5 nó anterior->proximo agora aponta para o novo node
                novoAnterior->proximo = novo;

            }
            else
            {
                printf("erro ao alocar memoria\n");
            }
        }
    }
    // 1.2 se ele não existe
    else
    {
        printf("\nvalor de referencia %d não encontrado\n", ant);
    }
}

void remover_inicio(list *myList, int value)
{
    if (myList->cabeca)
    {   
        node *remover = myList->cabeca;
        // se há apenas 1 elemento na lista
        if (myList->cabeca == myList->cauda)
        {
            myList->cabeca = NULL;
            myList->cauda = NULL;
        }
        else
        {   
            // o nó seguinte do nó removido vai ter null como anterior
            // pois ele vai virar a nova cabeça
            remover->proximo->anterior = NULL;
            myList->cabeca = remover->proximo;
        }
        free(remover);
        
    }
    else
    {
        printf("lista vazia\n");
    }
}

void remover_final(list *myList, int value)
{
    if (myList->cabeca)
    {   
        node *remover = myList->cauda;
        // se há apenas 1 elemento na lista
        if (myList->cabeca == myList->cauda)
        {
            myList->cabeca = NULL;
            myList->cauda = NULL;
        }
        else
        {   
            // o nó anterior ao que vai ser removido aponta para NULL como próximo
            // e a cauda recebe o esse nó anterior
            remover->anterior->proximo = NULL;
            myList->cauda = remover->anterior;
        }
        free(remover);
        
    }
    else
    {
        printf("lista vazia\n");
    }
}

void remover_elemento(list *myList, int value)
{
    // 1. buscar elemento value na lista
    node *remover = buscar(myList, value);
    // 1.1 se o elemento for encontrado
    if (remover)
    {
        if (remover == myList->cabeca)
        {
            remover_inicio(myList, value);
        }
        else if (remover == myList->cauda)
        {
            remover_final(myList, value);
        }
        else
        {
            remover->anterior->proximo = remover->proximo;
            remover->proximo->anterior = remover->anterior;
            free(remover);
        }
    }
    // 1.2 se o elemento não for encontrado
    else
    {
        printf("elemento %d não encontrado\n", value);
    }
    
    
}

void mostrar_crescente(list *myList)
{
    node *ptr = myList->cabeca;

    if (ptr)
    {
        while(ptr)
        {
            printf("%d ", ptr->valor);
            ptr = ptr->proximo;
        }
        printf("\n");
    } 
    else
    {
        printf("lista vazia\n");
    }
}

void mostrar_decrescente(list *myList)
{
    node *ptr = myList->cauda;

    if (ptr)
    {
        while(ptr)
        {
            printf("%d ", ptr->valor);
            ptr = ptr->anterior;
        }
        printf("\n");
    } 
    else
    {
        printf("lista vazia\n");
    }
}

void liberar_memoria(list *myList)
{
    node *ptr = myList->cabeca;
    node *prox = ptr;

    while(ptr && prox)
    {
        prox = ptr->proximo;
        free(ptr);
        ptr = prox;
    }

    myList->cabeca = NULL;
    myList->cauda = NULL;
}



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define NUM_CHARS 256

typedef struct ArvoreTrie
{
    struct ArvoreTrie *filhos[NUM_CHARS];
    bool termino;
} ArvoreTrie;

// Cria um novo nó para a árvore Trie.
// Aloca memória para a estrutura ArvoreTrie.
// Inicializa todos os ponteiros dos filhos como NULL.
// Define o marcador de término de palavra ('termino') como falso.
// Retorna o ponteiro para o nó recém-criado.
ArvoreTrie *CriaArvore()
{
    ArvoreTrie *arv = (ArvoreTrie *)malloc(sizeof(ArvoreTrie));
    for (int i = 0; i < NUM_CHARS; i++)
    {
        arv->filhos[i] = NULL;
    }
    arv->termino = false;
    return arv;
}

// Insere uma string (palavra) na árvore Trie.
// Recebe um ponteiro para um ponteiro para a raiz da árvore e a string a ser inserida.
// Se a árvore estiver vazia, cria o nó raiz.
// Percorre a árvore caractere a caractere da string.
// Para cada caractere, verifica se o nó filho correspondente existe.
// Se não existir, cria um novo nó filho.
// Move o ponteiro temporário para o nó filho correspondente ao caractere atual.
// Após percorrer toda a string, marca o nó final como 'termino' = true.
// Retorna true se a inserção for bem-sucedida (a palavra não existia), false caso contrário (a palavra já existia).
bool adicionarNaArvore(ArvoreTrie **arv, char *texto)
{
    if (*arv == NULL)
    {
        *arv = CriaArvore();
    }

    ArvoreTrie *tmp = *arv;
    unsigned char *caracter = (unsigned char *)texto;
    int comprimento = strlen(texto);

    for (int i = 0; i < comprimento; i++)
    {
        if (tmp->filhos[caracter[i]] == NULL)
        {
            tmp->filhos[caracter[i]] = CriaArvore();
        }
        tmp = tmp->filhos[caracter[i]];
    }

    if (tmp->termino)
    {
        return false;
    }
    else
    {
        tmp->termino = true;
        return true;
    }
}

// Função recursiva auxiliar para imprimir todas as palavras na árvore Trie.
// Percorre a árvore em profundidade.
// 'no_atual': o nó sendo visitado no momento.
// 'prefixo': o caminho (string) percorrido da raiz até o nó atual.
// 'comprimento': o comprimento atual do prefixo.
// Quando um nó tem 'termino' = true, significa que o 'prefixo' atual forma uma palavra válida na árvore, e ela é impressa.
// Em seguida, itera por todos os possíveis caracteres (0 a NUM_CHARS-1).
// Se um filho para um caractere específico existir, adiciona o caractere ao prefixo e chama recursivamente a função para o nó filho.
void ImprimeArvoreRec(ArvoreTrie *arv, unsigned char *prefixo, int comprimento)
{
    unsigned char novoprefixo[comprimento + 2];
    memcpy(novoprefixo, prefixo, comprimento);
    novoprefixo[comprimento + 1] = 0;

    if (arv->termino)
    {
        printf("Palavra: %s\n", prefixo);
    }

    for (int i = 0; i < NUM_CHARS; i++)
    {
        if (arv->filhos[i] != NULL)
        {
            novoprefixo[comprimento] = i;
            ImprimeArvoreRec(arv->filhos[i], novoprefixo, comprimento + 1);
        }
    }
}

void ImprimeArvore(ArvoreTrie *arv)
{
    if (arv == NULL)
    {
        printf("Arvore Vazia\n");
        return;
    }

    unsigned char prefixo[1] = {0};
    ImprimeArvoreRec(arv, prefixo, 0);
}

bool procurarNaArvore(ArvoreTrie *arv, char *texto)
{
    unsigned char *caracter = (unsigned char *)texto;
    int comprimento = strlen(texto);
    ArvoreTrie *tmp = arv;

    for (int i = 0; i < comprimento; i++)
    {
        if (tmp->filhos[caracter[i]] == NULL)
        {
            return false;
        }
        tmp = tmp->filhos[caracter[i]];
    }
    return tmp->termino;
}

bool no_tem_filho(ArvoreTrie *arv)
{
    if (arv == NULL)
        return false;
    for (int i = 0; i < NUM_CHARS; i++)
    {
        if (arv->filhos[i] != NULL)
        {
            return true;
        }
    }
    return false;
}

ArvoreTrie *RemoveArvRec(ArvoreTrie *arv, unsigned char *texto, bool *removido)
{
    if (arv == NULL)
        return arv;

    if (*texto == '\0')
    {
        if (arv->termino)
        {
            arv->termino = false;
            *removido = true;
            if (!no_tem_filho(arv))
            {
                free(arv);
                arv = NULL;
            }
        }
        return arv;
    }

    arv->filhos[*texto] = RemoveArvRec(arv->filhos[*texto], texto + 1, removido);

    if (*removido && !no_tem_filho(arv) && !arv->termino)
    {
        free(arv);
        arv = NULL;
    }

    return arv;
}

bool RemoveArv(ArvoreTrie **arv, char *texto)
{
    unsigned char *caracter = (unsigned char *)texto;
    bool resultado = false;

    if (*arv == NULL)
        return false;

    *arv = RemoveArvRec(*arv, caracter, &resultado);
    return resultado;
}
// Verifica se existe pelo menos uma palavra na árvore Trie que começa com um dado prefixo.
// Percorre a árvore caractere a caractere do prefixo.
// Para cada caractere, verifica se o nó filho correspondente existe.
// Se em algum ponto o nó filho não existir, não há nenhuma palavra com este prefixo na árvore, retorna false.
// Se percorrer todo o prefixo com sucesso, significa que o caminho para o prefixo existe na árvore.
// Retorna true, indicando que existe pelo menos um caminho (e possivelmente palavras) a partir do nó que representa o fim do prefixo.
bool VerificaPrefixo(ArvoreTrie *arv, char *prefixo)
{
    unsigned char *caracter = (unsigned char *)prefixo;
    int comprimento = strlen(prefixo);

    if (comprimento == 0)
    {
        if (arv == NULL)
            return false;
        return arv->termino || no_tem_filho(arv);
    }

    if (arv == NULL)
        return false;

    ArvoreTrie *tmp = arv;

    for (int i = 0; i < comprimento; i++)
    {
        if (tmp->filhos[caracter[i]] == NULL)
        {
            return false;
        }
        tmp = tmp->filhos[caracter[i]];
    }
    return true;
}

void VisualizaArvoreRec(ArvoreTrie *no_atual, char char_vindo_do_pai, int nivel)
{
    if (no_atual == NULL)
    {
        return;
    }

    // Imprimir indentação
    for (int i = 0; i < nivel; i++)
    {
        printf("  "); // Dois espaços por nível para indentação
    }

    // Imprimir o caractere que leva a este nó (ignorando para a raiz)
    if (nivel > 0)
    {
        printf("'%c'", char_vindo_do_pai);
    }
    else
    {
        printf("Raiz");
    }

    // Indicar se este nó marca o fim de uma palavra
    if (no_atual->termino)
    {
        printf(" *");
    }

    printf("\n");

    // Chamar recursivamente para cada filho
    for (int i = 0; i < NUM_CHARS; i++)
    {
        if (no_atual->filhos[i] != NULL)
        {
            VisualizaArvoreRec(no_atual->filhos[i], (char)i, nivel + 1);
        }
    }
}

// Função wrapper para iniciar a visualização da árvore
void VisualizaArvore(ArvoreTrie *arv)
{
    if (arv == NULL)
    {
        printf("Arvore Vazia\n");
        return;
    }

    printf("\nVisualizacao da Arvore Trie:\n");
    // Começa da raiz (nível 0), sem caractere vindo do pai
    VisualizaArvoreRec(arv, ' ', 0);
}

void menu()
{
    printf("\nMenu:\n");
    printf("1. Inserir palavra\n");
    printf("2. Buscar palavra\n");
    printf("3. Remover palavra\n");
    printf("4. Imprimir todas as palavras\n");
    printf("5. Verificar prefixo\n");
    printf("7. Visualizar Arvore (Estrutura)\n");
    printf("6. Sair\n");
    printf("Escolha uma opcao: ");
}

// FUNÇÃO MAIN MODIFICADA
int main()
{
    ArvoreTrie *arvore = NULL;
    int opcao;
    char palavra[100]; // Buffer fixo

    // TODO: Implementar LiberaArvoreRec e chamá-la antes de sair

    do
    {
        menu();
        // Verifica se a leitura da opção foi bem-sucedida
        if (scanf("%d", &opcao) != 1)
        {
            printf("Entrada invalida. Digite um numero.\n");
            while (getchar() != '\n')
                ; // Limpa o resto da linha de entrada
            continue;
        }
        getchar(); // Limpar o buffer do teclado (\n restante)

        bool input_valido = true;
        int comprimento = 0;

        // Processa a entrada da palavra para opções que precisam dela
        if (opcao >= 1 && opcao <= 5)
        {
            printf("Digite a string (apenas letras): ");
            fgets(palavra, sizeof(palavra), stdin);
            palavra[strcspn(palavra, "\n")] = 0; // Remove newline

            comprimento = strlen(palavra);

            // --- Usando lib (ctype.h) para Validar e Converter ---
            for (int i = 0; i < comprimento; i++)
            {
                unsigned char current_char = palavra[i];
                // Validação: Usando isalpha() de <ctype.h>
                if (!isalpha(current_char))
                {
                    printf("Erro: Entrada contem caracteres nao alfabeticos ('%c' invalido). Por favor, digite apenas letras.\n", current_char);
                    input_valido = false; // Marca como inválido
                    break;                // Sai do loop de validação
                }
                // Conversão: Usando toupper() de <ctype.h>
                palavra[i] = toupper(current_char); // Converte para maiúscula IN PLACE
            }
            // --- Fim da Validação e Conversão ---

            // Tratar string vazia após a validação/conversão
            if (input_valido && comprimento == 0)
            {
                printf("Erro: Entrada vazia. Por favor, digite uma string de letras.\n");
                input_valido = false;
            }
        }

        // Somente procede com as operações da Trie se a entrada foi válida (para 1-5)
        if (opcao >= 1 && opcao >= 5 && !input_valido)
        {             // Correção na condição: deve ser && opcao <= 5
            continue; // Volta para o início do loop do/while
        }
        if (opcao >= 1 && opcao <= 5 && !input_valido)
        {
            continue;
        }

        switch (opcao)
        {
        case 1:
            // Passa a string JÁ VALIDADA e em MAIÚSCULAS para a função da Trie
            if (adicionarNaArvore(&arvore, palavra))
            {
                printf("Palavra inserida com sucesso!\n");
            }
            else
            {
                printf("Falha ao inserir palavra (ja existe ou erro de memoria)!\n");
            }
            break;
        case 2:
            // Passa a string VALIDADA e em MAIÚSCULAS para a função da Trie
            if (procurarNaArvore(arvore, palavra))
            {
                printf("Palavra encontrada!\n");
            }
            else
            {
                printf("Palavra nao encontrada!\n");
            }
            break;
        case 3:
            // Passa a string VALIDADA e em MAIÚSCULAS para a função da Trie
            if (RemoveArv(&arvore, palavra))
            {
                printf("Palavra removida com sucesso!\n");
            }
            else
            {
                printf("Palavra nao encontrada!\n");
            }
            break;
        case 4:
            printf("Palavras na arvore:\n");
            ImprimeArvore(arvore); // Imprime as palavras como estão armazenadas (MAIÚSCULAS)
            break;
        case 5:
            // Passa a string VALIDADA e em MAIÚSCULAS para a função da Trie
            if (VerificaPrefixo(arvore, palavra))
            {
                printf("Existe palavra com o prefixo!\n");
            }
            else
            {
                printf("Nenhum prefixo encontrado!\n");
            }
            break;
        case 7:
            printf("Visualizando a arvore com as palavras em MAIUSCULAS:\n");
            VisualizaArvore(arvore); // Visualiza a arvore com caracteres em MAIÚSCULAS
            break;
        case 6:
            printf("Saindo...\n");
            // TODO: Implementar e chamar LiberaArvoreRec(arvore); aqui
            break;
        default:
            printf("Opcao invalida!\n");
        }
    } while (opcao != 6);

    // TODO: A liberação completa da memória da árvore deve ser implementada e chamada aqui
    // LiberaArvoreRec(arvore);

    return 0;
}

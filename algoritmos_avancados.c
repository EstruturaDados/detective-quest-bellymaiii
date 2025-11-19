#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NOME_SALA 50
#define MAX_PISTA 50
#define MAX_SUSPEITO 30
#define TAMANHO_HASH 7 

typedef struct Sala {
    char nome[MAX_NOME_SALA];
    char pista[MAX_PISTA]; 
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

typedef struct PistaNode {
    char pista[MAX_PISTA];
    struct PistaNode *esquerda;
    struct PistaNode *direita;
} PistaNode;

typedef struct HashNode {
    char pista[MAX_PISTA];
    char suspeito[MAX_SUSPEITO];
    struct HashNode *proximo; 
} HashNode;

void limparTela();
void limpar_buffer();

Sala* criarSala(const char *nome, const char *pista);
void explorarSalas(Sala *salaAtual, PistaNode **raizPistas, HashNode *tabelaHash[]);
void construirMapa(Sala **raiz);

PistaNode* inserirPista(PistaNode *raiz, const char *pista);
void listarPistasEmOrdem(PistaNode *raiz);

unsigned int funcaoHash(const char *chave);
void inicializarHash(HashNode *tabelaHash[]);
void inserirNaHash(HashNode *tabelaHash[], const char *pista, const char *suspeito);
void analisarSuspeitos(HashNode *tabelaHash[]);

void modo_novato();
void modo_aventureiro();
void modo_mestre();

int main(int argc, char *argv[]) {
    int modo;

    do {
        limparTela();
        printf("========================================================\n");
        printf("      DESAFIO DETECTIVE QUEST - Seleção de Nível\n");
        printf("========================================================\n");
        printf("1.  Mapa da Mansão \n");
        printf("2.  Mapa + Organização de Pistas \n");
        printf("3.  Mapa + Pistas + Solução do Caso \n");
        printf("0. Sair\n");
        printf("Escolha o modo: ");

        if (scanf("%d", &modo) != 1) { modo = -1; }
        limpar_buffer();

        switch (modo) {
            case 1: modo_novato(); break;
            case 2: modo_aventureiro(); break;
            case 3: modo_mestre(); break;
            case 0: printf("\nEncerrando o Desafio Detective Quest.\n"); break;
            default: printf("\nOpção inválida. Tente novamente.\n");
        }
        
        if (modo != 0) { printf("\nPressione ENTER para voltar ao menu..."); getchar(); }

    } while (modo != 0);

    return 0;
}

void modo_novato() {
    Sala *raiz = NULL;
    construirMapa(&raiz);

    limparTela();
    printf("---  Explorando a Mansão ---\n");
    printf("Você está no Hall de Entrada. Use 'e' (esquerda) ou 'd' (direita).\n");
    printf("Digite 's' para sair.\n\n");
    
    explorarSalas(raiz, NULL, NULL); 
}

void modo_aventureiro() {
    Sala *raizMapa = NULL;
    construirMapa(&raizMapa);
    PistaNode *raizPistas = NULL;
    
    limparTela();
    printf("---  Coletando e Organizando Pistas ---\n");
    printf("Ao visitar cômodos, você pode encontrar pistas que serão ordenadas.\n");
    printf("Digite 'l' para listar as pistas coletadas em ordem alfabética.\n");
    printf("Use 'e' (esquerda) ou 'd' (direita). Digite 's' para sair.\n\n");
    
    explorarSalas(raizMapa, &raizPistas, NULL);

    printf("\n--- ANÁLISE FINAL DE PISTAS ---\n");
    printf("Pistas coletadas (em Ordem Alfabética):\n");
    listarPistasEmOrdem(raizPistas);
}

void modo_mestre() {
    Sala *raizMapa = NULL;
    construirMapa(&raizMapa);
    PistaNode *raizPistas = NULL; 
    
    HashNode *tabelaHash[TAMANHO_HASH];
    inicializarHash(tabelaHash);

    limparTela();
    printf("---  Solucionando o Mistério ---\n");
    printf("Pistas encontradas serão vinculadas a Suspeitos .\n");
    printf("Use 'e' (esquerda) ou 'd' (direita). Digite 's' para sair.\n\n");
    
    explorarSalas(raizMapa, &raizPistas, tabelaHash);

    printf("\n--- ANÁLISE FINAL DO CASO ---\n");
    analisarSuspeitos(tabelaHash);
}

void limparTela() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void limpar_buffer() { 
    int c; 
    while ((c = getchar()) != '\n' && c != EOF); 
}

Sala* criarSala(const char *nome, const char *pista) {
    Sala *novaSala = (Sala*)malloc(sizeof(Sala));
    if (novaSala == NULL) {
        perror("Erro de alocação de memória para sala.");
        exit(EXIT_FAILURE);
    }
    strcpy(novaSala->nome, nome);
    strcpy(novaSala->pista, pista);
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;
    return novaSala;
}

void construirMapa(Sala **raiz) {
    
    *raiz = criarSala("Hall de Entrada", "");

    (*raiz)->esquerda = criarSala("Biblioteca", "Lupa manchada de tinta");
    (*raiz)->direita = criarSala("Sala de Jantar", "Pedaço de papel queimado");

    (*raiz)->esquerda->esquerda = criarSala("Escritório Privado", "");
    (*raiz)->esquerda->direita = criarSala("Cozinha", "Faca de cozinha faltando");

    (*raiz)->direita->esquerda = criarSala("Quarto Principal", "Um relógio quebrado");
    (*raiz)->direita->direita = criarSala("Porão (Fim da Linha)", "Um sapato enlameado");
}

void explorarSalas(Sala *salaAtual, PistaNode **raizPistas, HashNode *tabelaHash[]) {
    char acao;
    Sala *anterior = NULL; 
    
    while (salaAtual != NULL) {
        printf("\nVocê está em: **%s**\n", salaAtual->nome);
        
        if (strlen(salaAtual->pista) > 0) {
            printf(" **PISTA ENCONTRADA:** %s\n", salaAtual->pista);
            
            if (raizPistas != NULL) { 
                *raizPistas = inserirPista(*raizPistas, salaAtual->pista);
            }
            if (tabelaHash != NULL) { 
                const char *suspeitos[] = {"Mordomo", "Jardineiro", "Dama de Companhia", "Modista", "Rei"};
                int indice = (int)strlen(salaAtual->pista) % 5;
                inserirNaHash(tabelaHash, salaAtual->pista, suspeitos[indice]);
                printf("Suspeito Associado: **%s**\n", suspeitos[indice]);
            }
            strcpy(salaAtual->pista, ""); 
        }

        if (salaAtual->esquerda == NULL && salaAtual->direita == NULL) {
            printf("\nFim do caminho . Retornando ao Hall de Entrada...\n");
            return;
        }

        printf("Caminhos disponíveis: ");
        if (salaAtual->esquerda) printf("'e' (Esquerda)");
        if (salaAtual->esquerda && salaAtual->direita) printf(" / ");
        if (salaAtual->direita) printf("'d' (Direita)");
        
        if (raizPistas != NULL && tabelaHash == NULL) {
             printf(" / 'l' (Listar Pistas)");
        }
        
        printf(" / 's' (Sair)\n");
        printf("Sua ação: ");

        if (scanf(" %c", &acao) != 1) acao = 's';
        limpar_buffer();
        acao = tolower(acao);

        anterior = salaAtual;
        if (acao == 'e') {
            salaAtual = salaAtual->esquerda;
        } else if (acao == 'd') {
            salaAtual = salaAtual->direita;
        } else if (acao == 'l' && raizPistas != NULL && tabelaHash == NULL) {
             printf("\n--- PISTAS COLETADAS ---\n");
             listarPistasEmOrdem(*raizPistas);
             printf("\nPressione ENTER para continuar a exploração...");
             getchar();
             salaAtual = anterior; 
        } else if (acao == 's') {
            printf("\nSaindo da exploração da Mansão...\n");
            return;
        } else {
            printf("Comando inválido. Tente novamente.\n");
            salaAtual = anterior; 
        }
    }
}

PistaNode* inserirPista(PistaNode *raiz, const char *pista) {
    if (raiz == NULL) {
        PistaNode *novo = (PistaNode*)malloc(sizeof(PistaNode));
        if (novo == NULL) {
            perror("Erro de alocação de memória para pista.");
            exit(EXIT_FAILURE);
        }
        strcpy(novo->pista, pista);
        novo->esquerda = novo->direita = NULL;
        printf("(Pista armazenada em BST)\n");
        return novo;
    }

    int comparacao = strcmp(pista, raiz->pista);

    if (comparacao < 0) {
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    } else if (comparacao > 0) {
        raiz->direita = inserirPista(raiz->direita, pista);
    } else {
        printf("(Pista duplicada ignorada: %s)\n", pista);
    }

    return raiz;
}

void listarPistasEmOrdem(PistaNode *raiz) {
    if (raiz != NULL) {
        listarPistasEmOrdem(raiz->esquerda);
        printf("- %s\n", raiz->pista);
        listarPistasEmOrdem(raiz->direita);
    }
}

unsigned int funcaoHash(const char *chave) {
    unsigned int hash = 0;
    int len = strlen(chave);
    for (int i = 0; i < 3 && i < len; i++) {
        hash = (hash * 31) + chave[i];
    }
    return hash % TAMANHO_HASH;
}

void inicializarHash(HashNode *tabelaHash[]) {
    for (int i = 0; i < TAMANHO_HASH; i++) {
        tabelaHash[i] = NULL;
    }
}

void inserirNaHash(HashNode *tabelaHash[], const char *pista, const char *suspeito) {
    unsigned int indice = funcaoHash(pista);

    HashNode *novoNo = (HashNode*)malloc(sizeof(HashNode));
    if (novoNo == NULL) {
        perror("Erro de alocação de memória para HashNode.");
        exit(EXIT_FAILURE);
    }
    strcpy(novoNo->pista, pista);
    strcpy(novoNo->suspeito, suspeito);
    
    novoNo->proximo = tabelaHash[indice];
    tabelaHash[indice] = novoNo;
    printf("(Pista vinculada a Suspeito . Índice: %d)\n", indice);
}

void analisarSuspeitos(HashNode *tabelaHash[]) {
    int contagemSuspeitos[MAX_SUSPEITO] = {0};
    char nomesSuspeitos[MAX_SUSPEITO][MAX_SUSPEITO];
    int numSuspeitos = 0;
    
    printf("\n--- ASSOCIAÇÕES PISTA -> SUSPEITO ---\n");

    for (int i = 0; i < TAMANHO_HASH; i++) {
        HashNode *atual = tabelaHash[i];
        while (atual != NULL) {
            printf("Bucket %d: Pista '%s' -> Suspeito: %s\n", i, atual->pista, atual->suspeito);

            int encontrado = 0;
            for (int j = 0; j < numSuspeitos; j++) {
                if (strcmp(nomesSuspeitos[j], atual->suspeito) == 0) {
                    contagemSuspeitos[j]++;
                    encontrado = 1;
                    break;
                }
            }
            if (!encontrado && numSuspeitos < MAX_SUSPEITO) {
                strcpy(nomesSuspeitos[numSuspeitos], atual->suspeito);
                contagemSuspeitos[numSuspeitos]++;
                numSuspeitos++;
            }

            atual = atual->proximo;
        }
    }
    
    if (numSuspeitos > 0) {
        int maxContagem = 0;
        char culpado[MAX_SUSPEITO] = "N/A";

        for (int i = 0; i < numSuspeitos; i++) {
            if (contagemSuspeitos[i] > maxContagem) {
                maxContagem = contagemSuspeitos[i];
                strcpy(culpado, nomesSuspeitos[i]);
            }
        }
        
        printf("\n============================================\n");
        printf(" **VEREDITO DO DETETIVE:**\n");
        printf("O Suspeito mais citado nas pistas é: **%s**\n", culpado);
        printf("Com um total de %d evidência(s).\n", maxContagem);
        printf("============================================\n");
    } else {
        printf("\nNenhuma pista foi coletada para análise.\n");
    }
}
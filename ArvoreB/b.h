#define M 2
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct cliente Cliente;
struct cliente
{
        int cod;
        char nome[100];
};

typedef struct arvB ArvB;
struct arvB
{
        ArvB* pai;
        int numChaves;
        ArvB* filho[2*M + 1];
        Cliente*clientes[2*M];
        int pos;
};



void insere(ArvB* arv, Cliente* cliente, FILE* arqMeta, FILE* arqDados);
void busca(ArvB* arv, int num);
void particiona(ArvB* arv,  Cliente* cliente, FILE*arqMeta, FILE*arqDados);
void posicionaFilho(ArvB* pai, ArvB* filho, FILE* arqMeta, FILE* arqDados);
ArvB* insereFilho(ArvB* pai, ArvB* filho, Cliente* cliente, FILE* arqMeta, FILE*arqDados);
void inserePaginaNaoCheia(ArvB* arv, Cliente* cliente, FILE* arqMeta, FILE* arqDados);
void exclui(ArvB* arv, int num, FILE* arqMeta, FILE* arqDados);
void escreve(ArvB* arv, FILE*arqDados, int pos);
ArvB* criaNo(FILE* arqDados, int num);
void menu()
{
        printf("------------------ MENU ----------------\n");
        printf("1- Registro de Cliente\n2- Excluir Registro \n3- Buscar cliente cadastrado\n4- Imprimir registros\n0- Sair\n");
}

int tamanho()
{
        return
                sizeof(int)*8 +
               4* ( sizeof(int) + sizeof(char)*100);
}

void insercao(ArvB* arv, FILE* arqMeta, FILE*arqDados)
{
       Cliente* c = (Cliente*) malloc(sizeof(Cliente));
       // c->cod = cod;
       // strcpy(c->nome, nome);

        printf("______________EFETUAR REGISTRO DE CLIENTE_______________\n");
        printf("Digite o código do cliente:\n");
        scanf("%d", &c->cod);

        printf("Digite o nome do cliente: \n");
        scanf("%s", c->nome);

   //    printf("%d %s\n", c->cod, c->nome);

        insere(arv, c, arqMeta, arqDados);

}


// void insercao(ArvB* arv, FILE* arqMeta, FILE*arqDados, int cod, char nome[100])
// {
//        Cliente* c = (Cliente*) malloc(sizeof(Cliente));
//        c->cod = cod;
//        strcpy(c->nome, nome);

//         insere(arv, c, arqMeta, arqDados);

// }


ArvB* exclusao(ArvB* arv, FILE* arqMeta, FILE* arqDados /*, int cod*/)
{
        int cod;

        printf("______________APAGAR REGISTRO_____________________\n");
        printf("Digite o código do cliente:\n");
        scanf("%d", &cod);

        exclui(arv, cod, arqMeta, arqDados);
        return arv;
}

void procura(ArvB* arv)
{
        int cod;

        printf("_______________BUSCAR REGISTRO DO CLIENTE______\n");
        printf("Digite o código do cliente: \n");
        scanf("%d", &cod);

        busca(arv, cod);
}

void imprimeDados(FILE* arqMeta, FILE*arqDados)
{
            int num, cont = 0;
            char nome[100];
            rewind(arqDados);
            rewind(arqMeta);

           printf("................................  METADADOS .........................\n");

           fread(&num, sizeof(int), 1, arqMeta);
           printf("Raiz:  %d\n", num);
           fread(&num, sizeof(int), 1, arqMeta);
           printf("Próxima posição livre:  %d\n", num);

           printf("...................................... DADOS .....................................\n");

            while(fread(&num, sizeof(int), 1, arqDados) > 0)
            {
                    printf("_______________________________________POSIÇÂO:%d________________________\n", cont);
                    cont++;

                     printf("Posição: \n%d\n", num);
                     fread(&num, sizeof(int), 1, arqDados);
                     printf("Número de clientes: \n%d\n", num);
                     fread(&num, sizeof(int), 1, arqDados);
                     printf("Ponteiro para o pai: \n%d\n", num);

                     printf("Filhos: \n");
                     for(int i = 0; i < 2*M+1; i++)
                    {
                              fread(&num, sizeof(int), 1, arqDados);
                              printf("%d\n", num);
                    }
                    printf("Registros: \n");
                     for(int i = 0; i < 2*M; i++)
                    {
                              fread(&num, sizeof(int), 1, arqDados);
                              printf("%d\n", num);
                              fread(nome, sizeof(char), sizeof(nome), arqDados);
                              printf( "%s\n", nome);

                    }
            }

}

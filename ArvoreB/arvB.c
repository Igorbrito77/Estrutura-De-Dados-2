#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "b.h"
#define M 2

ArvB* criaVazia()
{
        ArvB* arv = (ArvB*) malloc(sizeof(ArvB));
        arv->numChaves = 0;
        arv->pai = NULL;
        arv->pos = 0;

        for(int i = 0; i  < 2*M + 1; i++)
        {
               // arv->clientes[i] = NULL;
                 arv->filho[i] = NULL;
        }


        return arv;
}


int verificaFolha(ArvB* arv)
{
        for(int i = 0; i < arv->numChaves; i++)
        {
                if(arv->filho[i] != NULL)
                    return 0;
        }
        return 1;
}


void atualizaPai(FILE* arqMeta, int pos)
{
                rewind(arqMeta);
                fwrite(&pos, sizeof(int), 1, arqMeta);
}

void atualizaNo(ArvB* arv, FILE* arqMeta, FILE*arqDados)
{
          escreve(arv, arqDados, arv->pos);
}

int proxPosicaoLivre(FILE* arqMeta)
{
         int pos;
         fseek(arqMeta, sizeof(int), SEEK_SET);
         fread(&pos, sizeof(int), 1, arqMeta);
        int prox =  pos+ 1;
         fseek(arqMeta, sizeof(int), SEEK_SET);
         fwrite(&prox, sizeof(int), 1, arqMeta);

         return pos;
}

void  escreveNovo(ArvB* arv, FILE*arqMeta, FILE*arqDados)
{
                int pos = proxPosicaoLivre(arqMeta);
                escreve(arv, arqDados, pos);
}


void escreve(ArvB* arv, FILE*arqDados, int pos)
{

                fseek(arqDados, pos* tamanho(), SEEK_SET);

                    int pai;
                    char vazio[100] = "vazio";

                        if(arv->pai == NULL)
                                pai = -1;
                        else
                            pai = arv->pai->pos;


                    fwrite(&arv->pos, sizeof(int), 1, arqDados);
                    fwrite(&arv->numChaves, sizeof(int), 1 ,arqDados);
                    fwrite(&pai, sizeof(int), 1, arqDados);
                    for(int i = 0; i < 2*M + 1; i++)
                    {
                            if(arv->filho[i] !=  NULL)
                                fwrite(&arv->filho[i]->pos, sizeof(int), 1, arqDados);
                            else
                            {
                                    pai = -1;
                                    fwrite(&pai, sizeof(int), 1, arqDados);
                            }
                    }

                    for(int j = 0; j < 2*M; j++)
                   {
                        if(arv->clientes[j] == NULL)
                          {
                                pai = -1;
                                fwrite(&pai, sizeof(int), 1, arqDados);
                                fwrite(vazio, sizeof(char), sizeof(vazio), arqDados);
                         }

                         else
                        {
                             fwrite(&arv->clientes[j]->cod, sizeof(int), 1, arqDados);
                             fwrite(arv->clientes[j]->nome, sizeof(char), sizeof(arv->clientes[j]->nome), arqDados);
                        }
                }
}


void inserePaginaNaoCheia(ArvB* arv, Cliente* cliente, FILE* arqMeta, FILE* arqDados)
{

            int i = arv->numChaves, pai;
            int num = cliente->cod;

                    while(i > 0 && num < arv->clientes[i- 1]->cod)
                    {
                            arv->clientes[i] = arv->clientes[i- 1];
                            i--;
                    }


                    arv->clientes[i] = cliente;
                    arv->numChaves++;

                int pos;
                fseek(arqMeta, sizeof(int), SEEK_SET);
                fread(&pos, sizeof(int), 1, arqMeta);

                if(pos == 0)
                {

                     escreveNovo(arv, arqMeta, arqDados);
                     arv->pos  = 0;
                     atualizaPai(arqMeta, arv->pos);
                }
            else
                        atualizaNo(arv, arqMeta, arqDados);

}


ArvB* insereFilho(ArvB* pai, ArvB* filho, Cliente* cliente, FILE* arqMeta, FILE*arqDados)
{
        int i = pai->numChaves;
        int num = filho->clientes[0]->cod;

         while(i > 0 && num < pai->clientes[i - 1]->cod)
        {
                pai->filho[i+1] = pai->filho[i];
                pai->clientes[i] = pai->clientes[i - 1];
                  i--;
        }

        pai->clientes[i] = cliente;
        pai->filho[i+ 1]  = filho;
        pai->numChaves++;
        pai->filho[i + 1]->pos = proxPosicaoLivre(arqMeta);
        atualizaNo(pai->filho[i + 1], arqMeta, arqDados);
        atualizaNo(pai, arqMeta, arqDados);
        return pai;
}

void posicionaFilho(ArvB* pai, ArvB* filho, FILE* arqMeta, FILE* arqDados)
{
        int num = filho->clientes[0]->cod;

         int i = pai->numChaves;
                    while(i > 0 && num < pai->clientes[i - 1]->cod)
                            i--;

        if(pai->filho[i] != NULL && pai->filho[i]->clientes[0]->cod != num)
                posicionaFilho(pai->filho[i], filho, arqMeta, arqDados);

        else
             {
                 pai->filho[i] = filho;
                 filho->pai = pai;
                 atualizaNo(pai->filho[i], arqMeta, arqDados);
             }

}

void particiona(ArvB* arv,  Cliente* cliente, FILE*arqMeta, FILE*arqDados)
{
    int pos = arv->pos;

        Cliente** v = (Cliente**) malloc(sizeof(Cliente*));
        for(int i = 0; i < 1+2*M; i++)
            v[i] = (Cliente*) malloc(sizeof(Cliente));

        int i = 2*M ;
        v = arv->clientes;
        v[i] = cliente;

        ArvB*  filho1 = criaVazia();
        ArvB* filho2 = criaVazia();


        while(i > 0 && v[i]->cod < v[i-1]->cod)
        {
                           Cliente* aux= v[i];
                            v[i] = v[i- 1];
                            v[i - 1] = aux;
                            i--;
        }


        for(int i = 0; i < M; i++)
        {
                filho1->clientes[i] = v[i];
                filho2->clientes[i] = v[i + M+ 1];
                filho2->filho[i] = arv->filho[i + M + 1];
                filho1->filho[i] = arv->filho[i];
        }

        filho1->pos = pos;
        filho1->filho[M] = arv->filho[M];
        filho1->numChaves = M;
        filho2->numChaves = M;


        if(arv->pai == NULL)// caso a página seja a raiz
        {
                arv->numChaves = 1;
                arv->clientes[0] = v[M];

                arv->filho[0] = filho1;
                arv->filho[1]= filho2;
                arv->pai = NULL;
                filho2->pai = arv;
                filho1->pai = arv;


                for(int i = 0; i < filho1->numChaves+1; i++)
                    if(filho1->filho[i]!= NULL)
                        filho1->filho[i]->pai = filho1;

                  for(int i = 0; i < filho2->numChaves+1; i++)
                          if(filho2->filho[i]!= NULL)
                                filho2->filho[i]->pai = filho2;


              for(int i = 1; i < 2*M; i++)// zera as clientes restantes da arvore (clientes a partir do M)
              {
                        arv->clientes[i] = NULL;
                        arv->filho[i + 1] = NULL;
             }

                arv->pos = proxPosicaoLivre(arqMeta);
                arv->filho[1]->pos = proxPosicaoLivre(arqMeta);


                  atualizaNo(arv, arqMeta, arqDados);
                  atualizaPai(arqMeta, arv->pos);
                  atualizaNo(arv->filho[1], arqMeta, arqDados);
                  atualizaNo(arv->filho[0], arqMeta, arqDados);

        }

        else
        {

            if(arv->pai->numChaves == 2*M)// Caso a página pai também esteja cheia
            {

                    filho2->pos = proxPosicaoLivre(arqMeta);

                    particiona(arv->pai, v[M], arqMeta, arqDados);
                    posicionaFilho(arv->pai, filho1, arqMeta, arqDados);
                    posicionaFilho(arv->pai, filho2, arqMeta, arqDados);
                    atualizaNo(arv->pai, arqMeta, arqDados);
            }

            else
            {
                filho2->pai = arv->pai;
                filho1->pai = arv->pai;

                int i = arv->pai->numChaves;

                  while(i > 0 && filho1->clientes[0] < arv->pai->clientes[i - 1])
                        i--;


                arv->pai->filho[i]  = filho1;
                // arv->pai->filho[i]->pos = pos;
                 arv->pai =  insereFilho(arv->pai, filho2, v[M] , arqMeta, arqDados);

                atualizaNo(arv->pai->filho[i], arqMeta, arqDados);
            }

        }

}


void insere(ArvB* arv, Cliente* cliente, FILE* arqMeta, FILE* arqDados)
{
        if(arv->numChaves == 2*M && verificaFolha(arv) == 1)
        {
                particiona(arv, cliente, arqMeta, arqDados);
        }
        else
        {

            if(verificaFolha(arv) == 0)
            {
                    int i = arv->numChaves;
                    while(i > 0 && cliente->cod < arv->clientes[i - 1]->cod)
                            i--;

                    insere(arv->filho[i], cliente, arqMeta, arqDados);
            }
            else
                inserePaginaNaoCheia(arv, cliente, arqMeta, arqDados);

        }
}



void busca(ArvB* arv, int num)
{

        if(arv == NULL)
            printf("REGISTRO NÃO ENCONTRADO\n");

        else
        {
                int i = 0;

                while(i < arv->numChaves && num > arv->clientes[i]->cod)
                    i++;

                if(num == arv->clientes[i]->cod)
                {
                        printf("REGISTRO  ENCONTRADO .\n");
                        printf("Nome: %s\n", arv->clientes[i]->nome);
                        printf("Código: %d\n", arv->clientes[i]->cod);
                }

                else
                    busca(arv->filho[i], num);
    }

}


ArvB* criaArvore(FILE* arqMeta, FILE* arqDados)
{
        ArvB* arv = criaVazia();
        int t = sizeof(int)*3, tam = sizeof(int)*8;


        int raiz;
        fread(&raiz, sizeof(int), 1, arqMeta);

        fseek(arqDados, raiz*tamanho(), SEEK_SET);


       if( fread(&arv->pos, sizeof(int), 1, arqDados) > 0)
       {
             arv->pai = NULL;
        }
        else
            return arv;

            fread(&arv->numChaves, sizeof(int), 1, arqDados);
            fseek(arqDados, sizeof(int), SEEK_CUR);

            arv = criaNo(arqDados, raiz);

        return arv;
}

ArvB* criaNo(FILE* arqDados, int num)
{
        ArvB* arv = criaVazia();
        int t = sizeof(int)*3, tam = sizeof(int)*8;

        fseek(arqDados , num* tamanho() , SEEK_SET);

        int n;

        fread(&arv->pos, sizeof(int), 1, arqDados);
        fread(&arv->numChaves, sizeof(int), 1, arqDados);

        fseek(arqDados, sizeof(int), SEEK_CUR);

        for(int i = 0; i < 2*M+1; i++)
        {
            fread(&n, sizeof(int), 1, arqDados);
            if(n != -1)
             {
                    arv->filho[i] = criaNo(arqDados, n);
                    arv->filho[i]->pai = arv;
                         t+= sizeof(int);
                        fseek(arqDados, num* tamanho() + t, SEEK_SET);

             }
            else
                arv->filho[i] =  NULL;
        }

        fseek(arqDados, num* tamanho() + tam, SEEK_SET);

        for(int i = 0; i < 2*M; i++)
        {
                Cliente* c = (Cliente*) malloc(sizeof(Cliente));
                fread(&c->cod, sizeof(int), 1, arqDados);
                fread(c->nome, sizeof(char), sizeof(c->nome), arqDados);

                arv->clientes[i] = c;
        }

        return arv;
}



void concatenacao(ArvB* arv, FILE* arqMeta, FILE* arqDados)
{
            int i = 0, posTroca = arv->pai->pos;
            while(arv->pai->filho[i]->clientes[0]->cod != arv->clientes[0]->cod)
                i++;

            if(arv->numChaves - 1+ arv->pai->filho[i + 1]->numChaves < 2*M)// condição para fazer a concatenação
            {
                    arv->clientes[arv->numChaves] = arv->pai->clientes[i]; // recebe o cliente do pai
                    arv->numChaves++;
                    arv->pai->numChaves--;
                    arv->filho[arv->numChaves] = arv->pai->filho[i+1]->filho[0];// recebe o primeiro filho do irmão adjacente

                        for(int j = 0; j < arv->pai->filho[i+1]->numChaves; j++)
                        {
                                arv->clientes[arv->numChaves] = arv->pai->filho[i+1]->clientes[j];
                                arv->filho[arv->numChaves +1] = arv->pai->filho[i + 1]->filho[j+1];// o primeiro filho é inserido depois
                                arv->numChaves++;
                        }


                        if(arv->pai->numChaves == 0)
                        {
                                if(arv->pai->pai == NULL) // Caso o nó pai seja a raiz, deve- se atualizar o novo nó raiz no arquivo
                                    atualizaPai(arqMeta, arv->pos);

                                  arv->pai->clientes[0] = NULL;

                                 ArvB* vazia = criaVazia();

                                 vazia->pos = arv->pai->filho[i+1]->pos;
                                 atualizaNo(vazia, arqMeta, arqDados);

                                  arv->pai->filho[i + 1] = NULL;
                                  arv->pai = NULL;

                                  vazia->pos = posTroca;
                                  atualizaNo(vazia, arqMeta, arqDados);
                        }
                        else
                            atualizaNo(arv->pai, arqMeta, arqDados);

                        atualizaNo(arv, arqMeta, arqDados);
            }

}


void redistribuicao(ArvB* arv, int pos, int posIrmao, int posPai, FILE* arqMeta, FILE* arqDados)
{
            int i = 0, j, posTroca;

            while(arv->pai->filho[i]->clientes[0]->cod != arv->clientes[0]->cod)
                i++;

            int posArv = arv->pos, posArvAnt = arv->pai->filho[posIrmao]->pos;
            int tamV =  arv->numChaves + arv->pai->filho[posIrmao]->numChaves;

            if(tamV % 2 == 0)
                posTroca = tamV/2 - 1;
            else
                posTroca = tamV/2;

            Cliente** v = (Cliente**) malloc(sizeof(Cliente*));
                for(int i = 0; i < tamV; i++)
                        v[i] = (Cliente*) malloc(sizeof(Cliente));


            for(int k = pos; k < arv->numChaves; k++)
            {
                        arv->clientes[k] = arv->clientes[k + 1];
                        arv->clientes[k +  1] = NULL;
            }
            arv->numChaves--;

            for(j = 0; j < arv->pai->filho[posIrmao]->numChaves; j++)
                    v[j] = arv->pai->filho[posIrmao]->clientes[j];
            for(int k = 0; k < 2*M; k++)
                arv->pai->filho[posIrmao]->clientes[k] = NULL;

            v[j] = arv->pai->clientes[posPai];
            j++;

            for(int k = 0; k < arv->numChaves; k++)
             {
                    v[j] = arv->clientes[k];
                    j++;
            }

            //ordena
                            int k = tamV -1;

                             for(int i = 0; i < tamV; i++)
                            {
                                for(int j = 0; j < k; j++)
                                {
                                        if(v[j + 1]->cod < v[j]->cod)
                                        {
                                                Cliente* aux = v[j];
                                                v[j] = v[j + 1];
                                                v[j + 1]  = aux;
                                        }
                                }

                                k--;
                            }

            for(j = 0; j < tamV; j++)
                printf("vetor de distr:: %d\n", v[j]->cod);
            arv->pai->clientes[posPai] = v[posTroca];// o pai recebe o cliente contido no meio do vetor


            //atualizar os filhos

             if(posIrmao ==  i-1)
             {
                    arv->pai->filho[posIrmao]->numChaves = 0;

                    for(j = 0; j < tamV; j++)
                    {
                            if( j < posTroca)
                             {
                                 arv->pai->filho[posIrmao]->clientes[j] = v[j];
                                 arv->pai->filho[posIrmao]->numChaves++;
                            }
                            else
                                arv->pai->filho[posIrmao]->clientes[j] = NULL;
                    }

                    arv->numChaves = 0;
                    for(int k = 0, j =  posTroca +1;  j < tamV; j++ , k++)
                     {
                             arv->clientes[k] = v[j];
                             arv->numChaves++;
                    }
            }

            else
            {
                    arv->numChaves = 0;
                    for(j = 0; j < tamV; j++)
                    {
                            if( j < posTroca)
                             {
                                 arv->clientes[j] = v[j];
                                 arv->numChaves++;
                            }
                            else
                                arv->clientes[j] = NULL;
                    }

                    arv->pai->filho[posIrmao]->numChaves = 0;
                    for(int k = 0, j =  posTroca +1;  j < tamV; j++ , k++)
                     {
                             arv->pai->filho[posIrmao]->clientes[k] = v[j];
                             arv->pai->filho[posIrmao]->numChaves++;
                    }
            }

            arv->pos = posArv;
            arv->pai->filho[posIrmao]->pos = posArvAnt;
            atualizaNo(arv->pai, arqMeta, arqDados);
            atualizaNo(arv, arqMeta, arqDados);
            atualizaNo(arv->pai->filho[posIrmao], arqMeta, arqDados);
}


void excluiRegistro(ArvB* arv, int pos, FILE* arqMeta, FILE* arqDados)// pos = posição da chave
{
            int i = 0;
            int posIrmao, posExclusao, posPai;

            if(arv->pai != NULL)
            {
                while(arv->pai->filho[i]->clientes[pos]->cod != arv->clientes[pos]->cod)
                          i++;

            }

                        if(arv->pai->filho[i-1] != NULL && i-1 >=0)
                         {
                               posIrmao = i-1;
                               posExclusao = i+1;
                               posPai = i-1;
                        }
                      else if(arv->pai->filho[i+1] != NULL)
                        {
                                posIrmao = i+1;
                                posExclusao = i-1;
                                posPai = i;
                        }


            if(verificaFolha(arv) == 1 && arv->numChaves-1 >= M || verificaFolha(arv) == 1 && arv->pai == NULL)// Exclui registro de nó folha (numero de clientes - 1 >= M)
            {

                    arv->clientes[pos] = NULL;
                    for(int i = pos; i < arv->numChaves; i++)
                    {
                            arv->clientes[i] = arv->clientes[i + 1];
                            arv->clientes[i +  1] = NULL;
                    }
                    arv->numChaves--;

                        atualizaNo(arv, arqMeta, arqDados);
             }

            else if(arv->numChaves - 1 < M) //considerar o filho seguinte , caso o nó atual seja o primeiro
            {
                        int posTroca = arv->pos;
                        int posTrocaPai = arv->pai->pos;

                    if(arv->numChaves -  1+ arv->pai->filho[posIrmao]->numChaves  < 2*M)
                    {

                             int posAnt = arv->pai->filho[posIrmao]->numChaves;

                             arv->pai->filho[posIrmao]->clientes[posAnt] = arv->pai->clientes[posPai]; // o filho anterior recebe a chave do pai
                             arv->pai->clientes[posExclusao] = NULL;
                             arv->pai->numChaves--;
                             arv->pai->filho[posIrmao]->numChaves++;
                             posAnt++;

                              for(int k = pos; k < 2*M; k++) // reordena as clientes aós a exclusão do elemento
                            {
                                   arv->clientes[k] = arv->clientes[k + 1];
                                    arv->clientes[k +  1] = NULL;
                                    arv->filho[k] = arv->filho[k+1];
                                    arv->filho[k+1] = NULL;
                                    // depois reordenar os filhos
                            }

                            arv->numChaves--;

                            for(int j = 0; j < arv->numChaves; j++)// recebe as clientes e os filhos do nó que será eliminado
                            {
                                    arv->pai->filho[posIrmao]->clientes[posAnt] = arv->clientes[j];
                                    arv->pai->filho[posIrmao]->filho[posAnt + 1] = arv->filho[j + 1];///////

                                    if(arv->pai->filho[posIrmao]->filho[posAnt+ 1] != NULL)
                                        arv->pai->filho[posIrmao]->filho[posAnt+1]->pai = arv->pai->filho[posIrmao];

                                    arv->pai->filho[posIrmao]->numChaves++;
                                    arv->clientes[j] = NULL;
                                    arv->filho[j+1] = NULL;
                                    arv->numChaves--;
                                    posAnt++;
                            }

                            int k = arv->pai->filho[posIrmao]->numChaves -1;
                             for(int i = 0; i < arv->pai->filho[posIrmao]->numChaves; i++)
                            {
                                for(int j = 0; j < k; j++)
                                {
                                        if(arv->pai->filho[posIrmao]->clientes[j + 1]->cod < arv->pai->filho[posIrmao]->clientes[j]->cod)
                                        {
                                            Cliente* aux = arv->pai->filho[posIrmao]->clientes[j];
                                            arv->pai->filho[posIrmao]->clientes[j] = arv->pai->filho[posIrmao]->clientes[j + 1];
                                            arv->pai->filho[posIrmao]->clientes[j + 1]  = aux;
                                        }
                                }

                                k--;
                            }

                            ArvB* vazia = criaVazia(); // árvore criada apenas para atualizar o nó excluído no arquivo.
                            arv->pai->filho[i] = NULL;

                                    if(arv->pai->numChaves == 0)
                                    {
                                         atualizaPai(arqMeta, arv->pai->filho[posIrmao]->pos);
                                         arv->pai->filho[posIrmao]->pai = NULL;
                                         vazia->pos = posTrocaPai;
                                         atualizaNo(vazia, arqMeta, arqDados);
                                    }
                                    else
                                            atualizaNo(arv->pai, arqMeta, arqDados);

                            vazia->pos = posTroca;
                            atualizaNo(vazia, arqMeta, arqDados);
                            atualizaNo(arv->pai->filho[posIrmao], arqMeta, arqDados);


                            if(arv->pai->numChaves < M && arv->pai->pai != NULL)
                            {

                                    concatenacao(arv->pai, arqMeta, arqDados);
                            }

                              arv = NULL;
                    }
                          else
                          {
                                redistribuicao(arv, pos, posIrmao, posPai, arqMeta, arqDados);
                          }

            }

            else
            {

                    int i;

                    arv->clientes[pos] = arv->filho[pos + 1]->clientes[0];

                    arv->filho[pos + 1]->clientes[0] = NULL;
                    for( i = 0; i < arv->filho[pos+1]->numChaves; i++)
                    {
                        arv->filho[pos + 1]->clientes[i] = arv->filho[pos + 1]->clientes[i + 1];
                        arv->filho[pos + 1]->filho[i] = arv->filho[pos+1]->filho[i+1];
                        arv->filho[pos +1]->clientes[i +  1] = NULL;
                    }

                    arv->filho[pos+1]->numChaves--;
                    arv->filho[pos+1]->filho[i] = NULL;

                    atualizaNo(arv, arqMeta, arqDados);
                    atualizaNo(arv->filho[pos+1], arqMeta, arqDados);

            }
}


void exclui(ArvB* arv, int num, FILE* arqMeta, FILE* arqDados)
{

        if(arv == NULL)
            printf("REGISTRO NÃO ENCONTRADO\n");

        else
        {
                int i = 0;

                while(i < arv->numChaves && num > arv->clientes[i]->cod)
                    i++;

                if(num == arv->clientes[i ]->cod)
                {

                        excluiRegistro(arv, i, arqMeta, arqDados);
                }

                else
                   {
                         exclui(arv->filho[i], num, arqMeta, arqDados);
                    }
        }

}


int main()
{
    FILE* arqMetaDados = fopen("metaDados.dat", "r+b");
    FILE* arqDados;
    ArvB* arv;

    if(arqMetaDados == NULL)
    {
        printf("Erro ao abrir arquivo de Metadados\n");
        return 0;
    }

    char endereco[100];

    fseek(arqMetaDados, sizeof(int)*2, SEEK_SET);
    if(fread(endereco, sizeof(char), sizeof(endereco), arqMetaDados) > 0)
    {

            arqDados = fopen(endereco, "r+b");
            printf("%s\n", endereco);
            if(arqDados == NULL)
            {
                printf("Erro ao abrir o arquivo de Dados\n");
                return 0;
            }

            rewind(arqDados);
            rewind(arqMetaDados);
            arv = criaArvore(arqMetaDados, arqDados);
    }
    else
    {
            int n;
            rewind(arqMetaDados);
            n = -1;
            fwrite(&n, sizeof(int), 1, arqMetaDados);
            n = 0;
            fwrite(&n, sizeof(int), 1, arqMetaDados);
            printf("Criando novo arquivo de dados...\n");
            arqDados = fopen(endereco, "w+b");

             rewind(arqDados);
            rewind(arqMetaDados);
            arv = criaVazia();
    }
        rewind(arqDados);
        rewind(arqMetaDados);


       int sel;

        do{

                menu();
                scanf("%d", &sel);

                if(sel == 1)
                    insercao(arv, arqMetaDados, arqDados);
                else if(sel == 2)
                    exclusao(arv, arqMetaDados, arqDados);
                else if(sel == 3)
                    procura(arv);
                else if(sel == 4)
                    imprimeDados(arqMetaDados, arqDados);
        }while(sel != 0);



    fclose(arqDados);
    fclose(arqMetaDados);
    return 0;
}
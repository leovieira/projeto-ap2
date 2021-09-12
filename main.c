#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "libs/sqlite3.h"
#include "src/functions.h"

int checkMenu(int res) {
  if (res >= 1 && res <= 7) {
    return res;
  }

  printf("Opção inválida!\n\n");
  printf(">_ ");
  scanf("%d", &res);

  return checkMenu(res);
}

int main(void) {
  sqlite3 *db;
  char nome_categoria[50], nome_alimento[50], porcao[50], confirm, data[11], horario[6];
  int res, cod_categoria, calorias, cod_alimento, qtd_alimento, total_calorias, i, j, consumo_total;
  float porcentagem;

  struct Alimento ali[1];
  struct Categoria cat[1];
  struct AlimentoAdd alis[25];
  struct Historico hist;

  time_t t;
  struct tm tm;

  system("chcp 65001");
  system("cls");

  printf("|-------------------------------------------------|\n");
  printf("|             CALCULADORA DE CALORIAS             |\n");
  printf("|    Desenvolvida por Elias, Gustavo e Leonardo   |\n");
  printf("|                       Filho    Rezende    Paiva |\n");
  printf("|-------------------------------------------------|\n");

  if (openDB(&db)) {
    printf("\n");
    system("pause");
    return 1;
  }

  do {
    printf("\nO que deseja fazer?\n\n");
    printf("1 - Calcular calorias da refeição\n");
    printf("2 - Consultar histórico\n");
    printf("3 - Cadastrar categoria\n");
    printf("4 - Cadastrar alimento\n");
    printf("5 - Apagar categoria\n");
    printf("6 - Apagar alimento\n");
    printf("7 - Sair\n\n");

    printf(">_ ");
    scanf("%d", &res);

    checkMenu(res);

    system("cls");

    switch (res) {
      case 1:
        i = 0;
        consumo_total = 0;

        do {
          printf("\nEscolha uma categoria:\n\n");

          if (selectCategorias(&db)) {
            printf("\n");
            system("pause");
            return 1;
          }

          printf("\n>_ ");
          scanf("%d", &cod_categoria);

          system("cls");

          printf("\nEscolha um alimento:\n\n");

          if (selectAlimentos(&db, cod_categoria)) {
            printf("\n");
            system("pause");
            return 1;
          }

          printf("\n>_ ");
          scanf("%d", &cod_alimento);

          if (selectAlimento(&db, cod_alimento, ali)) {
            printf("\n");
            system("pause");
            return 1;
          }

          calorias = ali[0].calorias;

          printf("\nDigite a quantidade (consultar tabela):\n");
          scanf("%d", &qtd_alimento);

          system("cls");

          total_calorias = calorias * qtd_alimento;

          consumo_total += total_calorias;

          alis[i].cod = ali[0].cod;
          strcpy(alis[i].nome, ali[0].nome);
          strcpy(alis[i].porcao, ali[0].porcao);
          alis[i].calorias = ali[0].calorias;
          alis[i].cod_categoria = ali[0].cod_categoria;
          alis[i].qtd = qtd_alimento;
          alis[i].total_calorias = total_calorias;
          i++;

          printf("\nDeseja adicionar outro alimento? (s - sim | n - não)\n");
          printf("\n>_ ");
          fflush(stdin);
          scanf("%c", &confirm);

          system("cls");
        } while(confirm == 's' || confirm == 'S');

        t = time(NULL);
        tm = *localtime(&t);

        sprintf(data, "%02d/%02d/%04d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
        sprintf(horario, "%02d:%02d", tm.tm_hour, tm.tm_min);

        strcpy(hist.data, data);
        strcpy(hist.horario, horario);
        hist.consumo_total = consumo_total;

        if (writeFile(&hist)) {
          printf("\n");
          system("pause");
          return 1;
        }

        printf("\n---- RELATÓRIO ----\n");

        for (j = 0; j < i; j++) {
          if (selectCategoria(&db, alis[j].cod_categoria, cat)) {
            printf("\n");
            system("pause");
            return 1;
          }

          printf("\nAlimento: %s\n", alis[j].nome);
          printf("Categoria: %s\n", cat[0].nome);
          printf("Porção: %s\n", alis[j].porcao);
          printf("Quantidade: %d\n", alis[j].qtd);
          printf("Total de calorias: %d kcal\n", alis[j].total_calorias);
        }

        printf ("\n---- RESUMO GERAL ----\n");

        printf ("\nConsumo total: %d kcal\n", consumo_total);

        if (consumo_total > 2500) {
          printf("\nVocê excedeu o consumo total de calorias no período de 24h.\n");
        } else {
          porcentagem = 100 * consumo_total / 2500;
          printf("\nO total representa %0.2f%% da quantidade de kcal recomendada para ser consumida no período de 24h.\n", porcentagem);
        }
        break;

      case 2:
        printf("\n---- HISTÓRICO ----\n");

        if (readFile()) {
          printf("\n");
          system("pause");
          return 1;
        }
        break;

      case 3:
        printf("\nDigite o nome da categoria:\n");
        fflush(stdin);
        scanf("%[^\n]", nome_categoria);
        printf("\n");

        if (insertCategoria(&db, nome_categoria)) {
          printf("\n");
          system("pause");
          return 1;
        }
        break;

      case 4:
        printf("\nEscolha uma categoria:\n\n");

        if (selectCategorias(&db)) {
          printf("\n");
          system("pause");
          return 1;
        }

        printf("\n>_ ");
        scanf("%d", &cod_categoria);

        system("cls");

        printf("\nDigite o nome do alimento:\n");
        fflush(stdin);
        scanf("%[^\n]", nome_alimento);

        printf("\nDigite a porção do alimento:\n");
        fflush(stdin);
        scanf("%[^\n]", porcao);

        printf("\nDigite a quantidade de calorias referente a essa porção:\n");
        scanf("%d",&calorias);
        printf("\n");

        if (insertAlimento(&db, nome_alimento, porcao, calorias, cod_categoria)) {
          printf("\n");
          system("pause");
          return 1;
        }
        break;

      case 5:
        printf("\nEscolha uma categoria:\n\n");

        if (selectCategorias(&db)) {
          printf("\n");
          system("pause");
          return 1;
        }

        printf("\n>_ ");
        scanf("%d", &cod_categoria);

        system("cls");

        printf("\nDeseja realmente apagar a categoria? (s - sim | n - não)\n");
        printf("Os alimentos pertencentes a essa categoria serão apagados.\n");

        printf("\n>_ ");
        fflush(stdin);
        scanf("%c", &confirm);
        printf("\n");

        if (confirm == 's' || confirm == 'S') {
          if (deleteCategoria(&db, cod_categoria)) {
            printf("\n");
            system("pause");
            return 1;
          }
        }
        break;

      case 6:
        printf("\nEscolha uma categoria:\n\n");

        if (selectCategorias(&db)) {
          printf("\n");
          system("pause");
          return 1;
        }

        printf("\n>_ ");
        scanf("%d", &cod_categoria);

        system("cls");

        printf("\nEscolha um alimento:\n\n");

        if (selectAlimentos(&db, cod_categoria)) {
          printf("\n");
          system("pause");
          return 1;
        }

        printf("\n>_ ");
        scanf("%d", &cod_alimento);

        system("cls");

        printf("\nDeseja realmente apagar o alimento? (s - sim | n - não)\n");

        printf("\n>_ ");
        fflush(stdin);
        scanf("%c", &confirm);
        printf("\n");

        if (confirm == 's' || confirm == 'S') {
          if (deleteAlimento(&db, cod_alimento)) {
            printf("\n");
            system("pause");
            return 1;
          }
        }
        break;

      case 7:
        printf("Até logo :)\n");
        break;
    }
  } while (res != 7);

  sqlite3_close(db);

  printf("\n");
  system("pause");
  return 0;
}

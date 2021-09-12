#include <stdio.h>
#include <string.h>
#include "../libs/sqlite3.h"
#include "functions.h"

int openDB(sqlite3 **db) {
  int rc = sqlite3_open("banco.db", &(*db));

  if (rc != SQLITE_OK) {
    printf("Oops... não foi possível abrir o banco de dados.\n");
    printf("Erro: %s\n", sqlite3_errmsg(*db));

    sqlite3_close(*db);

    return 1;
  }

  return 0;
}

int selectCategorias(sqlite3 **db) {
  sqlite3_stmt *stmt;

  char *sql = "SELECT * FROM categorias;";

  int rc = sqlite3_prepare_v2(*db, sql, -1, &stmt, NULL);

  if (rc != SQLITE_OK) {
    printf("Oops... não foi possível buscar as categorias.\n");
    printf("Erro: %s\n", sqlite3_errmsg(*db));

    sqlite3_finalize(stmt);
    sqlite3_close(*db);

    return 1;
  }

  int step = sqlite3_step(stmt);

  while (step == SQLITE_ROW) {
    printf("%d - %s\n", sqlite3_column_int(stmt, 0), sqlite3_column_text(stmt, 1));
    step = sqlite3_step(stmt);
  }

  sqlite3_finalize(stmt);

  return 0;
}

int selectAlimentos(sqlite3 **db, int cod_categoria) {
  sqlite3_stmt *stmt;
  char *sql;

  if (cod_categoria == 0) {
    sql = "SELECT * FROM alimentos;";
  } else {
    sql = "SELECT * FROM alimentos WHERE cod_categoria = ?;";
  }

  int rc = sqlite3_prepare_v2(*db, sql, -1, &stmt, NULL);

  if (rc == SQLITE_OK) {
    if (cod_categoria != 0) {
      sqlite3_bind_int(stmt, 1, cod_categoria);
    }
  } else {
    printf("Oops... não foi possível buscar os alimentos.\n");
    printf("Erro: %s\n", sqlite3_errmsg(*db));

    sqlite3_finalize(stmt);
    sqlite3_close(*db);

    return 1;
  }

  int step = sqlite3_step(stmt);

  while (step == SQLITE_ROW) {
    printf(
            "%d - %s | %s\n",
            sqlite3_column_int(stmt, 0),
            sqlite3_column_text(stmt, 1),
            sqlite3_column_text(stmt, 2)
    );
    step = sqlite3_step(stmt);
  }

  sqlite3_finalize(stmt);

  return 0;
}

int selectCategoria(sqlite3 **db, int cod, struct Categoria *cat) {
  sqlite3_stmt *stmt;

  char *sql = "SELECT * FROM categorias WHERE cod = ?;";

  int rc = sqlite3_prepare_v2(*db, sql, -1, &stmt, NULL);

  if (rc == SQLITE_OK) {
    sqlite3_bind_int(stmt, 1, cod);
  } else {
    printf("Oops... não foi possível buscar a categoria.\n");
    printf("Erro: %s\n", sqlite3_errmsg(*db));

    sqlite3_finalize(stmt);
    sqlite3_close(*db);

    return 1;
  }

  int step = sqlite3_step(stmt);

  if (step == SQLITE_ROW) {
    cat[0].cod = sqlite3_column_int(stmt, 0);
    strcpy(cat[0].nome, sqlite3_column_text(stmt, 1));
  } else {
    printf("Oops... não foi possível buscar a categoria.\n");
    printf("Erro: %s\n", sqlite3_errmsg(*db));

    sqlite3_finalize(stmt);
    sqlite3_close(*db);

    return 1;
  }

  sqlite3_finalize(stmt);

  return 0;
}

int selectAlimento(sqlite3 **db, int cod, struct Alimento *ali) {
  sqlite3_stmt *stmt;

  char *sql = "SELECT * FROM alimentos WHERE cod = ?;";

  int rc = sqlite3_prepare_v2(*db, sql, -1, &stmt, NULL);

  if (rc == SQLITE_OK) {
    sqlite3_bind_int(stmt, 1, cod);
  } else {
    printf("Oops... não foi possível buscar o alimento.\n");
    printf("Erro: %s\n", sqlite3_errmsg(*db));

    sqlite3_finalize(stmt);
    sqlite3_close(*db);

    return 1;
  }

  int step = sqlite3_step(stmt);

  if (step == SQLITE_ROW) {
    ali[0].cod = sqlite3_column_int(stmt, 0);
    strcpy(ali[0].nome, sqlite3_column_text(stmt, 1));
    strcpy(ali[0].porcao, sqlite3_column_text(stmt, 2));
    ali[0].calorias = sqlite3_column_int(stmt, 3);
    ali[0].cod_categoria = sqlite3_column_int(stmt, 4);
  } else {
    printf("Oops... não foi possível buscar o alimento.\n");
    printf("Erro: %s\n", sqlite3_errmsg(*db));

    sqlite3_finalize(stmt);
    sqlite3_close(*db);

    return 1;
  }

  sqlite3_finalize(stmt);

  return 0;
}

int insertCategoria(sqlite3 **db, char *nome) {
  sqlite3_stmt *stmt;

  char *sql = "INSERT INTO categorias(nome) VALUES(:nome);";

  int rc = sqlite3_prepare_v2(*db, sql, -1, &stmt, NULL);

  if (rc == SQLITE_OK) {
    int idx_nome = sqlite3_bind_parameter_index(stmt, ":nome");
    int rc = sqlite3_bind_text(stmt, idx_nome, nome, -1, NULL);

    if (rc != SQLITE_OK) {
      printf("Oops... não foi possível cadastrar a categoria.\n");
      printf("Erro: %s\n", sqlite3_errmsg(*db));

      sqlite3_finalize(stmt);
      sqlite3_close(*db);

      return 1;
    }
  } else {
    printf("Oops... não foi possível cadastrar a categoria.\n");
    printf("Erro: %s\n", sqlite3_errmsg(*db));

    sqlite3_finalize(stmt);
    sqlite3_close(*db);

    return 1;
  }

  int step = sqlite3_step(stmt);

  if (step == SQLITE_DONE) {
    printf("Categoria salva com sucesso!\n");
  } else {
    printf("Oops... não foi possível cadastrar a categoria.\n");
    printf("Erro: %s\n", sqlite3_errmsg(*db));

    sqlite3_finalize(stmt);
    sqlite3_close(*db);

    return 1;
  }

  sqlite3_finalize(stmt);

  return 0;
}

int insertAlimento(sqlite3 **db, char *nome, char *porcao, int calorias, int cod_categoria) {
  sqlite3_stmt *stmt;

  char *sql = "INSERT INTO alimentos(nome, porcao, calorias, cod_categoria) VALUES(?, ?, ?, ?);";

  int rc = sqlite3_prepare_v2(*db, sql, -1, &stmt, NULL);

  if (rc == SQLITE_OK) {
    sqlite3_bind_text(stmt, 1, nome, -1, NULL);
    sqlite3_bind_text(stmt, 2, porcao, -1, NULL);
    sqlite3_bind_int(stmt, 3, calorias);
    sqlite3_bind_int(stmt, 4, cod_categoria);
  } else {
    printf("Oops... não foi possível cadastrar o alimento.\n");
    printf("Erro: %s\n", sqlite3_errmsg(*db));

    sqlite3_finalize(stmt);
    sqlite3_close(*db);

    return 1;
  }

  int step = sqlite3_step(stmt);

  if (step == SQLITE_DONE) {
    printf("Alimento cadastrado com sucesso!\n");
  } else {
    printf("Oops... não foi possível cadastrar o alimento.\n");
    printf("Erro: %s\n", sqlite3_errmsg(*db));

    sqlite3_finalize(stmt);
    sqlite3_close(*db);

    return 1;
  }

  sqlite3_finalize(stmt);

  return 0;
}

int deleteCategoria(sqlite3 **db, int cod) {
  sqlite3_stmt *stmt;

  char *sql = "DELETE FROM categorias WHERE cod = ?;";

  int rc = sqlite3_prepare_v2(*db, sql, -1, &stmt, NULL);

  if (rc == SQLITE_OK) {
    sqlite3_bind_int(stmt, 1, cod);
  } else {
    printf("Oops... não foi possível apagar a categoria.\n");
    printf("Erro: %s\n", sqlite3_errmsg(*db));

    sqlite3_finalize(stmt);
    sqlite3_close(*db);

    return 1;
  }

  int step = sqlite3_step(stmt);

  if (step != SQLITE_DONE) {
    printf("Oops... não foi possível apagar a categoria.\n");
    printf("Erro: %s\n", sqlite3_errmsg(*db));

    sqlite3_finalize(stmt);
    sqlite3_close(*db);

    return 1;
  }

  sql = "DELETE FROM alimentos WHERE cod_categoria = ?;";

  rc = sqlite3_prepare_v2(*db, sql, -1, &stmt, NULL);

  if (rc == SQLITE_OK) {
    sqlite3_bind_int(stmt, 1, cod);
  } else {
    printf("Oops... não foi possível apagar a categoria.\n");
    printf("Erro: %s\n", sqlite3_errmsg(*db));

    sqlite3_finalize(stmt);
    sqlite3_close(*db);

    return 1;
  }

  step = sqlite3_step(stmt);

  if (step == SQLITE_DONE) {
    printf("Categoria apagada com sucesso!\n");
  } else {
    printf("Oops... não foi possível apagar a categoria.\n");
    printf("Erro: %s\n", sqlite3_errmsg(*db));

    sqlite3_finalize(stmt);
    sqlite3_close(*db);

    return 1;
  }

  sqlite3_finalize(stmt);

  return 0;
}

int deleteAlimento(sqlite3 **db, int cod) {
  sqlite3_stmt *stmt;

  char *sql = "DELETE FROM alimentos WHERE cod = ?;";

  int rc = sqlite3_prepare_v2(*db, sql, -1, &stmt, NULL);

  if (rc == SQLITE_OK) {
    sqlite3_bind_int(stmt, 1, cod);
  } else {
    printf("Oops... não foi possível apagar o alimento.\n");
    printf("Erro: %s\n", sqlite3_errmsg(*db));

    sqlite3_finalize(stmt);
    sqlite3_close(*db);

    return 1;
  }

  int step = sqlite3_step(stmt);

  if (step == SQLITE_DONE) {
    printf("Alimento apagado com sucesso!\n");
  } else {
    printf("Oops... não foi possível apagar o alimento.\n");
    printf("Erro: %s\n", sqlite3_errmsg(*db));

    sqlite3_finalize(stmt);
    sqlite3_close(*db);

    return 1;
  }

  sqlite3_finalize(stmt);

  return 0;
}

int openFile(FILE **arq, char mode[5]) {
  *arq = fopen("historico.bin", mode);
  if (arq == NULL) {
    printf("Oops... algo deu errado!\n");

    return 1;
  }

  return 0;
}

int writeFile(struct Historico *hist) {
  FILE *arq;

  if (openFile(&arq, "ab")) {
    printf("Oops... algo deu errado!\n");

    return 1;
  }

  fwrite(&(*hist), sizeof(struct Historico), 1, arq);
  fclose(arq);

  return 0;
}

int readFile() {
  FILE *arq;
  struct Historico hist;

  if (openFile(&arq, "rb")) {
    printf("Oops... algo deu errado!\n");

    return 1;
  }

  while (fread(&hist, sizeof(struct Historico), 1, arq) != 0) {
    printf("\nData: %s\n", hist.data);
    printf("Horário: %s\n", hist.horario);
    printf("Consumo total: %d kcal\n", hist.consumo_total);
  }

  fclose(arq);

  return 0;
}

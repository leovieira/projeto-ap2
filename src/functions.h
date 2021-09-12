#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

struct Categoria {
  int cod;
  char nome[50];
};

struct Alimento {
  int cod;
  char nome[50];
  char porcao[50];
  int calorias;
  int cod_categoria;
};

struct AlimentoAdd {
  int cod;
  char nome[50];
  char porcao[50];
  int calorias;
  int cod_categoria;
  int qtd;
  int total_calorias;
};

struct Historico {
  char data[11];
  char horario[6];
  int consumo_total;
};

int openDB(sqlite3 **db);
int selectCategorias(sqlite3 **db);
int selectAlimentos(sqlite3 **db, int cod_categoria);
int selectCategoria(sqlite3 **db, int cod, struct Categoria *cat);
int selectAlimento(sqlite3 **db, int cod, struct Alimento *ali);
int insertCategoria(sqlite3 **db, char *nome);
int insertAlimento(sqlite3 **db, char *nome, char *porcao, int calorias, int cod_categoria);
int deleteCategoria(sqlite3 **db, int cod);
int deleteAlimento(sqlite3 **db, int cod);
int openFile(FILE **arq, char mode[5]);
int writeFile(struct Historico *hist);
int readFile();

#endif

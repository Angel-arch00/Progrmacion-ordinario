#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
¿
#define SERVER "localhost"
#define USER "root"
#define PASSWORD ""
#define DATABASE "casos_corrupcion"

¿
MYSQL* conectarBD() {
    MYSQL *conn = mysql_init(NULL);
    if (conn == NULL) {
        fprintf(stderr, "Error al inicializar MySQL: %s\n", mysql_error(conn));
        exit(1);
    }

    if (mysql_real_connect(conn, SERVER, USER, PASSWORD, DATABASE, 0, NULL, 0) == NULL) {
        fprintf(stderr, "Error de conexion: %s\n", mysql_error(conn));
        mysql_close(conn);
        exit(1);
    }
    printf("Conexion a la base de datos exitosa.\n");
    return conn;
}

void ejecutarConsulta(MYSQL *conn, const char *query) {
    printf("Ejecutando consulta: %s\n", query);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Error en la consulta: %s\n", mysql_error(conn));
    } else {
        printf("Operacion realizada con exito.\n");
    }
}

void mostrarResultadosConsulta(MYSQL *conn, const char *query) {
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Error al obtener datos: %s\n", mysql_error(conn));
        return;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL) {
        fprintf(stderr, "Error al almacenar resultados: %s\n", mysql_error(conn));
        return;
    }

    int num_fields = mysql_num_fields(res);
    MYSQL_ROW row;
    MYSQL_FIELD *field;

    for (int i = 0; i < num_fields; i++) {
        field = mysql_fetch_field_nr(res, i);
        printf("%-25s", field->name);
    }
    printf("\n");
    for (int i = 0; i < num_fields; i++) {
        printf("-------------------------");
    }
    printf("\n");

    while ((row = mysql_fetch_row(res))) {
        for (int i = 0; i < num_fields; i++) {
            printf("%-25s", row[i] ? row[i] : "NULL");
        }
        printf("\n");
    }
    printf("---------------------------------------\n");
    mysql_free_result(res);
}

void mostrarTabla(MYSQL *conn, const char *tableName) {
    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query), "SELECT * FROM %s", tableName);
    printf("\n--- Datos de la tabla '%s' ---\n", tableName);
    mostrarResultadosConsulta(conn, query);
}

void readStringInput(char *buffer, int bufferSize) {
    fgets(buffer, bufferSize, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
}

int readIntInput(const char *prompt) {
    int value;
    printf("%s", prompt);
    while (scanf("%d", &value) != 1) {
        printf("Entrada invalida. Por favor, ingrese un numero entero: ");
        while (getchar() != '\n');
    }
    getchar();
    return value;
}

double readDoubleInput(const char *prompt) {
    double value;
    printf("%s", prompt);
    while (scanf("%lf", &value) != 1) {
        printf("Entrada invalida. Por favor, ingrese un numero: ");
        while (getchar() != '\n');
    }
    getchar();
    return value;
}

void readLongTextInput(char *buffer, int bufferSize) {
    printf("Ingrese texto (presione Enter al final): ");
    fflush(stdin);
    char line[MAX_STRING_LEN];
    buffer[0] = '\0';
    while (fgets(line, sizeof(line), stdin) != NULL) {
        if (strcmp(line, "\n") == 0) {
            break;
        }
        size_t len_line = strlen(line);
        if (len_line > 0 && line[len_line - 1] == '\n') {
            line[len_line - 1] = '\0';
        }
        if (strlen(buffer) + strlen(line) + 1 < bufferSize) {
            strcat(buffer, line);
            strcat(buffer, "\n");
        } else {
            printf("Advertencia: Se alcanzo el limite de texto largo para la descripcion/dictamen.\n");
            break;
        }
    }
    if (strlen(buffer) > 0 && buffer[strlen(buffer) - 1] == '\n') {
        buffer[strlen(buffer) - 1] = '\0';
    }
}

void readDateInput(char *buffer, int bufferSize, const char *prompt) {
    printf("%s (YYYY-MM-DD): ", prompt);
    readStringInput(buffer, bufferSize);
}

void insertarJuez(MYSQL *conn) {
    char nombre_juez[MAX_STRING_LEN], direccion_juez[MAX_STRING_LEN];
    char fecha_nacimiento[MAX_FECHA_LEN], fecha_inicio_ejercicio[MAX_FECHA_LEN];
    char query[MAX_QUERY_LEN];

    printf("Nombre del Juez: "); readStringInput(nombre_juez, sizeof(nombre_juez));
    printf("Direccion del Juez: "); readStringInput(direccion_juez, sizeof(direccion_juez));
    readDateInput(fecha_nacimiento, sizeof(fecha_nacimiento), "Fecha de Nacimiento");
    readDateInput(fecha_inicio_ejercicio, sizeof(fecha_inicio_ejercicio), "Fecha de Inicio de Ejercicio");

    snprintf(query, sizeof(query),
             "INSERT INTO JUEZ (nombre_juez, direccion_juez, fecha_nacimiento, fecha_inicio_ejercicio) VALUES ('%s', '%s', '%s', '%s')",
             nombre_juez, direccion_juez, fecha_nacimiento, fecha_inicio_ejercicio);
    ejecutarConsulta(conn, query);
}

void editarJuez(MYSQL *conn) {
    int idjuez = readIntInput("ID del Juez a editar: ");
    char nombre_juez[MAX_STRING_LEN], direccion_juez[MAX_STRING_LEN];
    char fecha_nacimiento[MAX_FECHA_LEN], fecha_inicio_ejercicio[MAX_FECHA_LEN];
    char query[MAX_QUERY_LEN];

    printf("Nuevo Nombre del Juez: "); readStringInput(nombre_juez, sizeof(nombre_juez));
    printf("Nueva Direccion del Juez: "); readStringInput(direccion_juez, sizeof(direccion_juez));
    readDateInput(fecha_nacimiento, sizeof(fecha_nacimiento), "Nueva Fecha de Nacimiento");
    readDateInput(fecha_inicio_ejercicio, sizeof(fecha_inicio_ejercicio), "Nueva Fecha de Inicio de Ejercicio");

    snprintf(query, sizeof(query),
             "UPDATE JUEZ SET nombre_juez = '%s', direccion_juez = '%s', fecha_nacimiento = '%s', fecha_inicio_ejercicio = '%s' WHERE idjuez = %d",
             nombre_juez, direccion_juez, fecha_nacimiento, fecha_inicio_ejercicio, idjuez);
    ejecutarConsulta(conn, query);
}

void borrarJuez(MYSQL *conn) {
    int idjuez = readIntInput("ID del Juez a borrar: ");
    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query), "DELETE FROM JUEZ WHERE idjuez = %d", idjuez);
    ejecutarConsulta(conn, query);
}

void insertarPartidoPolitico(MYSQL *conn) {
    char nombre_partido[MAX_PARTIDO_LEN], direccion_sede[MAX_STRING_LEN];
    char query[MAX_QUERY_LEN];

    printf("Nombre del Partido Politico: "); readStringInput(nombre_partido, sizeof(nombre_partido));
    printf("Direccion de la Sede Central: "); readStringInput(direccion_sede, sizeof(direccion_sede));

    snprintf(query, sizeof(query),
             "INSERT INTO PARTIDO_POLITICO (nombre_partido, direccion_sede) VALUES ('%s', '%s')",
             nombre_partido, direccion_sede);
    ejecutarConsulta(conn, query);
}

void editarPartidoPolitico(MYSQL *conn) {
    char nombre_partido_old[MAX_PARTIDO_LEN];
    printf("Nombre del Partido Politico a editar: "); readStringInput(nombre_partido_old, sizeof(nombre_partido_old));
    
    char nombre_partido_new[MAX_PARTIDO_LEN], direccion_sede[MAX_STRING_LEN];
    char query[MAX_QUERY_LEN];

    printf("Nuevo Nombre del Partido Politico: "); readStringInput(nombre_partido_new, sizeof(nombre_partido_new));
    printf("Nueva Direccion de la Sede Central: "); readStringInput(direccion_sede, sizeof(direccion_sede));

    snprintf(query, sizeof(query),
             "UPDATE PARTIDO_POLITICO SET nombre_partido = '%s', direccion_sede = '%s' WHERE nombre_partido = '%s'",
             nombre_partido_new, direccion_sede, nombre_partido_old);
    ejecutarConsulta(conn, query);
}

void borrarPartidoPolitico(MYSQL *conn) {
    char nombre_partido[MAX_PARTIDO_LEN];
    printf("Nombre del Partido Politico a borrar: "); readStringInput(nombre_partido, sizeof(nombre_partido));
    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query), "DELETE FROM PARTIDO_POLITICO WHERE nombre_partido = '%s'", nombre_partido);
    ejecutarConsulta(conn, query);
}

void insertarPeriodico(MYSQL *conn) {
    char nombre_periodico[MAX_PERIODICO_LEN], direccion_periodico[MAX_STRING_LEN];
    int tirada;
    char query[MAX_QUERY_LEN];

    printf("Nombre del Periodico: "); readStringInput(nombre_periodico, sizeof(nombre_periodico));
    printf("Direccion del Periodico: "); readStringInput(direccion_periodico, sizeof(direccion_periodico));
    tirada = readIntInput("Tirada: ");

    snprintf(query, sizeof(query),
             "INSERT INTO PERIODICO (nombre_periodico, direccion_periodico, tirada) VALUES ('%s', '%s', %d)",
             nombre_periodico, direccion_periodico, tirada);
    ejecutarConsulta(conn, query);
}

void editarPeriodico(MYSQL *conn) {
    char nombre_periodico_old[MAX_PERIODICO_LEN];
    printf("Nombre del Periodico a editar: "); readStringInput(nombre_periodico_old, sizeof(nombre_periodico_old));
    
    char nombre_periodico_new[MAX_PERIODICO_LEN], direccion_periodico[MAX_STRING_LEN];
    int tirada;
    char query[MAX_QUERY_LEN];

    printf("Nuevo Nombre del Periodico: "); readStringInput(nombre_periodico_new, sizeof(nombre_periodico_new));
    printf("Nueva Direccion del Periodico: "); readStringInput(direccion_periodico, sizeof(direccion_periodico));
    tirada = readIntInput("Nueva Tirada: ");

    snprintf(query, sizeof(query),
             "UPDATE PERIODICO SET nombre_periodico = '%s', direccion_periodico = '%s', tirada = %d WHERE nombre_periodico = '%s'",
             nombre_periodico_new, direccion_periodico, tirada, nombre_periodico_old);
    ejecutarConsulta(conn, query);
}

void borrarPeriodico(MYSQL *conn) {
    char nombre_periodico[MAX_PERIODICO_LEN];
    printf("Nombre del Periodico a borrar: "); readStringInput(nombre_periodico, sizeof(nombre_periodico));
    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query), "DELETE FROM PERIODICO WHERE nombre_periodico = '%s'", nombre_periodico);
    ejecutarConsulta(conn, query);
}

void insertarCasoCorrupcion(MYSQL *conn) {
    int codigo_caso;
    char nombre_caso[MAX_STRING_LEN];
    char descripcion[MAX_QUERY_LEN];
    double millones_desviados;
    char dictamen[MAX_QUERY_LEN];
    int juez_idjuez;
    char periodico_nombre_periodico[MAX_PERIODICO_LEN];
    char fecha_descubrimiento[MAX_FECHA_LEN];
    char query[MAX_QUERY_LEN];

    codigo_caso = readIntInput("Codigo del Caso: ");
    printf("Nombre del Caso: "); readStringInput(nombre_caso, sizeof(nombre_caso));
    printf("Descripcion del Caso:\n"); readLongTextInput(descripcion, sizeof(descripcion));
    millones_desviados = readDoubleInput("Estimacion de Millones Desviados: ");
    printf("Dictamen (opcional, deje vacio si no hay):\n"); readLongTextInput(dictamen, sizeof(dictamen));
    juez_idjuez = readIntInput("ID del Juez que investiga el caso: ");
    printf("Nombre del Periodico que descubrio el caso: "); readStringInput(periodico_nombre_periodico, sizeof(periodico_nombre_periodico));
    readDateInput(fecha_descubrimiento, sizeof(fecha_descubrimiento), "Fecha de Descubrimiento");

    snprintf(query, sizeof(query),
             "INSERT INTO CASO_CORRUPCION (codigo_caso, nombre_caso, descripcion, millones_desviados, dictamen, juez_idjuez, periodico_nombre_periodico, fecha_descubrimiento) VALUES (%d, '%s', '%s', %.2f, '%s', %d, '%s', '%s')",
             codigo_caso, nombre_caso, descripcion, millones_desviados, dictamen, juez_idjuez, periodico_nombre_periodico, fecha_descubrimiento);
    ejecutarConsulta(conn, query);
}

void editarCasoCorrupcion(MYSQL *conn) {
    int codigo_caso_old = readIntInput("Codigo del Caso a editar: ");
    char nombre_caso[MAX_STRING_LEN];
    char descripcion[MAX_QUERY_LEN];
    double millones_desviados;
    char dictamen[MAX_QUERY_LEN];
    int juez_idjuez;
    char periodico_nombre_periodico[MAX_PERIODICO_LEN];
    char fecha_descubrimiento[MAX_FECHA_LEN];
    char query[MAX_QUERY_LEN];

    printf("Nuevo Nombre del Caso: "); readStringInput(nombre_caso, sizeof(nombre_caso));
    printf("Nueva Descripcion del Caso:\n"); readLongTextInput(descripcion, sizeof(descripcion));
    millones_desviados = readDoubleInput("Nueva Estimacion de Millones Desviados: ");
    printf("Nuevo Dictamen (opcional, deje vacio si no hay):\n"); readLongTextInput(dictamen, sizeof(dictamen));
    juez_idjuez = readIntInput("Nuevo ID del Juez que investiga el caso: ");
    printf("Nuevo Nombre del Periodico que descubrio el caso: "); readStringInput(periodico_nombre_periodico, sizeof(periodico_nombre_periodico));
    readDateInput(fecha_descubrimiento, sizeof(fecha_descubrimiento), "Nueva Fecha de Descubrimiento");

    snprintf(query, sizeof(query),
             "UPDATE CASO_CORRUPCION SET nombre_caso = '%s', descripcion = '%s', millones_desviados = %.2f, dictamen = '%s', juez_idjuez = %d, periodico_nombre_periodico = '%s', fecha_descubrimiento = '%s' WHERE codigo_caso = %d",
             nombre_caso, descripcion, millones_desviados, dictamen, juez_idjuez, periodico_nombre_periodico, fecha_descubrimiento, codigo_caso_old);
    ejecutarConsulta(conn, query);
}

void borrarCasoCorrupcion(MYSQL *conn) {
    int codigo_caso = readIntInput("Codigo del Caso a borrar: ");
    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query), "DELETE FROM CASO_CORRUPCION WHERE codigo_caso = %d", codigo_caso);
    ejecutarConsulta(conn, query);
}

void insertarCiudadano(MYSQL *conn) {
    char dni[MAX_DNI_LEN], nombre_ciudadano[MAX_STRING_LEN], direccion_ciudadano[MAX_STRING_LEN];
    double patrimonio;
    char cargo_principal[MAX_STRING_LEN], partido_politico_nombre_partido[MAX_PARTIDO_LEN];
    char query[MAX_QUERY_LEN];

    printf("DNI del Ciudadano: "); readStringInput(dni, sizeof(dni));
    printf("Nombre del Ciudadano: "); readStringInput(nombre_ciudadano, sizeof(nombre_ciudadano));
    printf("Direccion del Ciudadano: "); readStringInput(direccion_ciudadano, sizeof(direccion_ciudadano));
    patrimonio = readDoubleInput("Patrimonio: ");
    printf("Cargo Principal: "); readStringInput(cargo_principal, sizeof(cargo_principal));
    printf("Nombre del Partido Politico (deje vacio si no pertenece): "); readStringInput(partido_politico_nombre_partido, sizeof(partido_politico_nombre_partido));

    if (strlen(partido_politico_nombre_partido) == 0) {
        snprintf(query, sizeof(query),
                 "INSERT INTO CIUDADANO (dni, nombre_ciudadano, direccion_ciudadano, patrimonio, cargo_principal, partido_politico_nombre_partido) VALUES ('%s', '%s', '%s', %.2f, '%s', NULL)",
                 dni, nombre_ciudadano, direccion_ciudadano, patrimonio, cargo_principal);
    } else {
        snprintf(query, sizeof(query),
                 "INSERT INTO CIUDADANO (dni, nombre_ciudadano, direccion_ciudadano, patrimonio, cargo_principal, partido_politico_nombre_partido) VALUES ('%s', '%s', '%s', %.2f, '%s', '%s')",
                 dni, nombre_ciudadano, direccion_ciudadano, patrimonio, cargo_principal, partido_politico_nombre_partido);
    }
    ejecutarConsulta(conn, query);
}

void editarCiudadano(MYSQL *conn) {
    char dni_old[MAX_DNI_LEN];
    printf("DNI del Ciudadano a editar: "); readStringInput(dni_old, sizeof(dni_old));
    
    char dni_new[MAX_DNI_LEN], nombre_ciudadano[MAX_STRING_LEN], direccion_ciudadano[MAX_STRING_LEN];
    double patrimonio;
    char cargo_principal[MAX_STRING_LEN], partido_politico_nombre_partido[MAX_PARTIDO_LEN];
    char query[MAX_QUERY_LEN];

    printf("Nuevo DNI del Ciudadano: "); readStringInput(dni_new, sizeof(dni_new));
    printf("Nuevo Nombre del Ciudadano: "); readStringInput(nombre_ciudadano, sizeof(nombre_ciudadano));
    printf("Nueva Direccion del Ciudadano: "); readStringInput(direccion_ciudadano, sizeof(direccion_ciudadano));
    patrimonio = readDoubleInput("Nuevo Patrimonio: ");
    printf("Nuevo Cargo Principal: "); readStringInput(cargo_principal, sizeof(cargo_principal));
    printf("Nuevo Nombre del Partido Politico (deje vacio si no pertenece): "); readStringInput(partido_politico_nombre_partido, sizeof(partido_politico_nombre_partido));

    if (strlen(partido_politico_nombre_partido) == 0) {
        snprintf(query, sizeof(query),
                 "UPDATE CIUDADANO SET dni = '%s', nombre_ciudadano = '%s', direccion_ciudadano = '%s', patrimonio = %.2f, cargo_principal = '%s', partido_politico_nombre_partido = NULL WHERE dni = '%s'",
                 dni_new, nombre_ciudadano, direccion_ciudadano, patrimonio, cargo_principal, dni_old);
    } else {
        snprintf(query, sizeof(query),
                 "UPDATE CIUDADANO SET dni = '%s', nombre_ciudadano = '%s', direccion_ciudadano = '%s', patrimonio = %.2f, cargo_principal = '%s', partido_politico_nombre_partido = '%s' WHERE dni = '%s'",
                 dni_new, nombre_ciudadano, direccion_ciudadano, patrimonio, cargo_principal, partido_politico_nombre_partido, dni_old);
    }
    ejecutarConsulta(conn, query);
}

void borrarCiudadano(MYSQL *conn) {
    char dni[MAX_DNI_LEN];
    printf("DNI del Ciudadano a borrar: "); readStringInput(dni, sizeof(dni));
    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query), "DELETE FROM CIUDADANO WHERE dni = '%s'", dni);
    ejecutarConsulta(conn, query);
}

void insertarTelefonoPartido(MYSQL *conn) {
    char numero[MAX_STRING_LEN], descripcion[MAX_STRING_LEN], partido_politico_nombre_partido[MAX_PARTIDO_LEN];
    char query[MAX_QUERY_LEN];

    printf("Numero de Telefono: "); readStringInput(numero, sizeof(numero));
    printf("Descripcion (ej. 'Central', 'Atencion'): "); readStringInput(descripcion, sizeof(descripcion));
    printf("Nombre del Partido Politico al que pertenece: "); readStringInput(partido_politico_nombre_partido, sizeof(partido_politico_nombre_partido));

    snprintf(query, sizeof(query),
             "INSERT INTO TELEFONO_PARTIDO (numero, descripcion, partido_politico_nombre_partido) VALUES ('%s', '%s', '%s')",
             numero, descripcion, partido_politico_nombre_partido);
    ejecutarConsulta(conn, query);
}

void editarTelefonoPartido(MYSQL *conn) {
    int id_telefono = readIntInput("ID del Telefono a editar: ");
    char numero[MAX_STRING_LEN], descripcion[MAX_STRING_LEN], partido_politico_nombre_partido[MAX_PARTIDO_LEN];
    char query[MAX_QUERY_LEN];

    printf("Nuevo Numero de Telefono: "); readStringInput(numero, sizeof(numero));
    printf("Nueva Descripcion: "); readStringInput(descripcion, sizeof(descripcion));
    printf("Nuevo Nombre del Partido Politico al que pertenece: "); readStringInput(partido_politico_nombre_partido, sizeof(partido_politico_nombre_partido));

    snprintf(query, sizeof(query),
             "UPDATE TELEFONO_PARTIDO SET numero = '%s', descripcion = '%s', partido_politico_nombre_partido = '%s' WHERE id_telefono = %d",
             numero, descripcion, partido_politico_nombre_partido, id_telefono);
    ejecutarConsulta(conn, query);
}

void borrarTelefonoPartido(MYSQL *conn) {
    int id_telefono = readIntInput("ID del Telefono a borrar: ");
    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query), "DELETE FROM TELEFONO_PARTIDO WHERE id_telefono = %d", id_telefono);
    ejecutarConsulta(conn, query);
}

void insertarImplicacionCaso(MYSQL *conn) {
    int codigo_caso = readIntInput("Codigo del Caso de Corrupcion: ");
    char ciudadano_dni[MAX_DNI_LEN];
    printf("DNI del Ciudadano Implicado: "); readStringInput(ciudadano_dni, sizeof(ciudadano_dni));
    char query[MAX_QUERY_LEN];

    snprintf(query, sizeof(query),
             "INSERT INTO IMPLICACION_CASO (codigo_caso, ciudadano_dni) VALUES (%d, '%s')",
             codigo_caso, ciudadano_dni);
    ejecutarConsulta(conn, query);
}

void borrarImplicacionCaso(MYSQL *conn) {
    int codigo_caso = readIntInput("Codigo del Caso de Corrupcion (para borrar implicacion): ");
    char ciudadano_dni[MAX_DNI_LEN];
    printf("DNI del Ciudadano Implicado (para borrar implicacion): "); readStringInput(ciudadano_dni, sizeof(ciudadano_dni));
    char query[MAX_QUERY_LEN];

    snprintf(query, sizeof(query),
             "DELETE FROM IMPLICACION_CASO WHERE codigo_caso = %d AND ciudadano_dni = '%s'",
             codigo_caso, ciudadano_dni);
    ejecutarConsulta(conn, query);
}

void insertarPuestoPartido(MYSQL *conn) {
    char ciudadano_dni[MAX_DNI_LEN];
    char partido_politico_nombre_partido[MAX_PARTIDO_LEN];
    char puesto[MAX_STRING_LEN];
    char query[MAX_QUERY_LEN];

    printf("DNI del Ciudadano: "); readStringInput(ciudadano_dni, sizeof(ciudadano_dni));
    printf("Nombre del Partido Politico: "); readStringInput(partido_politico_nombre_partido, sizeof(partido_politico_nombre_partido));
    printf("Puesto en el Partido: "); readStringInput(puesto, sizeof(puesto));

    snprintf(query, sizeof(query),
             "INSERT INTO PUESTO_PARTIDO (ciudadano_dni, partido_politico_nombre_partido, puesto) VALUES ('%s', '%s', '%s')",
             ciudadano_dni, partido_politico_nombre_partido, puesto);
    ejecutarConsulta(conn, query);
}

void editarPuestoPartido(MYSQL *conn) {
    char ciudadano_dni[MAX_DNI_LEN];
    char partido_politico_nombre_partido[MAX_PARTIDO_LEN];
    printf("DNI del Ciudadano (para editar puesto): "); readStringInput(ciudadano_dni, sizeof(ciudadano_dni));
    printf("Nombre del Partido Politico (para editar puesto): "); readStringInput(partido_politico_nombre_partido, sizeof(partido_politico_nombre_partido));
    
    char nuevo_puesto[MAX_STRING_LEN];
    char query[MAX_QUERY_LEN];
    printf("Nuevo Puesto en el Partido: "); readStringInput(nuevo_puesto, sizeof(nuevo_puesto));

    snprintf(query, sizeof(query),
             "UPDATE PUESTO_PARTIDO SET puesto = '%s' WHERE ciudadano_dni = '%s' AND partido_politico_nombre_partido = '%s'",
             nuevo_puesto, ciudadano_dni, partido_politico_nombre_partido);
    ejecutarConsulta(conn, query);
}

void borrarPuestoPartido(MYSQL *conn) {
    char ciudadano_dni[MAX_DNI_LEN];
    char partido_politico_nombre_partido[MAX_PARTIDO_LEN];
    printf("DNI del Ciudadano (para borrar puesto): "); readStringInput(ciudadano_dni, sizeof(ciudadano_dni));
    printf("Nombre del Partido Politico (para borrar puesto): "); readStringInput(partido_politico_nombre_partido, sizeof(partido_politico_nombre_partido));
    char query[MAX_QUERY_LEN];

    snprintf(query, sizeof(query),
             "DELETE FROM PUESTO_PARTIDO WHERE ciudadano_dni = '%s' AND partido_politico_nombre_partido = '%s'",
             ciudadano_dni, partido_politico_nombre_partido);
    ejecutarConsulta(conn, query);
}

void insertarAfinidadPeriodicoPartido(MYSQL *conn) {
    char periodico_nombre_periodico[MAX_PERIODICO_LEN];
    char partido_politico_nombre_partido[MAX_PARTIDO_LEN];
    char query[MAX_QUERY_LEN];

    printf("Nombre del Periodico: "); readStringInput(periodico_nombre_periodico, sizeof(periodico_nombre_periodico));
    printf("Nombre del Partido Politico: "); readStringInput(partido_politico_nombre_partido, sizeof(partido_politico_nombre_partido));

    snprintf(query, sizeof(query),
             "INSERT INTO AFINIDAD_PERIODICO_PARTIDO (periodico_nombre_periodico, partido_politico_nombre_partido) VALUES ('%s', '%s')",
             periodico_nombre_periodico, partido_politico_nombre_partido);
    ejecutarConsulta(conn, query);
}

void borrarAfinidadPeriodicoPartido(MYSQL *conn) {
    char periodico_nombre_periodico[MAX_PERIODICO_LEN];
    char partido_politico_nombre_partido[MAX_PARTIDO_LEN];
    printf("Nombre del Periodico (para borrar afinidad): "); readStringInput(periodico_nombre_periodico, sizeof(periodico_nombre_periodico));
    printf("Nombre del Partido Politico (para borrar afinidad): "); readStringInput(partido_politico_nombre_partido, sizeof(partido_politico_nombre_partido));
    char query[MAX_QUERY_LEN];

    snprintf(query, sizeof(query),
             "DELETE FROM AFINIDAD_PERIODICO_PARTIDO WHERE periodico_nombre_periodico = '%s' AND partido_politico_nombre_partido = '%s'",
             periodico_nombre_periodico, partido_politico_nombre_partido);
    ejecutarConsulta(conn, query);
}

void consulta_jueces_casos(MYSQL *conn) {
    printf("\n--- Consulta: Jueces y Casos de Corrupcion que Investigan ---\n");
    const char *query =
        "SELECT J.nombre_juez AS Juez, CC.nombre_caso AS Caso_Corrupcion, CC.millones_desviados AS Millones_Desviados "
        "FROM JUEZ J JOIN CASO_CORRUPCION CC ON J.idjuez = CC.juez_idjuez "
        "ORDER BY J.nombre_juez, CC.nombre_caso";
    mostrarResultadosConsulta(conn, query);
}

void consulta_ciudadanos_implicados_y_partido(MYSQL *conn) {
    printf("\n--- Consulta: Ciudadanos Implicados y su Afiliacion Politica ---\n");
    const char *query =
        "SELECT C.nombre_ciudadano AS Ciudadano, C.dni AS DNI, C.patrimonio AS Patrimonio, "
        "PP.nombre_partido AS Partido_Politico, C.cargo_principal AS Cargo_Principal "
        "FROM CIUDADANO C "
        "LEFT JOIN PARTIDO_POLITICO PP ON C.partido_politico_nombre_partido = PP.nombre_partido "
        "WHERE C.dni IN (SELECT ciudadano_dni FROM IMPLICACION_CASO) "
        "ORDER BY C.nombre_ciudadano";
    mostrarResultadosConsulta(conn, query);
}

void consulta_periodicos_casos_descubiertos(MYSQL *conn) {
    printf("\n--- Consulta: Periodicos y Cantidad de Casos Descubiertos ---\n");
    const char *query =
        "SELECT P.nombre_periodico AS Periodico, COUNT(CC.codigo_caso) AS Casos_Descubiertos "
        "FROM PERIODICO P "
        "LEFT JOIN CASO_CORRUPCION CC ON P.nombre_periodico = CC.periodico_nombre_periodico "
        "GROUP BY P.nombre_periodico "
        "ORDER BY Casos_Descubiertos DESC, P.nombre_periodico";
    mostrarResultadosConsulta(conn, query);
}

void consulta_casos_por_millones_desviados(MYSQL *conn) {
    double min_millones = readDoubleInput("\n--- Consulta: Casos por Millones Desviados ---\nIngrese la cantidad minima de millones desviados: ");
    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
             "SELECT codigo_caso, nombre_caso, millones_desviados, descripcion "
             "FROM CASO_CORRUPCION WHERE millones_desviados >= %.2f "
             "ORDER BY millones_desviados DESC", min_millones);
    mostrarResultadosConsulta(conn, query);
}

void consulta_partidos_ciudadanos_implicados(MYSQL *conn) {
    printf("\n--- Consulta: Partidos Politicos y Ciudadanos Implicados ---\n");
    const char *query =
        "SELECT PP.nombre_partido AS Partido_Politico, COUNT(C.dni) AS Ciudadanos_Implicados "
        "FROM PARTIDO_POLITICO PP "
        "JOIN CIUDADANO C ON PP.nombre_partido = C.partido_politico_nombre_partido "
        "JOIN IMPLICACION_CASO IC ON C.dni = IC.ciudadano_dni "
        "GROUP BY PP.nombre_partido "
        "ORDER BY Ciudadanos_Implicados DESC, PP.nombre_partido";
    mostrarResultadosConsulta(conn, query);
}

int main() {
    MYSQL *conn = conectarBD();
    if (conn == NULL) {
        return 1;
    }

    int opcion;

    do {
        printf("\n--- MENU PRINCIPAL - CASOS DE CORRUPCION ---\n");
        printf("--- GESTION DE ENTIDADES ---\n");
        printf(" 1. Insertar Juez\n");
        printf(" 2. Editar Juez\n");
        printf(" 3. Borrar Juez\n");
        printf(" 4. Mostrar Jueces\n");
        printf(" 5. Insertar Partido Politico\n");
        printf(" 6. Editar Partido Politico\n");
        printf(" 7. Borrar Partido Politico\n");
        printf(" 8. Mostrar Partidos Politicos\n");
        printf(" 9. Insertar Periodico\n");
        printf("10. Editar Periodico\n");
        printf("11. Borrar Periodico\n");
        printf("12. Mostrar Periodicos\n");
        printf("13. Insertar Caso de Corrupcion\n");
        printf("14. Editar Caso de Corrupcion\n");
        printf("15. Borrar Caso de Corrupcion\n");
        printf("16. Mostrar Casos de Corrupcion\n");
        printf("17. Insertar Ciudadano\n");
        printf("18. Editar Ciudadano\n");
        printf("19. Borrar Ciudadano\n");
        printf("20. Mostrar Ciudadanos\n");
        printf("21. Insertar Telefono de Partido\n");
        printf("22. Editar Telefono de Partido\n");
        printf("23. Borrar Telefono de Partido\n");
        printf("24. Mostrar Telefonos de Partido\n");
        printf("25. Insertar Implicacion en Caso\n");
        printf("26. Borrar Implicacion en Caso\n");
        printf("27. Mostrar Implicaciones en Caso\n");
        printf("28. Insertar Puesto en Partido\n");
        printf("29. Editar Puesto en Partido\n");
        printf("30. Borrar Puesto en Partido\n");
        printf("31. Mostrar Puestos en Partido\n");
        printf("32. Insertar Afinidad Periodico-Partido\n");
        printf("33. Borrar Afinidad Periodico-Partido\n");
        printf("34. Mostrar Afinidad Periodico-Partido\n");
        
        printf("\n--- CONSULTAS PREDEFINIDAS ---\n");
        printf("35. Jueces y Casos de Corrupcion que Investigan\n");
        printf("36. Ciudadanos Implicados y su Afiliacion Politica\n");
        printf("37. Periodicos y Cantidad de Casos Descubiertos\n");
        printf("38. Casos por Millones Desviados (min. X millones)\n");
        printf("39. Partidos Politicos y Ciudadanos Implicados\n");
        
        printf("0. Salir\n");
        printf("Opcion: ");
        scanf("%d", &opcion);
        getchar();

        switch (opcion) {
            case 1: insertarJuez(conn); break;
            case 2: editarJuez(conn); break;
            case 3: borrarJuez(conn); break;
            case 4: mostrarTabla(conn, "JUEZ"); break;
            case 5: insertarPartidoPolitico(conn); break;
            case 6: editarPartidoPolitico(conn); break;
            case 7: borrarPartidoPolitico(conn); break;
            case 8: mostrarTabla(conn, "PARTIDO_POLITICO"); break;
            case 9: insertarPeriodico(conn); break;
            case 10: editarPeriodico(conn); break;
            case 11: borrarPeriodico(conn); break;
            case 12: mostrarTabla(conn, "PERIODICO"); break;
            case 13: insertarCasoCorrupcion(conn); break;
            case 14: editarCasoCorrupcion(conn); break;
            case 15: borrarCasoCorrupcion(conn); break;
            case 16: mostrarTabla(conn, "CASO_CORRUPCION"); break;
            case 17: insertarCiudadano(conn); break;
            case 18: editarCiudadano(conn); break;
            case 19: borrarCiudadano(conn); break;
            case 20: mostrarTabla(conn, "CIUDADANO"); break;
            case 21: insertarTelefonoPartido(conn); break;
            case 22: editarTelefonoPartido(conn); break;
            case 23: borrarTelefonoPartido(conn); break;
            case 24: mostrarTabla(conn, "TELEFONO_PARTIDO"); break;
            case 25: insertarImplicacionCaso(conn); break;
            case 26: borrarImplicacionCaso(conn); break;
            case 27: mostrarTabla(conn, "IMPLICACION_CASO"); break;
            case 28: insertarPuestoPartido(conn); break;
            case 29: editarPuestoPartido(conn); break;
            case 30: borrarPuestoPartido(conn); break;
            case 31: mostrarTabla(conn, "PUESTO_PARTIDO"); break;
            case 32: insertarAfinidadPeriodicoPartido(conn); break;
            case 33: borrarAfinidadPeriodicoPartido(conn); break;
            case 34: mostrarTabla(conn, "AFINIDAD_PERIODICO_PARTIDO"); break;
            
            case 35: consulta_jueces_casos(conn); break;
            case 36: consulta_ciudadanos_implicados_y_partido(conn); break;
            case 37: consulta_periodicos_casos_descubiertos(conn); break;
            case 38: consulta_casos_por_millones_desviados(conn); break;
            case 39: consulta_partidos_ciudadanos_implicados(conn); break;
            
            case 0: printf("Saliendo del programa. Gracias por usar!\n"); break;
            default: printf("Opcion no valida. Por favor, intente de nuevo.\n");
        }
    } while (opcion != 0);

    mysql_close(conn);
    return 0;
}
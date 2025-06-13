-- SQL Script para la Base de Datos "casos_corrupcion"
-- Generado a partir del Modelo EER de MySQL Workbench

-- Deshacer tablas existentes (opcional, para limpieza en pruebas)
DROP TABLE IF EXISTS afinidad_periodico_partido;
DROP TABLE IF EXISTS puesto_partido;
DROP TABLE IF EXISTS implicacion_caso;
DROP TABLE IF EXISTS telefono_partido;
DROP TABLE IF EXISTS caso_corrupcion;
DROP TABLE IF EXISTS ciudadano;
DROP TABLE IF EXISTS partido_politico;
DROP TABLE IF EXISTS periodico;
DROP TABLE IF EXISTS juez;

-- Creación de la base de datos
CREATE DATABASE IF NOT EXISTS casos_corrupcion;
USE casos_corrupcion;

-- Tabla JUEZ
CREATE TABLE JUEZ (
    idjuez INT AUTO_INCREMENT PRIMARY KEY,
    nombre_juez VARCHAR(150) NOT NULL,
    direccion_juez VARCHAR(255),
    fecha_nacimiento DATE,
    fecha_inicio_ejercicio DATE
);

-- Tabla PARTIDO_POLITICO
CREATE TABLE PARTIDO_POLITICO (
    nombre_partido VARCHAR(100) PRIMARY KEY, -- Usamos VARCHAR(100) para nombres de partidos
    direccion_sede VARCHAR(255)
);

-- Tabla PERIODICO
CREATE TABLE PERIODICO (
    nombre_periodico VARCHAR(100) PRIMARY KEY, -- Corregido a VARCHAR como PK
    direccion_periodico VARCHAR(255),
    tirada INT
);

-- Tabla CASO_CORRUPCION
CREATE TABLE CASO_CORRUPCION (
    codigo_caso INT PRIMARY KEY, -- O VARCHAR(20) si el código es alfanumérico
    nombre_caso VARCHAR(100) NOT NULL,
    descripcion LONGTEXT,
    millones_desviados DECIMAL(15,2), -- Corregido a DECIMAL
    dictamen LONGTEXT,
    juez_idjuez INT NOT NULL, -- Clave Foránea de JUEZ
    periodico_nombre_periodico VARCHAR(100) NOT NULL, -- Clave Foránea de PERIODICO
    fecha_descubrimiento DATE NOT NULL, -- Atributo de la relación "descubre"
    FOREIGN KEY (juez_idjuez) REFERENCES JUEZ(idjuez),
    FOREIGN KEY (periodico_nombre_periodico) REFERENCES PERIODICO(nombre_periodico)
);

-- Tabla CIUDADANO
CREATE TABLE CIUDADANO (
    dni VARCHAR(20) PRIMARY KEY, -- Usamos VARCHAR(20) para DNI (puede incluir letras)
    nombre_ciudadano VARCHAR(100) NOT NULL,
    direccion_ciudadano VARCHAR(255),
    patrimonio DECIMAL(15,2),
    cargo_principal VARCHAR(100),
    partido_politico_nombre_partido VARCHAR(100), -- Clave Foránea de PARTIDO_POLITICO, puede ser NULL
    FOREIGN KEY (partido_politico_nombre_partido) REFERENCES PARTIDO_POLITICO(nombre_partido)
);

-- Tabla TELEFONO_PARTIDO
CREATE TABLE TELEFONO_PARTIDO (
    id_telefono INT AUTO_INCREMENT PRIMARY KEY,
    numero VARCHAR(50) NOT NULL,
    descripcion VARCHAR(100),
    partido_politico_nombre_partido VARCHAR(100) NOT NULL, -- Clave Foránea de PARTIDO_POLITICO
    FOREIGN KEY (partido_politico_nombre_partido) REFERENCES PARTIDO_POLITICO(nombre_partido)
);

-- Tabla de Unión: IMPLICACION_CASO (CASO_CORRUPCION - CIUDADANO)
-- Relación N:M "implica"
CREATE TABLE IMPLICACION_CASO (
    codigo_caso INT NOT NULL, -- O VARCHAR(20) si codigo_caso es alfanumérico
    ciudadano_dni VARCHAR(20) NOT NULL,
    PRIMARY KEY (codigo_caso, ciudadano_dni), -- Clave Primaria Compuesta
    FOREIGN KEY (codigo_caso) REFERENCES CASO_CORRUPCION(codigo_caso),
    FOREIGN KEY (ciudadano_dni) REFERENCES CIUDADANO(dni)
);

-- Tabla de Unión: PUESTO_PARTIDO (CIUDADANO - PARTIDO_POLITICO)
-- Relación N:M "desempeña_puesto" (implícita por el atributo puesto)
CREATE TABLE PUESTO_PARTIDO (
    ciudadano_dni VARCHAR(20) NOT NULL,
    partido_politico_nombre_partido VARCHAR(100) NOT NULL,
    puesto VARCHAR(100), -- Atributo de la relación
    PRIMARY KEY (ciudadano_dni, partido_politico_nombre_partido), -- Clave Primaria Compuesta
    FOREIGN KEY (ciudadano_dni) REFERENCES CIUDADANO(dni),
    FOREIGN KEY (partido_politico_nombre_partido) REFERENCES PARTIDO_POLITICO(nombre_partido)
);

-- Tabla de Unión: AFINIDAD_PERIODICO_PARTIDO (PERIODICO - PARTIDO_POLITICO)
-- Relación N:M "tiene_afinidad"
CREATE TABLE AFINIDAD_PERIODICO_PARTIDO (
    periodico_nombre_periodico VARCHAR(100) NOT NULL,
    partido_politico_nombre_partido VARCHAR(100) NOT NULL,
    PRIMARY KEY (periodico_nombre_periodico, partido_politico_nombre_partido), -- Clave Primaria Compuesta
    FOREIGN KEY (periodico_nombre_periodico) REFERENCES PERIODICO(nombre_periodico),
    FOREIGN KEY (partido_politico_nombre_partido) REFERENCES PARTIDO_POLITICO(nombre_partido)
);
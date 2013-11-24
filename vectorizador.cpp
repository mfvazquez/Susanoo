/**
 vectorizador.cpp

 Copyright 2013 Debora Elisa Martin <debbie1new.world@gmail.com>
 Gaston Alberto Martinez <Gaston.martinez.90@gmail.com>
 Matias Fernando Vazquez <mfvazquezfiuba@gmail.com>

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses>
 */

#include "vectorizador.h"

#include <dirent.h>
#include <stddef.h>
#include <algorithm>    // std::sort
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <iterator>
#include <utility>

#include "Porter.h"

//#include "vector_modelo.h"

using std::string;
using std::vector;
using std::map;

using std::sort;

using std::cout;
using std::cerr;
using std::endl;

using std::ifstream;
using std::ofstream;

#define CARPETA_TEMPORAL "./temp"
#define EXTENSION_BASES  ".res"

#define BUFFSIZE 200

void a_minuscula(string& cadena) {
	transform(cadena.begin(), cadena.end(), cadena.begin(), ::tolower);
}

bool es_caracter(char caracter) {
	return (('A' <= caracter and caracter <= 'Z')
			or ('a' <= caracter and caracter <= 'z'));
}

void Vectorizador::contar_palabras(ifstream& arch, map<string, int>& palabras) {
	while (arch.good()) {

		char c = arch.get();
		string res = string("");

		while (arch.good() && es_caracter(c)) {
			res += c;
			c = arch.get();
		}
		if (res.size() < 3)
			continue; //funcion es palabra valida
		//que valida eso y saca palabras
		//cortas tipo if in on out at etc.
		a_minuscula(res);

		int cant = palabras.count(res);
		if (cant == 0) {
			palabras[res] = 1;
			continue;
		}
		palabras[res] = palabras[res] + 1;
	}
}

void Vectorizador::reducir_palabras(map<string, int>& palabras,
		map<string, int>& palabras_reducidas) {
	for (map<string, int>::iterator it = palabras.begin(); it != palabras.end();
			++it) {
		string res;
		if (reduccion_palabras.count(it->first) == 0) {
			char* palabra = (char*) (it->first.c_str());
			stemmer_t* z = create_stemmer();
			stemword(z, palabra, it->first.size());
			res = string(palabra);
			reduccion_palabras[it->first] = res;
		} else {
			res = reduccion_palabras[it->first];
		}
		if (res == " ")
			continue;

		int cant = palabras_reducidas.count(res);
		//FIXME
		if (cant == 0) {
			palabras_reducidas[res] = it->second;
		} else {
			palabras_reducidas[res] += it->second;
		}
	}
}

void Vectorizador::contar(const string& directorio, const string& archivo,
		int numero_archivo) {
	ifstream arch;

	string path_archivo = directorio + "/";
	path_archivo += archivo;

	arch.open(path_archivo.c_str());
	map<string, int> palabras = map<string, int>();

	contar_palabras(arch, palabras);

	map<string, int> palabras_reducidas;

	reducir_palabras(palabras, palabras_reducidas);

	for (map<string, int>::iterator it = palabras_reducidas.begin();
			it != palabras_reducidas.end(); ++it) {
		string res = it->first;

		int cant = palabras_archivos.count(res);

		if (cant == 0) {
			palabras_archivos[res] = vector<int>();
			palabras_archivos[res].push_back(1);
			palabras_archivos[res].push_back(numero_archivo);
			continue;
		} else {
			if (palabras_archivos[res][1] == numero_archivo)
				continue;
			palabras_archivos[res][0]++;
			palabras_archivos[res][1] = numero_archivo;
		}

	}

	ofstream out;
	string path = CARPETA_TEMPORAL;
	path += "/" + archivo + EXTENSION_BASES;
	out.open(path.c_str());
	for (map<string, int>::iterator it = palabras_reducidas.begin();
			it != palabras_reducidas.end(); ++it) {
		out << it->first << "=" << it->second << endl;
	}
	out.close();
}

void Vectorizador::generar_bases(const string& directorio,
		const vector<string>& archivos) {

	/* TODO: agregar al diccionario las stopwords con
	 * reduccion_palabras[<stopwords>] = " ";
	 */

	double completado = 0;
	double porcentaje_por_archivo = 100.0 / archivos.size();
	printf("Iniciando generacion de bases\n");
	for (unsigned int i = 0; i < archivos.size(); i++) {
		contar(directorio, archivos[i], i);
		completado += porcentaje_por_archivo;
		printf("[%3.2f%%] - Generado:%s\n", completado, archivos[i].c_str());
	}

	printf("\n");

#ifdef _DEBUG
	ofstream out;

	string path_salida = CARPETA_TEMPORAL;
	path_salida += "/Total.trs";

	out.open(path_salida.c_str());

	for (map<string, vector<int> >::iterator it = palabras_archivos.begin();
			it != palabras_archivos.end(); ++it) {
		if (it->second[0] < 2)
		continue;
		out << it->first << "=" << it->second[0] << endl;
	}
	out.close();
#endif //_DEBUG

	int i=0;
	for (map<string, vector<int> >::iterator it = palabras_archivos.begin();
			it != palabras_archivos.end(); ++it) {
		if (it->second[0] < 2)
			continue;
		coordenadas_vector[it->first] = i;
		i++;
	}
}

void Vectorizador::obtener_archivos(string directorio,
		vector<string>& archivos) {
	DIR *dp;
	struct dirent *dirp;

	if ((dp = opendir(directorio.c_str())) == NULL) {
		cerr << "Error abriendo " << directorio << endl;
	}

	while ((dirp = readdir(dp)) != NULL) {
		if (dirp->d_name[0] == '.')
			continue;
		archivos.push_back(string(dirp->d_name));
	}
	closedir(dp);
}

/**
 * Genera una carpeta en el sistema para agrupar
 * diferentes archivos intermedios
 */
void Vectorizador::generar_carpeta(const string& path_carpeta) {
	string comando_carpeta_temporal = "mkdir ";
	comando_carpeta_temporal += path_carpeta;
	system(comando_carpeta_temporal.c_str());
}

void Vectorizador::generar_vector(const string& archivo, Vector_Modelo modelo) {
	//----------------Archivo Datos----------------------
	string path_base = CARPETA_TEMPORAL;
	path_base += "/" + archivo + EXTENSION_BASES;

	ifstream arch;
	arch.open(path_base.c_str());
	//----------------Archivo Vector---------------------
	string path_vector = CARPETA_VECTORES;
	path_vector += "/" + archivo + EXTENSION_VECTORES;

	ofstream vect;
	vect.open(path_vector.c_str());
	//----------------------------------------------------

	char buffer[BUFFSIZE];

	while (arch.good()) {
		arch.getline(buffer, BUFFSIZE - 1);
		string datos = string(buffer);

		size_t separador = datos.find("=");
		string clave = datos.substr(0, separador);
		string valor = datos.substr(separador + 1);

		int frecuencia_documento = atoi(valor.c_str());

		if(coordenadas_vector.count(clave) == 0) continue;

		//TODO: pasar a valores normalizados

		//TODO: escribirlo en binario de la forma: 4bytes coordenada,4bytes valor
		vect << coordenadas_vector[clave] << "-" <<frecuencia_documento << endl;
	}

	arch.close();
	vect.close();
}

/**
 * Genera los archivos de vectores que se van a utilizar
 */
void Vectorizador::generar_vectores(const vector<string>& archivos,
		map<string, vector<int> > palabras_archivos) {

	vector<string> coordenadas;
	vector<int> valores;

	for (map<string, vector<int> >::iterator it = palabras_archivos.begin();
			it != palabras_archivos.end(); ++it) {
		if (it->second[0] < 2)
			continue;
		coordenadas.push_back(it->first);
		//TODO: el map esta mal formado y hay que sacar de esta manera el
		//valor
		valores.push_back(it->second[0]);
	}

	Vector_Modelo modelo = Vector_Modelo(coordenadas);

	double completado = 0;
	double porcentaje_por_archivo = 100.0 / archivos.size();
	printf("Iniciando generacion de vectores\n");
	for (size_t i = 0; i < archivos.size(); i++) {
		//TODO: hacer de a mas y con threads
		generar_vector(archivos[i], modelo);
		completado += porcentaje_por_archivo;
		printf("[%3.2f%%] - Generado:%s \n", completado, archivos[i].c_str());
	}
}

/**
 * 	Genera los archivos de vectores a utilizar
 */

vector<string> Vectorizador::vectorizar(const string& directorio) {
	vector<string>();

	obtener_archivos(directorio, archivos);
	sort(archivos.begin(), archivos.end());

	generar_carpeta(CARPETA_TEMPORAL);//TODO: eliminar los datos innecesarios del vector
	generar_bases(directorio, archivos);

	generar_carpeta(CARPETA_VECTORES);
	generar_vectores(archivos, palabras_archivos);

	return archivos;
}

Vectorizador::Vectorizador() {
	archivos = vector<string>();
}

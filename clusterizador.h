/**
 clusterizador.h

 Copyright 2013 Gaston Martinez Gaston.martinez.90@gmail.com

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses
 */
#ifndef CLUSTERIZADOR_H_
#define CLUSTERIZADOR_H_

#include <string>
#include <vector>
#include <math.h>

#include "medidor_distancia.h"

class Clusterizador {

	std::vector<double>* centroides_viejos;
	std::vector<std::string>* clusters_viejos;
	std::vector<double>* centroides_nuevos;
	std::vector<std::string>* clusters_nuevos;

	int clusters;
	std::string carpeta_origen;
	std::vector<std::string> archivos;

private:
	void inicializar();

public:
	Clusterizador(int n_clusters,const std::string& carpeta_vectores, const std::vector<std::string>& archivos);
	virtual ~Clusterizador();
	void hacer_clusters();
};

#endif /* CLUSTERIZADOR_H_ */
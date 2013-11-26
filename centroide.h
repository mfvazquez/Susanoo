/**
 Centroide.h

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
#ifndef CENTROIDE_H_
#define CENTROIDE_H_

#include <map>
#include <vector>

class Centroide {

private:
	std::vector<float> promedios;

	float modulo_cuadrado;
	int vectores_asociados;

public:
	Centroide(int dimensiones, bool random = true);

	/**
	 * Calcula el coseno contra un vector representado por un map
	 * (el vector esta normalizado)
	 */
	float calcular_coseno(std::map<int, float> vector_reducido);

	/**
	 * Calcula el coseno contra otro centroide
	 * (ambos centroides deben estar normalizados)
	 */
	float calcular_coseno(Centroide otro_centroide);

	void agregar_vector(std::map<int, float> vector_reducido);
	void normalizar();
};

#endif /* CENTROIDE_H_ */

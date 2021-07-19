/*
*	Robin LEMAITRE et Rayan KHEMMAR - Génération de terrains - LIFPROJET EG4
*/

#include "Perlin.hpp"

/// <summary>
/// Permet de gérer la génération d'un terrain et d'une image avec le bruit de perlin
/// </summary>
Perlin::Perlin()
{
	//On remplit la table de permutation
	for (int i = 0; i < 512; i++)
		p[i] = permutation[i % 256];
}

/// <summary>
/// Calcule une valeur de bruit pour un point (x, y)
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <returns>valeur de bruit pour le point(x, y)</returns>
float Perlin::noise(float x, float y) {
	//x /= t_size;
	//y /= t_size;

	//On trouve la position dans la table de permutation
	int X = (int)floor(x) & 255, Y = (int)floor(y) & 255;
	int AA = p[p[X] + Y], BA = p[p[X] + Y + 1], AB = p[p[X + 1] + Y], BB = p[p[X + 1] + Y + 1];

	//On transorme x et y en points relatifs (float entre 0 et 1 pour x et pour y)
	x -= (int)floor(x), y -= (int)floor(y);

	float u = fade(x), v = fade(y);

	float gradAA = influence(AA, x, y);
	float gradAB = influence(AB, x - 1, y);
	float gradBA = influence(BA, x, y - 1);
	float gradBB = influence(BB, x - 1, y - 1);

	//std::cout << std::endl << "grads: " << gradAA << " " << gradAB << " " << gradBA << " " << gradBB << std::endl;

	float final_interp;

	bool do_cos_interp = false;

	if (!do_cos_interp) {
		float x1 = line_interp(gradAA, gradAB, u);
		float x2 = line_interp(gradBA, gradBB, u);
		final_interp = line_interp(x1, x2, v);
	}
	else {
		float x1 = cos_interp(gradAA, gradAB, u);
		float x2 = cos_interp(gradBA, gradBB, u);
		final_interp = cos_interp(x1, x2, v);
	}

	//std::cout << "final : " << final_interp << std::endl;

	return (final_interp);
}

/// <summary>
/// renvoit l'influence d'un point hash sur son point relatif (x, y)
/// </summary>
/// <param name="hash">valeur dans la table de permutation</param>
/// <param name="x">position relative en "x" du point</param>
/// <param name="y">position relative en "y" du point</param>
/// <returns>Influence d'un point sur son point relatif</returns>
float Perlin::influence(int hash, float x, float y) {
	//on calcule le gradient, qui peut prendre 4 valeurs possibles, selon  la valeur de hash
	//Comme gradient ne comptient que des 1 ou des -1, on applique directment cela à x et y
	//On peut alors faire le produit scalaire qui consiste, en version "courte", à faire x + y, et on a l'influence
	//std::cout << hash << " : " << ((hash & 1) ? "1" : "-1") << ";" << ((hash & 2) ? "1" : "-1") << std::endl;
	//hash = hash & 15;

	(hash & 1) ? x = x : x = -x;
	(hash & 2) ? y = y : y = -y;

	//std::cout << x + y << std::endl;
	//influence sur le point (x,y);
	return x + y;
}

/// <summary>
/// Interpolation linéaire
/// </summary>
/// <param name="a">influence</param>
/// <param name="b">influence</param>
/// <param name="t">fade</param>
/// <returns>la hauteur finale du point</returns>
float Perlin::line_interp(float a, float b, float t) {
	return a + t * (b - a);
}

/// <summary>
/// Interpolation cosinusoidale
/// </summary>
/// <param name="a">influence</param>
/// <param name="b">influence</param>
/// <param name="t">fade</param>
/// <returns>la hauteur finale du point</returns>
float Perlin::cos_interp(float a, float b, float t) {
	double c = (1 - cos(t * 3.1415927)) * 0.5;

	return (1 - c) * a + c * b;
}

/// <summary>
/// Pour "adoucir" les valeurs entre les points
/// </summary>
/// <param name="t">valeur à adoucir</param>
/// <returns>La valeur adoucie</returns>
float Perlin::fade(float t) {
	return t * t * t * (t * (t * 6 - 15) + 10);
	//return t;
}

/// <summary>
/// Exporte un terrain OBJ ou une image PGM avec le bruit de perlin selon les paramètres définis dans la fonction.
/// </summary>
/// <param name="export_obj">Exporte un terrain OBJ</param>
/// <param name="export_pgm">Exporte une image PGM</param>
/// <param name="path">Chemin du fichier OBJ ou PGM pour créer un OBJ ou PGM depuis ce fichier</param>
void Perlin::exporte(bool export_obj, bool export_pgm, std::string path) {

	int t_size = 50, octaves = 1;
	float min = 0, max = 0, freq = 0.1f, amplMax = 20.0f;

	std::cout << "Generation lancee" << std::endl << std::endl;

	if (path.empty()) {
		int rec = 1;
		std::cout << "Paramètres recommandés ? (0 / 1) : ";
		do {
			std::cin >> rec;
		} while (rec != 0 && rec != 1);
		if (rec == 0) {
			std::cout << "Taille du terrain : ";
			std::cin >> t_size;
			std::cout << "Frequence : ";
			std::cin >> freq;
			std::cout << "amplitude max : ";
			std::cin >> amplMax;
			std::cout << "octaves (fbm) : ";
			std::cin >> octaves;
		}
		else {
			t_size = 300;
			freq = 0.01;
			amplMax = 110;
			octaves = 6;

			if (export_pgm)
				amplMax = 255;
		}
	}

	std::ofstream obj_perlin;
	std::ofstream pgm_perlin;

	//Si on crée des objets à partir d'autres objets
	std::ifstream pgm_copy, obj_copy;

	if (export_obj) {
		obj_perlin.open("TerrainPerlin.OBJ");
		if (!obj_perlin)
			std::cerr << "Erreur d'ouverture du fichier" << std::endl;
		obj_perlin << "#test terrain" << std::endl << std::endl;
		obj_perlin << "o terrain" << std::endl << std::endl;
		obj_perlin << std::endl;
	}

	if (export_pgm) {
		pgm_perlin.open("ImagePerlin.PGM");
		if (!pgm_perlin)
			std::cerr << "Erreur d'ouverture du fichier" << std::endl;
		pgm_perlin << "P2" << std::endl;
		pgm_perlin << t_size << " " << t_size << std::endl << (int)amplMax << std::endl;
	}

	if (path.empty()) {
		//D'abord, on écrit les coordonnées et les hauteurs correspondantes
		std::cout << "\x1B[97m(terrain de taille " << t_size << " x " << t_size << ")\x1B[0m" << std::endl << std::endl;
		std::cout << "\x1B[34mRecuperation des valeurs de bruit... \x1B[0m";

		for (int y = 0; y < t_size; y++) {
			for (int x = 0; x < t_size; x++) {
				float nx = x * freq;
				float ny = y * freq;

				// Pour plus de simplicité (et également pour avoir des pixels entiers) on cast en int
				// (les valeurs ne valent pas 0 ou 1 car on multiplie par amplMax avant de caster)
				float pnoise = 0;
				float persistance = 0.6f, pers = 1.0f;
				float lac = 2.0f;
				float ampl = amplMax;
				//float pnoise = (noise(nx, ny) + 1) * amplMax * 0.5;
				//int pnoise = 0;

				for (int i = 1; i <= octaves; i++) {
					pnoise += ((noise(nx, ny) + 1) * ampl * 0.5) * pers;
					nx *= lac;
					ny *= lac;
					pers *= persistance;
					ampl *= 0.5;
				}

				pnoise = floor(pnoise);

				//debug pour vérifier valeur min max
				pnoise > max ? max = pnoise : max = max;
				pnoise < min ? min = pnoise : min = min;

				//Pour export l'image
				if (export_pgm)
					pgm_perlin << pnoise << " ";

				//Pour export l'object 3D
				if (export_obj) {
					float color_id = 0;
					if (pnoise > amplMax * 0.95) {
						color_id = 1;
						float prob = (-(pnoise - amplMax * 0.95)) / (amplMax - amplMax * 0.95) + 1;
						int r = rand() % 10;
						r *= prob;
						if (r > 5)
							color_id = 2;
					}
					else if (pnoise > amplMax * 0.85) {
						color_id = 2;
						float prob = (-(pnoise - amplMax * 0.85)) / (amplMax * 0.95 - amplMax * 0.85) + 1;
						int r = rand() % 10;
						r *= prob;
						if (r > 5)
							color_id = 3;
						prob = 1 - prob;
						r = rand() % 10;
						r *= prob;
						if (r > 5)
							color_id = 1;
					}
					else {
						color_id = 3;
						float prob = 1 - (-(pnoise - amplMax * 0.85)) / (amplMax * 0.95 - amplMax * 0.85) + 1;
						int r = rand() % 10;
						r *= prob;
						if (r > 5)
							color_id = 2;
					}
						

					obj_perlin << "v " << x << " " << y << " " << pnoise << " ";
					if (color_id == 1)
						obj_perlin << "1 1 1" << std::endl;
					else if (color_id == 2)
						obj_perlin << "0.549 0.568 0.549" << std::endl;
					else
						obj_perlin << "0.247 0.560 0.305" << std::endl;
				}
					
			}

			if (export_obj)
				pgm_perlin << std::endl;
		}

		std::string c = "0.235 0.372 0.549";
		int alt = amplMax * 0.6;
		//60, 95, 140

		obj_perlin << "v 0 0 " << alt << " " << c << std::endl;
		obj_perlin << "v " << t_size - 1 << " 0 " << alt << " " << c << std::endl;
		obj_perlin << "v 0 " << t_size - 1 << " " << alt << " " << c << std::endl;
		obj_perlin << "v " << t_size - 1 << " " << t_size - 1 << " " << alt << " " << c << std::endl;

		std::cout << "\t\t\x1B[32m[OK]\x1B[0m" << std::endl;
		std::cout << "\x1B[97m(hauteur min : " << min << " max : " << max << ")\x1B[0m" << std::endl << std::endl;
	}
	else {
		if (export_obj) {
			//On crée un terrain OBJ à partir d'une image PGM

			std::cout << "Quelle hauteur max souhaitez-vous ?" << std::endl;
			std::cin >> amplMax;
			int max_pgm_value = 0;	//valeur maximale du fichier PGM (qui sera ensuite réduite à amplMax)

			std::cout << "\x1B[34mRecuperation des valeurs de bruit dans le fichier PGM... \x1B[0m";
			pgm_copy.open(path.c_str());
			if (pgm_copy) {
				std::string temp = "";
				int wordcount = 0;
				while (pgm_copy >> temp) {
					if (wordcount == 1)
						t_size = std::stof(temp);
					if (wordcount == 2)
						max_pgm_value = std::stoi(temp);
					if (wordcount > 2) {
						for (int y = 0; y < t_size; y++)
							for (int x = 0; x < t_size; x++) {
								pgm_copy >> temp;
								obj_perlin << "v " << x << " " << y << " " << (std::stoi(temp) * amplMax) / max_pgm_value << std::endl;

								temp = "";
							}
					}

					temp = "";
					wordcount++;
				}

				std::cout << "\t\t\x1B[32m[OK]\x1B[0m" << std::endl;
			}
			else
				std::cerr << "Erreur ouverture fichier PGM source" << std::endl;
		}

		else if (export_pgm) {
			//On crée une image PGM à partir d'un terrain OBJ

			/*amplMax = 255;

			std::cout << "\x1B[34mRecuperation des valeurs de bruit dans le fichier OBJ... \x1B[0m";
			obj_copy.open(path.c_str());
			if (pgm_copy) {
				std::string temp = "";
				int wordcount = 0;
				while (pgm_copy >> temp) {
					if (wordcount == 1)
						t_size = std::stof(temp);
					if (wordcount == 2)
						max_pgm_value = std::stoi(temp);
					if (wordcount > 2) {
						for (int y = 0; y < t_size; y++)
							for (int x = 0; x < t_size; x++) {
								pgm_copy >> temp;
								obj_perlin << "v " << x << " " << y << " " << (std::stoi(temp) * amplMax) / max_pgm_value << std::endl;

								temp = "";
							}
					}

					temp = "";
					wordcount++;
				}

				std::cout << "\t\t\x1B[32m[OK]\x1B[0m" << std::endl;
			}
			else
				std::cerr << "Erreur ouverture fichier PGM source" << std::endl;*/

			std::cout << "Erreur : a faire !" << std::endl;
		}
	}

	if (export_obj) {
		std::cout << "\x1B[34mLiaison des points (triangles)... \x1B[0m";

		//relier les poitns entre eux
		int voisin1, voisin2;

		obj_perlin << std::endl;

		int indice = 0;
		for (int y = 0; y < t_size; y++) {
			for (int x = 0; x < t_size; x++) {
				indice++;
				//triangle 1 2 4
				if (x < t_size - 1 && y < t_size - 1) {
					voisin1 = indice + 1;
					voisin2 = indice + t_size + 1;
					obj_perlin << "f " << indice << " " << voisin1 << " " << voisin2 << std::endl;

					voisin1 = indice + t_size;
					voisin2 = indice + t_size + 1;
					obj_perlin << "f " << indice << " " << voisin1 << " " << voisin2 << std::endl;
				}
			}
		}
		//lacs
		indice++;
		obj_perlin << "f " << indice << " " << indice + 1 << " " << indice + 3 << std::endl;
		obj_perlin << "f " << indice << " " << indice + 2 << " " << indice + 3 << std::endl;

		std::cout << "\t\t\x1B[32m[OK]\x1B[0m" << std::endl << std::endl;
	}

	std::cout << "\x1B[32mFin\x1B[0m : fichier " << (export_obj ? "OBJ" : "") << (export_pgm ? "PGM" : "") << " exporte." << std::endl;
}
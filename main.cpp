/*
*	Robin LEMAITRE et Rayan KHEMMAR - Génération de terrains - LIFPROJET EG4
*/

#include "Perlin.hpp"

void ExportOBJ(std::string path = "") {
	Perlin perlin;
	perlin.exporte(true, false, path);
}

void ExportPGM(std::string path = "") {
	Perlin perlin;
	perlin.exporte(false, true, path);
}

int main()
{
	srand(time(NULL));

	std::cout << "Perlin Noise - Console avant Qt\n\tRayan KHEMMAR et Robin LEMAITRE" << std::endl;

	//Un beau menu console avant le passage a Qt
	bool menu = true, exit = false;
	int menu_choice = 0;
	while (!exit) {
		std::cout << "MENU" << std::endl;
		std::cout << "1.\tExporter un terrain OBJ" << std::endl;
		std::cout << "2.\tExporter un terrain OBJ a partir d'une image PGM" << std::endl;
		std::cout << "3.\tExporter une image PGM" << std::endl;
		std::cout << "(4.\tExporter une image PGM a partir d'un terrain OBJ)" << std::endl;
		std::cout << "5.\tQuitter" << std::endl;
		while (menu) {
			std::cin >> menu_choice;
			std::string path = "";
			switch (menu_choice)
			{
			case 1:
				ExportOBJ();
				break;
			case 2:
				std::cout << "Chemin de l'image PGM :" << std::endl;
				std::cin >> path;
				ExportOBJ(path);
				break;
			case 3:
				ExportPGM();
				break;
			case 4:
				std::cout << "Chemin du terrain OBJ :" << std::endl;
				std::cin >> path;
				ExportPGM(path);
				break;
			case 5:
				menu = false;
				exit = true;
				break;
			default:
				std::cout << "Entrez une valeur entre 1 et 5." << std::endl;
				break;
			}
		}
	}

	return 0;
}
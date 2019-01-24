#pragma once
#include <bitset>
#include <stdio.h>
#include <iostream>
#include <vector>
#include "kernel.h"
#include <chrono>
#include "BitMapSaver.h"

using namespace std;


void evolve(unsigned char *matrix, unsigned char *previousRow,unsigned char RULE, unsigned int DIMENSION) {

	matrix[0] = (RULE & 1 << (7 & (previousRow[1] << 2 | previousRow[0] << 1 | previousRow[DIMENSION - 1]))) ? 1 : 0;
	matrix[DIMENSION-1] = (RULE & 1 << (7 & (previousRow[0] << 2 | previousRow[DIMENSION - 1] << 1 | previousRow[DIMENSION - 2]))) ? 1 : 0;
	for (unsigned int i = 1; i < DIMENSION - 1; i++) {
		matrix[i] = (RULE & 1 << (7 & (previousRow[i + 1] << 2 | previousRow[i] << 1 | previousRow[i - 1]))) ? 1 : 0;
	}
}

void processCPU(unsigned char RULE, unsigned int DIMENSION, unsigned int ITERATION, bool isImage) {
	//INICJOWANIE MACIERZY WYNIKOWEJ
	unsigned char *matrix = new unsigned char[DIMENSION*ITERATION];
	for (unsigned int i = 0; i < DIMENSION*ITERATION; i++)
		matrix[i] = 0;
	matrix[DIMENSION / 2] = 1;

	auto chrono_time_start = chrono::system_clock::now();

	for (unsigned int i = 1; i < ITERATION; i++) {
		evolve(&matrix[i*DIMENSION], &matrix[(i - 1)*DIMENSION], RULE, DIMENSION);
	}

	auto chrono_time_end = chrono::system_clock::now();

	if (isImage) {
		BitMapSaver::SaveBitmap(matrix, DIMENSION, ITERATION, "CPUImageResult");
	}

	auto elapsed = chrono::duration_cast<chrono::milliseconds>(chrono_time_end - chrono_time_start);
	auto elapsedMicro = chrono::duration_cast<chrono::microseconds>(chrono_time_end - chrono_time_start);
	cout << ">CPU< Czas chrono: " << elapsed.count() << "milisekund |  "<< elapsedMicro.count() << " microsekund."<< endl;

	delete matrix;
}


int main() {

	bool isGPU = false;
	bool isGenerateImage = false;
	unsigned char RULE = 90;
	unsigned int DIMENSION = 1080;
	unsigned int ITERATION = 720;
	kernel kernel;

	//INICJOWANIE WARTOSCI
	string string_input;
	do {
		cout << "Wprowadz zasade (1-255 // Domyslne: 90) >>";
		getline(cin, string_input);
		cout << endl;
		if (string_input.empty())
			break;
	} while (stoi(string_input) > 255 || stoi(string_input) < 2);

	if (!string_input.empty())
	RULE =(unsigned char) stoi(string_input);

	string_input = "";
		cout << "Wprowadz szerokosc macierzy (Domyslne: 1080 if input = 0 || ENTER) >>";
		getline(cin, string_input);
		cout << endl;
		if(!string_input.empty())
			if(stoi(string_input) != 0) 
				DIMENSION = (unsigned int)stoi(string_input);
		string_input = "";

		cout << "Wprowadz ilosc iteracji (Domyslne: 720 if input = 0 || ENTER) >>";
		getline(cin, string_input);
		cout << endl;

		if (!string_input.empty())
			if (stoi(string_input) != 0)
				ITERATION = (unsigned int)stoi(string_input);
		string_input = "";

		do {
			cout << "Wybierz urzadzenie wykonawcze" << endl;
			cout << "CPU >> 1 || ENTER" << endl;
			cout << "GPU >> 2" << endl;
			cout << ">> ";
			getline(cin, string_input);
			cout << endl;
			if (string_input.length() == 0)
				break;

			if (string_input.length() > 1)
				string_input = string_input[0];
			if (!isdigit((char)string_input[0]))
				string_input ="3";
			cout << (stoi(string_input) == 2) << endl;
		} while (!(stoi(string_input)==1 || stoi(string_input) == 2));

		if(string_input.length() == 1)
			if(stoi(string_input) == 2)
		isGPU = (bool)stoi(string_input);

		string_input = "";

		do {
			cout << "Czy generowac bitmape ?" << endl;
			cout << "1.TAK" << endl;
			cout << "2.NIE (Domyslne)" << endl;
			cout << ">> ";
			getline(cin, string_input);
			cout << endl;
			if (string_input.length() == 0)
				break;

			if (string_input.length() > 1)
				string_input = string_input[0];
			if (!isdigit((char)string_input[0]))
				string_input = "3";
			cout << (stoi(string_input) == 2) << endl;
		} while (!(stoi(string_input) == 1 || stoi(string_input) == 2));

		if (string_input.length() == 1) {
			if (string_input == "1") {
				isGenerateImage = 1;
			}
			else
			{
				isGenerateImage = 0;
			}
		}

		cout<<">>>>KONFIGURACJA<<<<<<" << endl
			<< "IsGPU: " << isGPU << endl
			<< "RULE: " <<(int)RULE << endl
			<< "Iteration: " << ITERATION << endl
			<< "Dimension: " << DIMENSION << endl
			<< "IsImage: " << isGenerateImage << endl;
		system("pause");
///////////////////////KONIEC INICJOWANIA WARTOSCI\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

		if (isGPU) {
			kernel.printCUDA(isGenerateImage,RULE, ITERATION, DIMENSION);
		}
		else {
			processCPU(RULE, DIMENSION, ITERATION, isGenerateImage);
		}

		system("pause");	

	return 0;
}
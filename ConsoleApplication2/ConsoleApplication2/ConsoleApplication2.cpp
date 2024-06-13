#include "pch.h"

#define _CLR_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <malloc.h>
#include <windows.h>
#include <string>
using namespace std;
using namespace System;
using namespace System::IO;
#define ENTER 13
#define ESC 27
#define UP 72
#define DOWN 80
#define HOME 71
#define END 79




struct services {
	char service[20];
	char errorOccureTime[5];
	long errorCode;
	int serviceDownPeriod;
	int serviceRepairPeriod;
};
struct alphabetList {
	char service[20];
	int serviceDownPeriod;
	struct alphabetList* sled;
	struct alphabetList* pred;
};

int menu(int);
void LongestDownTime(struct services*, int);
void DiffServicesWSameError(struct services*, int);
void Alphabet(struct services*, int, struct alphabetList**);
void LeastRepairTimeWArgument(struct services*, long errorCode, int);
void EveryServiceUntillTime(struct services*, char errorOccureTime[5], int);
void Diagram(struct services*, int, struct alphabetList**);
void DiffServicesSameError(struct services*, int);
void vstavka(struct services* , char*, int, struct alphabetList**);

int main()
{
	int chisloStrok;
	char menuList[][150] = {
		"Самый долгий по простою сервис                          ",
		"Все сервисы по алфавиту                                 ",
		"Самое маленькое время исправления по определенной ошибке",
		"Все сервисы с ошибкой до пределенного времени           ",
		"Разные сервисы с определенной ошибкой                   ",
		"Диаграмма общего веремени простоя сервисов              ",
		"Выход                                                   "
	};
	char BlankLine[] = "                                                          ";
	system("chcp 1251");
	int i, menuLenght;
	FILE* inputFile;
	struct services* service;
	Console::CursorVisible::set(false);
	Console::BufferHeight = Console::WindowHeight;
	Console::BufferWidth = Console::WindowWidth;
	if ((inputFile = fopen("errors.txt", "r")) == NULL)
	{
		printf("\nФайл errors.txt не открыт !");
		getch(); exit(1);
	}
	fscanf(inputFile, "%d", &chisloStrok);
	service = (struct services*)malloc(chisloStrok * sizeof(struct services));
	for (i = 0; i < chisloStrok; i++)
		fscanf(inputFile, "%s%s%ld%d%d", service[i].service, service[i].errorOccureTime,
			&service[i].errorCode, &service[i].serviceDownPeriod, &service[i].serviceRepairPeriod);
	for (i = 0; i < chisloStrok; i++)
		printf("\n%-10s %-10s %7ld %7d %ld", service[i].service, service[i].errorOccureTime,
			service[i].errorCode, service[i].serviceDownPeriod, service[i].serviceRepairPeriod);
	getch();
	struct alphabetList* spisok = 0;
	while (true)
	{
		Console::ForegroundColor = ConsoleColor::White;
		Console::BackgroundColor = ConsoleColor::DarkGreen;
		Console::Clear();
		Console::ForegroundColor = ConsoleColor::White;
		Console::BackgroundColor = ConsoleColor::DarkCyan;
		Console::CursorLeft = 10;
		Console::CursorTop = 4;
		printf(BlankLine);
		for (i = 0; i < 7; i++)
		{
			Console::CursorLeft = 10;
			Console::CursorTop = i + 5;
			printf(" %s ", menuList[i]);
		}
		Console::CursorLeft = 10;
		Console::CursorTop = 12;
		printf(BlankLine);

		char tmpCharInput[20];
		menuLenght = menu(7);
		switch (menuLenght) {
		case 1: {
			Console::CursorTop = 14;
			Console::CursorLeft = 11;
			LongestDownTime(service, chisloStrok); break;
		}
		case 2: Alphabet(service, chisloStrok, &spisok); break;
		case 3: {
			Console::CursorTop = 14;
			Console::CursorLeft = 11;
			long tmpCode;
			printf("Введите номер ошибки: ");
			scanf("%ld", &tmpCode);
			LeastRepairTimeWArgument(service, tmpCode, chisloStrok); break;
		}
		case 4: {
			Console::CursorTop = 14;
			Console::CursorLeft = 11;
			char tmpTime[5];
			printf("Введите верхнюю цену: ");
			scanf("%s", tmpTime);
			EveryServiceUntillTime(service, tmpTime, chisloStrok); break;
		}
		case 5: DiffServicesSameError(service, chisloStrok); break;
		case 6: Diagram(service, chisloStrok, &spisok); break;
		case 7: exit(0);
		}
	} // конец while(1)...
	return 0;
} // конец main()

int menu(int n)
{
	char menuList[][150] = {
		"Самый долгий по простою сервис                          ",
		"Все сервисы по алфавиту                                 ",
		"Самое маленькое время исправления по определенной ошибке",
		"Все сервисы с ошибкой до пределенного времени           ",
		"Разные сервисы с определенной ошибкой                   ",
		"Диаграмма общего веремени простоя сервисов              ",
		"Выход                                                   "
	};

	int y1 = 0, y2 = n - 1;
	char c = 1;
	while (c != ESC)
	{
		switch (c) {
		case DOWN: y2 = y1; y1++; break;
		case UP: y2 = y1; y1--; break;
		case HOME: y2 = y1; y1 = 0; break;
		case END: y2 = y1; y1 = 6; break;
		case ENTER: return y1 + 1;
		}
		if (y1 > n - 1) { y2 = n - 1; y1 = 0; }
		if (y1 < 0) { y2 = 0; y1 = n - 1; }
		Console::ForegroundColor = ConsoleColor::Black;
		Console::BackgroundColor = ConsoleColor::DarkRed;
		Console::CursorLeft = 11;
		Console::CursorTop = y1 + 5;
		printf("%s", menuList[y1]);
		Console::ForegroundColor = ConsoleColor::White;
		Console::BackgroundColor = ConsoleColor::DarkCyan;
		Console::CursorLeft = 11;
		Console::CursorTop = y2 + 5;
		printf("%s", menuList[y2]);
		c = getch();
	} // конец while(c!=ESC)...
	exit(0);
}

void LongestDownTime(struct services* service, int chisloStrok) {
	Console::ForegroundColor = ConsoleColor::White;
	Console::BackgroundColor = ConsoleColor::DarkGreen;
	Console::CursorTop = 16;
	Console::CursorLeft = 11;
	char tmp[20], tmp2[20], tmp3[20];
	strcpy(tmp3, "с молоком ");
	long tmpSum = service[0].serviceDownPeriod;
	strcpy(tmp, service[0].service);
	strcpy(tmp2, service[0].errorOccureTime);

	for (int i = 1; i < chisloStrok; i++) {
		if (service[i].serviceDownPeriod > tmpSum) {
			strcpy(tmp, service[i].service);
			strcpy(tmp2, service[i].errorOccureTime);
			tmpSum = service[i].serviceDownPeriod;
		}
	}
	printf("Самый по простою сервис %s не работал в %s %d минут", tmp, tmp2, tmpSum);
	getch();
}

void vstavka(struct services* service, char* name, int chisloStrok, struct alphabetList** spisok)
{
	int i;
	struct alphabetList* nov, * nt, * z = 0;
	for (nt = *spisok; nt != 0 && strcmp(nt->service, name) < 0; z = nt, nt = nt->sled);
	if (nt && strcmp(nt->service, name) == 0) return;
	nov = (struct alphabetList*)malloc(sizeof(struct alphabetList));
	strcpy(nov->service, name);
	nov->sled = nt;
	nov->pred = z;
	nov->serviceDownPeriod = 0;
	int tmp;
	int tmpi = 0;
	for (i = 0; i < chisloStrok; i++) {
		if (strcmp(service[i].service, name) == 0) {
			nov->serviceDownPeriod += service[i].serviceDownPeriod;
			tmpi++;
		}
	}

	if (!z)
		*spisok = nov;
	else
		z->sled = nov;
	if (nt)
		nt->pred = nov;
	return;
}


void Alphabet(struct services* service, int chisloStrok, struct alphabetList**spisok)
{
	int i;
	struct alphabetList* nt;
	struct alphabetList* z;
	Console::ForegroundColor = ConsoleColor::White;
	Console::BackgroundColor = ConsoleColor::DarkCyan;
	Console::Clear();
	if (!*spisok) {
		for (i = 0; i < chisloStrok; i++) {
			vstavka(service, service[i].service, chisloStrok, spisok);
		}
	}
	Console::Clear();
	printf("\n Алфавитный список сервисов                           Обратный алфавитный список сервисов");
	printf("\n \n");
	printf(" %-30s %s   %-30s %s", "Сервис", "Общее время простоя", "Сервис", "Общее время простоя");
	printf("\n --------------------------------------------------   --------------------------------------------------");
	for (nt = *spisok, z = 0; nt != 0; z = nt, nt = nt->sled);
	for (nt = z, i = 0; nt != 0; i++, nt = nt->pred)
	{
		printf("\n %s %-30s %d", "                                                    ", nt->service, nt->serviceDownPeriod);
	}
	for (nt = *spisok, i = 0; nt != 0; i++, nt = nt->sled) {
		Console::CursorTop = 4 + i;
		printf("\n %-30s %d", nt->service, nt->serviceDownPeriod);
	}
	getch();
}

void LeastRepairTimeWArgument(struct services* service, long errorCode, int chisloStrok)
{
	Console::ForegroundColor = ConsoleColor::White;
	Console::BackgroundColor = ConsoleColor::DarkGreen;
	int overall = service[0].serviceRepairPeriod;
	long tmpCode = service[0].errorCode;
	char tmp[30], tmp2[5];
	Console::CursorTop = 16;
	Console::CursorLeft = 11;

	for (int i = 0; i < chisloStrok; i++) {
		if ((service[i].serviceRepairPeriod) <= overall && service[i].errorCode == errorCode) {
			strcpy(tmp, service[i].service);
			tmpCode = service[i].errorCode;
			overall = service[i].serviceRepairPeriod;
		}
	}
	printf("Сервис %s с ошибкой номер %ld чинился %d минут", tmp, tmpCode, overall);
	getch();
}

void EveryServiceUntillTime(struct services* service, char errorOccureTime[5], int chisloStrok)
{
	Console::ForegroundColor = ConsoleColor::White;
	Console::BackgroundColor = ConsoleColor::DarkGreen;
	Console::CursorTop = 16;
	Console::CursorLeft = 11;
	int overallHour, overallMinute, hour, minute;
	char* tmpHour, * tmpMinute;
	tmpHour = strtok(errorOccureTime, ":");
	tmpMinute = strtok(NULL, ":");
	overallHour = stoi(tmpHour);
	overallMinute = stoi(tmpMinute);

	char tmp[30], tmp2[30], tmp3[30];
	printf("Все сервисы с ошибками до %s: ", errorOccureTime);
	for (int i = 0; i < chisloStrok; i++) {
		tmpHour = strtok(service[i].errorOccureTime, ":");
		tmpMinute = strtok(NULL, ":");
		hour = stoi(tmpHour);
		minute = stoi(tmpMinute);
		if (hour > overallHour) {
			continue;
		}
		if (hour == overallHour && minute > overallMinute) {
			continue;
		}
		printf("%s №%ld, ", service[i].service, service[i].errorCode);
	}
	getch();
}

void Diagram(struct services* service, int chisloStrok, struct alphabetList**spisok)
{
	struct alphabetList* nt;
	int len, i, NColor;
	long sum = 0;
	char str1[20];
	char str2[20];
	System::ConsoleColor Color;
	Console::ForegroundColor = ConsoleColor::Black;
	Console::BackgroundColor = ConsoleColor::White;
	Console::Clear();
	if (!*spisok)
		for (i = 0; i < chisloStrok; i++)
			vstavka(service, service[i].service, chisloStrok);
	for (nt = *spisok, i = 0; nt != 0; nt = nt->sled, i++)
	{
		sum += nt->serviceDownPeriod;
	}
	if (!*spisok)
		for (i = 0; i < chisloStrok; i++)
			vstavka(service, service[i].service, chisloStrok);
	Color = ConsoleColor::Black; NColor = 0;
	for (nt = *spisok, i = 0; nt != 0; nt = nt->sled, i++)
	{
		sprintf(str1, "%s", nt->service);
		sprintf(str2, "%3.1f%%", (nt->serviceDownPeriod * 100. / sum));
		Console::ForegroundColor = ConsoleColor::White;
		Console::BackgroundColor = ConsoleColor::DarkCyan;
		Console::CursorLeft = 5; Console::CursorTop = i + 1;
		printf(str1);
		Console::CursorLeft = 20;
		printf(" % s", str2);
		Console::BackgroundColor = ++Color; NColor++;
		Console::CursorLeft = 30;
		for (len = 0; len < nt->serviceDownPeriod * 100 / sum; len++) printf(" ");
		if (NColor == 14)
		{
			Color = ConsoleColor::Black; NColor = 0;
		}
	}
	getch();
	return;
}

void DiffServicesSameError(struct services* service, int chisloStrok) {
	bool check = false;
	Console::BackgroundColor = ConsoleColor::DarkGreen;
	Console::CursorTop = 16;
	Console::CursorLeft = 11;
	printf("Одинаковый код ошибки: ");
	for (int i = 0; i < chisloStrok; i++) {
		for (int j = i + 1; j < chisloStrok; j++) {
			if ((service[i].errorCode == service[j].errorCode) && strcmp(service[i].service, service[j].service) != 0) {
				Console::CursorTop = 17;
				Console::CursorLeft = 11;
				printf("Сервис: %s Код: %ld", service[i].service, service[i].errorCode);
				Console::CursorTop = 18;
				Console::CursorLeft = 11;
				printf("Сервис: %s Код: %ld", service[j].service, service[j].errorCode);
				check = true;
				break;
			}
		}
		if (check) break;
	}
	if (!check) printf("Нет совпадений");
	getch();
}
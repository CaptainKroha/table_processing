#include <iostream>
#include <string>
#include <Windows.h>
#include <conio.h>
#include <regex>
#include <iomanip>
#include <stdio.h>
#include <ctime>
#define N 15


using namespace std;
//Получение дескриптора командной строки
HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

typedef struct date{int day, month, year;} sDATE; 
typedef struct product_for_save{ 
	char name[N]; 
	sDATE cr_date; 
	int exp_time; 
	char creator[N]; 
	float price;
} sSAVE;

struct product{
	int id; //id
	string name; // название продукта
	sDATE cr_date; // дата изготовления
	int exp_time; // срок годности
	string creator; // изгтовитель
	float price; // цена
};

const int p_size = sizeof(sSAVE), ESC = 27, ENTER = 13, BACKSPACE = 8, A_UP = 72, A_DOWN = 80, A_RIGHT = 77, A_LEFT = 75,
DEL = 83; 

struct my_list{
	product data; // информационное поле
	my_list *prev, *next; // указатели на предыдущий и следующий элемент
};

bool is_list_changed = true;
//----считывание данных---
string get_str();
sDATE get_date();
int get_int();
float get_float();
product get_data();
string get_filename(const char i);
int get_id(my_list* end);
//------------------------
my_list* init_table(my_list *beg, my_list **end); // инициализация таблицы(списка)
my_list* output_table(my_list *beg, my_list **end, int counter); // вывод таблицы на экран
my_list* add_item(my_list *end);
my_list* delete_item(my_list *beg, my_list **end, int note_to_delete = 0);
void item_correction(my_list *beg, my_list *end);
my_list* sort_list(my_list *beg, my_list *end);
void save_list_to_file(my_list *beg);
my_list* get_list_from_txt_file(my_list *beg, my_list **end);
my_list* get_list_from_dat_file(my_list *beg, my_list **end);
void search(my_list *beg);
void count_expired(my_list *beg, my_list *end);
my_list* clear_all_data(my_list *beg); // очистка памяти


//Вспомогательные функции
char get_char(char from, char to);
string do_str_date(product data);
void item_print(my_list *temp);
void GoToXY(short x, short y);
void print_head();
void print_fields(product &temp);
void print_footer();
void fill_empty_fields(product *temp_data, int *check);
void ids_correction(my_list *beg);
my_list* cr_list_by_name(my_list *beg, my_list *begl, string str);
my_list* add_item_w_data(my_list *beg, product data);
int print_menu(string menu[], int size, short x = 0, short y = 1);
void ConsoleCursorVisible(bool show);
bool is_expired(product data);
my_list* do_choise_and_delete(my_list *beg, my_list **end, int f_note, int size = 10, short x = 82, short y = 3);
void swap(my_list*, my_list*);
int datecmp(product, product);

int main(){
	ConsoleCursorVisible(false);
	system("chcp 1251>nul");
	SetConsoleTitleW(L"Products list");
	my_list *beg=NULL, *end=NULL;
	beg = init_table(beg,&end);
	if(beg!=NULL) system("pause");
	do{
		system("cls");
		cout<<"Main menu"<<endl;
		string menu[]{"Создание списка", "Вывод  таблицы", "Добавление  записи", "Удаление записи по ID",
		"Изменение содержимого записи по ID", "Сортировка списка", "Сохранение списка в файл",
	 	"Поиск записей по введенному имени продукта", "Подсчет просроченных товаров", "Выход"};
		int choise = print_menu(menu, 10);
		switch(choise){
			case 1:{
				beg = init_table(beg,&end);
				system("pause");
				break;
			}
			case 2:{
				beg = output_table(beg,&end,0);
				break;
			}
			case 3:{
				end = add_item(end);
				system("pause");
				break;
			}
			case 4:{
				beg = delete_item(beg,&end);
				puts("");
				system("pause");
				break;
			}
			case 5:{
				item_correction(beg, end);
				break;
			}
			case 6:{
				beg = sort_list(beg, end);
				system("pause");
				break;
			}
			case 7:{
				save_list_to_file(beg);
				system("pause");
				break;
			}
			case 8:{
				search(beg);
				break;
			}
			case 9:{
				count_expired(beg, end);
				system("pause");
				break;
			}
			case 0: {
				if(beg==NULL) return 0;
				if(is_list_changed){
					cout<<"В списке присутствуют несохраненные изменения.\n"
					"Желаете сохранить список в файл перед выходом?(1 - да, 0 - нет)->";
					switch(get_char('0','1')){
						case '1':{
							save_list_to_file(beg);
							break;
						}
						case '0': break;
					}
				}
				beg = clear_all_data(beg);
				if(beg==NULL) return 0;
				else exit(1);
			}
		}
	}while(1);
	return 0;
}
//-----------функции ввода данных с клавиатуры----------
//--(строка)---
string get_str(){
	string temp_str;
	do{
		//if(cin.peek()=='\n') cin.ignore();
		cin.sync();
		getline(cin, temp_str);
		//проверка
		if(temp_str.length() > N){
		cout<<"Программа принимает названия короче 15 символов, пожалуйста, введите название еще раз только короче:";
		cin.clear();
		continue;
		}
		break;
	}while(1);
	return temp_str;
}
//--(дата)---
sDATE get_date(){
	string temp_str;
	sDATE temp_date;
	regex cr_date_template("(0[1-9]|[12][0-9]|3[01])(\\.)(0[1-9]|1[0-2])(\\.)(19[0-9]{2}|20[0-1][0-9]|202[0-3])");
	cmatch result;
	do{
	cin.sync();
	getline(cin,temp_str);
	// проверка
	if(regex_match(temp_str.c_str(), result, cr_date_template)){
		temp_str = result[1];
		temp_date.day = atoi(temp_str.c_str());
		temp_str = result[3];
		temp_date.month = atoi(temp_str.c_str());
		temp_str = result[5];
		temp_date.year = atoi(temp_str.c_str());
		if(temp_date.month == 2 && temp_date.day >29){
			cout<<"В феврале не может быть больше 29 дней, введите дату еще раз.\n->";
			continue;
		}
		break;
	}
	else{
		cout<<"Введенная вами дата "<<temp_str<<" введена некорректно, введите дату еще раз.\n->";
		cin.clear();
		continue;
	}
	}while(1);
	return temp_date;
}
//--(int)----
int get_int(){
	regex int_templ("\\d+");
	cmatch result;
	string temp_str;
	do{
		cin.sync();
		getline(cin, temp_str);
		if(regex_match(temp_str.c_str(), result, int_templ)) return atoi(temp_str.c_str());
		cout<<"Введено некорректное значение.\n Повторите ввод->";
		cin.clear();
	}while(1);
}
//--(float)---
float get_float(){
	regex float_templ("(\\d+)(\\.\\d{1,6})?");
	cmatch result;
	string temp_str;
	do{
		cin.sync();
		getline(cin, temp_str);
		if(regex_match(temp_str.c_str(), result, float_templ)) return atof(temp_str.c_str());
		cout<<"Введено некорректное значение.\n Повторите ввод->";
		cin.clear();
	}while(1);
}
//---(file_name)----
string get_filename(const char i){
	string temp_str;
	regex txt_templ("\\w+\\.txt"), dat_templ("\\w+\\.dat");
	cmatch result;
	cout<<"Введите название файла вместе с расширением(.txt - текстовый файл|.dat - типизированный файл)->";
	do{
		cin.sync();
		getline(cin, temp_str);
		if(i=='1'&&regex_match(temp_str.c_str(), result, txt_templ)) return temp_str;
		else if(i=='2'&&regex_match(temp_str.c_str(), result, dat_templ)) return temp_str;
		cout<<"Введено некорректное значение.\n Повторите ввод->";
		cin.clear();
	}while(1);
}
//----(id)-----
int get_id(my_list* end){
	int num;
	do{
		cout<<"Введите номер(ID) записи(всего в списке "<<end->data.id<<" записей)->";
		num = get_int();
		if(num > end->data.id || num == 0){
			cout<<"Записи с указанным номером(ID) в списке нет.\n Хотите ввести номер еще раз(нажмите любую клавишу) или выйти в меню(нажмите 0)->";
			switch(getchar()){
				case '0': return 0;
				default: continue;
				}		
		}
		break;
	}while(1);
	return num;
}
//--о продукте--
product get_data(){
	product data;
	string temp_str;
	cout<<"Ввод данных о продукте"<<endl;
	cout<<"Введите название продукта:";
	data.name = get_str();
	//-----------------------
	cout<<"Введите дату производства в формате(ДД.ММ.ГГГГ):";
	data.cr_date = get_date();
	//-----------------------
	cout<<"Введите срок годности(кол-во дней):";
	data.exp_time = get_int();
	//-----------------------
	cout<<"Введите производителя:";
	data.creator = get_str();
	//-----------------------
	cout<<"Введите стоимость:";
	data.price = get_float();
	return data;
}
//-------функция инициализации таблицы------
my_list* init_table(my_list *beg, my_list **end){
	if(beg==NULL){
		cout<<"Информационная база пуста. Предлагаем внести в нее данные одним из указанных способов:"<<endl;
	}
	else{
		cout<<"Таблица уже создана!"<<endl;
		if(is_list_changed) cout<<"В ней есть несохраненные изменения. Рекомендуем выйти в меню и сохранить список в файл."<<endl;
		cout<<"Нажмите ESC для выхода в меню, любую клавишу для создания нового списка.->\n";
		if(getch() == ESC) return beg;
		beg = clear_all_data(beg);
		system("cls");
	}
	string menu[]{"Ручной ввод", "Чтение данных из текстового файла", "Чтение данных из типизированного файла", "Выход в меню"};
	int choise = print_menu(menu, 4);
	int id = 0;
	switch(choise){
		case 1:{
			cout<<"Ручной ввод"<<endl;
			do{
				my_list *temp = new my_list;
				temp->data = get_data();
				temp->data.id = ++id;
				temp->next = NULL;
				if(beg==NULL){
					beg=temp;
					*end=temp;
					temp->prev=NULL;
				}
				else{
					temp->prev=*end;
					(*end)->next=temp;
					*end=temp;
				}
				cout<<"Продолжаем ввод?(0-нет)\n";
			}while(getch()!='0');
			break;
		}
		case 2:{
			puts("Выбран ввод из текстового файла файла");
			beg = get_list_from_txt_file(beg, end);
			break;
		}
		case 3:{
			puts("Выбран ввод из типизированного файла файла");
			beg = get_list_from_dat_file(beg, end);
			break;
		}
		case 0:{
			is_list_changed = false;
			return beg;
		}
	}
	is_list_changed = true;
	return beg;
}
//-----Функция вывода данных таблицы--------
my_list* output_table(my_list *beg,my_list **end,int counter){
	system("cls");
	if(beg==NULL){
		cout<<"Таблица пуста!\n";
		system("pause");
		return beg;
	}
	//вывод шапки
	print_head();
	my_list *temp = beg;
	int c;
	if(counter<0) counter = 0;
	if(counter==0){
		if((*end)->data.id<=10){//всего <10
			//вывод полей
			for(;temp!=NULL;temp=temp->next) print_fields(temp->data);
			//------------
			print_footer();
			cout<<"[ESC] Выход в меню"<<endl<<
			"[DEL] Удалить запись"<<endl;
			do{
				int x = getch();
				if(x==224) x = getch();
				switch(x){
					case DEL:{
						beg = do_choise_and_delete(beg, end, counter+1, (*end)->data.id);
						beg = output_table(beg, end, counter);
						break;
					}
					case ESC: return beg;
					default: continue;
				}
				return beg;
			}while(1);
		}
		else{//первые 10
			//вывод полей
			for(c=1;c<=10;c++,temp=temp->next) print_fields(temp->data);
			//-----------
			print_footer();
			cout<<"[ESC] Выход в меню"<<endl<<
			"[v] Единичный скрол вперед"<<endl<<
			"[>] Скрол 10 записей вперед"<<endl<<
			"[DEL] Удалить запись"<<endl;
			do{
				char x = getch();
				if(x==224) x = getch();
				switch((int)x){
					case A_DOWN:{
						beg = output_table(beg, end, counter+1);
						break;
					}
					case A_RIGHT:{
						beg = output_table(beg, end, counter+10);
						break;
					}
					case DEL:{
						beg = do_choise_and_delete(beg, end, counter+1);
						beg = output_table(beg, end, counter);
						break;
					}					
					case ESC: return beg;
					default: continue;
				}
				return beg;	
			}while(1);

		}
	}
	if(counter>0){
		if((*end)->data.id-counter<=10){ //<10 до конца
			counter=((*end)->data.id-10);
			for(c=1;c<=counter;c++,temp=temp->next);//перевод указателя на counter+1 запись
			//вывод полей
			for(;temp!=NULL;temp=temp->next) print_fields(temp->data);
			//---------
			print_footer();
			cout<<"[ESC] Выход в меню"<<endl<<
			"[/\\] Единичный скрол назад"<<endl<<
			"[<] Скрол 10 записей назад"<<endl<<
			"[DEL] Удалить запись"<<endl;
			do{
				char x = getch();
				if(x==224) x = getch();
				switch((int)x){
					case A_UP:{
						beg = output_table(beg, end, counter-1);
						break;
					}
					case A_LEFT:{
						beg = output_table(beg, end, counter-10);
						break;
					}
					case DEL:{
						beg = do_choise_and_delete(beg, end, counter+1);
						beg = output_table(beg, end, counter);
						break;
					}						
					case ESC: return beg;
					default: continue;
				}
				return beg;
			}while(1);
			
		}
		else{
			for(c=1;c<=counter;c++,temp=temp->next);
			//вывод полей
			for(;c<=counter+10;c++,temp=temp->next) print_fields(temp->data);
			//---------
			print_footer();
			cout<<"[ESC] Выход в меню"<<endl<<
			"[v] Единичный скрол вперед"<<endl<<
			"[>] Скрол 10 записей вперед"<<endl<<
			"[/\\] Единичный скрол назад"<<endl<<
			"[<] Скрол 10 записей назад"<<endl<<
			"[DEL] Удалить запись"<<endl;
			do{
				char x = getch();
				if(x==224) x = getch();
				switch((int)x){
					case A_DOWN:{
						beg = output_table(beg, end, counter+1);
						break;
					}
					case A_RIGHT:{
						beg = output_table(beg, end, counter+10);
						break;
					}
					case A_UP:{
						beg = output_table(beg, end, counter-1);
						break;
					}
					case A_LEFT:{
						beg = output_table(beg, end, counter-10);
						break;
					}
					case DEL:{
						beg = do_choise_and_delete(beg, end, counter+1);
						beg = output_table(beg, end, counter);
						break;
					}					
					case ESC: return beg;
					default: continue;
				}
				return beg;
			}while(1);
		}
	}
	return beg;
}
//-----Фукнция добавления элемента в таблицу-----
my_list* add_item(my_list *end){
	if(end==NULL) {
		cout<<"Список пуст\n";
		return end;
	}
	my_list *temp = new my_list;
	temp->data = get_data();
	temp->data.id = end->data.id+1;
	temp->next = NULL;
	temp->prev = end;
	end->next = temp;
	end = temp;
	is_list_changed = true;
	return end;
}
//
my_list* add_item_w_data(my_list *beg, product data){
	my_list *temp = new my_list;
	temp->data = data;
	temp->next = beg;
	if(beg) beg->prev = temp;
	beg = temp;
	return beg;
}
//-----Функция удаления элемента из таблицы------
my_list* delete_item(my_list *beg, my_list **end, int note_to_delete){
	if(beg ==NULL){
		cout<<"Список пуст!";
		return beg;
	}
	my_list *temp;
	int num = note_to_delete;
	if(!note_to_delete){
		num = get_id(*end);
		if(!num) return beg;
	}
	for(temp=beg;temp->data.id != num;temp=temp->next);
	if(temp==beg&&temp==(*end)){
		if(!note_to_delete){
			cout<<"В списке всего один элемент, и вы хотите его удалить,верно(список останется пустым)?\n"
			"Нажмите любую клавишу, если да, и 0, если хотите отменить удаление->";
			if(getch() == '0') return beg;
		}
		beg = NULL;
		*end = NULL;
	} else if(temp==beg){
		beg->next->prev = NULL;
		beg = beg->next;
	} else if(temp==*end){
		(*end)->prev->next=NULL;
		*end = (*end)->prev;
	} else{
		temp->prev->next = temp->next;
		temp->next->prev = temp->prev;
	}
	delete temp;
	ids_correction(beg);
	if(!note_to_delete) cout<<"Удаление прошло успешно."<<endl;
	is_list_changed = true;
	return beg;
}
//-----Функция корректировки записи в таблице----
void item_correction(my_list *beg, my_list *end){
	if(beg==NULL){
		cout<<"Список пуст!\n";
		system("pause");
		return;
	}
	my_list *temp;
	string temp_str;
	int num = get_id(end);
	if(!num) return;
	for(temp=beg;temp->data.id != num;temp=temp->next);
	do{
		item_print(temp);
		cout<<"Введите номер поля, которое хотите изменить(0-выход в меню)->";
		switch(get_char('0', '5')){
			case '1': {
				cout<<"Введите название продукта:";
				temp->data.name = get_str();
				break;
			}
			case '2': {
				cout<<"Введите дату производства в формате(ДД.ММ.ГГГГ):";
				temp->data.cr_date = get_date();
				break;
			}
			case '3': {
				cout<<"Введите срок годности(кол-во дней):";
				temp->data.exp_time = get_int();
				break;
			}
			case '4': {
				cout<<"Введите производителя:";
				temp->data.creator = get_str();
				break;
			}
			case '5': {
				cout<<"Введите стоимость:";
				temp->data.price = get_float();
				break;
			}
			case '0': {
				return;
			}
		}
		cout<<"После обновления:\n";
		is_list_changed = true;
	}while(1);
	
}
//----функция записи списка в файл----
void save_list_to_file(my_list *beg){
	if(beg==NULL){
		cout<<"Список пуст"<<endl;
		return;
	}
	system("cls");
	string file_name;
	cout<<"Сохранить в текстовый (1) или в типизированный(2) файл?->";
	char i = get_char('1','2');
	int choise = 0;
	do{
		file_name = get_filename(i);
		system("cls");
		cout<<"Введенное вами название файла: "<<file_name<<endl;
		cout<<"Предлагаемые действия:"<<endl;
		string menu[]{"Создать/перезаписать файл с указанным названием", "Дополнить файл с указанным названием",
		"Ввести другое название файла", "Выход в меню без сохранения"};
		choise = print_menu(menu, 4, 0, 2);
		switch(choise){
			case 3: continue;
			case 0: return;
			default: break;
		}
		break;
	}while(1);
	if(i == '1'){
		FILE* myFILE;
		if(choise == 1){
			myFILE = fopen(file_name.c_str(), "w");
		}
		else if(choise == 2){
			myFILE = fopen(file_name.c_str(), "a");
		}
		else exit(1);
		if(myFILE == NULL){
		cout<<"Ошибка открытия файла!"<<endl;
		return;
		}
		string temp_str;
		for(my_list *temp = beg; temp!=NULL;temp=temp->next){
			temp_str = "Название:";
			temp_str.append(temp->data.name).append("\n");
			fputs(temp_str.c_str(), myFILE);
			temp_str = "Дата производства:";
			temp_str.append(do_str_date(temp->data).c_str()).append("\n");
			fputs(temp_str.c_str(), myFILE);
			temp_str = "Срок годности:";
			temp_str.append(to_string(temp->data.exp_time).c_str()).append("\n");
			fputs(temp_str.c_str(), myFILE);
			temp_str = "Производитель:";
			temp_str.append(temp->data.creator).append("\n");
			fputs(temp_str.c_str(), myFILE);
			temp_str = "Цена:";
			temp_str.append(to_string(temp->data.price).c_str()).append("\n");
			fputs(temp_str.c_str(), myFILE);
			fputs("___\n", myFILE);
		}
		cout<<"Сохранение прошло успешно."<<endl;
		is_list_changed = false;
		fclose(myFILE);
		return;
	}
	else if(i == '2'){
		FILE* myFILE;
		if(choise == 1){
			myFILE = fopen(file_name.c_str(), "wb");	
		}
		else if(choise == 2){
			myFILE = fopen(file_name.c_str(), "ab");
		}
		else exit(1);
		if(myFILE == NULL){
		cout<<"Ошибка открытия файла!"<<endl;
		return;
		}
		for(my_list *temp = beg; temp!=NULL;temp=temp->next){
			sSAVE temp_data;
			strcpy(temp_data.name, (temp->data.name).c_str());
			strcpy(temp_data.creator, (temp->data.creator).c_str());
			temp_data.cr_date = temp->data.cr_date;
			temp_data.exp_time = temp->data.exp_time;
			temp_data.price = temp->data.price;
			fwrite(&temp_data, p_size, 1, myFILE);
		}
		cout<<"Сохранение прошло успешно."<<endl;
		is_list_changed = false;
		fclose(myFILE);
		return;
	}
	else exit(1);
}
//----Функция чтения списка из текстового файла----
my_list* get_list_from_txt_file(my_list *beg, my_list **end){
	string file_name = get_filename('1');
	FILE* file_to_open = fopen(file_name.c_str(), "r");
	if(file_to_open==NULL) {
		cout<<"Ошибка открытия файла!\n";
		system("pause");
		return beg;
	}
	char* buffer = new char[256];
	int id = 0;
	my_list *temp = NULL;
	product *temp_data = NULL;
	int *check;
	regex data_templ(".*:.*\n"),
	name_templ("Название:([-\" а-яА-ЯЁё\\w]{1,15})\n"),
	date_cr_templ("Дата производства:(0[1-9]|[12][0-9]|3[01])(\\.)(0[1-9]|1[0-2])(\\.)(19[0-9]{2}|20[0-1][0-9]|202[0-3])\n"),
	time_templ("Срок годности:(\\d{1,6})\n"),
	cr_templ("Производитель:([-\" а-яА-ЯЁё\\w]{1,15})\n"),
	price_templ("Цена:(\\d{1,6})(\\.\\d{1,6})?\n"),
	separator_template("_+\n"),
	comment_template("\\/\\/.*\n");
	cmatch result;
	bool is_data_reading = false;
	while(!feof(file_to_open)){
		if(fgets(buffer, 256, file_to_open)==NULL) break;
		string buf_to_str(buffer);
		if(regex_match(buffer, result, comment_template)){
			continue;
		}
		else if(regex_match(buffer,result,data_templ)){
			if(temp_data==NULL) {
				check = new int[5];
				for(int i =0; i<5;i++) check[i]=0;
				temp_data = new product;
				is_data_reading = true;
			}
			if(regex_match(buffer, result, name_templ)){
				temp_data->name = (string)result[1];
				check[0] = 1;
				continue;
			}
			else if(regex_match(buffer, result, date_cr_templ)){
				string temp_str = result[1];
				temp_data->cr_date.day = atoi(temp_str.c_str());
				temp_str = result[3];
				temp_data->cr_date.month = atoi(temp_str.c_str());
				temp_str = result[5];
				temp_data->cr_date.year = atoi(temp_str.c_str());
				check[1] = 1;
				continue;
			}
			else if(regex_match(buffer, result, time_templ)){
				string temp_str = result[1];
				temp_data->exp_time = atoi(temp_str.c_str());
				check[2] = 1;
				continue;
			}
			else if(regex_match(buffer, result, cr_templ)){
				temp_data->creator = (string)result[1];
				check[3] = 1;
				continue;
			}
			else if(regex_match(buffer, result, price_templ)){
				string temp_str = result[1];
				temp_str.append(result[2]);
				temp_data->price = atof(temp_str.c_str());
				check[4] = 1;
				continue;
			}
		}
		else if(regex_match(buffer, result, separator_template)){
			if(!is_data_reading) continue;
			if(is_data_reading){
				fill_empty_fields(temp_data, check);
				temp = new my_list;
				temp->data = *temp_data;
				temp_data=NULL;
				temp->data.id = ++id;
				temp->next = NULL;
				if(beg==NULL){
					beg=temp;
					*end=temp;
					temp->prev=NULL;
				}
				else{
					temp->prev=*end;
					(*end)->next=temp;
					*end=temp;
				}
				is_data_reading = false;
				delete []check;
			}
		}
		else continue;
	}
	if(is_data_reading){
		fill_empty_fields(temp_data, check);
		temp = new my_list;
		temp->data = *temp_data;
		temp_data=NULL;
		temp->data.id = ++id;
		temp->next = NULL;
		if(beg==NULL){
			beg=temp;
			*end=temp;
			temp->prev=NULL;
		}
		else{
			temp->prev=*end;
			(*end)->next=temp;
			*end=temp;
		}
		is_data_reading = false;
		delete []check;
	}
	delete []buffer;
	fclose(file_to_open);
	cout<<"Считывание данных прошло успешно."<<endl;
	return beg;
}
//----Функция чтения списка из типизированного файла----
my_list* get_list_from_dat_file(my_list *beg, my_list **end){
	string file_name = get_filename('2');
	FILE* myFILE = fopen(file_name.c_str(), "rb");
	if(myFILE==NULL) {
		cout<<"Ошибка открытия файла!\n";
		system("pause");
		return beg;
	}
	sSAVE temp_data;
	my_list *temp;
	int id = 0;
	while(fread(&temp_data, p_size, 1, myFILE)){
		temp = new my_list;
		string temp_str1(temp_data.name);
		string temp_str2(temp_data.creator);
		temp->data.id = ++id;
		temp->data.name = temp_str1;
		temp->data.cr_date = temp_data.cr_date; 
		temp->data.exp_time = temp_data.exp_time; 
		temp->data.creator = temp_str2;
		temp->data.price = temp_data.price;
		temp->next = NULL;
		if(beg==NULL){
			beg=temp;
			*end=temp;
			temp->prev=NULL;
		}
		else{
			temp->prev=*end;
			(*end)->next=temp;
			*end=temp;
		}
	}
	fclose(myFILE);
	cout<<"Считывание данных прошло успешно."<<endl;
	return beg;
}
//----Функция сортировки таблицы по названию----
my_list* sort_list(my_list *beg, my_list *end){
	if(beg==NULL){
		cout<<"Список пуст!\n";
		return beg;
	}
	my_list *temp1, *temp2;
	string menu[]{"Название","Дата изготовления","Срок годности","Изготовитель","Цена", "Выход в меню"};
	cout<<"Выберите поле, по которому хотите произвести сортировку"<<endl;
	int choise = print_menu(menu, 6);
	if(choise == 0) return beg;
	string menu_[]{"По возрастанию", "По убыванию"};
	int ch = print_menu(menu_, 2);
	bool i;
	for(temp1=end; temp1 != beg; temp1=temp1->prev)
		for(temp2 = beg; temp2!=temp1; temp2= temp2->next)
			switch(choise){
				case 1:{
					i = temp2->data.name<temp2->next->data.name;
					if(ch?i:!i) swap(temp2, temp2->next);
					break;
				}
				case 2:{
					i = datecmp(temp2->data, temp2->next->data)>0;
					if(ch?i:!i) swap(temp2, temp2->next);
					break;
				}
				case 3:{
					i = temp2->data.exp_time>temp2->next->data.exp_time;
					if(ch?i:!i) swap(temp2, temp2->next);
					break;
				}
				case 4:{
					i = temp2->data.creator<temp2->next->data.creator;
					if(ch?i:!i) swap(temp2, temp2->next);
					break;
				}
				case 5:{
					i = temp2->data.price>temp2->next->data.price;
					if(ch?i:!i) swap(temp2, temp2->next);
					break;
				}
			}
	ids_correction(beg);
	cout<<"\nСортировка прошла успешно\n";
	is_list_changed = true;
	return beg;
}
//--------Функция поиска по имени--------
void search(my_list *beg){
	string my_str{""};
	int counter = 0, i;
	int ch;
	my_list *begl = NULL, *temp;
	system("cls");
	while(1){
		GoToXY(0, 0);
		cout<<"Input name of product(ESC - exit)->";
		GoToXY(35, 0);
		if(counter != 0) cout<<my_str;
		cin.sync();
		ch=getch();
		system("cls");
		if(ch==ESC) {
			begl=clear_all_data(begl);
			return;
		}
		if(ch == ENTER){
			if(my_str.size()){
				for(temp=begl;temp->next!=NULL;temp=temp->next);
				output_table(begl, &temp, 0);
			}
			begl=clear_all_data(begl);
			return;
		} 
		if(counter<N && ch != BACKSPACE){
			my_str+=(char)ch;
			counter++;
		} 
		else if(ch == BACKSPACE && counter>0){
			my_str = my_str.substr(0, my_str.size()-1);
			counter--;
		}
		if(counter > 0){
			begl = cr_list_by_name(beg, begl, my_str);
			if(!begl) continue;
			else{
				GoToXY(0, 5);
				print_head();
				for(i = 1, temp = begl; i<6 && temp!=NULL; temp=temp->next, i++) print_fields(temp->data);
				print_footer();	
			}	
		}
	}
}
//------Функция формирования списка----
my_list* cr_list_by_name(my_list *beg, my_list *begl, string str){
	begl = clear_all_data(begl);
	for(my_list *temp = beg; temp!=NULL; temp=temp->next){
		if(temp->data.name.compare(0, str.size(), str.c_str()) == 0) begl = add_item_w_data(begl, temp->data);
	}
	ids_correction(begl);
	return begl;
}
//-----Функции печати таблицы-------
void print_head(){
	cout.setf(ios::left);
	string line(81, '_');
	cout<<line<<endl;
	cout<<"|  ID";
	cout<<"|       Название";
	cout<<"|  Дата произ-ва";
	cout<<"|  Срок годности";
	cout<<"|  Производитель";
	cout<<"|      Цена|"<<endl;
	cout<<line<<endl;
	cout.unsetf(ios::left);
}
void print_fields(product &temp){
	cout<<"|"<<setw(4)<<temp.id<<"|"<<setw(15)<<temp.name<<"|"<<setw(15)<<do_str_date(temp)<<"|"<<setw(15)<<temp.exp_time<<"|"<<
	setw(15)<<temp.creator<<"|"<<setw(10)<<temp.price<<"|"<<endl;
}
void print_footer(){
	string line(81, '_');
	cout<<line<<endl;
}
//--------Функция заполнения пустых полей после считывания данных из файла---
void fill_empty_fields(product *temp_data, int *check){
	for(int i=0;i<5;i++){
		if(check[i]) continue;
		else switch(i){
			case 0:{
				temp_data->name = "Undefined";
				break;
			}
			case 1:{
				temp_data->cr_date.day = 1;
				temp_data->cr_date.month = 1;
				temp_data->cr_date.year = 2023;
				break;
			}
			case 2:{
				temp_data->exp_time = 0;
				break;
			}
			case 3:{
				temp_data->creator = "Undefined";
				break;
			}
			case 4:{
				temp_data->price = 0;
				break;
			}
		}
	}
}
//--------Функция ввода символа в указанных пределах------
char get_char(char from, char to){
	do{
		cin.sync();
		char x = getchar();
		if(from<=x&&x<=to){
			if(cin.peek()=='\n') cin.ignore();
			return x;
		} 
		cout<<"Введен неправильный символ, повторите ввод"<<endl;
	}while(1);
}
//--------Функция формиравания строки с датой-------
string do_str_date(product data){
	string date = "";
	if(data.cr_date.day<10) date +='0'+to_string(data.cr_date.day)+'.';
		else date += to_string(data.cr_date.day)+'.';
	if(data.cr_date.month<10) date +='0'+to_string(data.cr_date.month)+'.';
		else date += to_string(data.cr_date.month)+'.';
	date += to_string(data.cr_date.year);
	return date;
}
//--------Функция печати содержимого одной записи-----
void item_print(my_list *temp){
	cout<<"Содержимое записи(id ="<<temp->data.id<<"):"<<endl<<
	"1. Название: "<<temp->data.name<<endl<<
	"2. Дата произ-ва: "<<do_str_date(temp->data)<<endl<<
	"3. Срок годности: "<<temp->data.exp_time<<endl<<
	"4. Производитель: "<<temp->data.creator<<endl<<
	"5. Цена: "<<temp->data.price<<endl;
	return;
}
//-----Функция очистки памяти-------
my_list* clear_all_data(my_list *beg){
	for(my_list *temp = beg; beg!=NULL;temp=beg){
		beg = beg->next;
		delete(temp);
	}
	return beg;
}
//------Функция корректировки id у списка------
void ids_correction(my_list *beg){
	my_list *temp;
	int id;
	for(temp=beg, id = 1 ;temp!=NULL;temp=temp->next, id++)
		temp->data.id = id;
	return;
}
//-----Функция переноса курсора по координатам----
void GoToXY(short x, short y){
	SetConsoleCursorPosition(hStdOut, {x,y});
}
//------Функция отображения главного меню----
int print_menu(string menu[], int size, short x, short y){
	int active = 1;
	while(1){
		short _x = x, _y = y;
		for(int i=0; i<size; i++){
			if(i+1 == active) SetConsoleTextAttribute(hStdOut, BACKGROUND_BLUE | BACKGROUND_INTENSITY | FOREGROUND_RED |
			FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
			else SetConsoleTextAttribute(hStdOut, BACKGROUND_BLUE | BACKGROUND_RED | BACKGROUND_GREEN |BACKGROUND_INTENSITY);
			GoToXY(_x, _y++);
			cout<<menu[i]<<endl;
		}
		cin.sync();
		char ch = getch();
		if(ch == 224) ch = getch();
		switch((int)ch){
			case A_UP:{
				if(active>1) active--;
				else active = size;
				break;
			}
			case A_DOWN:{
				if(active<size) active++;
				else active = 1;
                break;
			}
			case ENTER: {
				SetConsoleTextAttribute(hStdOut, BACKGROUND_BLUE | BACKGROUND_RED | BACKGROUND_GREEN |BACKGROUND_INTENSITY);
				system("cls");
				if(active == size) return 0;
				return active;
			}
			default: continue;
		}
	}
}
//------Функция управления отображением курсора---
void ConsoleCursorVisible(bool show){
    _CONSOLE_CURSOR_INFO sCursorInfo;
    GetConsoleCursorInfo(hStdOut, &sCursorInfo);
    sCursorInfo.bVisible = show;
    SetConsoleCursorInfo(hStdOut, &sCursorInfo);
}
//------Функция подсчета убытков от просрока-----
void count_expired(my_list *beg, my_list *end){
	if(beg==NULL){
		cout<<"Список пуст!\n";
		return;
	}
	product exp_list[5];
	int counter_exp = 0;
	float price_of_exp = 0;
	for(my_list *temp = beg; temp!=NULL; temp = temp->next){
		if(is_expired(temp->data)){
			if(counter_exp<5){
				exp_list[counter_exp] = temp->data;
				counter_exp++;
				price_of_exp+=temp->data.price;
			}
			else{
				int i_min = 0;
				for(int i = 1; i<5;i++) if(exp_list[i].price<exp_list[i_min].price) i_min = i;
				exp_list[i_min] = temp->data;
				counter_exp++;
				price_of_exp+=temp->data.price;
			}
		}
	}
	for(int i = counter_exp>5?5:counter_exp; i>1; i--)
		for(int j = 0; j<i-1;j++)
			if(exp_list[j].price<exp_list[j+1].price){
				product temp = exp_list[j];
				exp_list[j] = exp_list[j+1];
				exp_list[j+1] = temp;
			}
	system("cls");
	cout<<"Результаты обработки списка:"<<endl;
	if(!counter_exp) {
		cout<<"Просроченных товаров в списке нет.\n";
		return;
	}
	else{
		cout<<"Всего "<<counter_exp<<" просроченных товаров, на сумму "<<price_of_exp<<endl<<
		"Список 5 самых дорогих товаров:"<<endl;
		print_head();
		for(int i = 0; i<(counter_exp>5?5:counter_exp); i++) print_fields(exp_list[i]);
		print_footer();
		system("pause");
	}
	system("cls");
	cout<<"Хотите ли вы сохранить результат обработки в файл?(1-да, 0-нет)"<<endl<<"->";
	if(get_char('0', '1') == '0') return;
	if(cin.peek()=='\n') cin.ignore();
	cout<<"Введите название текстового файла, в который будут записаны данные."<<
	endl<<"Внимание, если файл существует, его данные будут стерты."<<endl;
	string file_name = get_filename('1');
	FILE *myFILE = fopen(file_name.c_str(), "w");
	fprintf(myFILE, "Всего %d просроченных товаров, на сумму %g\n", counter_exp, price_of_exp);
	fputs("Список 5 самых дорогих товаров:", myFILE);
	for(int i = 0; i<(counter_exp>5?5:counter_exp); i++){
		fprintf(myFILE,"\n%d\t%s\t%s\t%d\t%s\t%g", exp_list[i].id, exp_list[i].name.c_str(), do_str_date(exp_list[i]).c_str(),exp_list[i].exp_time, exp_list[i].creator.c_str(),
		exp_list[i].price);
	}
	fclose(myFILE);
}
//----Функция проверки продукта на проскрок-----
bool is_expired(product data){
	time_t  t = time(NULL);
	tm *now = localtime(&t);
	// sDATE date_today{now->tm_mday, now->tm_mon+1, now->tm_year+1900};
	int days_now = now->tm_mday + (now->tm_mon+1)*30 + now->tm_year*365;
	int days_of_cr = data.cr_date.day + data.cr_date.month*30+(data.cr_date.year-1900)*365;
	if(days_of_cr + data.exp_time<days_now) return true;
	return false;
}
//----Функция выбора записи в меню вывода таблицы---
my_list* do_choise_and_delete(my_list *beg, my_list **end, int f_note, int size, short x, short y){
	int active = f_note;
	while(1){
		short _x = x, _y = y;
		for(int i=f_note; i<size+f_note; i++){
			GoToXY(_x, _y++);
			if(i == active) cout<<"<";
			else cout<<" ";
		}
		GoToXY(0, size + 4);
		for(int i = 0; i< 7; i++)
			cout<<string(40, ' ')<<endl;
		GoToXY(0, size+4);
		cout<<"Двигайте курсор стрелочками вверх и вниз, нажмите ENTER, чтобы удалить выбранную запись."<<endl<<
		"ESC - выход";
		cin.sync();
		char ch = getch();
		if(ch == 224) ch = getch();
		switch((int)ch){
			case A_UP:{
				if(active>f_note) active--;
				else active = f_note+size-1;
				continue;
			}
			case A_DOWN:{
				if(active<f_note+size-1) active++;
				else active = f_note;
                continue;
			}
			case ENTER: {
				beg = delete_item(beg, end, active);
				return beg;
			}
			case ESC: return beg;
			default: continue;
		}
	}
	return beg;
}
//----Функция обмена данными между записями-----
void swap(my_list* temp1, my_list* temp2){
	product temp_data = temp1->data;
	temp1->data = temp2->data;
	temp2->data = temp_data;
	return;
}
//----Функция сравнения дат---
int datecmp(product d1, product d2){
	return d1.cr_date.day + d1.cr_date.month*30 + d1.cr_date.year * 365-(d2.cr_date.day + d2.cr_date.month*30 + d2.cr_date.year * 365);
}
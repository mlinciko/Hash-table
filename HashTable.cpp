// СИАОД ПР2 БИНАРНЫЙ.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <Windows.h>
#include <chrono>
using namespace std;

//хеш-функция
int hash_function(int key, int buffer_size)
{
	switch (key % 1000000)
	{
	case 0:
		key *= 13452;
		break;
	case 1:
		key *= 14958;
		break;
	case 2:
		key *= 20984;
		break;
	case 3:
		key *= 33946;
		break;
	case 4:
		key *= 40493;
		break;
	case 5:
		key *= 52931;
		break;
	case 6:
		key *= 60397;
		break;
	case 7:
		key *= 70983;
		break;
	case 8:
		key *= 88389;
		break;
	case 9:
		key *= 93642;
		break;
	default:
		break;
	}
	//cout << "Хеш-функция: " << key % buffer_size<< endl;
	return key % buffer_size;
}

//класс хеш-таблицы
class hash_table
{
private:
	const int default_size = 10;//начальный размер хеш-таблицы
	struct Node
	{
		int index_in_file = 0;//необходимо для того, чтобы переписать элементы в файл в исходном порядке(при удалении)
		int key = 0;
		char name[20] = "";
		char address[35] = "";
	};
	Node* array;
	int buffer_size;//размер самого массива, сколько памяти выделено под хранение всей таблицы
	int amount_of_records = 0;//количество записей в файле
	int file_index = 0;//индекс элемента в файле
	int last_index = 0;//индекс последнего записанного в файл элемента
public:
	hash_table()
	{
		buffer_size = default_size;
		array = new Node[buffer_size];
	}
	//устанавливает количество записей в файле
	/*void set_amount_of_records(int n)
	{
		amount_of_records = n;
	}*/
	//добавляет запись в конец файла
	void add_to_file(int key, char *name, char *address)
	{
		FILE* file_write = fopen("file.bin", "ab+");
		fwrite(&key, sizeof(key), 1, file_write);
		fwrite(&name, sizeof(name), 1, file_write);
		fwrite(&address, sizeof(address), 1, file_write);
		cout << "Запись в файл: " << key << " " << name << " " << address << endl;
		fclose(file_write);
	}
	//добавляет элемент в хеш-таблицу и в файл
	bool add_to_hash(int key, char *name, char *address)
	{
		int position = hash_function(key,buffer_size);
		//если свободное место для записи есть
		if (array[position].key == 0)
		{
			//запись в массив структур
			array[position].index_in_file = ++file_index;
			array[position].key = key;
			for (int i = 0; i < 20; i++)
				array[position].name[i] = name[i];
			for (int i = 0; i < 35; i++)
				array[position].address[i] = address[i];
			//запись в файл
			add_to_file(key, name, address);
			last_index = array[position].index_in_file;
			amount_of_records++;
			cout << "Новая запись с позицией "<< position << ": " <<array[position].key << " " << array[position].name << " " << array[position].address << endl;
			cout << "Amount of records: " << amount_of_records << endl;
			return true;
		}
		else
		{
			int new_position = position + 1;
			if (new_position < buffer_size)
			{
				//если позиция меньше массива, то проходимся по массиву в поисках пустого места
				int q = 1;
				while (new_position < buffer_size)
				{
					q++;
					if (array[new_position].key == 0)
					{
						array[new_position].index_in_file = ++file_index;
						array[new_position].key = key;
						for (int i = 0; i < 20; i++)
							array[new_position].name[i] = name[i];
						for (int i = 0; i < 35; i++)
							array[new_position].address[i] = address[i];
						//запись в файл
						add_to_file(key, name, address);
						last_index = array[new_position].index_in_file;
						amount_of_records++;
						cout << "Новая запись с позицией " << new_position << ": " << array[new_position].key << " " << array[new_position].name << " " << array[new_position].address << endl;
						cout << "Amount of records: " << amount_of_records << endl;
						return true;
					}
					else new_position += q;
				}
			}
			//если позиция больше размера массива, то проходим массив от начала до позиции в поисках пустого места
			if (new_position >= buffer_size)
			{
				new_position = 0;
				int q = 1;
				while (new_position < position)
				{
					if (array[new_position].key == 0)
					{
						array[new_position].index_in_file = ++file_index;
						array[new_position].key = key;
						for (int i = 0; i < 20; i++)
							array[new_position].name[i] = name[i];
						for (int i = 0; i < 35; i++)
							array[new_position].address[i] = address[i];
						//запись в файл
						add_to_file(key, name, address);
						last_index = array[new_position].index_in_file;
						amount_of_records++;
						cout << "Новая запись с позицией " <<new_position << ": " << array[new_position].key << " " << array[new_position].name << " " << array[new_position].address << endl;
						cout << "Amount of records: " << amount_of_records << endl;
						return true;
					}
					else new_position += q;
				}
			}
			//если все места в массиве заняты, то увеличиваем массив
			cout << "Элемент не может быть добавлен. Надо увеличить массив.";
			resize(key, name, address);
		}
		return false;
	}
	//функция увеличивает массив
	void resize(int key, char *name, char *address)
	{
		int past_buffer_size = buffer_size;
		buffer_size *= 2;
		cout << "Массив увеличен. Новый размер массива: " << buffer_size << endl;
		Node *array_new = new Node[buffer_size];
		swap(array, array_new);
		for (int i = 0; i < past_buffer_size; i++)
		{
			//добавление элементов в новый массив
			array[i].index_in_file = array_new[i].index_in_file;
			array[i].key = array_new[i].key;
			for (int j = 0; j < 20; j++)
				array[i].name[j] = array_new[i].name[j];
			for (int j = 0; j < 35; j++)
				array[i].address[j] = array_new[i].address[j];
		}
		add_to_hash(key, name, address);
		//удаление предыдущего массива
		delete[] array_new;
	}
	//удаление элемента из хеш-таблицы и файла
	bool remove(int key)
	{
		int position = rehesh(key);
		if (position != -1)
		{
			int temp = array[position].index_in_file;
			array[position].index_in_file = 0;
			array[position].key = 0;
			for (int i = 0; i < 20; i++)
				array[position].name[i] = '\0';
			for (int i = 0; i < 35; i++)
				array[position].address[i] = '\0';
			for (int i = 0; i < buffer_size; i++)
				if ((array[i].key != 0) && (array[i].index_in_file > temp))
					array[i].index_in_file--;
			amount_of_records--;
			file_index--;
			remove_from_file(key);//удаляет запись из файла
			return true;
		}
		else return false;
	}
	//удаление из файла
	void remove_from_file(int key)
	{
		FILE *file_write = fopen("file.bin", "wb");
		for (int i = 1; i <= last_index; i++)
		{
			for(int j = 0; j < buffer_size;j++)
				if (array[j].index_in_file == i)
				{
					fwrite(&array[j].key, sizeof(array[j].key), 1, file_write);
					fwrite(&array[j].name, sizeof(array[j].name), 1, file_write);
					fwrite(&array[j].address, sizeof(array[j].address), 1, file_write);
				}
				
		}
		fclose(file_write);
	}
	//поиск элемента
	void find(int key)
	{
		int position = rehesh(key);
		if (position != -1)
		{
			cout << "Успешно найден элемент: " << endl << position << ": " << array[position].key << " " << array[position].name << " " << array[position].address << endl;
		}
		else cout << "Указан неверный номер!" << endl;
	}
	//функция рехеша
	int rehesh(int key)
	{
		int position = hash_function(key, buffer_size);
		if (array[position].key == key)
		{
			return position;
		}
		else
		{
			int new_position = position + 1;
			if (new_position < buffer_size)
			{
				//если позиция меньше массива, то проходимся по массиву в поисках пустого места
				int q = 1;
				while (new_position < buffer_size)
				{
					q++;
					if (array[new_position].key == key)
					{
						return new_position;
					}
					else new_position += q;
				}
			}
			//если позиция больше размера массива, то проходим массив от начала до позиции в поисках пустого места
			if (new_position >= buffer_size)
			{
				new_position = 0;
				int q = 1;
				while (new_position < position)
				{
					if (array[new_position].key == key)
					{
						return new_position;
					}
					else new_position += q;
				}
			}
		}
		return -1;
	}
	void print_hash()
	{
		for (int i = 0; i < buffer_size; i++)
		{
			cout << array[i].index_in_file << " " << array[i].key << " " << array[i].name << " " << array[i].address << endl;
		}
	}
	void read_file()
	{
		FILE *file_read = fopen("file.bin", "rb");
		for (int i = 0; i < amount_of_records; i++)
		{
			Node record;
			fread(&record.key, sizeof(record.key), 1, file_read);
			fread(&record.name, sizeof(record.name), 1, file_read);
			fread(&record.address, sizeof(record.address), 1, file_read);
			cout << record.key << " " << record.name << " " << record.address << endl;
			//printf("%d %s %s\n", record.key, record.name, record.address);
		}
		fclose(file_read);
	}
};
int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	setlocale(LC_ALL, "rus");
	cout << "Лабораторная работа №2. Хеширование и организация быстрого поиска данных. Овчаренко Мария" << endl;
	hash_table table;
	//количество записей в файле
	int n;
	cout << "Введите количество записей: ";
	cin >> n;
	int key = 0;
	char name[20] = "";
	char address[35] = "";
	cout << "Введите " << n << " записей: " << endl;
	for (int i = 0; i < n; i++)
	{
		scanf("%d" "%s" "%s", &key, &name, &address);
		table.add_to_hash(key, name, address);
	}
	int next = 1;
	int key_test;
	int option;
	while (next != 0)
	{
		cout << "Выберите действие: " << endl << "0 - вставка новой записи в файл"<< endl <<"1 - поиск записи в файле" << endl <<
			"2 - удаление записи из файла" << endl << "3 - вывод записей файла" << endl << "4 - вывод хеш-таблицы" << endl << "5 - выйти из меню" << endl;
		cin >> option;
		switch (option)
		{
		case 0:
			cout << "Введите запись: " << endl;
			scanf("%d" "%s" "%s", &key_test, &name, &address);
			if (table.add_to_hash(key_test, name, address)) cout << "Вставка произведена успешно!" << endl;
			else cout << "Ошибка. Неправльный формат записи." << endl;
			break;
		case 1:
			cout << "Введите ключ записи: ";
			cin >> key_test;
			table.find(key_test);
			break;
		case 2:
		{
			cout << "Введите ключ записи: ";
			cin >> key_test;
			if (table.remove(key_test)) cout << "Удаление произведено успешно!" << endl;
			else cout << "Ошибка. Неправльный ключ." << endl;
			break;
		}
		case 3:
			table.read_file();
			break;
		case 4:
			table.print_hash();
			break;
		case 5:
			break;
		default:
			break;
		}
		cout << "Продолжить работу? (1 - да; 0 - нет)" << endl;
		cin >> next;
	}
	cout << "Тестирование на большом объёме данных" << endl;
	cout << "Количество записей: " << n << endl;
	cout << "Введите ключ записи: ";
	auto begin = std::chrono::steady_clock::now();
	cin >> key_test;
	table.find(key_test);
	auto end = std::chrono::steady_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
	std::cout << "Время работы программы: " << time.count() <<"ms" <<endl;//время работы программы в миллисекундах
	return 0;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.

#include <pqxx/pqxx>
#include <iostream>
#include <locale>
#include <Windows.h>
#pragma execution_character_set("utf-8")
#include <cstring>

class WorkersDB {
private:

	std::string data = "host=localhost "
		"port=5432 "
		"dbname=task4 "
		"user=postgres "
		"password=651991";

public:
	void createDB() {
		pqxx::connection c(data);
		pqxx::transaction t(c);

		t.exec("CREATE TABLE IF NOT EXISTS workers (id INT NOT NULL GENERATED ALWAYS AS IDENTITY, name text NOT NULL, surname text NOT NULL, email text NOT NULL, CONSTRAINT worker_PK PRIMARY KEY (id));");
		t.exec("CREATE TABLE IF NOT EXISTS PhoneNum (PhoneNumId INT NOT NULL GENERATED ALWAYS AS IDENTITY, Number text, workers_id INTEGER NOT NULL REFERENCES workers(id));");
		t.commit();
	}

	void add_newworker(std::tuple<std::string, std::string, std::string> x) {

		pqxx::connection c(data);
		pqxx::transaction t(c);
		t.exec("insert into workers (name, surname, email) values ('" + t.esc(std::get<0>(x)) + "', '" + t.esc(std::get<1>(x)) + "', '" + t.esc(std::get<2>(x)) + "');");
		t.commit();
	};

	void add_newnumber(std::tuple<std::string, int> x) {

		pqxx::connection c(data);
		pqxx::transaction t(c);
		t.exec("insert into PhoneNum (Number, workers_id) values ('" + t.esc(std::get<0>(x)) + "'," + t.esc(std::to_string(std::get<1>(x))) + ");");
		t.commit();
	};

	void updata_worker(std::tuple<int, std::string, std::string, std::string> x) {

		pqxx::connection c(data);
		pqxx::transaction t(c);
		t.exec("UPDATE workers set name = ('" + t.esc(std::get<1>(x)) + "') , surname = ('" + t.esc(std::get<2>(x)) + "'),  email = ('" + t.esc(std::get<3>(x)) + "') where id = " + t.esc(std::to_string(std::get<0>(x))) + ";");
		t.commit();
	};

	void delete_phone_to_worker(int x) {

		pqxx::connection c(data);
		pqxx::transaction t(c);
		t.exec("DELETE FROM PhoneNum WHERE workers_id = " + t.esc(std::to_string(x)) + ";");
		t.commit();
	};

	void delete_worker(int x) {

		pqxx::connection c(data);
		pqxx::transaction t(c);
		t.exec("DELETE FROM PhoneNum WHERE workers_id = " + t.esc(std::to_string(x)) + ";\nDELETE FROM workers WHERE id = " + t.esc(std::to_string(x)) + ";");
		t.commit();
	};




	std::vector<std::tuple<int, std::string, std::string, std::string>> search_worker(std::tuple<std::string, std::string, std::string> x) {
		
		std::vector<std::tuple<int, std::string, std::string, std::string>> v;

		pqxx::connection c(data);
		pqxx::transaction t(c);

		std::string name = (std::empty(std::get<0>(x)) ? "" : "name = '" + t.esc(std::get<0>(x)) + "'");
		std::string surname = (std::empty(std::get<1>(x)) ? "" : " surname = '" + t.esc(std::get<1>(x)) + "'");
		std::string email = (std::empty(std::get<2>(x)) ? "" : " email = '" + t.esc(std::get<2>(x)) + "'");

		std::string or_1 = "";
		std::string or_2 = "";
		if (!std::empty(std::get<1>(x))) {
			or_1 = "OR";
		}
		if (!std::empty(std::get<1>(x)) || !std::empty(std::get<2>(x))) {
			or_2 = "OR";
		}

		auto result = t.query<int, std::string, std::string, std::string>("SELECT * FROM workers WHERE "+ name +" "+ or_1 +" "+ surname +" " + or_2 + " "+ email +";");

		for (std::tuple<int, std::string, std::string, std::string> record : result) {
			v.push_back({ std::get<0>(record), std::get<1>(record), std::get<2>(record), std::get<3>(record) });
		}
		return v;
	};





	std::vector<std::tuple<int, std::string, std::string, std::string>> search_worker_by_num(std::string str) {

		std::vector<std::tuple<int, std::string, std::string, std::string>> v;
		pqxx::connection c(data);
		pqxx::transaction t(c);

		auto result = t.query<int, std::string, std::string, std::string>("SELECT * FROM workers WHERE id = (SELECT workers_id FROM PhoneNum WHERE Number = '" + t.esc(str) + "');");

		for (std::tuple<int, std::string, std::string, std::string> record : result) {
			v.push_back({ std::get<0>(record), std::get<1>(record), std::get<2>(record), std::get<3>(record) });
		}
		return v;
	};

};

//"+ std::get<0>(x) +"

int main() {

	setlocale(LC_ALL, ".UTF-8");
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
	setvbuf(stdout, nullptr, _IOFBF, 1000);


	try {
		
		WorkersDB workerdb;
		workerdb.createDB();
		
		std::cout << "Введите данные" << std::endl;
		std::tuple<std::string, std::string, std::string> x;
		std::cout << "Имя: ";
		std::cin >> std::get<0>(x);
		std::cout << "Фамилия: ";
		std::cin >> std::get<1>(x);
		std::cout << "E-mail: ";
		std::cin >> std::get<2>(x);
		workerdb.add_newworker(x);
		
		std::string std_bool_num;
		std::tuple<std::string, int> y;

		while (true) {
			std::cout << "Добавить номер телефона? y/n" << std::endl;
			std::cin >> std_bool_num;

			if (std_bool_num == "n") {
				break;
			};

			std::cout << "Номер: ";
			std::cin >> std::get<0>(y);
			std::cout << "Для какого сотрудника: ";
			std::cin >> std::get<1>(y);
			workerdb.add_newnumber(y);
		};
		
		std::cout << "Обновляем данные" << std::endl;
		std::tuple<int, std::string, std::string, std::string> updata;
		std::cout << "Для какого сотрудника? ";
		std::cin >> std::get<0>(updata);
		std::cout << "Имя: ";
		std::cin >> std::get<1>(updata);
		std::cout << "Фамилия: ";
		std::cin >> std::get<2>(updata);
		std::cout << "E-mail: ";
		std::cin >> std::get<3>(updata);
		workerdb.updata_worker(updata);
		
		std::cout << "У какого сотрудника удалить телефоны?" << std::endl;
		int delete_phone;
		std::cin >> delete_phone;
		workerdb.delete_phone_to_worker(delete_phone);

		std::cout << "Какого сотрудника удалить?" << std::endl;
		int delete_worker;
		std::cin >> delete_worker;
		workerdb.delete_worker(delete_worker);
		
		std::cout << "Поиск в базе" << std::endl;
		std::tuple<std::string, std::string, std::string> search_db;
		std::cout << "Имя: ";
		std::getline(std::cin, std::get<0>(search_db));
		std::cout << "Фамилия: ";
		std::getline(std::cin, std::get<1>(search_db));
		std::cout << "E-mail: ";
		std::getline(std::cin, std::get<2>(search_db));
		
		auto v = workerdb.search_worker(search_db);
		if (v.size() > 0) {
			for (const auto& i : v)
			{
				std::cout << "ID: " << std::get<0>(i) << " Имя: " << std::get<1>(i) << " Фамилия: " << std::get<2>(i) << " E-mail: " << std::get<3>(i) << std::endl;
			}
		};
		
		std::cout << "Поиск по номеру" << std::endl;
		std::string number;
		std::cout << "Введите номер телефона: ";
		std::getline(std::cin, number);

		auto v_num = workerdb.search_worker_by_num(number);
		if (v_num.size() > 0) {
			for (const auto& i : v_num)
			{
				std::cout << "ID: " << std::get<0>(i) << " Имя: " << std::get<1>(i) << " Фамилия: " << std::get<2>(i) << " E-mail: " << std::get<3>(i) << std::endl;
			}
		};

	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	
	std::system("pause");
}



# hlib
Collection of C++ libraries(Still under-development)
About the Library.
---------------------------------------
<p>
  Hlib is designed for easy integration to,  efficient and easy to maintain modern C++ applications.
  So far only Sqlite3 wrapper is available(HBase).
</p>

How to use
----------------------------------------
using Hbase(sqlite3 wrapper)
--------------------------------------
Here is the example code.

#include <iostream>

#include "hlib.h"
#pragma comment(lib, "hlib.lib")

using namespace hlib;
hbase db;

//for printing the table.
void out_put(std::string table_name, std::string& error) {
	std::vector<hbase::row_> rows_;
	if (!db.get_records(rows_, table_name, error))
		std::cout << error << " occured!" << std::endl;
	else
		for (auto headings : rows_) {
			for (const auto heading : headings.fields) {
				std::cout << heading.name << "\t|";
			}
			std::cout << std::endl;
			break;
		}

	for (auto row : rows_) {
		for (auto field : row.fields) {
			std::cout << field.item + "\t";
		}
		std::cout << std::endl;
	}
}

int main() {
	std::string error;
	auto on_error = [](std::string& error) { std::cout << error << "occured!" << std::endl; };

	hbase::database_file_ file = { "test.db", "pass" };
	hbase::tables_ tables;

	hbase::table_ users;
	users.name = "sample_table";
	users.columns = {
		{"ID", hbase::column_type_::text_, hbase::constraint_::not_null},
		{"Username", hbase::column_type_::text_, hbase::constraint_::not_null},
		{"User_type", hbase::column_type_::text_, hbase::constraint_::null}
	};
	users.primary_key = "ID";
	tables.add(users);

	if (!db.connect(file, tables, error)) {
		on_error(error);
		return 0;
	}
	else
		std::cout << "database connected successfully!" << std::endl;

	std::vector<hbase::row_> rows;
	hbase::row_ row0 = {
		{
			{ {"ID"}, {"n145"} },
			{ {"Username"}, {"User3"} },
			{ {"User_type"}, {"guest"} }
		}
	};

	hbase::row_ row1 = {
		{
			{ {"ID"}, {"3453hk"} },
			{ {"Username"}, {"User7"} },
			{ {"User_type"}, {"Guest"} }
		}
	};

	rows.push_back(row0);
	rows.push_back(row1);

	for (auto row : rows)
		if (!db.insert_row(row, users.name, error))
			std::cout << error << " occurred!" << std::endl;
		else
			std::cout << "record saved!" << std::endl;

	out_put(users.name, error);

	size_t num;
	if (db.count_records(users.name, num, error))
		std::cout << num << " records" << std::endl;

	hbase::field_ field = { "Username", "User7" };
	if (!db.delete_row(field, users.name, error))
		std::cout << error << " occurred!" << std::endl;
	else
		std::cout << "deleted successfully" << std::endl;

	hbase::row_ update = {
		{
			{ {"ID"}, {"n00r"} },
			{ {"Username"}, {"Hkay"} },
			{ {"User_type"}, {"Guest"} }
		}
	};
	out_put(users.name, error);
	if (!db.update_record(row0, update, users.name, error))
		std::cout << error << " occurred!" << std::endl;
	else
		std::cout << "updated successfully!" << std::endl;

	out_put(users.name, error);

	return 0;
}






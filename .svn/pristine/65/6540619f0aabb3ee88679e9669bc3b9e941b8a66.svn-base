#pragma once


#ifdef _WIN64

#ifdef HLIB_EXPORTS
#define HLIB_API __declspec(dllexport)
#pragma comment(lib, "sqlcipher64.lib")
#else
#define HLIB_API __declspec(dllimport)
#endif

#else

#ifdef HLIB_EXPORTS
#define HLIB_API __declspec(dllexport)
#pragma comment(lib, "sqlcipher32.lib")
#else
#define HLIB_API __declspec(dllimport)
#endif

#endif


#include <string>
#include <vector>
#include <map>

namespace hlib {
	class HLIB_API hbase {
	public:

		struct database_file_ {
			std::string name;
			std::string password;
		};

		enum class column_type_ {
			text_,
			integer_,
			float_,
			blob_
		};

		enum class constraint_ {
			not_null,
			null
		};

		struct column_ {
			std::string name;
			column_type_ type = column_type_::text_;
			constraint_ constraint = constraint_::null;
		};

		class columns_ {
		private:
			std::vector<column_> items;
		public:

			std::vector<column_> get() {
				return items;
			}

			void add(const column_& col) {
				items.push_back(col);
			}
		};

		struct table_ {
			std::string name;
			std::vector<column_> columns;
			std::string primary_key;
		};

		class tables_ {
		private:
			std::vector<table_> items;
		public:
			std::vector<table_> get() {
				return items;
			}

			void add(const table_& table) {
				items.push_back(table);
			}
		};

		/// <summary>
		/// Connecting to the table and creating tables.
		/// </summary>
		/// 
		/// <param name="file">
		/// Specifies the database name and password.
		/// </param>
		/// 
		/// <param name="tables">
		/// Speifies the tables to be created.
		/// </param>
		/// 
		/// <param name="error">
		/// Collects an error if occured.
		/// </param>
		/// 
		/// <returns>
		/// Returns true if successful.
		/// </returns>
		bool connect(const database_file_& file,
			tables_& tables,
			std::string& error);

		struct field_ {
			std::string name;
			std::string item;
		};

		struct row_ {
			std::vector<field_> fields;
		};

		/// <summary>
		/// Inserting table records.
		/// </summary>
		/// 
		/// <param name="row">
		/// Specifies record to be inserted.
		/// </param>
		/// 
		/// <param name="table_name">
		/// Specifies the table name.
		/// </param>
		/// 
		/// <param name="error">
		/// Collects the error if occured.
		/// </param>
		/// 
		/// <returns>
		/// Return true if successful.
		/// </returns>
		bool insert_row(row_& row,
			const std::string& table_name,
			std::string& error);

		/// <summary>
		/// Deletes the table record.
		/// </summary>
		/// 
		/// <param name="field">
		/// Specifies the field to query the table.
		/// </param>
		/// 
		/// <param name="table_name">
		/// Specifies the table name.
		/// </param>
		/// 
		/// <param name="error">
		/// Collects the error if it occures.
		/// </param>
		/// 
		/// <returns>
		/// Returns true if successful.
		/// </returns>
		bool delete_row(field_& field,
			const std::string& table_name,
			std::string& error);

		/// <summary>
		/// Count table records using where.
		/// </summary>
		/// 
		/// <param name="field">
		/// Specifies the field to query the table.
		/// </param>
		/// 
		/// <param name="table_name">
		/// Specifies the table name.
		/// </param>
		/// 
		/// <param name="records">
		/// Collects the # of records.
		/// </param>
		/// 
		/// <param name="error">
		/// Collects the error if occured.
		/// </param>
		/// 
		/// <returns>
		/// Return true if successsful.
		/// </returns>
		bool count_records(field_& field,
			const std::string& table_name,
			size_t& records,
			std::string& error);

		/// <summary>
		/// Counts all table records.
		/// </summary>
		/// 
		/// <param name="table_name">
		/// Specifies table name.
		/// </param>
		/// 
		/// <param name="records">
		/// Collects the # of records.
		/// </param>
		/// 
		/// <param name="error">
		/// Collect the error if occured.
		/// </param>
		/// 
		/// <returns>
		/// Returns true if successful.
		/// </returns>
		bool count_records(const std::string& table_name,
			size_t& records,
			std::string& error);

		/// <summary>
		/// Gets table records using where.
		/// </summary>
		/// 
		/// <param name="rows">
		/// Collects the records from the table.
		/// </param>
		/// 
		/// <param name="field">
		/// Specifies the field to query the table.
		/// </param>
		/// 
		/// <param name="table_name">
		/// Specifies the table name.
		/// </param>
		/// 
		/// <param name="error">
		/// Collects the error, if it occurs.
		/// </param>
		/// 
		/// <returns>
		/// Returns true is successful.
		/// </returns>
		bool get_records(std::vector<row_>& rows,
			const field_& field,
			const std::string& table_name,
			std::string& error);

		/// <summary>
		/// Gets all table records.
		/// </summary>
		/// 
		/// <param name="rows">
		/// Collects the table records.
		/// </param>
		/// 
		/// <param name="table_name">
		/// Specifies the table name.
		/// </param>
		/// 
		/// <param name="error">
		/// Collects error, if it occurs.
		/// </param>
		/// 
		/// <returns>
		/// Returns true if successful.
		/// </returns>
		bool get_records(std::vector<row_>& rows,
			const std::string& table_name,
			std::string& error);

		/// <summary>
		/// Updates table row.
		/// </summary>
		/// 
		/// <param name="row_to_update_">
		/// Specifies the row to be updated.
		/// </param>
		/// 
		/// <param name="row_update">
		/// Row data for updating.
		/// </param>
		/// 
		/// <param name="table_name">
		/// Specifies the table name.
		/// </param>
		/// 
		/// <param name="error">
		/// Collects an error if it occurs.
		/// </param>
		/// 
		/// <returns>
		/// Returns true if successful.
		/// </returns>
		bool update_record(row_& row_to_update_,
			row_& row_update,
			const std::string& table_name,
			std::string& error);

		hbase();
		~hbase();
	private:
		class hbase_impl;
		hbase_impl& d_;
	};
};

/*
==============================================================================
Below is a working Exmaple, copy and paste.
==============================================================================


#include <iostream>

#include "hlib.h"
#pragma comment(lib, "hlib.lib")

using namespace hlib;
hbase db;


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
*/
//---------------------------------------------------------- -
//Copyright(c) 2020. Tawanda M.Nyoni(hkay dot tee at outlook dot com)
//
////VERSION !.0.0
//
//
//This file is part of the Hlib library which is released
//under the Creative Commons Attribution Non - Commercial
//2.0 Generic license(CC BY - NC 2.0).
//
//See accompanying file CC - BY - NC - 2.0.txt
//
//Below is a short summary of what this license means.
//
//Under this license, you are free to :
//------------------------------------
//SHARE : copy and redistribute the material in any medium or format.
//	ADAPT : remix, transform, and build upon the material.
//
//	Under the following terms :
//--------------------------
//ATTRIBUTION : You must give appropriate credit, provide a link to the
//	license, and indicate if changes were made.You may do so in any
//	reasonable manner, but not in any way that suggests the licensor endorses
//	you or your use.
//	NON - COMMERCIAL : You may not use the material for commercial purposes.For
//	a commercial license contact the copyright holder using the contact
//	information provided in this file.
//	NO ADDITIONAL RESTRICTIONS : You may not apply legal terms or technological
//	measures that legally restrict others from doing anything the license
//	permits.
//----------------------------------------------------------------------------------

#include "hlib.h"

#include "sqlite3.h"
#include "picosha2.h"

using table = std::vector<std::map<std::string, std::string>>;

class hlib::hbase::hbase_impl {
	friend hbase;

	bool connected_;
	sqlite3* db_;

public:
	hbase_impl() :
		connected_(false),
		db_(nullptr) {}

	~hbase_impl() {
		if (db_) {
			// close database
			sqlite3_close(db_);
			db_ = nullptr;
		}
	}

	std::string sqlite_error(int error_code) {
		std::string error = sqlite3_errstr(error_code);
		if (error == "not an error") error.clear();
		if (error.length() > 0) error[0] = toupper(error[0]);
		return error;
	}

	std::string sqlite_error() {
		if (db_) {
			std::string error = sqlite3_errmsg(db_);
			if (error == "not an error") error.clear();
			if (error.length() > 0) error[0] = toupper(error[0]);
			return error;
		}
		else {
			return "Database not open"; return false;
		}
	}

	bool sqlite_query(const std::string& query,
		table& table,
		std::string& error) {
		table.clear();

		if (db_) {
			sqlite3_stmt* statement = nullptr;

			if (sqlite3_prepare_v2(db_, query.c_str(), -1, &statement, 0) == SQLITE_OK) {
				const int columns = sqlite3_column_count(statement);

				while (true) {
					if (sqlite3_step(statement) == SQLITE_ROW) {
						std::map<std::string, std::string> values;

						for (int column = 0; column < columns; column++) {
							std::string column_name, value;

							// get column name
							char* ccColumn = (char*)sqlite3_column_name(statement, column);

							if (ccColumn) {
								column_name = ccColumn;

								// get data
								char* ccData = (char*)sqlite3_column_text(statement, column);

								if (ccData)
									value = ccData;

								values.insert(std::make_pair(column_name, value));
							}
						}

						table.push_back(values);
					}
					else
						break;
				}
				sqlite3_finalize(statement);
			}
			else {
				error = sqlite_error();
				return false;
			}

			return true;
		}
		else {
			error = "Database not open";
			return false;
		}
	}

	std::string type_to_string(hbase::column_type_ type) {

		std::string _type;
		switch (type)
		{
		case column_type_::blob_:
			_type = "BLOB";
			break;
		case column_type_::float_:
			_type = "FLOAT";
			break;
		case column_type_::integer_:
			_type = "INTEGER";
			break;
		case column_type_::text_:
		default:
			_type = "TEXT";
			break;
		}
		return _type;
	}

	std::string constraint_to_string(constraint_ constraint) {

		std::string _constraint;
		switch (constraint)
		{
		case constraint_::not_null:
			_constraint = "NOT NULL";
			break;
		default:
			_constraint = "NULL";
			break;
		}
		return _constraint;
	}

};

bool hlib::hbase::connect(const file_& file,
	std::vector<table_>& tables,
	std::string& error) {

	if (d_.connected_) 
		return true;

	int error_code = 0;
	if (file.password.empty()) {
		error_code = sqlite3_open_v2(file.name.c_str(), &d_.db_,
			SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX, NULL);
		error_code = sqlite3_exec(d_.db_, "SELECT count(*) FROM sqlite_master;", NULL, NULL, NULL);
	}
	else {
		error_code = sqlite3_open_v2(file.name.c_str(), &d_.db_,
			SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX, NULL);

		sqlite3_stmt* stm = nullptr;
		const char* pzTail = nullptr;

		// key the database
		auto pragma_string = "PRAGMA key = '" + file.password + "';";
		sqlite3_prepare(d_.db_, pragma_string.c_str(), -1, &stm, &pzTail);
		sqlite3_step(stm);
		sqlite3_finalize(stm);

		// test if key is correct
		error_code = sqlite3_exec(d_.db_, "SELECT count(*) FROM sqlite_master;", NULL, NULL, NULL);
	}

	if (error_code != SQLITE_OK) {
		// an error occured
		error = d_.sqlite_error(error_code);

		// close database
		sqlite3_close(d_.db_);
		d_.db_ = nullptr;
		return false;
	}

	// create tables
	table table;
	for (const auto& table_ : tables) {
		std::string sql = "CREATE TABLE " + table_.name + "(";

		for (const auto& col : table_.columns) 
			sql += col.name + " " + d_.type_to_string(col.type) + " " + d_.constraint_to_string(col.constraint) + ",";
		
		std::string composite_key;
		size_t count_keys = 1;
		for (const auto key : table_.primary_key) {	
			composite_key += key;

			if (count_keys < table_.primary_key.size()) {
				composite_key += ',';
			}
			count_keys++;
		}

		sql += "PRIMARY KEY (" + composite_key + "));";

		if (!d_.sqlite_query(sql, table, error)) 
			if (error.find("already exists") == std::string::npos) 
				return false;
		
		sql.clear();
	}

	d_.connected_ = true;
	return true;
}

bool hlib::hbase::insert_row(std::vector<field_>& row,
	const std::string& table_name,
	std::string& error) {

	if (!d_.connected_) {
		error = "Not connected to database"; 
		return false;
	}

	size_t index = 1;
	std::string colums;
	std::string values;

	for (const auto& field : row) {
		colums += field.name;
		values += "'"+ field.value +"'";

		if (index < row.size()) {
			colums += ",";
			values += ",";
		}
		index++;
	}

	std::string sql = "INSERT INTO " + table_name + "(";
	sql += (colums + ") VALUES (" + values + ");");

	table table;
	if (!d_.sqlite_query(sql, table, error)) 
		return false;
	
	return true;
}

bool hlib::hbase::delete_row(const field_& field,
	const std::string& table_name,
	std::string& error) {

	if (!d_.connected_) {
		error = "Not connected to database"; 
		return false;
	}

	table table_;
	std::string sql = "DELETE FROM " + table_name;
	sql += " WHERE " + field.name + " = '" + field.value + "';";

	if (!d_.sqlite_query(sql ,table_, error)) 
		return false;

	return true;
}

bool hlib::hbase::count_records(const field_& field, 
	const std::string& table_name,
	size_t& records,
	std::string& error) {

	if (!d_.connected_) {
		error = "Not connected to database";
		return false;
	}

	table table_;
	std::string sql = "SELECT COUNT(*) FROM  " + table_name;
	sql += " WHERE " + field.name + " = '" + field.value + "';";

	if (!d_.sqlite_query(sql, table_, error)) 
		return false;

	try {
		if (!table_.empty())
			records = std::atoi(table_[0].at("COUNT(*)").c_str());
		else {
			error = "The table is empty!";
			return false;
		}
		return true;
	}
	catch (const std::exception & e) {
		error = e.what();
		return false;
	}
}

bool hlib::hbase::count_records(const std::string& table_name,
	size_t& records,
	std::string& error) {

	if (!d_.connected_) {
		error = "Not connected to database";
		return false;
	}

	table table_;
	std::string sql = "SELECT COUNT(*) FROM  '" + table_name + "';";
	if (!d_.sqlite_query(sql, table_, error)) 
		return false;

	try {
		if (!table_.empty())
			records = std::atoi(table_[0].at("COUNT(*)").c_str());
		else {
			error = "The table is empty!";
			return false;
		}
		return true;
	}
	catch (const std::exception & e) {
		error = e.what();
		return false;
	}
}

bool hlib::hbase::get_records(table& records,
	const std::vector<field_>& compound_keys,
	const std::string& table_name,
	std::string& error) {

	if (!d_.connected_) { 
		error = "Not connected to database"; 
		return false; 
	}

	std::string keys;
	int indx = 1;
	for (auto key : compound_keys) {
		keys += key.name + " = '" + key.value +"'";
		if (indx < compound_keys.size())
			keys += " AND ";
		indx++;
	}

	table table_;
	std::string sql = "SELECT * FROM " + table_name;
	sql += " WHERE " + keys + ";";

	if (!d_.sqlite_query(sql, table_, error)) 
		return false;

	if (!table_.empty()) {
		for (const auto& row : table_) 
			records.push_back(row);
		return true;
	}
	else {
		error = "the table is empty!";
		return false;
	}
}


bool hlib::hbase::get_records(table& records,
	const std::string& table_name,
	std::string& error) {

	if (!d_.connected_) {
		error = "Not connected to database";
		return false;
	}

	table table_;
	const std::string sql = "SELECT * FROM '" + table_name + "';";
	if (!d_.sqlite_query(sql, table_, error))
		return false;

	if (!table_.empty()) {
		for (const auto row : table_)
			records.push_back(row);
		return true;
	}
	else {
		error = "the table is empty!";
		return false;
	}
}


bool hlib::hbase::get_records_with_sort_by(table& records,
	const field_& sort_by_field,
	const std::string& table_name,
	std::string& error) {

	if (!d_.connected_) {
		error = "Not connected to database";
		return false;
	}

	table table_;
	std::string sql = "SELECT * FROM " + table_name;

	sql += " ORDER BY '" + sort_by_field.name + "';";

	if (!d_.sqlite_query(sql, table_, error))
		return false;

	if (!table_.empty()) {
		for (const auto& row : table_)
			records.push_back(row);
		return true;
	}
	else {
		error = "the table is empty!";
		return false;
	}
}

bool hlib::hbase::get_records_with_and_sort_by(table& records,
	const std::vector<field_>& compound_keys,
	const field_& sort_by_field,
	const std::string& table_name,
	std::string& error) {

	if (!d_.connected_) {
		error = "Not connected to database";
		return false;
	}

	std::string keys;
	int indx = 1;
	for (auto key : compound_keys) {
		keys += key.name + " = '" + key.value + "'";
		if (indx < compound_keys.size())
			keys += " AND ";
		indx++;
	}

	table table_;
	std::string sql = "SELECT * FROM " + table_name;
	sql += " WHERE " + keys + " ORDER BY '" + sort_by_field.name + "';";

	if (!d_.sqlite_query(sql, table_, error))
		return false;

	if (!table_.empty()) {
		for (const auto& row : table_)
			records.push_back(row);
		return true;
	}
	else {
		error = "the table is empty!";
		return false;
	}
}

bool hlib::hbase::get_records_using_custom_query(table& records,
	const std::string& custom_query_statement,
	std::string& error) {

	if (!d_.connected_) {
		error = "Not connected to database";
		return false;
	}

	table table_;
	if (!d_.sqlite_query(custom_query_statement, table_, error))
		return false;

	if (!table_.empty()) {
		for (const auto& row : table_)
			records.push_back(row);
		return true;
	}
	else {
		error = "the table is empty!";
		return false;
	}
}

bool hlib::hbase::get_records_with_or_sort_by(table& records,
	const std::vector<field_>& compound_keys,
	const field_& sort_by_field,
	const std::string& table_name,
	std::string& error) {

	if (!d_.connected_) {
		error = "Not connected to database";
		return false;
	}

	std::string keys;
	int indx = 1;
	for (auto key : compound_keys) {
		keys +=  key.name + " = '" + key.value + "'";
		if (indx < compound_keys.size())
			keys += " OR ";
		indx++;
	}

	table table_;
	std::string sql = "SELECT * FROM " + table_name;
	sql += " WHERE " + keys + " ORDER BY '" + sort_by_field.name + "';";

	if (!d_.sqlite_query(sql, table_, error))
		return false;

	if (!table_.empty()) {
		for (const auto& row : table_)
			records.push_back(row);
		return true;
	}
	else {
		error = "the table is empty!";
		return false;
	}
}



bool hlib::hbase::custom_query(const std::string& custom_query_, std::string& error) {

	if (!d_.connected_) {
		error = "Not connected to database";
		return false;
	}

	table table_;
	if (!d_.sqlite_query(custom_query_, table_, error))
		return false;

	if (!table_.empty()) 
		return true;
	else {
		error = "the table is empty!";
		return false;
	}
}

bool hlib::hbase::update_record(const field_ field,
	std::vector<field_>& row_update,
	const std::string& table_name,
	std::string& error) {

	if (!d_.connected_) { 
		error = "Not connected to database"; 
		return false; 
	}

	size_t index = 1;
	std::string fields;
	for (auto field_ : row_update) {
		fields += field_.name + " = '" + field_.value + "'";

		if (index < row_update.size())
			fields += ",";
		index++;
	}

	table table_;
	std::string sql = "UPDATE " + table_name + " SET ";
	sql += fields + " WHERE " + field.name + " = '" + field.value +"';";
;
	if (!d_.sqlite_query(sql, table_, error)) 
		return false;

	return true;
}

hlib::hbase::hbase() :
	d_(*new hbase_impl()) {}

hlib::hbase::~hbase() {
	delete& d_;
}

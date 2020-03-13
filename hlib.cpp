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
		std::string& error)
	{
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

bool hlib::hbase::connect(const database_file_& file,
	tables_& tables,
	std::string& error) {

	if (d_.connected_) return true;

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
	std::string sql = "CREATE TABLE ";
	for (const auto& _table : tables.get()) {
		sql += _table.name + "(";
		for (const auto& col : _table.columns) {
			sql += col.name + " " + d_.type_to_string(col.type) + " " + d_.constraint_to_string(col.constraint) + ",";
		}
		sql += "PRIMARY KEY (" + _table.primary_key + "));";
	}

	if (!d_.sqlite_query(sql, table, error)) {
		if (error.find("already exists") == std::string::npos) return false;
	}

	d_.connected_ = true;
	return true;
}

bool hlib::hbase::insert_row(row_& row, 
	const std::string& table_name,
	std::string& error) {


	if (!d_.connected_) {
		error = "Not connected to database"; return false;
	}

	std::string sql = "INSERT INTO " + table_name + "(";

	size_t index = 1;
	for (const auto& fields : row.fields) {
		sql += "'" + fields.name + "'";

		if (index < row.fields.size()) {
			sql += ",";
		}

		index++;
	}

	sql += ") VALUES (";
	index = 1;
	for (const auto& fields : row.fields) {
		sql += "'"+ fields.item + "'";

		if (index < row.fields.size()) {
			sql += ",";
			index++;
		}
	}

	sql += ");";
	table table;
	if (!d_.sqlite_query(sql, table, error)) {
		if (error.find("already exists") == std::string::npos) return false;
	}

	return true;
}

bool hlib::hbase::delete_row(field_& field,
	const std::string& table_name,
	std::string& error) {

	if (!d_.connected_) {
		error = "Not connected to database"; return false;
	}

	table table_;
	const std::string sql = "DELETE FROM " + table_name + " WHERE " + field.name + " = '" + field.item + "';";
	if (!d_.sqlite_query(sql ,table_, error)) return false;

	return true;
}

bool hlib::hbase::count_records(field_& field, 
	const std::string& table_name,
	size_t& records,
	std::string& error) {

	table table_;
	const std::string sql = "SELECT COUNT(*) FROM  " + table_name + " WHERE "+ field.name +" = '"+ field.item +"';";
	if (!d_.sqlite_query(sql, table_, error)) return false;

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
	return true;
}

bool hlib::hbase::count_records(const std::string& table_name,
	size_t& records,
	std::string& error) {

	table table_;
	std::string sql = "SELECT COUNT(*) FROM  '" + table_name + "';";
	if (!d_.sqlite_query(sql, table_, error)) return false;

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

	return true;
}

bool hlib::hbase::get_records(std::vector<row_>& rows,
	const field_& field,
	const std::string& table_name,
	std::string& error){

	if (!d_.connected_) { error = "Not connected to database"; return false; }

	table table_;
	const std::string sql = "SELECT * FROM " + table_name + " WHERE " + field.name + " = '" + field.item + "';";
	if (!d_.sqlite_query(sql, table_, error)) return false;

	if (!table_.empty()) {
		for (const auto& _row : table_) {
			row_ row = {};
			field_ field = {};

			for (const auto& item : _row) {
				field.name = item.first;
				field.item = item.second;
				row.fields.push_back(field);
			}
			rows.push_back(row);
		}
	}
	else {
		error = "the table is empty!";
		return false;
	}

	return true;
}

bool hlib::hbase::get_records(std::vector<row_>& rows,
	const std::string& table_name,
	std::string& error) {

	if (!d_.connected_) { error = "Not connected to database"; return false; }

	table table_;
	const std::string sql = "SELECT * FROM '" + table_name + "';";
	if (!d_.sqlite_query(sql, table_, error)) return false;

	if (!table_.empty()) {
		for (const auto& _row : table_) {
			row_ row = {};
			field_ field = {};

			for (const auto& item : _row) {
				field.name = item.first;
				field.item = item.second;
				row.fields.push_back(field);
			}
			rows.push_back(row);
		}
	}
	else {
		error = "the table is empty!";
		return false;
	}

	return true;
}

bool hlib::hbase::update_record(row_& row_to_update,
	row_& row_update,
	const std::string& table_name,
	std::string& error) {

	if (!d_.connected_) { error = "Not connected to database"; return false; }

	table table_;
	std::string sql = "UPDATE " + table_name + " SET ";

	size_t index = 1;
	for (auto field_to_update : row_to_update.fields) {
		for (auto field_update : row_update.fields) {

			if (field_to_update.name == field_update.name) {
				sql += field_to_update.name+ " = '" +field_update.item+ "'";

				if (index < row_update.fields.size()) {
					sql += ",";
					index++;
				}
			}
		}
	}

	sql += ";";
	if (!d_.sqlite_query(sql, table_, error)) return false;

	return true;
}
hlib::hbase::hbase() :
	d_(*new hbase_impl()) {}

hlib::hbase::~hbase() {
	delete& d_;
}
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

#pragma once

//---------------------------------------------------------- -
//Copyright(c) 2020. Tawanda M.Nyoni(hkay dot tee at outlook dot com)
//
//VERSION !.0.0
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

#endif // _WIN64.


#include <string>
#include <vector>
#include <map>

namespace hlib {
	class HLIB_API hbase {
	public:

		struct file_ {
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

		struct table_ {
			std::string name;
			std::vector<column_> columns;
			std::vector<std::string> primary_key;
		};

		struct field_ {
			std::string name;
			std::string value;
		};

	public:
		bool connect(const file_& file,
			std::vector<table_>& tables,
			std::string& error);

		bool insert_row(std::vector<field_>& row,
			const std::string& table_name,
			std::string& error);

		bool delete_row(const field_& field,
			const std::string& table_name,
			std::string& error);

		bool count_records(const field_& field,
			const std::string& table_name,
			size_t& records,
			std::string& error);

		bool count_records(const std::string& table_name,
			size_t& records,
			std::string& error);

		using table = std::vector<std::map<std::string, std::string>>;
		bool get_records(table& records,
			const std::vector<field_>& compound_keys,
			const std::string& table_name,
			std::string& error);

		bool get_records_with_sort_by(table& records,
			const field_& field_sort_by,
			const std::string& table_name,
			std::string& error);

		bool get_records_with_and_sort_by(table& records,
			const std::vector<field_>& compound_keys,
			const field_& field_sort_by,
			const std::string& table_name,
			std::string& error);

		bool get_records_with_or_sort_by(table& records,
			const std::vector<field_>& compound_keys,
			const field_& field_sort_by,
			const std::string& table_name,
			std::string& error);

		bool get_records(table& records,
			const std::string& table_name,
			std::string& error);

		bool get_records_using_custom_query(table& records,
			const std::string& custom_query_statement,
			std::string& error);

		bool custom_query(const std::string& custom_query_, std::string& error);
		bool update_record(const field_ field,
			std::vector<field_>& row_update,
			const std::string& table_name,
			std::string& error);

		hbase();
		~hbase();

		hbase(hbase&) = delete;
		hbase operator=(hbase&) = delete;
	private:
		class hbase_impl;
		hbase_impl& d_;
	};
};
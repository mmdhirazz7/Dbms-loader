#pragma once
#include <pqxx/pqxx>
#include <string>
#include <vector>

typedef struct{
	int index;
	std::string title;
	std::string abstract;
	std::string venue;
	int year;
	std::vector<std::string> list_of_authors;
	std::vector<int> references;
}Info;

extern pqxx::connection db;
//pqxx::work W(db);

extern int total_papers;

//int search(const std::string table_name, const std::string search_condition, const std::string  pk_name);

Info parse(std::ifstream &);

void parse_info_struct(Info whole_info);

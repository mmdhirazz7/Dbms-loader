/*
 * Step 1: Parsing => Builds a struct Info by reading from file //easy
 * Step 2: Checking => Takes struct Info as input: creates necessary new structs //complicated
 * */

#include <cstdlib>
#include <ctime>
#include <pqxx/pqxx>
#include <iostream>
#include <fstream>
#include "header.h"

pqxx::connection db("dbname = research_papers_complete user = postgres password = cs20btech11012 hostaddr = 127.0.0.1 port = 5432");

void init_db(){
      char create_paper_table[] = "CREATE TABLE Paper("  \
      "index 			INT 	 PRIMARY KEY 		 		   NOT NULL," \
      "title            TEXT    							   NOT NULL," \
      "venue_id         INT 	references Venue(venue_id)     NOT NULL," \
	  "year				INT 								   NOT NULL,"\
      "abstract         TEXT );";

      char create_author_table[] = "CREATE TABLE Author("  \
      "author_id		SERIAL 		   PRIMARY KEY  NOT NULL," \
      "fname            TEXT    					NOT NULL," \
      "mname         	TEXT 				    			," \
      "lname        	TEXT								 );";

      char create_venue_table[] = "CREATE TABLE Venue("  \
      "venue_id			SERIAL 	PRIMARY KEY     NOT NULL," \
      "location        	TEXT );";

	  //the relation tables

      char create_authoredBy_table[] = "CREATE TABLE AuthoredBy("  \
	  "paper_id			INT 		  NOT NULL," \
	  "author_id		INT 		  NOT NULL," \
      "priority        	INT 		  NOT NULL,"\
	  "PRIMARY KEY (paper_id, author_id));";

      char create_references_table[] = "CREATE TABLE Reference("  \
      "paper_id						INT 	      NOT NULL," \
	  "reference_paper_id			INT 	      CHECK(paper_id != reference_paper_id),"\
	  "PRIMARY KEY (paper_id, reference_paper_id));";

	pqxx::work Work(db) ;

	Work.exec(create_author_table);
	Work.exec(create_venue_table);
	Work.exec(create_paper_table);

	//relation tables
	Work.exec(create_authoredBy_table);
	Work.exec(create_references_table);

	Work.commit();

	printf("Tables succesfully created\n");
}



int main(){

	//opening the file
	std::ifstream file;
	int total_papers = 0;
	file.open("source.txt");
	std::string temp_count;

	getline(file,temp_count);

	total_papers = atoi(temp_count.c_str());


	std::cout << "Total papers: " << total_papers << std::endl;

	if (db.is_open()){
		printf("Opened database succesfully: %s\n",db.dbname());
		init_db();
		int count = 0;
		//pqxx::work W(db);
		while (count < total_papers) {
			Info parsed_struct = parse(file);
			parse_info_struct(parsed_struct );
			count++;
		}


		std::cout << "succesfully entered " << total_papers << " papers \n";

	}
	else {
		printf("Cannot open database\n");
		exit(1) ;
	}


	db.disconnect();


}

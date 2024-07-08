#include "header.h"
#include <iostream>
#include <pqxx/transaction.hxx>
#include <string>
#include <map>


typedef struct Authorname {
	std::string fname;
	std::string mname;
	std::string lname;
	bool operator<(const Authorname& smk) const
    {
        if (fname < smk.fname)
        {
            return true;
        } else if (fname == smk.fname)
        {
            if (mname < smk.mname)
            {
                return true;
            }
            else if (mname == smk.mname)
            {
                if(lname < smk.lname)
				{
					return true;
				}
				else if(lname == smk.lname)
				{
					return false;
				}
				return false;
            }
			return false;
        }
        return false;
    }
}Authorname;



std::map<std::string,int> Venue;
std::map<Authorname,int> Author;
std::map<std::string,int> Paper;

template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}




void prepare_queries()
{
    db.prepare(
        "paper_check",
        "SELECT index FROM Paper "\
        "WHERE title=$1; ");
    db.prepare(
        "paper_enter",
        "INSERT INTO Paper "\
        "VALUES ($1, $2, $3, $4, $5)"\
		"RETURNING index;");
    db.prepare(
        "author_check",
        "SELECT author_id FROM Author "\
        "WHERE fname=$1 and mname=$2 and lname=$3 ;");
    db.prepare(
        "author_enter",
        "INSERT INTO Author (author_id,fname,mname,lname)"\
        "VALUES (DEFAULT,$1, $2, $3)"\
        "RETURNING author_id;");
    db.prepare(
        "AuthoredBy",
        "INSERT INTO AuthoredBy (paper_id, author_id, priority) "\
        "VALUES ($1, $2, $3);");
    db.prepare(
        "venue_check",
        "SELECT venue_id FROM venue "\
        "WHERE location=$1 ;");
    db.prepare(
        "venue_entry",
        "INSERT INTO venue (venue_id, location)"\
        "VALUES (DEFAULT,$1)"\
        "RETURNING venue_id;");
    db.prepare(
        "reference",
        "INSERT INTO reference "\
        "VALUES ($1, $2);");

}

void parse_info_struct(Info whole_info){
	pqxx::work W(db);

	//all create statements

	static int papers_count = -1;

	prepare_queries();

	//check if the paper is already in the table

	papers_count++;
	pqxx::result R;

	//R  = W.prepared("paper_check")(whole_info.title).exec();

	int paper_id ;
	/*
	auto i_t = Paper.find(whole_info.title);

	if(! (i_t == Paper.end()) )
	{
		//std::cout << "Paper with title " << whole_info.title << " is already present\n ";
		return;
	}
	*/
	//check if the venue of this paper are already present


	//searching in map
	auto it_ = Venue.find(whole_info.venue);
	int venue_id = it_->second;

	if (it_ == Venue.end() && whole_info.venue!=it_->first) {
		//this venue is not present in the table
		//create new venue and insert it into the table and to the map

		R = W.prepared("venue_entry")(whole_info.venue).exec();
		venue_id = R.begin().begin().as<int>();
		Venue.insert({whole_info.venue, venue_id});
	}

	//creating new entry in the paper table

	R = W.prepared("paper_enter")(whole_info.index)(whole_info.title)(venue_id)(whole_info.year)(whole_info.abstract).exec();
	//int newPaper = insert_paper(whole_info.index, whole_info.title, venue_id, whole_info.year, whole_info.abstract);
	paper_id = R.begin().begin().as<int>();
	Paper.insert({whole_info.title,paper_id});
	//std::cout << "Inserted paper " << R.begin().begin().as<int>() << std::endl;

	//dealing with authors


	int author_id;
	int author_prio = 0;
	//std::cout << whole_info.list_of_authors.size() << std::endl;
	for(auto author: whole_info.list_of_authors){
		std::vector<std::string> names = split(author, ' ');
		std::string fname, mname, lname;
		if(names.size() == 1){
			fname = names[0];
			mname = "NULL";
			lname = "NULL";
		}
		else if (names.size() == 2) {
			//no middle name
			fname = names[0];
			lname = names[1];
			mname = "NULL";
		}
		else if(names.size() == 3){
			fname = names[0];
			lname = names[2];
			mname = names[1];
		}
		else{
			//there are more then 3 words
			fname = names[0];
			lname = names.back();

			for(int i=1; i<names.size()-1; i++){
				mname.append(names[i]);
			}
		}

		//searching in map
	    Authorname authorname = {fname,mname,lname};
		auto _it = Author.find(authorname);
		if (_it == Author.end() && (_it->first.fname != fname || _it->first.mname != mname || _it->first.lname!= lname )) {
			//need to insert this author into the author table and into the map
			 R = W.prepared("author_enter")(fname)(mname)(lname).exec();
			 author_id = R.begin().begin().as<int>();
			 Author.insert({authorname, author_id});
			 //author_id = insert_author(fname, mname, lname);
		}
		else
			author_id = _it->second;

		//insert a new element into the AuthoredBy table

		if(author != "no author"){ author_prio++; }
		W.prepared("AuthoredBy")(paper_id)(author_id)(author_prio).exec();
		//std::cout << "Inserting " << paper_id << "and" << author_id << "\n";
		//insert_authoredBy(author_id,  papers_count, author_prio);
	}

	//inserting reference papers

	for(auto ref_paper: whole_info.references){
		W.prepared("reference")(papers_count)(ref_paper).exec();
		//insert_references(papers_count, ref_paper);
	}

	W.commit();

}

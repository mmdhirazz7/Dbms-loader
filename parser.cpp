#include<bits/stdc++.h>
#include<string.h>
#include "header.h"

using namespace std;



//this function removes the empty strings and any duplicates if found
std::vector<std::string> remove_duplicates(std::vector<std::string> input){
	//removing empty strings

	std::vector<std::string> output;
	for(std::string element : input){
		if(element.size() != 0){
			output.push_back(element);
		}
	}


	//removing duplicates
	for(int i=0; i<output.size(); i++){
		for(int j=i+1; j<output.size(); j++){
			if(output[i] == output[j]){
				for(int k=j; k<output.size()-1; k++){
					std::swap(output[k], output[k+1]);
				}
				output.pop_back();
			}
		}
	}


	return output;

}

//removing self references

std::vector<int> remove_self_ref(int index, std::vector<int> references){
	for(int i=0; i<references.size(); i++){
		if (references[i] == index) {
			std::swap(references[i], references.back());
			references.pop_back();
			i--;
		}
	}

	return references;
}


int n ;
Info parse (ifstream& f)
{
    Info information;
    information.index=0;
    information.abstract="no abstract";
    information.venue="no venue";
    information.year = 0;

    string s;

    while( true)

    {
        getline(f,s);
        if ( s=="\0")
        {
            break;

        }
        if ( s[1] == 't') //year
        {
            if (  s.size() == 2)
            {
                information.year = 0;

            }
            else
            {
             information.year = stoi (s.substr(2,4 ));
               // information.year = 0;

            }

        }
        else if( s[1]== 'i' ) //index
        {
            information.index =  stoi (  s.erase(0 , 6));
           // information.index=0;


        }
        else if ( s[1] == '*') //title
        {
            information.title = s.erase ( 0,2);
        }
        else if( s[1]== 'c') //venue
        {
            if ( s.size() == 2)
            {
                information.venue="no venue";

            }
            else
            {
                information.venue = s.erase( 0,2);
            }

        }
        else if( s[1] == '!' ) //abstract
        {
            if ( s.size() == 2)
            {
                information.abstract="no abstract";

            }
            else
            {
                information.abstract = s.erase(0,2);
            }
        }
        else if ( s[1] == '@') //authors
        {
            if ( s.size() == 2)
            {
                information.list_of_authors.push_back("no author");
            }
            else
            {
                int i = 2 ;
                s.push_back(',');
                string c ;
                while ( s[i]!='\0' )
                {
                    while ( s[i] != ',')
                    {
                        c = c + s[i];
                        i++;
                    }
                    if( c != " Jr.")
                    {
                        information.list_of_authors.push_back(c);
                    }
                    else
                    {
                        information.list_of_authors.back() += ","+c;
                       // information.list_of_authors.pop_back();
                        //information.list_of_authors.push_back(p);

                    }
                   // information.list_of_authors.push_back(c);
                    c.erase();
                    i++;
                }
            }
			information.list_of_authors = remove_duplicates(information.list_of_authors);
        }
       else if( s[1] == '%') //REFERENCES
        {
            information.references.push_back ( stoi(s.erase(0,2)) );
        }
	   information.references = remove_self_ref(information.index, information.references);


    }
    return information;

}


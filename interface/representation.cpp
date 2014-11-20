#include "representation.h"
#include <map>
#include <iostream>


using namespace std;

Knit knit_parsed;

// To build trapezoid in the parser

Trapezoid::Trapezoid(std::map<string,string> parameters) {
    height = get_argument_int("height",parameters);
    shift = get_argument_int("shift",parameters);
    lower_width = get_argument_int("lower_width",parameters);
    upper_width = get_argument_int("upper_width",parameters);
    Pattern *pat = new Pattern(get_argument("pattern",parameters));
    pattern = pat;
}

string get_argument(string arg, map<string,string> param_map) {
	map<string,string>::iterator it = param_map.find(arg);
	if (it != param_map.end()) {
            return it->second;
        }
	else {
	cerr << "Argument " << arg << " not found" << endl ;
	exit(1);		
	}
}

int get_argument_int(string arg, map<string,string> param_map) {
	string find = get_argument(arg,param_map);
        return atoi(find.c_str());
}

// Printers


ostream& operator <<(ostream &os, Element *element) {
    element->print(os);
    return os;
}


ostream& operator <<(ostream &os, map<string,Element*> const& elements) {
    for (map<string,Element*>::const_iterator it = elements.begin(); it!= elements.end(); ++it) {
        os << "piece " << it->first << " := start || ";
        (it->second)->print(os);
        os << endl << endl;
    }
    return os;
}

ostream& operator <<(ostream &os, Knit knit) {
    os << "Name : " << knit.name << endl << "Description : " << knit.description << "\n \n" << knit.elements;
    return os;
}


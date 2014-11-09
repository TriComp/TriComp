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
    Pattern *pat = new Pattern(get_argument("pattern",parameters)) ;
    pattern = pat ;
}

string get_argument(string arg, map<string,string> param_map) {
	map<string,string>::iterator it = param_map.find(arg);
	if (it != param_map.end()) {
        	return it->second ;
        }
	else {
	fprintf(stderr, "Argument not found");
	exit(1);		
	}
}

int get_argument_int(string arg, map<string,string> param_map) {
	string find = get_argument(arg,param_map);
        return atoi(find.c_str()) ;
}

// Printer functions

ostream& operator <<(ostream &os, Slot slot) {
    if (slot == Slot::Right) {
        os << "right" ;
    }
    else {
        os << "left" ;
    }
    return os;
}

ostream& operator <<(ostream &os, Trapezoid geom) {
    os << "Trapezoid( height : " << geom.height
                    << ", shift : " << geom.shift 
		    << ", upper_width : " << geom.upper_width
                    << ", lower_width : " << geom.lower_width
		    << ", pattern : " << (geom.pattern)->name ;
    return os; 
}

ostream& operator <<(ostream &os, Element* elt) {
    return (*elt).print(os);
}

ostream& operator <<(ostream &os, map<string,Element*> elements) {
    for (map<string,Element*>::iterator it = elements.begin(); it!=elements.end(); ++it) {
        os << it->first << " : " << it->second << endl ; 
    }
    return os ;
}

ostream& operator <<(ostream &os, Knit knit) {
    os << "Name : " << knit.name << "\n Description : " << knit.description << " \n Elements : \n " << knit.elements ;
    return os;
}


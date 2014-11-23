#include "representation.h"
#include <map>
#include <iostream>

using namespace std;

Knit knit_parsed;

QBrush setFirstBrush(string name) {
    if (name.compare("garter") == 0) {
        return QBrush(garter_stitch.brush);
    }
    else {
        return QBrush("#444"); // default
    }
}

// Printers

ostream& operator<<(ostream& os, Element* element)
{
    element->print(os);
    return os;
}

ostream& operator<<(ostream& os, map<string, Element*> const& elements)
{
    for (map<string, Element*>::const_iterator it = elements.begin(); it != elements.end(); ++it) {
        os << "piece " << it->first << " := start || ";
        (it->second)->print(os);
        os << endl << endl;
    }
    return os;
}

ostream& operator<<(ostream& os, Knit knit)
{
    os << "Name : " << knit.name << endl << "Description : " << knit.description << "\n \n" << knit.elements;
    return os;
}

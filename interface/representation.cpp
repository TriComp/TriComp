#include "representation.h"
#include <map>
#include <iostream>
#include <utility>

#include "editor.h"

using namespace std;

Knit knit_parsed;

QBrush setFirstBrush(string name)
{
    if (name == "garter") {
        return QBrush(garter_stitch.brush);
    }
    else if (name == "jersey") {
        return QBrush(jersey_stitch.brush);
    }
    else {
        return QBrush("#444"); // default
    }
}

Pattern* constructMyBrush()
{
    QPixmap pix(3,4);
    return new Pattern("my_stitch", QBrush(pix));
}

// Printers

ostream& operator<<(ostream& os, Element* element)
{
    element->print(os);
    return os;
}

ostream& operator<<(ostream& os, map< string, pair< int, Element*> > const& elements)
{
    for (map<string, pair<int, Element*> >::const_iterator it = elements.begin(); it != elements.end(); ++it) {
        os << "piece " << it->first << " := start ";
        os << (it->second).first;
        os << " || ";
        ((it->second).second)->print(os);
        os << endl << endl;
    }
    return os;
}

ostream& operator<<(ostream& os, Knit knit)
{
    os << "Name : " << knit.name << endl << "Description : " << knit.description << "\n \n" << knit.elements;
    return os;
}

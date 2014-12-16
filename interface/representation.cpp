
#include "representation.h"
#include <map>
#include <iostream>
#include <utility>

#include "editor.h"

using namespace std;

Knit knit_parsed;

QBrush setFirstBrush(string name)
{
    if (name == "jersey_raye") {
        return constructMyBrush(1,4,jersey_raye);
    }
    else if (name == "cotes_plates") {
        return constructMyBrush(7,2,cotes_plates);
    }
    else if (name == "cotes_piquees") {
        return constructMyBrush(10,4,cotes_piquees);
    }
    else if (name == "point_de_riz") {
        return constructMyBrush(2,2,point_de_riz);
    }
    else if (name == "fausses_cotes_anglaises") {
        return constructMyBrush(3,1,fausses_cotes_anglaises);
    }
    else if (name == "losanges") {
        return constructMyBrush(7,7,losanges);
    }
    else if (name == "cotes_1x1") {
        return constructMyBrush(2,2,cotes_1x1);
    }
    else if (name == "cotes_2x2") {
        return constructMyBrush(4,2,cotes_2x2);
    }
    else {
        return QBrush(garter_stitch.brush); // default
    }
}

QBrush constructMyBrush(int width, int height, int stitch[])
{
    int p = 3; // basic unity for the size of a stitch, depend on user screen
    QPixmap pix(width*p,height*p);
    QPainter paint(&pix);
    pix.fill(Qt::darkRed);
    for(int i=0; i<height; i++)
        for(int j=0; j<width; j++)
            if(stitch[i*width+j]== i%2)
            {
                paint.fillRect(i*p,j*p,p,p, Qt::darkGreen);
            }
    return QBrush(pix);
}

Pattern* constructMyPattern(int width, int height, int stitch[], string stitch_name)
{
    return new Pattern(stitch_name, constructMyBrush(width, height, stitch));
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

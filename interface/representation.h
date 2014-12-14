#pragma once

#include <string>
#include <vector>
#include <functional>
#include <map>
#include <list>
#include <utility>
#include <iostream>
#include <QBrush>
#include <QPainter>
#include <QPixmap>
#include <QDebug>

class EditorItem;

QBrush setFirstBrush(std::string name);

struct Pattern : QObject {
    Q_OBJECT
public:
    std::string name;
    QBrush brush;

    Pattern(std::string name, QBrush brush)
        : name(name)
        , brush(brush)
    {
    }
    Pattern(std::string name)
        : name(name)
    {
        brush = setFirstBrush(name);
    }

    ~Pattern()
    {
    }
};

// Some Knit patterns
static int jersey_raye[4] = {0,
                             1,
                             0,
                             0};
static int cotes_plates[14] = {0,0,0,0,0,1,1,
                               1,1,1,1,1,0,0,};
static int cotes_piquees[40] = {0,0,0,1,0,0,0,1,1,1,
                                1,1,1,0,1,1,1,0,0,0,
                                0,0,0,1,0,0,0,1,1,1,
                                1,1,1,1,1,1,1,1,1,1};
static int point_de_riz[4] = {0,1,
                              0,1};
static int fausse_cotes_anglaises[3] = {0,0,1};
static int losanges[49] = {0,0,0,1,0,0,0,
                           1,1,0,0,0,1,1,
                           0,1,1,1,1,1,0,
                           0,0,0,0,0,0,0,
                           0,1,1,1,1,1,0,
                           1,1,0,0,0,1,1,
                           0,0,0,1,0,0,0};


Pattern* constructMyBrush(int width, int height, int stitch[]);

// Graphical representation of different patterns  CURRENTLY USELESS
static Pattern garter_stitch("garter", QBrush(QColor("blue")));
static Pattern jersey_stitch("jersey", QBrush(QColor("blue"), Qt::Dense6Pattern));


class Trapezoid {
public:
    int height;
    int shift;
    int upper_width;
    Pattern* pattern;

    Trapezoid(std::map<std::string, std::string> parameters);
    Trapezoid(int h, int s, int u, std::string name)
        : height(h)
        , shift(s)
        , upper_width(u)
        , pattern(new Pattern(name))
    {
    }

    ~Trapezoid()
    {
        // Do not delete the pattern, for it is unique
    }
};

enum class ElementType { Trapezoid,
                         Split,
                         Link };

class Element {
public:
    const ElementType kind;
    bool visited;
    Element(ElementType k)
        : kind(k)
        , visited(false)
    {
    }
    // In subclasses, delegate to QGraphicsPolygonItem
    EditorItem* gfx;
    virtual ~Element() {}

    virtual void print(std::ostream& os) const = 0;
    virtual std::vector<Element*> children() const = 0;
    virtual int width() const = 0;
    virtual void forEach(std::function<void(Element*)> f) = 0;
};

class TrapezoidElem : public Element {
public:
    Trapezoid geom;
    Element* next;

    TrapezoidElem(Trapezoid t, Element* next)
        : Element(ElementType::Trapezoid)
        , geom(t)
        , next(next)
    {
    }

    ~TrapezoidElem()
    {
        qDebug() << "Delete next...\n";
        Q_ASSERT(next);
        //       if (next)
        //    delete next;
        qDebug() << "Deleted next...\n";
    }

    void print(std::ostream& os) const override
    {
        os << "trapezoid ( height : " << geom.height
           << ", shift : " << geom.shift
           << ", upper_width : " << geom.upper_width
           << ", pattern : " << (geom.pattern)->name
           << ")\n || ";
        next->print(os);
    }

    std::vector<Element*> children() const override { return { next }; }

    int width() const override { return geom.upper_width; } /* lower_width before */

    void forEach(std::function<void(Element*)> f) override
    {
        f(this);
        if (next)
            next->forEach(f);
    }
};

class splitData {
public:
    int position;
    int width;
    Element* next;

    splitData()
        : position(0),
          width(0),
          next(0)
    {
    }

    splitData(int position, int width, Element* next)
        : position(position),
          width(width),
          next(next)
        {
        }

    ~splitData()
    {
    }
};

class Split : public Element {
public:
    // Invariant: left != nullprt & right != nullptr
    std::list<splitData>* elements;

    Split(std::list<splitData>* elements)
        : Element(ElementType::Split)
        , elements(elements)
    {
    }

    ~Split()
    {
        /* Q_ASSERT(left && right);
        qDebug() << "Delete left...\n";
        //delete left;
        qDebug() << "Delete right...\n";
        //delete right;
        qDebug() << "Deleted both...\n"; */
    }

    void print(std::ostream& os) const override
    {
        if (elements == nullptr) {
            os << "stop";
        }
        else {
            os << "split ";
            for (std::list<splitData>::const_iterator it = elements->begin(); it != elements->end(); ++it) {
                qDebug() << "Print Split";
                os << it->position;
                os << " ";
                os << it->width;
                os << " {   ";
                (it->next)->print(os);
                os << " } ";
            }
        }
    }

    int width() const override { return 0 ; } /* TODO */

    std::vector<Element*> children() const override {
        std::vector<Element*> v;
        for (std::list<splitData>::const_iterator it = elements->begin(); it != elements->end(); ++it) {
            v.insert(v.begin(),it->next);
        }
        return v;
    }

    void forEach(std::function<void(Element*)> f) override
    {
        for (std::list<splitData>::const_iterator it = elements->begin(); it != elements->end(); ++it) {
            f(it->next);
        }
    }
};

class Link : public Element {
public:
    std::string name;
    int position;
    Link(std::string name, int position)
        : Element(ElementType::Link)
        , name(name)
        , position(position)
    {
    }

    ~Link()
    {
    }

    void print(std::ostream& os) const override
    {
        os << "link ";
        os << name;
        os << " ";
        os << position;
    }

    std::vector<Element*> children() const override { return {}; }

    int width() const override { return 100; } // hack

    void forEach(std::function<void(Element*)> f) override
    {
        f(this);
    }
};


class Knit {
public:
    std::string name;
    std::string description;
    std::map<std::string, std::pair< int, Element* > > elements;
    Knit(std::string name, std::string description, std::map<std::string, std::pair< int, Element*> > elements)
        : name(name)
        , description(description)
        , elements(elements)
    {
    }
    Knit() {}

    void destruct()
    {
        for (std::map< std::string, std::pair<int, Element*> >::const_iterator it = elements.begin(); it != elements.end(); ++it) {
            qDebug() << "Delete ...\n";
            if ((it->second).second) {
                delete (it->second).second;
            }
        }
        name = "DELETED";
        description = "DELETED";
        elements.clear();
    }
};

extern Knit knit_parsed;

// Printers

std::ostream& operator<<(std::ostream& os, Element const& element);
std::ostream& operator<<(std::ostream& os, std::map<std::string, std::pair <int, Element*> > const& elements);
std::ostream& operator<<(std::ostream& os, Knit knit);

template <typename T, typename A, typename B>
class ElementVisitor {
public:
    virtual T visitLink(Link* s, A a, B b) = 0;
    virtual T visitTrapezoid(TrapezoidElem* s, A a, B b) = 0;
    virtual T visitSplit(Split* s, A a, B b) = 0;

    T visit(Element* e, A a, B b)
    {
        switch (e->kind) {
        case ElementType::Link:
            visitLink(dynamic_cast<Link*>(e), a, b);
            break;
        case ElementType::Trapezoid:
            visitTrapezoid(dynamic_cast<TrapezoidElem*>(e), a, b);
            break;
        case ElementType::Split:
            visitSplit(dynamic_cast<Split*>(e), a, b);
        }
    }
};

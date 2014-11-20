#pragma once

#include <string>
#include <vector>
#include <functional>
#include <map>
#include <iostream>
#include <QBrush>

class EditorItem;

struct Pattern : QObject {
    Q_OBJECT
public:
    std::string name;
    QBrush brush;

    Pattern(std::string name, QBrush brush) : name(name), brush(brush) {}
    Pattern(std::string name) : name(name) {}
};

static Pattern  garter_stitch("garter", QBrush(QColor("blue")));

class Trapezoid {
public:
    int height;
    int shift;
    int upper_width;
    int lower_width;
    Pattern *pattern;

    Trapezoid(std::map<std::string,std::string> parameters);
    Trapezoid(int h, int s, int u, int l, std::string name) :
        height(h), shift(s), upper_width(u), lower_width(l), pattern(new Pattern(name)) {}

};

// Usefull function in the parser

std::string get_argument(std::string arg, std::map<std::string,std::string> param_map);
int get_argument_int(std::string arg, std::map<std::string,std::string> param_map);

enum class Slot { Left, Right };

/*
 * TODO: destructors, investigate hierachy features of GraphicsScene
 */

enum class ElementType {Trapezoid, Split, Stop, Link};

class Element {
public:
    const ElementType kind;
    Element(ElementType k) : kind(k) {}
    // In subclasses, delegate to QGraphicsPolygonItem
    EditorItem *gfx;
    virtual void print(std::ostream &os) const = 0;
    virtual std::vector<Element *> children() const  = 0;
    virtual int width() const = 0;
    virtual void forEach(std::function<void(Element *)> f) = 0;
};

class TrapezoidElem : public Element {
public:
    Trapezoid geom;
    Element *next;

    TrapezoidElem(Trapezoid t, Element *next) :
        Element(ElementType::Trapezoid), geom(t), next(next) {
    }

    void print(std::ostream &os) const override {
       os << "trapezoid ( height : " << geom.height
          << ", shift : " << geom.shift
          << ", upper_width : " << geom.upper_width
          << ", lower_width : " << geom.lower_width
          << ", pattern : " << (geom.pattern)->name
          << ")\n || ";
       next->print(os);
    }


    std::vector<Element *> children() const override { return {next}; }

    int width() const override { return geom.lower_width; }

    void forEach(std::function<void(Element *)> f) override {
        f(this);
        if (next)
            next->forEach(f);
    }
};

class Split : public Element {
public:
    // Invariant: left != nullprt & right != nullptr
    Element *left;
    Element *right;
    int gap;

    Split(Element *l, Element *r, int gap)
        : Element(ElementType::Split), left(l), right(r), gap(gap) {
    }

    void print(std::ostream &os) const override {
        os << "split " << gap << " { ";
        left->print(os);
        os << " }{ ";
        right->print(os);
        os << " }";
    }

    int width() const override { return left->width() + gap + right->width(); }

    std::vector<Element *> children() const override { return {left, right}; }

    void forEach(std::function<void(Element *)> f) override {
        f(this);
        if (left)
            left->forEach(f);
        if (right)
            right->forEach(f);
    }
};

class Link : public Element {
public:
    std::string name;
    Slot slot;
    Link(std::string name, Slot slot) : Element(ElementType::Link), name(name), slot(slot) {
    }

    void print(std::ostream &os) const override {
        os << "link ";
        if (slot == Slot::Right) {
            os << "right";
        }
        else {
            os << "left";
        }
        os << " " << name;
    }

    std::vector<Element *> children() const override { return {}; }

    int width() const override { return 100; } // hack

    void forEach(std::function<void(Element *)> f) override {
        f(this);
    }
};

class Stop : public Element {
public:
    Stop() : Element(ElementType::Stop) {
    }

    void print(std::ostream &os) const override {
        os << "stop";
    }

    std::vector<Element *> children() const override { return {}; }

    int width() const override { return 100; } // hack

    void forEach(std::function<void(Element *)> f) override {
        f(this);
    }
};

class Knit {
public:
    std::string name;
    std::string description;
    std::map< std::string,Element* > elements;
    Knit(std::string name, std::string description, std::map<std::string,Element*> elements) :
        name(name), description(description), elements(elements) {}
    Knit() {}
};

extern Knit knit_parsed;

// Printers

std::ostream& operator <<(std::ostream &os, Element const& element);
std::ostream& operator <<(std::ostream &os, std::map<std::string,Element*> const& elements );
std::ostream& operator <<(std::ostream &os, Knit knit);

template<typename T, typename A>
class ElementVisitor {
public:
    virtual T visitLink(Link *s, A a) = 0;
    virtual T visitTrapezoid(TrapezoidElem *s, A a) = 0;
    virtual T visitStop(Stop *s, A a) = 0;
    virtual T visitSplit(Split *s, A a) = 0;

    T visit(Element *e, A a) {
        switch (e->kind) {
        case ElementType::Link: visitLink(dynamic_cast<Link *>(e), a); break;
        case ElementType::Stop: visitStop(dynamic_cast<Stop *>(e), a); break;
        case ElementType::Trapezoid: visitTrapezoid(dynamic_cast<TrapezoidElem *>(e), a); break;
        case ElementType::Split: visitSplit(dynamic_cast<Split *>(e), a);
        }
    }
};

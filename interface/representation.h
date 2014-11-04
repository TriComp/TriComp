#pragma once

#include <string>
#include <vector>
#include <functional>

class EditorItem;

class Pattern {
    std::string name;
};

class Trapezoid {
public:
    int height;
    int shift;
    int lower_width;
    int upper_width;
    Pattern *pattern;
};

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
    virtual std::vector<Element *> children() const = 0;
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
    Link(std::string name) : Element(ElementType::Link), name(name) {
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
    std::vector<Element *> children() const override { return {}; }

    int width() const override { return 100; } // hack

    void forEach(std::function<void(Element *)> f) override {
        f(this);
    }
};

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

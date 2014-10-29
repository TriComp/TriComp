#include <string>
#include <QGraphicsItem>
#include <QGraphicsPolygonItem>
#include <vector>
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
 * By virtue of being a QGraphicsItem, we can know that a user clicked on an element for free.
 */

class Element {
public:
    // In subclasses, delegate to QGraphicsPolygonItem
    QGraphicsItem *gfx;
    virtual std::vector<Element *> children() const = 0;
};

class TrapezoidElem : public Element {
public:
    Trapezoid geom;
    Element *next;

    TrapezoidElem() {
        auto *item = new QGraphicsPolygonItem();
        item->setPolygon(QPolygon(QRect(10, 10, 20, 20))); // TODO: do it for realz

        gfx = item;
    }

    std::vector<Element *> children() const override { return {next}; }
};

class Split : public Element {
public:
    Element *left;
    Element *right;
    int gap_width;

    Split() {
        auto *item = new QGraphicsPolygonItem();
        item->setPolygon(QPolygon(QRect(10, 10, 20, 20))); // TODO: do it for realz

        gfx = item;
    }

    std::vector<Element *> children() const override { return {left, right}; }
};

class Link : public Element {
public:
    std::string name;
    Slot slot;
    Link() {
        auto *item = new QGraphicsPolygonItem();
        item->setPolygon(QPolygon(QRect(10, 10, 20, 20))); // TODO: do it for realz

        gfx = item;
    }
    std::vector<Element *> children() const override { return {}; }

};

class Stop : public Element {
    Stop() {
        auto *item = new QGraphicsPolygonItem();
        item->setPolygon(QPolygon(QRect(10, 10, 20, 20))); // TODO: do it for realz

        gfx = item;
    }
    std::vector<Element *> children() const override { return {}; }
};

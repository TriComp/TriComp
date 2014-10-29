#include <string>
#include <QGraphicsItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QVector2D>
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
 * TODO: destructors, investigate hierachy features of GraphicsScene
 */

class Element {
public:
    // In subclasses, delegate to QGraphicsPolygonItem
    QGraphicsItem *gfx;
    virtual std::vector<Element *> children() const = 0;
    virtual void addToScene(QGraphicsScene *s, QPointF origin) const = 0;
    virtual int width() const = 0;
};

class TrapezoidElem : public Element {
public:
    Trapezoid geom;
    Element *next;

    TrapezoidElem(Trapezoid t, Element *next) : next(next) {
        geom = t;
        QPointF p2(t.shift, -t.height); // Inverted y axis :-[
        QPointF p3(p2.x() + t.upper_width, p2.y());
        QPointF p4(t.lower_width, 0);
        QVector<QPointF> points {QPointF(0, 0), p2, p3, p4};
        auto *item = new QGraphicsPolygonItem();
        item->setPolygon(QPolygonF(points));

        gfx = item;
    }

    std::vector<Element *> children() const override { return {next}; }

    void addToScene(QGraphicsScene *s, QPointF origin) const override {
        s->addItem(gfx);
        gfx->setPos(origin);
        if (next)
            next->addToScene(s, QPointF(origin.x() + geom.shift, origin.y() - geom.height));
    }

    int width() const override { return geom.lower_width; }
};

class Split : public Element {
public:
    // Invariant: left != nullprt & right != nullptr
    Element *left;
    Element *right;
    int gap;

    Split(Element *l, Element *r, int gap) : right(r), left(l), gap(gap) {
        if (r && l) {
            auto *item = new QGraphicsLineItem(0, 0, width(), 0);
            gfx = item;
        } else {
            printf(":p %d", 2/0); // Whoever you are, I hate you
        }
    }

    int width() const override { return left->width() + gap + right->width(); }

    std::vector<Element *> children() const override { return {left, right}; }

    void addToScene(QGraphicsScene *s, QPointF origin) const override {
        gfx->setPos(origin);
        s->addItem(gfx);
        left->addToScene(s, origin);
        right->addToScene(s, origin + QPointF(left->width() + gap, 0));
    }
};

class Link : public Element {
public:
    std::string name;
    Slot slot;
    Link(std::string name) : name(name) {
        auto *item = new QGraphicsTextItem(QString("LINK to ") + name.c_str());
        gfx = item;
    }
    std::vector<Element *> children() const override { return {}; }

    void addToScene(QGraphicsScene *s, QPointF origin) const override {
        gfx->setPos(origin + QPointF(0, -20));
        s->addItem(gfx);
    }

    int width() const override { return 100; } // hack
};

class Stop : public Element {
public:
    Stop() {
        auto *item = new QGraphicsTextItem(QString("STOP"));
        gfx = item;
    }
    std::vector<Element *> children() const override { return {}; }

    void addToScene(QGraphicsScene *s, QPointF origin) const override {
        gfx->setPos(origin + QPointF(0, -20));
        s->addItem(gfx);
    }

    int width() const override { return 100; } // hack
};
















#include "editor.h"

TrapezoidItem::TrapezoidItem(TrapezoidElem *e) : elem(e) {
    Trapezoid t = e->geom;
    QPointF p2(t.shift, -t.height); // Inverted y axis :-[
    QPointF p3(p2.x() + t.upper_width, p2.y());
    QPointF p4(t.lower_width, 0);
    QVector<QPointF> points {QPointF(0, 0), p2, p3, p4};
    poly = new QGraphicsPolygonItem();
    poly->setPolygon(QPolygonF(points));
    addToGroup(poly);

    setAcceptHoverEvents(true);
    poly->setAcceptHoverEvents(true);
}

void TrapezoidItem::hoverEnterEvent (QGraphicsSceneHoverEvent * event) {
    if(!poly->isUnderMouse()) return;
    QBrush b(QColor("blue"));
    poly->setBrush(b);
    update();
}
void TrapezoidItem::hoverLeaveEvent (QGraphicsSceneHoverEvent * event) {
    if(poly->isUnderMouse()) return;
    QBrush b(QColor("green"));
    poly->setBrush(b);
    update();
}

SplitItem::SplitItem(Split *s) : elem(s) {
    line = new QGraphicsLineItem(0, 0, s->width(), 0);
    addToGroup(line);
}

StopItem::StopItem(Stop *s) : elem(s) {
    text = new QGraphicsTextItem("STOP");
    addToGroup(text);
}

LinkItem::LinkItem(Link *s) : elem(s) {
    text = new QGraphicsTextItem("Link");
    addToGroup(text);
}


class AttachItems : public ElementVisitor<void, QPointF> {
public:
    QGraphicsScene *scene;

    AttachItems(QGraphicsScene *scene) : scene(scene) {}

    void visitLink(Link *l, QPointF o) override {
        auto *item = new LinkItem(l);
        item->text->setPos(o);
        l->gfx = item;
        scene->addItem(item);
    }

    void visitTrapezoid(TrapezoidElem *e, QPointF o) override {
        visit(e->next, o + QPointF(e->geom.shift, -e->geom.height));
        auto *item = new TrapezoidItem(e);
        item->poly->setPos(o);
        e->gfx = item;
        scene->addItem(item);
    }

    void visitStop(Stop *s, QPointF o) override {
        auto *item = new StopItem(s);
        item->text->setPos(o);
        s->gfx = item;
        scene->addItem(item);
    }

    void visitSplit(Split *s, QPointF o) override {
        visit(s->left, o);
        visit(s->right, o + QPointF(s->left->width() + s->gap, 0));
        auto *item = new SplitItem(s);
        item->line->setPos(o);
        s->gfx = item;
        scene->addItem(item);
    }

    static void doIt(Element *e, QGraphicsScene *s) {
        AttachItems(s).visit(e, QPointF(0, 0));
    }
};

void attachItems(Element *e, QGraphicsScene *s) {
        AttachItems(s).visit(e, QPointF(0, 0));
}

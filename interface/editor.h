#pragma once

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsItemGroup>
#include <functional>

#include "representation.h"

class EditorItem : public QGraphicsItemGroup {
    virtual Element *element() = 0;
};

class TrapezoidItem : public EditorItem {
public:
    QGraphicsPolygonItem *poly;
    TrapezoidElem *elem;

    TrapezoidItem(TrapezoidElem *e);

    QBrush brush_normal;
    QBrush brush_selected;

    bool selected;

    void hoverEnterEvent ( QGraphicsSceneHoverEvent * event);
    void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event);
    void mousePressEvent ( QGraphicsSceneMouseEvent * event) override;
    Element *element() override { return elem; }
};

class SplitItem : public EditorItem {
public:
    QGraphicsLineItem *line;
    Split *elem;

    SplitItem(Split *s);

    Element *element() override { return elem; }
};

class StopItem : public EditorItem {
public:
    QGraphicsTextItem *text;
    Stop *elem;

    StopItem(Stop *s);

    Element *element() override { return elem; }
};

class LinkItem : public EditorItem {
public:
    QGraphicsTextItem *text;
    Link *elem;

    LinkItem(Link *s);

    Element *element() override { return elem; }
};


void attachItems(Element *e, QGraphicsScene *s);

class Editor : QObject {
    Q_OBJECT

public:

    QGraphicsView *view;
    QGraphicsScene *scene;
    Element *base;

    Editor(QGraphicsView *view, QGraphicsScene *scene, Element *base)
        : view(view), scene(scene), base(base) {

    }
};

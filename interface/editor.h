#pragma once

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsItemGroup>
#include <functional>

#include <set>

#include "representation.h"

class MainWindow;

class EditorManager : public QObject {
    Q_OBJECT
public:
    MainWindow* mw;
    std::set<EditorItem*> selected;

    void setSelected(EditorItem* it, bool sel);

    EditorManager(MainWindow* mw);

signals:
    void isModified();

public slots:
    void patternClicked(QObject* o);
};

class EditorItem : public QGraphicsItemGroup {
public:
    virtual Element* element() = 0;
    EditorManager* manager;

    EditorItem(EditorManager* m)
        : manager(m)
    {
    }
};

class TrapezoidItem : public EditorItem {
public:
    QGraphicsPolygonItem* poly;
    TrapezoidElem* elem;

    TrapezoidItem(TrapezoidElem* e, EditorManager* m);

    QBrush brush_normal;
    QPen pen_normal;
    QBrush brush_selected() const;
    QPen pen_selected() const;
    void updateBrush();

    bool selected;

    void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    Element* element() override { return elem; }
};

class SplitItem : public EditorItem {
public:
    QGraphicsLineItem* line;
    Split* elem;

    SplitItem(Split* s);

    Element* element() override { return elem; }
};

class StopItem : public EditorItem {
public:
    QGraphicsTextItem* text;
    Stop* elem;

    StopItem(Stop* s);

    Element* element() override { return elem; }
};

class LinkItem : public EditorItem {
public:
    QGraphicsTextItem* text;
    Link* elem;

    LinkItem(Link* s);

    Element* element() override { return elem; }
};

EditorManager* attachItems(Element* e, QGraphicsScene* s, MainWindow* mw, Knit* knit);

class Editor : QObject {
    Q_OBJECT

public:
    QGraphicsView* view;
    QGraphicsScene* scene;
    Element* base;

    Editor(QGraphicsView* view, QGraphicsScene* scene, Element* base)
        : view(view)
        , scene(scene)
        , base(base)
    {
    }
};

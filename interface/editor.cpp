#include "editor.h"
#include "mainwindow.h"
#include "representation.h"

EditorManager::EditorManager(MainWindow* mw)
    : mw(mw)
{
    connect(&mw->patternMapper, SIGNAL(mapped(QObject*)), this,
            SLOT(patternClicked(QObject*)));
}

void EditorManager::patternClicked(QObject* o)
{
    auto* p = (Pattern*)o; // o is a pattern
    for (auto* i : selected) {
        qDebug("patternClicked()");
        auto* t = (TrapezoidItem*)i;
        t->brush_normal = p->brush; // change the graphical object
        t->updateBrush();
        t->elem->geom.pattern = p; // change the c++ object
    }
}

void EditorManager::setSelected(EditorItem* it, bool sel)
{
    if (sel) {
        selected.insert(it);
    } else {
        selected.erase(it);
    }
}

TrapezoidItem::TrapezoidItem(TrapezoidElem* e, EditorManager* m)
    : elem(e)
    , selected(false)
    , EditorItem(m)
{
    Trapezoid t = e->geom;
    QPointF p2(t.shift, -t.height); // Inverted y axis :-[
    QPointF p3(p2.x() + t.upper_width, p2.y());
    QPointF p4(t.lower_width, 0);
    QVector<QPointF> points{ QPointF(0, 0), p2, p3, p4 };
    poly = new QGraphicsPolygonItem();
    poly->setPolygon(QPolygonF(points));
    addToGroup(poly);

    setAcceptHoverEvents(false);
    poly->setAcceptHoverEvents(true);

    brush_normal = QBrush("#444");

    poly->setBrush(brush_normal);
}

void TrapezoidItem::updateBrush()
{
    QBrush c = selected ? brush_selected() : brush_normal;
    poly->setBrush(c);
    update();
}

void TrapezoidItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    auto b = selected ? brush_selected() : brush_normal;
    b.setColor(b.color().light(120));
    poly->setBrush(b);
    update();
}

void TrapezoidItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    auto b = selected ? brush_selected() : brush_normal;
    poly->setBrush(b);
    update();
}

void TrapezoidItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    selected = !selected;
    manager->setSelected(this, selected);
    updateBrush();
}

SplitItem::SplitItem(Split* s)
    : elem(s)
    , EditorItem(nullptr)
{
    line = new QGraphicsLineItem(0, 0, s->width(), 0);
    addToGroup(line);
}

StopItem::StopItem(Stop* s)
    : elem(s)
    , EditorItem(nullptr)
{
    text = new QGraphicsTextItem("STOP");
    addToGroup(text);
}

LinkItem::LinkItem(Link* s)
    : elem(s)
    , EditorItem(nullptr)
{
    text = new QGraphicsTextItem("Link");
    addToGroup(text);
}

class AttachItems : public ElementVisitor<void, QPointF> {
public:
    QGraphicsScene* scene;
    EditorManager* manager;

    AttachItems(QGraphicsScene* scene, EditorManager* m)
        : scene(scene)
        , manager(m)
    {
    }

    void visitLink(Link* l, QPointF o) override
    {
        auto* item = new LinkItem(l);
        item->text->setPos(o);
        l->gfx = item;
        scene->addItem(item);
    }

    void visitTrapezoid(TrapezoidElem* e, QPointF o) override
    {
        visit(e->next, o + QPointF(e->geom.shift, -e->geom.height));
        auto* item = new TrapezoidItem(e, manager);
        item->poly->setPos(o);
        e->gfx = item;
        scene->addItem(item);
    }

    void visitStop(Stop* s, QPointF o) override
    {
        auto* item = new StopItem(s);
        item->text->setPos(o);
        s->gfx = item;
        scene->addItem(item);
    }

    void visitSplit(Split* s, QPointF o) override
    {
        visit(s->left, o);
        visit(s->right, o + QPointF(s->left->width() + s->gap, 0));
        auto* item = new SplitItem(s);
        item->line->setPos(o);
        s->gfx = item;
        scene->addItem(item);
    }
};

EditorManager* attachItems(Element* e, QGraphicsScene* s, MainWindow* mw)
{
    auto* m = new EditorManager(mw);
    AttachItems(s, m).visit(e, QPointF(0, 0));
    return m;
}

#include "editor.h"
#include "mainwindow.h"
#include "representation.h"
#include <QDebug>

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

    brush_normal = (e->geom).pattern->brush;

    poly->setBrush(brush_normal);
}

QBrush TrapezoidItem::brush_selected() const
{
    QBrush b(brush_normal.color().light(140));
    return b;
}
QPen TrapezoidItem::pen_selected() const
{
    auto p = pen_normal;
    p.setColor(QColor("red"));
    return p;
}

void TrapezoidItem::updateBrush()
{
    QBrush c = selected ? brush_selected() : brush_normal;
    QPen p = selected ? pen_selected() : pen_normal;
    poly->setPen(p);
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
    Knit* knit;

    AttachItems(QGraphicsScene* scene, EditorManager* m, Knit* knit)
        : scene(scene)
        , manager(m)
        , knit(knit)
    {
        // qDebug("AttachItems");
    }

    void visitLink(Link* l, QPointF o) override
    {
        // qDebug("visitLink");
        Q_ASSERT(l);
        if (l->slot == Slot::Right)
            return;
        if (knit->elements.count(l->name) > 0) {
            auto* elt = knit->elements[l->name];
            visit(elt, o);
        } else {
            qErrnoWarning("Unknown piece %s", l->name.c_str());
        }
    }

    void visitTrapezoid(TrapezoidElem* e, QPointF o) override
    {
        // qDebug("visitTrap");
        Q_ASSERT(e && e->next);
        visit(e->next, o + QPointF(e->geom.shift, -e->geom.height));
        auto* item = new TrapezoidItem(e, manager);
        item->poly->setPos(o);
        e->gfx = item;
        scene->addItem(item);
    }

    void visitStop(Stop* s, QPointF o) override
    {
        // qDebug("visitStop");
        Q_ASSERT(s);
        auto* item = new StopItem(s);
        item->text->setPos(o + QPointF(0,-20));
        s->gfx = item;
        scene->addItem(item);
    }

    void visitSplit(Split* s, QPointF o) override
    {
        // qDebug("visitSplit");
        Q_ASSERT(s && s->left && s->right);
        visit(s->left, o);
        visit(s->right, o + QPointF(s->left->width() + s->gap, 0));
        auto* item = new SplitItem(s);
        item->line->setPos(o);
        s->gfx = item;
        scene->addItem(item);
    }
};

EditorManager* attachItems(Element* e, QGraphicsScene* s, MainWindow* mw, Knit* knit)
{
    Q_ASSERT(e && s && mw && knit);
    auto* m = new EditorManager(mw);
    auto a = AttachItems(s, m, knit);
    a.visit(e, QPointF(0, 0));
    return m;
}

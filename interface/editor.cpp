#include "editor.h"
#include "mainwindow.h"
#include "representation.h"
#include <QDebug>
#include <list>

#define UNUSED(expr) do { (void)(expr); } while (0)

using namespace std;

EditorManager::EditorManager(MainWindow* mw)
    : mw(mw)
{
    connect(&mw->patternMapper, SIGNAL(mapped(QObject*)), this,
            SLOT(patternClicked(QObject*)));
    connect(this, SIGNAL(isModified()), mw, SLOT(modify()));
}

void EditorManager::patternClicked(QObject* o)
{
    auto* p = (Pattern*)o; // o is a pattern
    for (auto* i : selected) {
        // qDebug("patternClicked()");
        auto* t = (TrapezoidItem*)i;
        t->brush_normal = p->brush; // change the graphical object
        t->updateBrush();
        t->elem->geom.pattern = p; // change the c++ object
    }
    isModified();
}

void EditorManager::setSelected(EditorItem* it, bool sel)
{
    if (sel) {
        selected.insert(it);
    } else {
        selected.erase(it);
    }
}

TrapezoidItem::TrapezoidItem(TrapezoidElem* e, EditorManager* m, int lower_width)
    : EditorItem(m)
    , elem(e)
    , selected(false)
{
    Trapezoid t = e->geom;
    QPointF p2(t.shift, -t.height); // Inverted y axis :-[
    QPointF p3(p2.x() + t.upper_width, p2.y());
    QPointF p4(lower_width, 0);
    QVector<QPointF> points{ QPointF(0, 0), p2, p3, p4 };
    poly = new QGraphicsPolygonItem();
    poly->setPolygon(QPolygonF(points));
    addToGroup(poly);

    setAcceptHoverEvents(false);
    poly->setAcceptHoverEvents(true);

    Q_ASSERT(e->geom.pattern);
    brush_normal = e->geom.pattern->brush;

    poly->setBrush(brush_normal);
}

QBrush TrapezoidItem::brush_selected() const
{
    QBrush b = brush_normal;
    b.setColor(brush_normal.color().light(140));
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
    UNUSED(event);
    auto b = selected ? brush_selected() : brush_normal;
    b.setColor(b.color().light(120));
    poly->setBrush(b);
    update();
}

void TrapezoidItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    UNUSED(event);
    auto b = selected ? brush_selected() : brush_normal;
    poly->setBrush(b);
    update();
}

void TrapezoidItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    UNUSED(event);
    selected = !selected;
    manager->setSelected(this, selected);
    updateBrush();
}

SplitItem::SplitItem(Split* s)
    : EditorItem(nullptr)
    , elem(s)

{
    line = new QGraphicsLineItem(0, 0, s->width(), 0);
    addToGroup(line);
}

LinkItem::LinkItem(Link* s)
    : EditorItem(nullptr)
    , elem(s)
{
    text = new QGraphicsTextItem("Link");
    addToGroup(text);
}

class AttachItems : public ElementVisitor<void, QPointF, int> {
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

    void visitLink(Link* l, QPointF o, int start) override
    {
        // qDebug("visitLink");
        UNUSED(start);
        Q_ASSERT(l);
        if (!(l->visited)) {
            l->visited = true;
            if (knit->elements.count(l->name) > 0) {
                auto* elt = (knit->elements[l->name]).second;
                int new_start = (knit->elements[l->name]).first;
                visit(elt, o + QPoint(-l->position,0), new_start);
            } else {
                qErrnoWarning("Unknown piece %s", l->name.c_str());
            }
        }
    }

    void visitTrapezoid(TrapezoidElem* e, QPointF o, int start) override
    {
        // qDebug("visitTrap");
        Q_ASSERT(e && e->next);
        if (!(e->visited)) {
            e->visited = true;
            visit(e->next, o + QPointF(e->geom.shift, -e->geom.height), e->geom.upper_width);
            auto* item = new TrapezoidItem(e, manager, start);
            item->poly->setPos(o);
            e->gfx = item;
            scene->addItem(item);
        }
    }

    void visitSplit(Split* s, QPointF o, int start) override
    {
        // qDebug("visitSplit");
        UNUSED(start);
        if (!(s->visited)) {
            s->visited = true;
            if (s->elements != NULL) {
                for (list<splitData>::const_iterator it = s->elements->begin(); it != s->elements->end(); ++it) {
                    visit(it->next, o + QPointF(it->position,0), it->width);
                }
                auto* item = new SplitItem(s);
                item->line->setPos(o);
                s->gfx = item;
                scene->addItem(item);
            }
        }
    }
};

EditorManager* attachItems(Element* e, QGraphicsScene* s, MainWindow* mw, Knit* knit, int start)
{
    Q_ASSERT(e && s && mw && knit);
    auto* m = new EditorManager(mw);
    auto a = AttachItems(s, m, knit);
    a.visit(e, QPointF(0, 0), start);
    return m;
}

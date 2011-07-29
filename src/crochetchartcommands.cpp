/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "crochetchartcommands.h"

#include <QDebug>
#include <QObject>

/*************************************************\
| SetCellStitch                                   |
\*************************************************/
SetCellStitch::SetCellStitch(CrochetScene *s, CrochetCell *cell, QString newSt, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    scene = s;
    c = cell;
    oldStitch = c->name();
    newStitch = newSt;
    setText(QObject::tr("Change Stitch: %1 -> %2").arg(oldStitch).arg(newStitch));
}

void SetCellStitch::redo()
{
    QPoint pos = scene->findGridPosition(c);
    qDebug() << pos << newStitch;
    c->setStitch(newStitch, (pos.y() % 2));
}

void SetCellStitch::undo()
{
    QPoint pos = scene->findGridPosition(c);
    c->setStitch(oldStitch, (pos.y() % 2));
}

/*************************************************\
| SetCellColor                                    |
\*************************************************/
SetCellColor::SetCellColor(CrochetScene *s, CrochetCell *cell, QColor newCl, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    scene = s;
    c = cell;
    oldColor = c->color();
    newColor = newCl;
    setText(QObject::tr("Change Color: %1 -> %2").arg(oldColor.name()).arg(newColor.name()));
}

void SetCellColor::redo()
{
    c->setColor(newColor);
}

void SetCellColor::undo()
{
    c->setColor(oldColor);
}

/*************************************************\
| SetCellRotation                                 |
\*************************************************/
SetCellRotation::SetCellRotation(CrochetScene *s, CrochetCell *cell, qreal baseRot, qreal diff, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    scene = s;
    c = cell;
    delta = diff;
    baseRotation = baseRot;
    setText(QObject::tr("Change cell rotation"));
    
}

void SetCellRotation::redo()
{
    qreal final = baseRotation - delta;
    
    c->setTransform(QTransform().translate(32, 0).rotate(final).translate(-32, 0));
}

void SetCellRotation::undo()
{
    c->setTransform(QTransform().translate(32, 0).rotate(baseRotation).translate(-32, 0));
}

bool SetCellRotation::mergeWith(const QUndoCommand *command)
{
    if(command->id() != id())
        return false;
    //return false;
    const SetCellRotation *other = static_cast<const SetCellRotation*>(command);

    CrochetCell *otherC = other->c;
    
    if(otherC != c)
        return false;

    delta = other->delta;
    setText(QObject::tr("Change cell rotation"));
    return true;
}


/*************************************************\
| SetCellCoordinates                              |
\*************************************************/
SetCellCoordinates::SetCellCoordinates(CrochetScene *s, CrochetCell *cell, QPointF oldPos, QPointF newPos, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    scene = s;
    c = cell;
    oldCoord = oldPos;
    newCoord = newPos;
    setText(QObject::tr("Change cell position"));
    
}

void SetCellCoordinates::undo()
{
    c->setPos(oldCoord);
}

void SetCellCoordinates::redo()
{
    c->setPos(newCoord);
}

bool SetCellCoordinates::mergeWith(const QUndoCommand *command)
{
    if(command->id() != id())
        return false;
    
    const SetCellCoordinates *other = static_cast<const SetCellCoordinates*>(command);
    
    CrochetCell *otherC = other->c;
    
    if(otherC != c)
        return false;

    newCoord = other->newCoord;
    setText(QObject::tr("Change cell position"));
    return true;
}
 
/*************************************************\
 | SetCellScale                                   |
\*************************************************/
SetCellScale::SetCellScale(CrochetScene *s, CrochetCell *cell, qreal scl, QUndoCommand* parent)
: QUndoCommand(parent)
{
    scene = s;
    c = cell;

    if(scl > 0)
        delta = 0.075;
    else if(scl < 0)
        delta = -0.075;
    else
        delta = 0;
    deltas.append(delta);
    baseSize.append(1.0);
    
    setText(QObject::tr("Change cell position"));
}

void SetCellScale::undo()
{
    qreal change = c->scale();
    for(int i = deltas.count() - 1; i >= 0; --i) {
        qreal value = deltas.at(i);
        qreal baseS = baseSize.at(i);
        change = (1.0 + value)/c->scale();
        qDebug() << c->scale() << 1.0 + value << change << baseS;
        c->setScale(change);
    }
}

void SetCellScale::redo()
{
    foreach(qreal change, deltas) {
        c->setScale(c->scale() + change);
    }
}

bool SetCellScale::mergeWith(const QUndoCommand *command)
{
    if(command->id() != id())
        return false;

    const SetCellScale *other = static_cast<const SetCellScale*>(command);
    
    CrochetCell *otherC = other->c;
    
    if(otherC != c)
        return false;

    if(other->delta != 0)
        deltas.append(other->delta);
        baseSize.append(other->baseSize);
    
    setText(QObject::tr("Change cell position"));
    return true;
}

/*************************************************\
| AddCell                                         |
\*************************************************/
AddCell::AddCell(CrochetScene* s, QPoint pos, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    position = pos;
    c = new CrochetCell();
    c->setColor();
    scene = s;
}

void AddCell::redo()
{
    scene->addCell(position, c);
    scene->setCellPosition(position.y(), position.x(), c, scene->grid()[position.y()].count(), true);
    scene->redistributeCells(position.y());
}

void AddCell::undo()
{
    scene->removeCell(c);
    scene->redistributeCells(position.y());
}

/*************************************************\
| RemoveCell                                      |
\*************************************************/
RemoveCell::RemoveCell(CrochetScene* s, CrochetCell *cell, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    c = cell;
    scene = s;
    position = scene->findGridPosition(cell);

}

void RemoveCell::redo()
{
    scene->removeCell(c);
    scene->redistributeCells(position.y());
}

void RemoveCell::undo()
{
    scene->addCell(position, c);
    scene->setCellPosition(position.y(), position.x(), c, scene->grid()[position.y()].count(), true);
    scene->redistributeCells(position.y());
}

/*************************************************\
| AddRow                                          |
\*************************************************/
AddRow::AddRow(CrochetScene* s, QPointF pos, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    position = pos;
    scene = s;
}

void AddRow::redo()
{

}

void AddRow::undo()
{

}

/*************************************************\
| RemoveRow                                       |
\*************************************************/
RemoveRow::RemoveRow(CrochetScene* s, QPointF pos, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    position = pos;
    scene = s;
}

void RemoveRow::redo()
{

}

void RemoveRow::undo()
{

}

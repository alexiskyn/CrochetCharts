#include "stitchlibrarydelegate.h"

#include "stitch.h"
#include <QPainter>

#include <QDebug>

StitchLibraryDelegate::StitchLibraryDelegate(QWidget *parent)
    : QStyledItemDelegate(parent)
{
}

void StitchLibraryDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //fall back to the basic painter.
    QStyledItemDelegate::paint(painter, option, index);
}

QSize StitchLibraryDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(!index.isValid())
        return QSize(100, 10);

    Stitch *s = static_cast<Stitch*>(index.internalPointer());
    if(!s)
        return QSize(100, 10);

qDebug() << "size hint";
    QString text;

    switch(index.row()) {
        case 0:
            text = s->name();
            break;
        case 1:
            text = s->file(); //TODO: make this the icon. 32x32.
            break;
        case 2:
            text = s->description();
            break;
        case 3:
            text = s->category();
            break;
        case 4:
            text = s->wrongSide();
            break;
        default:
            text = "";
            break;
    }

    return option.fontMetrics.size(Qt::TextWordWrap, text);
}

QWidget* StitchLibraryDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void StitchLibraryDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
}

void StitchLibraryDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
}
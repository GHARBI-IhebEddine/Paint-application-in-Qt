/*
In here you will find all the scene items that will be used later in the mainwindow
*/
#ifndef SCENEITEMS_H
#define SCENEITEMS_H

#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QPointF>
#include <QGraphicsScene>
#include <QPen>
#include <QDialog>
#include <QLineEdit>

// rectangle item
class RectItem : public QGraphicsRectItem
{
public:
    RectItem(qreal x , qreal y, qreal w, qreal h, QPen p /*color+width*/);
protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
private:
    bool isResizing;
    QPointF initialMousePos;
    QPointF initialRectPos;
};

//Image item
class ImageItem : public QGraphicsPixmapItem
{
public:
    ImageItem(qreal x , qreal y, QPixmap pixmap);
    void resizeImage(const QSizeF& newSize);
protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;

private:
    bool isResizing;
    QPointF initialMousePos;
    QPointF initialImagePos;
    QSizeF initialPixmapSize;
    QGraphicsRectItem* resizingRectangle;
};

//line item
class LineItem : public QGraphicsLineItem
{
public:
    LineItem(qreal x , qreal y, double l, QPen p /*color+width*/);
protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
private:
    bool isResizing;
    QPointF initialMousePos;
    QPointF initialLinePos;
};

//Text Item
class TextItem : public QGraphicsTextItem
{
public:
    TextItem(qreal x , qreal y, qreal w, qreal h);
protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    /*
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    */
private:
    bool isResizing;
    QPointF initialMousePos;
    QPointF initialTextPos;
    QSizeF initialTextSize;
    QGraphicsRectItem* resizingRectangle;
};

#endif // SCENEITEMS_H

#include "sceneitems.h"
#include <QVBoxLayout>
#include <QPushButton>

//rectangle item
RectItem::RectItem(qreal x, qreal y, qreal w, qreal h, QPen p) : QGraphicsRectItem(0, 0, w, h)
{
    setPen(p);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges);
    setPos(QPoint(x, y));
    isResizing = false;
}

void RectItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
        QGraphicsRectItem::mouseMoveEvent(event);

    if (event->buttons() & Qt::RightButton)
        setRect(QRectF(QPointF(), event->pos()).normalized());
}
void RectItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsRectItem::mousePressEvent(event);
}
void RectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsRectItem::mouseReleaseEvent(event);
    if (event->button() == Qt::LeftButton)
    {
        isResizing = false;
    }
}

//Image Item
ImageItem::ImageItem(qreal x, qreal y, QPixmap pixmap) : QGraphicsPixmapItem(pixmap), isResizing(false)
{
    //setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges);
    setPos(QPointF(x, y));
    // Create and configure the resizing rectangle
    resizingRectangle = new QGraphicsRectItem(QRectF(0, 0, pixmap.size().width(), pixmap.size().height()), this);
    resizingRectangle->setPen(QPen(Qt::black,2,Qt::DashDotLine));
    resizingRectangle->setVisible(false);
}

void ImageItem::focusInEvent(QFocusEvent* event)
{
    QGraphicsPixmapItem::focusInEvent(event);

    // Show the resizing rectangle when the image item is focused
    resizingRectangle->setVisible(true);
    resizingRectangle->setRect(QRectF(0, 0, pixmap().width(), pixmap().height()));

}
void ImageItem::resizeImage(const QSizeF& newSize)
{
    QPixmap newPixmap = pixmap().scaled(newSize.toSize(), Qt::KeepAspectRatio, Qt::FastTransformation);
    setPixmap(newPixmap);
    resizingRectangle->setRect(QRectF(0, 0, newSize.width(), newSize.height()));
}
void ImageItem::focusOutEvent(QFocusEvent* event)
{
    QGraphicsPixmapItem::focusOutEvent(event);

    // Hide the resizing rectangle when the image item is unfocused
    resizingRectangle->setVisible(false);
}
void ImageItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsPixmapItem::mousePressEvent(event);

    if (event->button() == Qt::RightButton)
    {
        initialMousePos = event->pos();
        isResizing = true;
    }
}

void ImageItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton)
        QGraphicsPixmapItem::mouseMoveEvent(event);

    if (isResizing)
    {
        QPointF sizeDifference = event->pos() - initialMousePos;
        const qreal sensitivity = 0.01;
        QSizeF currentSize = resizingRectangle->rect().size(); // Use resizingRectangle size
        QSizeF newSize(currentSize.width() + sizeDifference.x() * sensitivity,
                       currentSize.height() + sizeDifference.y() * sensitivity);

        resizingRectangle->setRect(QRectF(0, 0, newSize.width(), newSize.height())); // Resize the rectangle
    }
}

void ImageItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsPixmapItem::mouseReleaseEvent(event);
    if (event->button() == Qt::RightButton)
    {
        isResizing = false; // Reset the resizing flag
    }
    resizeImage(resizingRectangle->rect().size()); // Resize the image based on rectangle size
    resizingRectangle->setRect(QRectF(0, 0, pixmap().width(), pixmap().height()));
}

//LineItem

LineItem::LineItem(qreal x, qreal y, double  l, QPen p) : QGraphicsLineItem()
{
    setPen(p);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges);
    setPos(QPoint(x, y));
    setLine(QLineF(QPointF(), QPointF(250, 0)));
    isResizing = false;
}

void LineItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
        QGraphicsLineItem::mouseMoveEvent(event);
    if (event->buttons() & Qt::RightButton)
    {
        setLine(QLineF(QPointF(), QPointF(event->pos().x(),event->pos().y())));
    }
}
void LineItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsLineItem::mousePressEvent(event);
}
void LineItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsLineItem::mouseReleaseEvent(event);

    if (event->button() == Qt::LeftButton)
    {
        isResizing = false;
    }
}

//TextItem

TextItem::TextItem(qreal x, qreal y, qreal w, qreal h) : QGraphicsTextItem()
{
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges);
    setPos(QPoint(x, y));
    isResizing = false; // Initialize the flag
}

void TextItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    int initialFontSize;
    if (event->buttons() & Qt::LeftButton)
    {
        QGraphicsTextItem::mouseMoveEvent(event);
    }

    if (event->buttons() & Qt::RightButton)
    {
        if (!isResizing)
        {
            // Determine the middle of the text item
            QPointF textMiddle = boundingRect().center();

            // Convert the text middle to scene coordinates
            QPointF textMiddleScene = mapToScene(textMiddle);

            // Define a margin around the text middle
            qreal margin = boundingRect().width() * 0.7; // Adjust the margin size as needed

            // Check if the mouse is outside the margin around the middle of the text
            if ((event->pos() - textMiddleScene).manhattanLength() > margin)
            {
                isResizing = true; // Start resizing
                initialMousePos = event->pos(); // Store the initial mouse position
                initialFontSize = font().pointSize(); // Store the initial font size
            }
        }

        if (isResizing)
        {
            // Calculate size difference
            QPointF sizeDifference = event->pos() - initialMousePos;

            // Calculate new font size while maintaining aspect ratio
            int newFontSize = qMax(1, static_cast<int>(initialFontSize + sizeDifference.y()));

            // Apply the new font size to the text item
            setFont(QFont(font().family(), newFontSize));
        }
    }
}
void TextItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsTextItem::mousePressEvent(event);
}
void TextItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsTextItem::mouseReleaseEvent(event);
    if (event->button() == Qt::LeftButton)
    {
        isResizing = false;
    }
}

#include "mainwindow.h"
#include "code128.h"
#include "sceneitems.h"
#include "ui_mainwindow.h"
#include <QImage>
#include <QPixmap>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsRectItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    ui->graphicsView_Image_View->setScene(scene);
    // spin box ui
    widthSpinBox = ui->spinBoxWidth;
    heightSpinBox = ui->spinBoxHeight;
    // range and initial values for the spin boxes
    widthSpinBox->setRange(32, 1000); // Set the allowed range for width
    widthSpinBox->setValue(256); // Set the initial value for width
    heightSpinBox->setRange(32, 1000); // Set the allowed range for height
    heightSpinBox->setValue(256); // Set the initial value for height

    // Connect the valueChanged signals of the spin boxes to the updateViewSize slot
    connect(widthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::updateViewSize);
    connect(heightSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::updateViewSize);
    //Save the template
    savedSceneListModel = new QStandardItemModel(this);
    ui->savedScene->setModel(savedSceneListModel);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_lineButton_clicked()
{
    LineItem *currentLine = new LineItem(0, 50, 5, QPen(selectedPenColor, 5));
    scene->addItem(currentLine);
}


void MainWindow::on_sendButton_clicked()
{
    int i=0;
    while(connectTCP()!=true) {i++;if(i>2) return;}

    QByteArray dataPacket = createDataPacket();

    // Send the data packet over the TCP socket
    if (TCPSocket->write(dataPacket) == -1) {
        qDebug() << "Failed to send data over TCP.";
    }

    TCPSocket->close();
}


void MainWindow::on_imageButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Image File", QDir::currentPath());

    if (fileName.isEmpty())
        return;

    QImage imageOriginal(fileName);

    if (imageOriginal.isNull())
    {
        QMessageBox::information(this, "Image Viewer", "Error Displaying image");
        return;
    }

    for (int y = 0; y < imageOriginal.height(); y++)
    {
        for (int x = 0; x < imageOriginal.width(); x++)
        {
            QColor color(imageOriginal.pixel(x, y));
            int red = color.red();
            int green = color.green();
            int blue = color.blue();

            // Check if the color is closer to red, black, or white
            if (red > 0xc9 && red > green + 70 && red > blue + 70)
            {
                // Red
                imageOriginal.setPixel(x, y, qRgb(255, 0, 0));
            }
            else if (red < 90)
            {
                // Black
                imageOriginal.setPixel(x, y, qRgb(0, 0, 0));
            }
            else
            {
                // White
                imageOriginal.setPixel(x, y, qRgb(255, 255, 255));
            }
        }
    }

    QPixmap image = QPixmap::fromImage(imageOriginal);

    ImageItem *currentImage = new ImageItem(0, 0, image);
    scene->addItem(currentImage);
}


void MainWindow::on_barecodeButton_clicked()
{
    QString qtString = ui->TextEdit->text(); // Assuming ui->LineEdit is a pointer to a QLineEdit

    // Using toUtf8() to convert QString to const char*
    const char *strUtf8 = qtString.toUtf8().constData();

    const char *str = strdup(strUtf8);

    size_t barcode_length = code128_estimate_len(str);
    char *barcode_data = (char *) malloc(barcode_length);

    barcode_length = code128_encode_gs1(str, barcode_data, barcode_length);

    // Create an image with the received data
    QImage barcodeImage(barcode_length, 50, QImage::Format_RGB32);
    barcodeImage.fill(Qt::white); // Fill the image with white background

    for (int i = 0; i < barcode_length; i++) {
        if (barcode_data[i]) {
            for (int y = 0; y < 50; y++) {
                barcodeImage.setPixel(i, y, qRgb(0, 0, 0)); // Set pixel to black for vertical bar
            }
        }
    }
    // Convert the image to a pixmap and add it to the scene
    QPixmap pixmap = QPixmap::fromImage(barcodeImage);

    ImageItem *currentImage = new ImageItem(0, 0, pixmap);
    scene->addItem(currentImage);
}


void MainWindow::on_textButton_clicked()
{
    QString text = ui->TextEdit->text();

    TextItem *textItem = new TextItem(50, 50, 100, 100); // Adjust the position and size values
    textItem->setPlainText(text);

    scene->addItem(textItem);
}


void MainWindow::on_saveButton_clicked()
{
    QString sceneName = generateRandomSceneName(); // Implement this function to generate a random name
    QGraphicsScene* newSavedScene = new QGraphicsScene;

    QList<QGraphicsItem*> itemsToCopy = scene->items();
    for (QGraphicsItem* item : itemsToCopy) {
        QGraphicsItem* copiedItem = nullptr;

        if (item->type() == QGraphicsRectItem::Type) {
            // Handle rectangles
            QGraphicsRectItem* rectItem = dynamic_cast<QGraphicsRectItem*>(item);
            if (rectItem) {
                QRectF rect = rectItem->rect();
                QPen pen = rectItem->pen();
                copiedItem = new QGraphicsRectItem(rect);
                copiedItem->setPos(rectItem->pos());
                dynamic_cast<QGraphicsRectItem*>(copiedItem)->setPen(pen);
            }
        } else if (item->type() == QGraphicsLineItem::Type) {
            // Handle lines
            QGraphicsLineItem* lineItem = dynamic_cast<QGraphicsLineItem*>(item);
            if (lineItem) {
                QLineF line = lineItem->line();
                QPen pen = lineItem->pen();
                copiedItem = new QGraphicsLineItem(line);
                copiedItem->setPos(lineItem->pos());
                dynamic_cast<QGraphicsLineItem*>(copiedItem)->setPen(pen);
            }
        }
        // In progress for img,...
        if (copiedItem) {
            newSavedScene->addItem(copiedItem);
        }
    }

    savedScenes.insert(sceneName, newSavedScene);

    QStandardItem* sceneItem = new QStandardItem(sceneName);
    savedSceneListModel->appendRow(sceneItem);
}

QByteArray MainWindow::createDataPacket()
{
    QByteArray dataPacket;
    dataPacket.append(char(4)); // SOF

    // Take a screenshot of the entire QGraphicsView
    QPixmap screenshot = ui->graphicsView_Image_View->grab();

    // Resize the screenshot according to the width and height from the spin boxes
    int width = widthSpinBox->value();
    int height = heightSpinBox->value();
    screenshot = screenshot.scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    // Convert the QPixmap to QImage
    QImage image = screenshot.toImage();

    // Iterate through the pixels of the image to determine the color code
    for (int y = 0; y < image.height(); y++) {
        for (int x = 0; x < image.width(); x++) {
            QColor color(image.pixel(x, y));
            int colorCode;
            if (color == Qt::red) {
                colorCode = 1;
            } else if (color == Qt::black) {
                colorCode = 0;
            } else {
                colorCode = 2;
            }
            dataPacket.append(char(colorCode));
        }
    }

    dataPacket.append(char(3)); // EOF
    // Save the screenshot as an image file on the desktop
    QString desktopPath = QDir::homePath() + "/Desktop/";
    QString fileName = "screenshot.png";
    QString filePath = desktopPath + fileName;
    screenshot.save(filePath);
    return dataPacket;
}

bool MainWindow::connectTCP()
{
    TCPSocket = new QTcpSocket(this);
    TCPSocket->connectToHost("localhost",8000);
    // TCPSocket->open(QIODevice::ReadWrite);
    if(TCPSocket->waitForConnected(3000))
    {
        qDebug() << "Connected!";
        return true;
    }
    else
    {
        qDebug() << "Not connected!";
        return false;
    }
}

void MainWindow::updateViewSize()
{
    double newWidth = widthSpinBox->value();
    double newHeight = heightSpinBox->value();


    // Set a maximum size for the view to prevent scaling larger than the predefined values
    int maxWidth = 550; // Set your maximum width here
    int maxHeight = 550; // Set your maximum height here

    if (newWidth > maxWidth)
        newWidth = maxWidth;
    if (newHeight > maxHeight)
        newHeight = maxHeight;

    // Set the maximum scale limit to the entered values
    ui->graphicsView_Image_View->setMaximumWidth(newWidth);
    ui->graphicsView_Image_View->setMaximumHeight(newHeight);

    ui->graphicsView_Image_View->setFixedSize(newWidth, newHeight);
}

void MainWindow::on_rectButton_clicked()
{
    RectItem *currentRectangle = new RectItem(0, 0, 50, 50, QPen(selectedPenColor,3));
    scene->addItem(currentRectangle);
}


void MainWindow::on_savedScene_doubleClicked(const QModelIndex &index)
{
    QString sceneName = savedSceneListModel->itemFromIndex(index)->text();

    if (savedScenes.contains(sceneName)) {
        QGraphicsScene* savedScene = savedScenes.value(sceneName);
        // Clear the current scene and replace it with the saved scene
        scene->clear();
        scene->setSceneRect(savedScene->sceneRect());
        scene = savedScene;
        ui->graphicsView_Image_View->setScene(scene);
    }
}


void MainWindow::on_colorComboBox_currentIndexChanged(int index)
{
    QColor penColor;

    switch (index) {
    case 0: // Black
        penColor = Qt::black;
        break;
    case 1: // White
        penColor = Qt::white;
        break;
    case 2: // Red
        penColor = Qt::red;
        break;
    default:
        break;
    }

    // Store the selected color for future use when creating items
    selectedPenColor = penColor;
}


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QTcpSocket>
#include <QSpinBox>
#include <QGraphicsScene>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    bool connectTCP();
    QByteArray createDataPacket();
private slots:
    void on_lineButton_clicked();
    void on_sendButton_clicked();
    void on_imageButton_clicked();
    void on_barecodeButton_clicked();
    void on_textButton_clicked();
    void on_saveButton_clicked();
    void updateViewSize();
    void on_rectButton_clicked();

    void on_savedScene_doubleClicked(const QModelIndex &index);

    void on_colorComboBox_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;
    //TCP
    QTcpSocket *TCPSocket;
    QByteArray *TCPMessage;
    //Graphic Scene
    QGraphicsScene *scene;
    QSpinBox *widthSpinBox;
    QSpinBox *heightSpinBox;
    QPen *p;
    QHash<QString, QGraphicsScene*> savedScenes;
    QStandardItemModel* savedSceneListModel;
    QColor selectedPenColor = Qt::black; // Default color is black

    QString generateRandomSceneName()
    {
        const QString possibleCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
        const int nameLength = 10;
        QString randomName;
        for (int i = 0; i < nameLength; ++i) {
            int index = rand() % possibleCharacters.length();
            QChar nextChar = possibleCharacters.at(index);
            randomName.append(nextChar);
        }
        return randomName;
    }

};
#endif // MAINWINDOW_H

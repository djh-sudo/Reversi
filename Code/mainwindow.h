#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QPoint>
#include<QTimer>
#include<QButtonGroup>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int Status[8][8];//checkerboard
    int copy[8][8];//copy
    int playRole;//play role of the game
    int H;//Height of screen
    int W;//Width of screen
    int x;//x position
    int y;//y position
    bool flag;//flag of  Game is over
    void Initial();//initial the game
    void Change_playRole();//change the play role
    QPoint p;//point(x,y)
    QPoint start;//left-top point of the screen
    QPoint end;//right-down point of the screen
    QTimer mTimer;//timer of player1
    QTimer mTimer1;//timer of player2
    int xy_value[8][8] = {//Static valuation situation
                          {100, -60, 10, 10, 10, 10, -60, 100},
                          {-60, -80, 5, 5, 5, 5, -80, -60},
                          {10, 5, 1, 1, 1, 1, 5, 10},
                          {5, 5, 1, 1, 1, 1, 5, 5},
                          {5, 5, 1, 1, 1, 1, 5, 5},
                          {10, 5, 1, 1, 1, 1, 5, 10},
                          {-60, -80, 5, 5, 5, 5, -80, -60},
                          {100, -60, 10, 10, 10, 10, -60, 100}
                         };
    int Judge(int x, int y, int currentRole, bool eatChess = true, int gridNum = 8);
    enum{White = 0,Black = 1,Mention = 2,Empty = -1};
    enum{DualPlay = 0,easyMode = 1,HardMode = 2};
    // 0 represent the White player
    // 1 represent the black player
    // 2 represent the optional location
private:
    Ui::MainWindow *ui;
    QButtonGroup*group;//optional the model
protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);
    void Result();
    void Canplay();
    void AIplay();
    void A_AIplay();
    void MakeMove(int player,int i,int j);
    void UnMakeMove(int i,int j);
private slots:
    void on_toolButton_clicked();
    void on_toolButton_2_clicked();
    void choice();
    void on_toolButton_3_clicked();
    void on_toolButton_4_clicked();
};
#endif // MAINWINDOW_H

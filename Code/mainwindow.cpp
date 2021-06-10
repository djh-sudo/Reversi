#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPaintEvent>
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QPixmap>
#include <QMessageBox>
#include <windows.h>
#include <QThread>
#include <QTimer>
#include <QButtonGroup>
#include <QSound>


int play = 0;//default is two poeple play!
static bool index = true;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)//Construction...
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowFlag(Qt::FramelessWindowHint);
    connect(&mTimer,&QTimer::timeout,this,&MainWindow::A_AIplay);
    connect(&mTimer1,&QTimer::timeout,this,&MainWindow::AIplay);
    QImage*img_1=new QImage;
    img_1->load("://Ai.png");
    ui->label->setPixmap(QPixmap::fromImage(*img_1));

    QImage*img_2 = new QImage;
    img_2->load("://w.png");
    ui->label_4->setPixmap(QPixmap::fromImage(*img_2));

    QImage*img_3 = new QImage;
    img_3->load("://b.png");
    ui->label_3->setPixmap(QPixmap::fromImage(*img_3));
    ui->label_4->setHidden(true);
    ui->label_6->setHidden(true);

    QImage*img_4 = new QImage;
    img_4->load("://m.png");

    QImage*img_5 = new QImage;
    img_5->load("://p.png");

    //setting the screen
    start = QPoint(385,36);
    end = QPoint(1011,666);

    //calculating the size of each grid
    H = (end.y()-start.y())/8;//Height of each grid
    W = (end.x()-start.x())/8;//Width of each grid

    //initialize the playGame
    Initial();

    //There are three mode you can choose
    group = new QButtonGroup(this);

    group->addButton(ui->radioButton,DualPlay);
    group->addButton(ui->radioButton_2,easyMode);
    group->addButton(ui->radioButton_3,HardMode);

    //set the default mode is dualplay
    ui->radioButton->setChecked(true);

    //hide the While player flag
    ui->label_4->setHidden(true);
    ui->label_6->setHidden(true);

    //build the connection between singal and slot function
    connect(ui->radioButton,SIGNAL(clicked(bool)),this,SLOT(choice()));
    connect(ui->radioButton_2,SIGNAL(clicked(bool)),this,SLOT(choice()));
    connect(ui->radioButton_3,SIGNAL(clicked(bool)),this,SLOT(choice()));
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::choice(){//three mode which one choose
    play = group->checkedId();
}

void MainWindow::Result(){
    //Judging whether the game wins or loses
    int white = 0;
    int black = 0;
    for(int i = 0; i < 8; i++){
        for(int k = 0;k < 8; k++){
            if(Status[i][k] == Black)
                black++;
            else
                if(Status[i][k] == White)
                    white++;
                else;
        }
    }

    //show the score of each player!
    ui->lcdNumber->display(black);
    ui->lcdNumber_2->display(white);

    //Game is not over....
    for(int i = 0;i < 8; i++){
        for(int k = 0;k < 8; k++){
            if(Judge(i,k,Black,false) > 0 || Judge(i,k,White,false) > 0)
                return;
        }
    }

    //Game is over ...
    if(white > black && flag){
        //White player is win
        flag = false;
        if(mTimer.isActive())
            this->mTimer.stop();
        if(mTimer1.isActive())
            this->mTimer1.stop();
        QMessageBox::information(this,"白方获胜!","\t\t你输了哦!再来一局吧！");
        ui->label_6->setHidden(true);
        QImage*img_4 = new QImage;
        img_4->load("://wsu.png");
        ui->label_4->setPixmap(QPixmap::fromImage(*img_4));
        ui->label_4->setHidden(false);
        return;
    }
    else if(black > white && flag){
        //Black player is win
        if(mTimer.isActive())
            this->mTimer.stop();
        if(mTimer1.isActive())
            this->mTimer1.stop();
        flag = false;
        QMessageBox::information(this,"黑方获胜!","\t\t恭喜你，赢得比赛!");
        QSound::play("://LevelWinSound.wav");
        ui->label_5->setHidden(true);
        QImage*img_5 = new QImage;
        img_5->load("://bsu.png");
        ui->label_3->setPixmap(QPixmap::fromImage(*img_5));
        ui->label_3->setHidden(false);
        return;
    }
    else if(flag){
        if(mTimer.isActive())
            this->mTimer.stop();
        if(mTimer1.isActive())
            this->mTimer1.stop();
        flag = false;
        QMessageBox::information(this,"平局!","\t\t实力相当呀！继续加油!");
        return;
    }
}

//paint the board according to the value of array
void MainWindow::paintEvent(QPaintEvent *){
    QPainter p(this);
    p.drawPixmap(this->rect(), QPixmap("://board.png"));
    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            if(Status[i][j] == Black)
            {
                //下黑棋
                p.drawPixmap(start.x() + i * W, start.y() + j * H, W - 2, H - 2, QPixmap(":/black.png"));
            }
            else if(Status[i][j] == White)
            {
                //下白棋
                p.drawPixmap(start.x() + i * W, start.y() + j * H, W - 2, H - 2, QPixmap(":/white.png"));
            }
            else if(Status[i][j]== Mention){
                p.drawPixmap(start.x() + i * W, start.y() + j * H, W - 2, H - 2, QPixmap(":/quan.png"));
            }
            else;
        }
    }
}

//initialize the play
void MainWindow::Initial(){
    flag = true;
    for(int i=0;i<8;i++){
        for(int k=0;k<8;k++){
            Status[i][k] = Empty;//empty
        }
    }

    //initialize the piece
    Status[3][3] = White;
    Status[3][4] = Black;
    Status[4][3] = Black;
    Status[4][4] = White;

    playRole = Black;//default is black play placement priority

    //give the place mention
    Canplay();

    //setting the score
    ui->lcdNumber->display(2);
    ui->lcdNumber_2->display(2);

    //hidden the white player's flag
    ui->label_4->setHidden(true);
    ui->label_6->setHidden(true);

}

//click the mouse to place the piece
void MainWindow::mousePressEvent(QMouseEvent *e){
    if((e->button() & Qt::LeftButton)&&flag)
    {
        p = e->globalPos()-this->frameGeometry().topLeft();
    }
    if(flag){
        int x = e->x();//mouse click position[x]
        int y = e->y();//mouse click position[y]
        int i;
        int k;
        if(x >= start.x() && x <= end.x() && y >= start.y() && y <= end.y()){
            if(index){
                QSound::play("://BackButtonSound.wav");
            }
            i = (e->x()-start.x())/(W);
            k = (e->y()-start.y())/(H);

            //draw mention
            Canplay();

            if(Judge(i,k,playRole) > 0){
                this->Change_playRole();
                Canplay();
                Result();
            }
            update();//update the GUI
            Result();
        }else{
            Result();
            if(mTimer.isActive())
                this->mTimer.stop();
            return;
        }
    }
    else
        return;

}

//mouse moveEvent,you can ignore
void MainWindow::mouseMoveEvent(QMouseEvent *e){//移动窗体
    if(e->buttons() & Qt::LeftButton)
        move(e->globalPos() - p);
    else return;
}

//Change the play role!
void MainWindow::Change_playRole(){//交换身份
    if(playRole == Black){
        ui->label_3->setHidden(true);
        ui->label_5->setHidden(true);
        ui->label_4->setHidden(false);
        ui->label_6->setHidden(false);
        playRole = White;

        if(play == DualPlay){//Dualplay mode
        }
        else if(play == easyMode){//easy mode
            mTimer1.start(1000);
        }
        else if(play == HardMode){//hard mode
            mTimer.start(1000);
        }
    }
    else
        if(playRole == White){
            playRole = Black;
            ui->label_3->setHidden(false);
            ui->label_5->setHidden(false);
            ui->label_4->setHidden(true);
            ui->label_6->setHidden(true);
        }
}

//return the (x,y) eatnumber
int  MainWindow::Judge(int x, int y, int currentRole,bool eatChess, int gridNum)//规则
{
    //eight direction (x,y)| 0 is null; -1 represent left ; 1 represent right
    int dir[8][2]={{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1},{0,1},{1,1}};
    //initial the parameter
    int tmpX = x, tmpY = y;
    int i = 0;
    int eatNum = 0; //eat the opponent's number of piece

    if(!((Status[tmpX][tmpY] == Empty)||(Status[tmpX][tmpY] == Mention)))//如果不为空，那么此坐标不可以落子
        return 0;
    for(i = 0 ; i <8; i++)//Traverse 8 directions
    {
        //Go in this direction
        tmpX += dir[i][0]; tmpY += dir[i][1];
        //If there is no out-of-bounds, and the adjacent piece is the opponent piece, then there is the possibility of taking a piece
        if((tmpX < gridNum && tmpX >=0 && tmpY < gridNum && tmpY >= 0)&& (Status[tmpX][tmpY] != currentRole) && !((Status[tmpX][tmpY] == Empty)||(Status[tmpX][tmpY] == Mention)))
        {
            //go in this direction
            tmpX += dir[i][0];
            tmpY += dir[i][1];
            while(tmpX < gridNum && tmpX >=0 && tmpY < gridNum && tmpY >= 0)
            {
                //This direction is impossible to placement
                if(Status[tmpX][tmpY] == Empty || Status[tmpX][tmpY] == Mention)
                    break;
                // Find your own piece, which means you can take a pawn
                if(Status[tmpX][tmpY] == currentRole)
                {
                    if(eatChess == true){ //flag is true
                        Status[x][y] = currentRole;
                        tmpX -= dir[i][0];//back
                        tmpY -= dir[i][1];
                        while((tmpX != x )||(tmpY != y))//back
                        {
                            //marke as own
                            Status[tmpX][tmpY] = currentRole;
                            //back
                            tmpX -= dir[i][0];
                            tmpY -= dir[i][1];
                            eatNum++;
                        }
                    }
                    else//flag = false only judge
                    {
                        tmpX -= dir[i][0];//back
                        tmpY -= dir[i][1];
                        while((tmpX != x )||(tmpY != y))//计算可以吃子的个数
                        {
                            tmpX -= dir[i][0];//back
                            tmpY -= dir[i][1];
                            eatNum++;
                        }
                    }
                    break;
                }
                //Did not find one's own piece, just take a step forward
                tmpX += dir[i][0]; tmpY += dir[i][1];
            }
        }
        //change the direction
        tmpX = x;
        tmpY = y;
    }
    return eatNum;
}

//This player can play? and give the mention
void MainWindow::Canplay(){
    bool index = true;
    for(int i = 0; i < 8; i++){
        for(int k = 0; k < 8; k++){
            if(Status[i][k] == Mention)
                Status[i][k] = Empty;
            if(Judge(i,k,playRole,false)>0){
                index = false;
                Status[i][k] = Mention;
            }
        }
    }

    if(index){
        Result();
        //This player can't placement the piece
        if(flag){
            this->Change_playRole();
            Canplay();
            update();
        }
    }
}


//nomal AI
void MainWindow::AIplay(){
    if(mTimer1.isActive())
        this->mTimer1.stop();
    int x;
    int y;
    int max = -999;
    int number = 0;
    int i = 0;
    int j = 0;
    for(i = 0; i < 8; i++){//二重循环寻找最大吃子点
        for(j = 0; j < 8; j++){
            number = Judge(i,j,White,false);//返回吃子个数
            if(number > 0){
                if(number > max){
                    max = number;
                    x = i;
                    y = j;
                }
            }
        }
    }
    if(max == -999){//没有吃子点
        this->Change_playRole();//交换角色
        Canplay();//给出人工是能下子的位置
        update();//更新渲染界面
        Result();//更新结果
        return;
    }
    if(Judge(x,y,White) > 0){//机器可以下子
        this->Change_playRole();//下完子之后交换角色
        Canplay();
        update();
        Result();
    }

}

//advanced AI
void MainWindow::A_AIplay()//hard
{
    if(mTimer.isActive())
        this->mTimer.stop();
    int max = -100;
    int eat_max = 0;
    int x, y;
    //find the best value point
    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            if(Judge(i, j, White, false))
            {
                if(xy_value[i][j] > max)
                {
                    x = i;
                    y = j;
                    max = xy_value[i][j];
                    eat_max = Judge(i, j, White, false);
                }
            }
        }
    }

    int number;
    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            if(max == xy_value[i][j]){
                number = Judge(i,j,White,false);
                if(number > eat_max){
                    eat_max = number;
                    x = i;
                    y = j;
                }
            }
        }
    }
    //This player can't place the piece
    if(max == -999)
    {
        this->Change_playRole();
        Canplay();
        update();
        Result();
        return;
    }else
    {
        if(Judge(x, y, 0) > 0)
        {
            //Change play role
            this->Change_playRole();
            //update
            Canplay();
            update();
            Result();
        }
    }
}

//restart the Game!
void MainWindow::on_toolButton_clicked()
{
    ui->lcdNumber_2->setHidden(false);
    Initial();
    update();
    QImage*img_4 = new QImage;
    img_4->load("://w.png");
    ui->label_4->setPixmap(QPixmap::fromImage(*img_4));
    ui->label_6->setHidden(true);
    QImage*img_5 = new QImage;
    img_5->load("://b.png");
    ui->label_3->setPixmap(QPixmap::fromImage(*img_5));
    ui->label_5->setHidden(false);
}

//quit
void MainWindow::on_toolButton_2_clicked()
{
    if(QMessageBox::Yes==QMessageBox::warning(this,"提示-info","确认退出游戏吗?",QMessageBox::Yes|QMessageBox::No,QMessageBox::No)){
        this->close();
    }
    else
        return;
}

//sound play
void MainWindow::on_toolButton_3_clicked()
{
    index = !index;
    if(index){
        ui->toolButton_3->setIcon(QIcon("://sound.png"));
    }
    else
    {
        ui->toolButton_3->setIcon(QIcon("://no.png"));
    }
    return;
}

//infomation
void MainWindow::on_toolButton_4_clicked()
{
    QMessageBox::information(this,"info-段小二提醒你","黑白棋规则:\n\t1.黑白双方轮流下棋,一次下一颗 黑方先手.\n\t2.一旦你的两颗棋子之间有对方棋子,将会吃掉对方的棋子,八个方向均有效.\n\t3.简单版的AI每次尽可能多的吃子\n\t4.困难版的AI会考虑一些关键点,优先占据.\n\t5.虚线的圈表明你可以下子的位置,如果一方不能下子，将会交换角色,机器不能下子时，可能没有虚线圈提醒.\n\t祝你好运!");
}

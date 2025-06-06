#include "userwidget.h"
#include "ui_userwidget.h"
#include "back_head/learner.h"
#include "back_head/database.h"
#include <QPainter>
#include <QTimer>
#define Min(x,y) x>y?y : x

UserWidget::UserWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UserWidget)
{
    ui->setupUi(this);
    connect(ui->clearButton,&QToolButton::clicked,this,&UserWidget::clear);
    connect(ui->refreshButton,&QToolButton::clicked,this,&UserWidget::refresh);
    setUser();
    setTotalAcc();
    setDays();
    setAcc();
    //setPic();
    update();
    qDebug()<<ui->accWidget->height()<<' '<<ui->accWidget->width();
}

UserWidget::~UserWidget()
{
    delete ui;
}

void UserWidget::setUser()
{
    Learner * learner=Learner::getInstance();
    ui->userLabel->setText(learner->getName());
    QFont *font=new QFont("黑体",16);
    font->setBold(true);
    ui->userLabel->setFont(*font);
    ui->userLabel->setAlignment(Qt::AlignCenter);
}

void UserWidget::setPic()
{

}

void UserWidget::setDays()
{
    dailyLearningData=WordDatabase::getAllDailyLearningCountInDays(30,1);
}

void UserWidget::setAcc()
{
    dailyLearningAcc=WordDatabase::getAllDailyLearningAccuracyInDays(30,1);
}

void UserWidget::setTotalAcc()
{
    double acc=(WordDatabase::getAllLearningAccuracy(30,1));
    ui->accLabel->setText("累计正确率："+QString::number(acc,'f',4));
    QFont *font=new QFont("黑体",16);
    // font->setBold(true);
    ui->accLabel->setFont(*font);
    ui->accLabel->setAlignment(Qt::AlignCenter);
}

void UserWidget::paintEvent(QPaintEvent *event)
{
    qDebug() << "paint";
    Q_UNUSED(event);
    QPainter painter(this);

    // Constants
    const int ROWS = 3;
    const int COLS = 10;
    const int max_count = 100;
    const int RADIUS = 6; // 圆角半径
    const int BORDER_WIDTH = 1;
    const QColor BORDER_COLOR(220, 220, 220);
    const QColor EMPTY_COLOR(235, 237, 240); // github灰色

    // 获取控件区域
    QRect dayRect = ui->dayWidget->geometry();
    QRect accRect = ui->accWidget->geometry();

    // 计算格子尺寸，使内容物在控件内居中并尽可能延展
    int gridWidth = (dayRect.width() - (COLS + 1) * MARGIN) / COLS;
    int gridHeight = (dayRect.height() - (ROWS + 1) * MARGIN) / ROWS;
    int cellSize = qMin(gridWidth, gridHeight);

    int totalGridWidth = cellSize * COLS + MARGIN * (COLS + 1);
    int totalGridHeight = cellSize * ROWS + MARGIN * (ROWS + 1);

    int offsetX = (dayRect.width() - totalGridWidth) / 2;
    int offsetY = (dayRect.height() - totalGridHeight) / 2;

    // 绘制天数统计
    painter.save();
    painter.translate(dayRect.topLeft() + QPoint(offsetX, offsetY));
    for (int i = 0; i < COLS; ++i) {
        for (int j = 0; j < ROWS; ++j) {
            int idx = j * COLS + i;
            int count = (idx < dailyLearningData.size()) ? dailyLearningData[idx] : 0;
            QColor color;
            // 仿github贡献色阶
            if (count == 0) {
                color = EMPTY_COLOR;
            } else if (count < max_count * 0.25) {
                color = QColor(155, 233, 168);
            } else if (count < max_count * 0.5) {
                color = QColor(64, 196, 99);
            } else if (count < max_count * 0.75) {
                color = QColor(48, 161, 78);
            } else {
                color = QColor(33, 110, 57);
            }
            int x = MARGIN * (i + 1) + cellSize * i;
            int y = MARGIN * (j + 1) + cellSize * j;
            QRectF rect(x, y, cellSize, cellSize);

            painter.setPen(Qt::NoPen);
            painter.setBrush(color);
            painter.drawRoundedRect(rect, RADIUS, RADIUS);

            // 画边框
            painter.setPen(QPen(BORDER_COLOR, BORDER_WIDTH));
            painter.setBrush(Qt::NoBrush);
            painter.drawRoundedRect(rect, RADIUS, RADIUS);
        }
    }
    painter.restore();

    // 计算accWidget的格子尺寸和偏移
    gridWidth = (accRect.width() - (COLS + 1) * MARGIN) / COLS;
    gridHeight = (accRect.height() - (ROWS + 1) * MARGIN) / ROWS;
    cellSize = qMin(gridWidth, gridHeight);
    totalGridWidth = cellSize * COLS + MARGIN * (COLS + 1);
    totalGridHeight = cellSize * ROWS + MARGIN * (ROWS + 1);
    offsetX = (accRect.width() - totalGridWidth) / 2;
    offsetY = (accRect.height() - totalGridHeight) / 2;

    // 绘制正确率统计
    painter.save();
    painter.translate(accRect.topLeft() + QPoint(offsetX, offsetY));
    for (int i = 0; i < COLS; ++i) {
        for (int j = 0; j < ROWS; ++j) {
            int idx = j * COLS + i;
            double acc = (idx < dailyLearningAcc.size()) ? dailyLearningAcc[idx] : 0.0;
            QColor color;
            // 仿github贡献色阶
            if (acc == 0.0) {
                color = EMPTY_COLOR;
            } else if (acc < 0.25) {
                color = QColor(255, 223, 186);
            } else if (acc < 0.5) {
                color = QColor(255, 179, 71);
            } else if (acc < 0.75) {
                color = QColor(255, 140, 0);
            } else {
                color = QColor(204, 102, 0);
            }
            int x = MARGIN * (i + 1) + cellSize * i;
            int y = MARGIN * (j + 1) + cellSize * j;
            QRectF rect(x, y, cellSize, cellSize);

            painter.setPen(Qt::NoPen);
            painter.setBrush(color);
            painter.drawRoundedRect(rect, RADIUS, RADIUS);

            // 画边框
            painter.setPen(QPen(BORDER_COLOR, BORDER_WIDTH));
            painter.setBrush(Qt::NoBrush);
            painter.drawRoundedRect(rect, RADIUS, RADIUS);
        }
    }
    painter.restore();
}

void UserWidget::clear()
{
    WordDatabase::resetAll();
    ui->clearLabel->setText("已清空");
    QFont *font=new QFont("黑体",16);
    font->setBold(true);
    ui->clearLabel->setFont(*font);
    ui->clearLabel->setAlignment(Qt::AlignCenter);
    refresh();
    QTimer::singleShot(1000,this,[=](){ui->clearLabel->setText("");});
}

void UserWidget::refresh()
{
    setUser();
    setTotalAcc();
    setDays();
    setAcc();
    //setPic();
    update();
}

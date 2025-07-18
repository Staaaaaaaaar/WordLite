#include "userwidget.h"
#include "ui_userwidget.h"
#include "../back_head/learner.h"
#include "../back_head/database.h"
#include <QPainter>
#include <QTimer>
#define Min(x,y) x>y?y : x

// 添加MARGIN常量定义
constexpr int MARGIN = 4;

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

    // 关键：设置小方格控件为透明背景
    ui->dayWidget->setAttribute(Qt::WA_TranslucentBackground, true);
    ui->accWidget->setAttribute(Qt::WA_TranslucentBackground, true);
    ui->dayWidget->setAutoFillBackground(false);
    ui->accWidget->setAutoFillBackground(false);
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
    const int RADIUS = 10;
    const int BORDER_WIDTH = 1;
    const QColor BORDER_COLOR(230, 103, 34); // #e67e22
    const QColor EMPTY_COLOR(34, 34, 34);    // #222222 深灰

    // 获取控件区域
    QRect dayRect = ui->dayWidget->geometry();
    QRect accRect = ui->accWidget->geometry();

    // 关键：先填充小方格区域为黑色，避免透明导致底色异常
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.drawRect(dayRect);
    painter.drawRect(accRect);
    painter.restore();

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
            // 色阶更明显的橙色系
            if (count == 0) {
                color = EMPTY_COLOR;
            } else if (count < max_count * 0.25) {
                color = QColor(80, 40, 0);      // 极深橙（接近黑）
            } else if (count < max_count * 0.5) {
                color = QColor(204, 102, 0);    // 深橙
            } else if (count < max_count * 0.75) {
                color = QColor(230, 126, 34);   // #e67e22
            } else {
                color = QColor(255, 220, 120);  // 亮橙（接近白）
            }
            int x = MARGIN * (i + 1) + cellSize * i;
            int y = MARGIN * (j + 1) + cellSize * j;
            QRectF rect(x, y, cellSize, cellSize);

            painter.setPen(Qt::NoPen);
            painter.setBrush(color);
            painter.drawRoundedRect(rect, RADIUS, RADIUS);

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
            // 色阶更明显的橙色系
            if (acc == 0.0) {
                color = EMPTY_COLOR;
            } else if (acc < 0.25) {
                color = QColor(80, 40, 0);      // 极深橙
            } else if (acc < 0.5) {
                color = QColor(204, 102, 0);    // 深橙
            } else if (acc < 0.75) {
                color = QColor(230, 126, 34);   // #e67e22
            } else {
                color = QColor(255, 220, 120);  // 亮橙
            }
            int x = MARGIN * (i + 1) + cellSize * i;
            int y = MARGIN * (j + 1) + cellSize * j;
            QRectF rect(x, y, cellSize, cellSize);

            painter.setPen(Qt::NoPen);
            painter.setBrush(color);
            painter.drawRoundedRect(rect, RADIUS, RADIUS);

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

// --- 修改：当用户点击“更新学习记录”按钮时 ---
void UserWidget::on_refreshButton_clicked()
{
    // 您可以在这里先刷新界面上的其他数据，例如调用您已有的 refresh() 函数
    refresh(); 

    // 然后检查成就
    checkLearningAchievements();
}

// --- 修改：当用户界面每次显示时，也会自动调用 ---
void UserWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    // 自动触发一次更新和检查
    on_refreshButton_clicked();
}

// --- 完整替换这个函数 ---
void UserWidget::checkLearningAchievements()
{
    Learner* learner = Learner::getInstance();
    if (!learner) return; 

    // --- 1. 计算所需指标 ---

    // 获取自用户开始学习以来的所有每日学习记录
    int daysSinceStart = learner->getStartTime().daysTo(QDateTime::currentDateTime()) + 1;
    if (daysSinceStart <= 0) daysSinceStart = 1; // 至少查询1天
    QVector<int> dailyCounts = WordDatabase::getAllDailyLearningCountInDays(daysSinceStart, 1);

    // a) 计算连续学习天数
    int continuousDays = 0;
    for (int i = dailyCounts.size() - 1; i >= 0; --i) {
        if (dailyCounts[i] > 0) {
            continuousDays++;
        } else {
            break;
        }
    }

    // b) 计算累计学习天数
    int totalDays = 0;
    for (int count : dailyCounts) {
        if (count > 0) {
            totalDays++;
        }
    }

    // --- 修改：注释掉未使用的变量以消除警告 ---
    // int totalLearnedWords = WordDatabase::getAllTotalWordCount();

    // d) 获取已掌握单词数 (用于成就 #10, #11, #12)
    int masteredWords = WordDatabase::getAllWordsWithDifficultyOneCount();

    // e) 获取已解锁成就总数
    int unlockedCount = learner->getUnlockedAchievements().size();


    // --- 2. 检查各项成就 (ID 5-13) ---

    // 连续学习天数成就
    if (continuousDays >= 7)   emit requestUnlockAchievement(5);
    if (continuousDays >= 30)  emit requestUnlockAchievement(6);
    if (continuousDays >= 100) emit requestUnlockAchievement(7);

    // 累计学习天数成就
    if (totalDays >= 100) emit requestUnlockAchievement(8);
    if (totalDays >= 365) emit requestUnlockAchievement(9);

    // 掌握单词数系列成就
    if (masteredWords >= 500)  emit requestUnlockAchievement(10); // 词汇大师
    if (masteredWords >= 1000) emit requestUnlockAchievement(11); // 词海遨游
    if (masteredWords >= 2000) emit requestUnlockAchievement(12); // 词霸精英


    // 解锁所有成就 (假设总共有13个非隐藏成就)
    if (unlockedCount >= 12 && !learner->getUnlockedAchievements().contains(13)) {
        emit requestUnlockAchievement(13);
    }
}

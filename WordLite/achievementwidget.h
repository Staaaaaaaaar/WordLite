#ifndef ACHIEVEMENTWIDGET_H
#define ACHIEVEMENTWIDGET_H

#include <QWidget>
#include <QDateTime>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QGroupBox>
#include <QLabel>

namespace Ui {
class AchievementWidget;
}

struct Achievement {
    int id;
    QString title;
    QString description;
    bool unlocked;
    QDateTime unlockDate;
    QString iconPath;
};

class AchievementWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AchievementWidget(QWidget *parent = nullptr);
    ~AchievementWidget();

    QVector<Achievement> testAchievements;

    // 设置UI和连接信号槽
    void setupUI();
    void connectSignals();
    void refreshUI();
    QVector<Achievement> getAchievementsFromSource();

    void refreshAchievements(int idx);

private:
    Ui::AchievementWidget *ui;

    QScrollArea *scrollArea;
    QWidget *scrollContent;
    QVBoxLayout *scrollLayout;
};

#endif // ACHIEVEMENTWIDGET_H

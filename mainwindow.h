﻿#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "yuv_player.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



private slots:
    void on_pushButton_clicked();

    void on_stopButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    YuvPlayer *m_player = nullptr;
};
#endif // MAINWINDOW_H

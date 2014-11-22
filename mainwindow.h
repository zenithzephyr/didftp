#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QDebug>
#include <QMenuBar>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QAction *refreshAction;
    QAction *controlAction;
    QAction *exitAction;

    QMenu *controlMenu;

    QTableWidget *ftpList;


    void createActions();
    void createMenus();
    void createTable();
    void updateStatusBar();

private slots:
    void showMonitor();
    void showUpload(QModelIndex index);
};

#endif // MAINWINDOW_H

#pragma once

#include <QMainWindow>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QApplication>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMessageBox>

#include <memory>

#include "Canvas.h"
#include "CustomWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    void loadRecipe(const QString& filename);
    void newRecipe();
    void saveRecipe(const QString& filename);
    void initToolTree(const QString& initFilename);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    std::unique_ptr<Canvas> m_scene;
};

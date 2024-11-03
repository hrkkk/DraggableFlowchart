#include "MainWindow.h"
#include "./ui_MainWindow.h"

#include "Canvas.h"
#include "CustomWidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Canvas* scene = new Canvas();
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setAcceptDrops(true);

    CustomWidget* widget1 = new CustomWidget(this);
    CustomWidget* widget2 = new CustomWidget(this);
    CustomWidget* widget3 = new CustomWidget(this);

    QVBoxLayout* layout = new QVBoxLayout(ui->widget);
    layout->addWidget(widget1);
    layout->addWidget(widget2);
    layout->addWidget(widget3);

    ui->widget->setLayout(layout);
}

MainWindow::~MainWindow()
{
    delete ui;
}

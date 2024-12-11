#include "MainWindow.h"
#include "./ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_scene(std::make_unique<Canvas>())
{
    ui->setupUi(this);

    this->setWindowTitle(u8"配方编辑器");

    // 获取主屏幕的几何形状
    QRect screenGeometry = QApplication::desktop()->availableGeometry();
    // 计算屏幕尺寸的80%
    int width = static_cast<int>(screenGeometry.width() * 0.8);
    int height = static_cast<int>(screenGeometry.height() * 0.8);
    // 设置窗口大小为屏幕尺寸的80%
    this->resize(width, height);

    ui->graphicsView->setScene(m_scene.get());
    ui->graphicsView->setAcceptDrops(true);
    connect(m_scene.get(), &Canvas::sig_scaleScene, this, [=](float x, float y) {
        ui->graphicsView->scale(x, y);
    });

    connect(ui->btn_load, &QPushButton::clicked, this, [=]() {
        // 弹出文件选择对话框
        QString filename = QFileDialog::getOpenFileName(this, "Open File", QDir::homePath(), "Ini Files (*.ini)");
        if (filename.isEmpty()) {
            return;
        }
        // 加载文件内容
        loadRecipe(filename);
    });
    connect(ui->btn_new, &QPushButton::clicked, this, [=]() {
        auto ret = QMessageBox::warning(this, "Warning", u8"当前配方将被清空，请确认是否要创建新配方？", QMessageBox::Yes, QMessageBox::No);
        if (ret != QMessageBox::Yes) {
            return;
        }
        newRecipe();
    });
    connect(ui->btn_save, &QPushButton::clicked, this, [=]() {
        // 弹出保存位置对话框
        QString filename = QFileDialog::getSaveFileName(this, "Save File", QDir::homePath()  + "/recipe.ini", "Ini Files (*.ini);;All Files (*)");
        if (filename.isEmpty()) {
            QMessageBox::warning(this, "Warning", u8"路径无效，保存失败");
            return;
        }
        // 保存到文件
        saveRecipe(filename);
    });

    // records.json文件放在exe文件同级目录下
    initToolTree("./records.json");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadRecipe(const QString& filename)
{
    m_scene->loadRecipe(filename);
}

void MainWindow::newRecipe()
{
    m_scene->clearCanvas();
}

void MainWindow::saveRecipe(const QString& filename)
{
    m_scene->saveRecipe(filename);
}

void MainWindow::initToolTree(const QString& initFilename)
{
    // 读取 json 文件内容
    QFile file(initFilename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // 处理错误，文件无法打开
        qDebug() << "无法打开文件：" << file.errorString();
        return;
    }

    QByteArray jsonData = file.readAll();

    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &jsonError);
    if (jsonError.error != QJsonParseError::NoError) {
        // 处理错误，JSON格式不正确
        qDebug() << "JSON解析错误：" << jsonError.errorString();
        return;
    }

    // 确保JSON文档是一个对象
    if (!jsonDoc.isObject()) {
        qDebug() << "JSON不是一个对象";
        return;
    }

    QJsonObject jsonObject = jsonDoc.object();
    QJsonArray groupsArray = jsonObject.value("groups").toArray();

    // 遍历每一组数据
    for (const QJsonValue& value : groupsArray) {
        if (value.isObject()) {
            QJsonObject obj = value.toObject();
            QString name = obj.value("name").toString();
            QString icon = obj.value("icon").toString();
            // 创建一级树节点
            QTreeWidgetItem* parentItem = new QTreeWidgetItem(ui->treeWidget);
            parentItem->setText(0, name);
            parentItem->setIcon(0, QIcon(icon));

            QJsonArray itemArray = obj.value("items").toArray();
            // 遍历该组中的每一个数据
            for (const QJsonValue& item : itemArray) {
                QJsonObject obj = item.toObject();
                QString name = obj.value("name").toString();
                QString icon = obj.value("icon").toString();
                QString text = obj.value("text").toString();
                // 创建二级树节点
                CustomWidget* widget = new CustomWidget(name, text, icon, this);
                QTreeWidgetItem* childItem = new QTreeWidgetItem(parentItem);
                ui->treeWidget->setItemWidget(childItem, 0, widget);
            }
        }
    }

    // 默认展开控件树
    ui->treeWidget->expandAll();
}

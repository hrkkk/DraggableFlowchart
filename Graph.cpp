#include "Graph.h"

#include <QDebug>

Graph::Graph() {}

void Graph::resetGraph()
{
    m_nodes.clear();
    m_connections.clear();
}

void Graph::addNode(const std::string& name, const std::string& text, float x, float y)
{
    if (m_nodes.find(name) == m_nodes.end()) {
        m_nodes[name] = Node(name, text, x, y);
    }
    m_connections[name] = std::list<std::string>();
}

void Graph::removeNode(const std::string& name)
{
    if (m_nodes.find(name) != m_nodes.end()) {
        // 从邻接表中移除所有与该节点相连的边
        for (auto& pair : m_connections) {
            pair.second.remove(name);
        }
        // 删除节点信息
        m_nodes.erase(name);
        m_connections.erase(name);
    }
}

void Graph::updateNode(const std::string& name, float x, float y)
{
    if (m_nodes.find(name) != m_nodes.end()) {
        m_nodes[name].x = x;
        m_nodes[name].y = y;
    }
}

std::pair<float, float> Graph::getNodePosition(const std::string& name)
{
    if (m_nodes.find(name) != m_nodes.end()) {
        return std::pair<float, float>(m_nodes[name].x, m_nodes[name].y);
    }
    return {};
}

void Graph::addEdge(const std::string& src, const std::string& dst)
{
    if (m_nodes.find(src) != m_nodes.end()) {
        m_connections[src].push_back(dst);
    }
}

void Graph::removeEdge(const std::string& src, const std::string& dst)
{
    if (m_nodes.find(src) != m_nodes.end()) {
        m_connections[src].remove(dst);
    }
}

void Graph::saveToFile(const std::string& filename)
{
    QSettings settings(QString::fromStdString(filename), QSettings::IniFormat);
    // 先清空原文件中的内容
    settings.clear();
    // 写入当前画布中的所有节点信息和连接信息
    for (auto& node : m_nodes) {
        settings.beginGroup(QString::fromStdString(node.first));
        settings.setValue("text", QString::fromStdString(node.second.text));
        settings.setValue("x", node.second.x);
        settings.setValue("y", node.second.y);
        if (m_connections.find(node.first) != m_connections.end()) {
            QString tmp;
            for (auto& node : m_connections[node.first]) {
                tmp.append(QString::fromStdString(node));
                tmp.append(",");
            }
            if (tmp.endsWith(",")) {
                tmp.chop(1);
            }
            settings.setValue("connections", tmp);
        }
        settings.endGroup();
    }
}

void Graph::loadFromFile(const std::string& filename)
{
    QSettings settings(QString::fromStdString(filename), QSettings::IniFormat);

    QStringList groups = settings.childGroups();
    foreach (const QString& group, groups) {
        // 进入该组
        settings.beginGroup(group);
        QString text = settings.value("text").toString();
        float x = settings.value("x").toFloat();
        float y = settings.value("y").toFloat();
        QString connections = settings.value("connections").toString();
        QStringList connectedNode = connections.split(",");
        // 添加节点
        addNode(group.toStdString(), text.toStdString(), x, y);
        // 添加连接信息
        foreach (const QString& dst, connectedNode) {
            if (!group.isEmpty() && !dst.isEmpty()) {
                addEdge(group.toStdString(), dst.toStdString());
            }
        }
        // 退出该组
        settings.endGroup();
    }
}

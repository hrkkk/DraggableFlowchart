#ifndef GRAPH_H
#define GRAPH_H

#include <QSettings>

#include <string>
#include <unordered_map>

struct Node {
    std::string name;   // 控件名称
    std::string text;   // 控件中文名
    float x, y;         // 控件位置信息

    Node() {}
    Node(std::string name, std::string text, float x, float y) : name(name), text(text), x(x), y(y) {}
};

class Graph
{
public:
    Graph();

    void resetGraph();
    void addNode(const std::string& name, const std::string& text, float x, float y);
    void removeNode(const std::string& name);
    void updateNode(const std::string& name, float x, float y);
    std::pair<float, float> getNodePosition(const std::string& name);

    void addEdge(const std::string& src, const std::string& dst);
    void removeEdge(const std::string& src, const std::string& dst);

    void saveToFile(const std::string& filename);
    void loadFromFile(const std::string& filename);

public:
    std::unordered_map<std::string, Node> m_nodes;      // 存储图中的所有节点
    std::unordered_map<std::string, std::list<std::string>> m_connections;      // 存储节点间的连接关系（邻接表）
};

#endif // GRAPH_H

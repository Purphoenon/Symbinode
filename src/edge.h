#ifndef EDGE_H
#define EDGE_H
#include <QQuickItem>
#include <QQuickView>
#include <QJsonObject>
#include "cubicbezier.h"

class Socket;
class Scene;

class Edge: public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QVector2D startPosition READ startPosition WRITE setStartPosition NOTIFY startPositionChanged)
    Q_PROPERTY(QVector2D endPosition READ endPosition WRITE setEndPosition NOTIFY endPositionChanged)
    Q_PROPERTY(bool selected READ selected WRITE setSelected)
public:
    Edge(QQuickItem *parent = nullptr);
    Edge(const Edge &edge);
    ~Edge();
    QVector2D startPosition();
    void setStartPosition(QVector2D pos);
    QVector2D endPosition();
    void setEndPosition(QVector2D pos);
    Socket *startSocket();
    void setStartSocket(Socket *socket);
    Socket *endSocket();
    void setEndSocket(Socket *socket);
    Socket *findSockets(Scene* scene, float x, float y);
    bool selected();
    void setSelected(bool selected);
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json);
signals:
    void startPositionChanged(QVector2D pos);
    void endPositionChanged(QVector2D pos);
public slots:
    void updateScale(float scale);
    void pressedEdge(bool controlModifier);
private:
    CubicBezier *grEdge;
    QVector2D m_startPos;
    QVector2D m_endPos;
    Socket *m_startSocket = nullptr;
    Socket *m_endSocket = nullptr;
    float m_scale = 1.0f;
    bool m_selected = false;
};

#endif // EDGE_H

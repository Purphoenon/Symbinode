#ifndef TAB_H
#define TAB_H
#include <QQuickItem>
#include <QQuickView>
#include "scene.h"

class Tab: public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(Scene *scene READ scene)
public:
    Tab(QQuickItem *parent = nullptr);
    ~Tab();
    Scene *scene();    
    void setTitle(QString fileName, bool modified);
    void setSelected(bool select);
public slots:
    void activate();
    void close();
signals:
    void changeActiveTab(Tab *tab);
    void closedTab(Tab *tab);
    void movedTab(Tab *tab, int index);
private:
    Scene *m_scene = nullptr;
    QQuickView *view;
    QQuickItem *grTab;
    float dragX = 0.0f;
    float dragY = 0.0f;
    bool selected = true;
};

#endif // TAB_H

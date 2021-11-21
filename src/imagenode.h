#ifndef IMAGENODE_H
#define IMAGENODE_H

#include "node.h"
#include "image.h"

class ImageNode: public Node
{
    Q_OBJECT
public:
    ImageNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), QString file = "", float transX = 0.0f, float transY = 0.0f);
    ~ImageNode();
    unsigned int &getPreviewTexture() override;
    void serialize(QJsonObject &json) const override;
    void deserialize(const QJsonObject &json, QHash<QUuid, Socket*> &hash) override;
    void operation() override;
    ImageNode *clone() override;    
    float translationX();
    void setTranslationX(float x);
    float translationY();
    void setTranslationY(float y);
signals:
    void fileChosed(QString path);
public slots:
    void openFile();
    void setOutput();
    void loadTexture(QString path);
    void updateTransX(qreal x);
    void updateTransY(qreal y);
    void previewGenerated();
private:
    ImageObject *preview;
    QString m_filePath = "";
    QString m_fileName = "Open";
    float m_transX = 0.0f;
    float m_transY = 0.0f;
};

#endif // IMAGENODE_H

/*
 * Copyright © 2020 Gukova Anastasiia
 * Copyright © 2020 Gukov Anton <fexcron@gmail.com>
 *
 *
 * This file is part of Symbinode.
 *
 * Symbinode is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Symbinode is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Symbinode.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef VORONOINODE_H
#define VORONOINODE_H

#include "node.h"
#include "voronoi.h"

struct VoronoiParams {
    float jitter = 1.0f;
    float intensity = 1.0f;
    float borders = 0.0f;
    int scale = 5;
    int scaleX = 1;
    int scaleY = 1;
    int seed = 1;
    bool inverse = false;
};

class VoronoiNode: public Node
{
    Q_OBJECT
public:
    VoronoiNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024),
                GLint bpc = GL_RGBA16, VoronoiParams crystals = VoronoiParams(),
                VoronoiParams borders = VoronoiParams(), VoronoiParams solid = VoronoiParams(),
                VoronoiParams worley = VoronoiParams(), QString voronoiType = "crystals");
    ~VoronoiNode();
    void operation() override;
    unsigned int &getPreviewTexture() override;
    void saveTexture(QString fileName) override;
    VoronoiNode *clone() override;
    void serialize(QJsonObject &json) const override;
    void deserialize(const QJsonObject &json, QHash<QUuid, Socket*> &hash) override;
    VoronoiParams crystalsParam();
    VoronoiParams bordersParam();
    VoronoiParams solidParam();
    VoronoiParams worleyParam();
    QString voronoiType();
    void setVoronoiType(QString type);
    int voronoiScale();
    void setVoronoiScale(int scale);
    int scaleX();
    void setScaleX(int scale);
    int scaleY();
    void setScaleY(int scale);
    float jitter();
    void setJitter(float jitter);
    bool inverse();
    void setInverse(bool inverse);
    float intensity();
    void setIntensity(float intensity);
    float bordersSize();
    void setBordersSize(float size);
    int seed();
    void setSeed(int seed);
signals:
    void voronoiTypeChanged(QString type);
    void voronoiScaleChanged(int scale);
    void scaleXChanged(int scale);
    void scaleYChanged(int scale);
    void jitterChanged(float jitter);
    void inverseChanged(bool inverse);
    void intensityChanged(float intensity);
    void bordersSizeChanged(float size);
    void seedChanged(int seed);
public slots:
    void updateScale(float scale);
    void setOutput();
    void previewGenerated();
    void updateVoronoiType(QString type);
    void updateVoronoiScale(int scale);
    void updateScaleX(int scale);
    void updateScaleY(int scale);
    void updateJitter(qreal jitter);
    void updateInverse(bool inverse);
    void updateIntensity(qreal intensity);
    void updateBordersSize(qreal size);
    void updateSeed(int seed);
private:
    VoronoiObject *preview;
    QString m_voronoiType = "crystals";
    VoronoiParams m_crystals;
    VoronoiParams m_borders;
    VoronoiParams m_solid;
    VoronoiParams m_worley;
};

#endif // VORONOINODE_H

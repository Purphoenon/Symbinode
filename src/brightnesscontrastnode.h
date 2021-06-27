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

#ifndef BRIGHTNESSCONTRASTNODE_H
#define BRIGHTNESSCONTRASTNODE_H

#include "node.h"
#include "brightnesscontrast.h"

class BrightnessContrastNode: public Node
{
    Q_OBJECT
public:
    BrightnessContrastNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), GLint bpc = GL_RGBA8, float brightness = 0.0f, float contrast = 0.0f);
    ~BrightnessContrastNode();
    void operation() override;
    unsigned int &getPreviewTexture() override;
    void saveTexture(QString fileName) override;
    BrightnessContrastNode *clone() override;
    void serialize(QJsonObject &json) const override;
    void deserialize(const QJsonObject &json, QHash<QUuid, Socket*> &hash) override;
    float brightness();
    void setBrightness(float value);
    float contrast();
    void setContrast(float value);
signals:
    void brightnessChanged(float value);
    void contrastChanged(float value);
public slots:
    void updateScale(float scale);
    void previewGenerated();
    void setOutput();
    void updateBrightness(qreal value);
    void updateContrast(qreal value);
private:
    BrightnessContrastObject *preview;
    float m_brightness = 0.0f;
    float m_contrast = 0.0f;
};

#endif // BRIGHTNESSCONTRASTNODE_H

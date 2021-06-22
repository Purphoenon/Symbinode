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

#ifndef MIXNODE_H
#define MIXNODE_H
#include "node.h"
#include "mix.h"

class MixNode: public Node
{
    Q_OBJECT
public:
    MixNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), GLint bpc = GL_RGBA8, float factor = 1.0f, int foregroundOpacity = 100, int backgroundOpacity = 100, int mode = 0, bool includingAlpha = true);
    ~MixNode();
    void operation();
    unsigned int &getPreviewTexture();
    void saveTexture(QString fileName);
    float factor();
    void setFactor(float f);
    int mode();
    void setMode(int mode);
    bool includingAlpha();
    void setIncludingAlpha(bool including);
    int foregroundOpacity();
    void setForegroundOpacity(int opacity);
    int backgroundOpacity();
    void setBackgroundOpacity(int opacity);
    void setOutput();
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json, QHash<QUuid, Socket*> &hash);
signals:
    void factorChanged(float f);
    void modeChanged(int mode);
    void includingAlphaChanged(bool including);
    void foregroundOpacityChanged(int opacity);
    void backgroundOpacityChanged(int opacity);
public slots:
    void previewGenerated();
    void updateFactor(qreal f);
    void updateMode(int mode);
    void updateIncludingAlpha(bool including);
    void updateForegroundOpacity(int opacity);
    void updateBackgroundOpacity(int opacity);
    void updateScale(float scale);
private:
    MixObject *preview = nullptr;
    float m_factor = 1.0f;
    int m_fOpacity = 100;
    int m_bOpacity = 100;
    int m_mode = 0;
    bool m_includingAlpha = true;
};

#endif // MIXNODE_H

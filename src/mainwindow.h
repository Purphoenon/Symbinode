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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QQuickWindow>
#include "tab.h"
#include "clipboard.h"
#include "noisenode.h"
#include "mixnode.h"
#include "albedonode.h"
#include "metalnode.h"
#include "roughnode.h"
#include "normalmapnode.h"
#include "normalnode.h"
#include "heightnode.h"
#include "emissionnode.h"
#include "voronoinode.h"
#include "polygonnode.h"
#include "circlenode.h"
#include "transformnode.h"
#include "tilenode.h"
#include "warpnode.h"
#include "blurnode.h"
#include "inversenode.h"
#include "colorrampnode.h"
#include "colornode.h"
#include "coloringnode.h"
#include "mappingnode.h"
#include "mirrornode.h"
#include "brightnesscontrastnode.h"
#include "thresholdnode.h"
#include "grayscalenode.h"
#include "gradientnode.h"
#include "directionalwarpnode.h"
#include "directionalblurnode.h"
#include "slopeblurnode.h"
#include "bevelnode.h"
#include "polartransformnode.h"
#include "bricksnode.h"
#include "hexagonsnode.h"
#include "frame.h"

class MainWindow: public QQuickWindow
{
    Q_OBJECT
    Q_PROPERTY(Node* activeNode READ activeNode)
    Q_PROPERTY(Node* pinnedNode READ pinnedNode)
public:
    Q_INVOKABLE void createNode(float x, float y, int nodeType);
    Q_INVOKABLE void createFrame(float x, float y);
    Q_INVOKABLE void newDocument();
    Q_INVOKABLE void copy();
    Q_INVOKABLE void paste();
    Q_INVOKABLE void cut();
    Q_INVOKABLE void deleteItems(bool saveConnection);
    Q_INVOKABLE bool saveScene();
    Q_INVOKABLE void saveSceneAs();
    Q_INVOKABLE void loadScene();
    Q_INVOKABLE void exportTextures();
    Q_INVOKABLE void saveCurrentTexture();
    Q_INVOKABLE void changeResolution(QVector2D res);
    Q_INVOKABLE void changePrimitive(int id);
    Q_INVOKABLE void changeTilePreview3D(int id);
    Q_INVOKABLE void changeHeightScale(qreal scale);
    Q_INVOKABLE void changeEmissiveStrenght(qreal strenght);
    Q_INVOKABLE void changeBloomRadius(qreal radius);
    Q_INVOKABLE void changeBloomIntensity(qreal intensity);
    Q_INVOKABLE void changeBloomThreshold(qreal threshold);
    Q_INVOKABLE void changeBloom(bool enable);
    Q_INVOKABLE void undo();
    Q_INVOKABLE void redo();
    Q_INVOKABLE void pin(bool pinned);
    Q_INVOKABLE void closeTab(Tab *tab);
    Q_INVOKABLE int tabsCount();
    Q_INVOKABLE Tab *tab(int index);
    MainWindow(QWindow *parent = nullptr);
    ~MainWindow();
    void keyPressEvent(QKeyEvent *event);
    void duplicate();
    void removeFromFrame();
    void addToFrame();
    void focusNode();
    void setActiveTab(Tab *tab);    
    Node *pinnedNode();
    Node *activeNode();
    void activeItemChanged();
    void loadFile(QString filename);
signals:
    void addTab(Tab *tab);
    void tabClosing(Tab *tab);
    void propertiesPanelChanged(QQuickItem *oldPanel, QQuickItem *newPanel);
    void preview3DChanged(QQuickItem *oldPreview, QQuickItem *newPreview);
    void previewUpdate(unsigned int previewData);
    void resolutionChanged(QVector2D res);
private:
    Tab *activeTab = nullptr;
    QQuickItem *m_activeItem = nullptr;
    Node *m_activeNode = nullptr;
    Node *m_pinnedNode = nullptr;
    QList<Tab*> tabs;
    Clipboard *m_clipboard = nullptr;
};

#endif // MAINWINDOW_H

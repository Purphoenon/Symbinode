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

#include "mainwindow.h"
#include <iostream>
#include <QtWidgets/QFileDialog>
#include <QApplication>

MainWindow::MainWindow(QWindow *parent):QQuickWindow (parent)
{
    setVisibility(QWindow::Maximized);
    m_clipboard = new Clipboard();
}

MainWindow::~MainWindow() {
    activeTab = nullptr;
    m_activeNode = nullptr;
    m_pinnedNode = nullptr;
    m_activeItem = nullptr;
    for(auto tab: tabs) {
        delete tab;
    }
    delete m_clipboard;
}

void MainWindow::createNode(float x, float y, int nodeType) {
    if(activeTab) {
        Node *n = nullptr;
        switch (nodeType) {
            case 0:
                n = new ColorRampNode(activeTab->scene(), activeTab->scene()->resolution());
                break;
            case 1:
                n = new ColorNode(activeTab->scene(), activeTab->scene()->resolution());
                break;
            case 2:
                n = new ColoringNode(activeTab->scene(), activeTab->scene()->resolution());
                break;
            case 3:
                n = new MappingNode(activeTab->scene(), activeTab->scene()->resolution());
                break;
            case 4:
                if(activeTab->scene()->heightConnected()) break;
                n = new HeightNode(activeTab->scene(), activeTab->scene()->resolution());
                break;
            case 5:
                n = new MirrorNode(activeTab->scene(), activeTab->scene()->resolution());
                break;
            case 6:
                n = new NoiseNode(activeTab->scene(), activeTab->scene()->resolution());
                break;
            case 7:
                n = new MixNode(activeTab->scene(), activeTab->scene()->resolution());
                break;
            case 8:
                if(activeTab->scene()->albedoConnected()) break;
                n = new AlbedoNode(activeTab->scene(), activeTab->scene()->resolution());
                break;
            case 9:
                if(activeTab->scene()->metalConnected()) break;
                n = new MetalNode(activeTab->scene(), activeTab->scene()->resolution());
                break;
            case 10:
                if(activeTab->scene()->roughConnected()) break;
                n = new RoughNode(activeTab->scene(), activeTab->scene()->resolution());
                break;
            case 11:
                n = new NormalMapNode(activeTab->scene(), activeTab->scene()->resolution());
                break;
            case 12:
                if(activeTab->scene()->normalConnected()) break;
                n = new NormalNode(activeTab->scene(), activeTab->scene()->resolution());
                break;
            case 13:
                n = new VoronoiNode(activeTab->scene(), activeTab->scene()->resolution());
                break;
            case 14:
                n = new PolygonNode(activeTab->scene(), activeTab->scene()->resolution());
                break;
            case 15:
                n = new CircleNode(activeTab->scene(), activeTab->scene()->resolution());
                break;
            case 16:
                n = new TransformNode(activeTab->scene(), activeTab->scene()->resolution());
                break;
            case 17:
                n = new TileNode(activeTab->scene(), activeTab->scene()->resolution());
                break;
            case 18:
                n = new WarpNode(activeTab->scene(), activeTab->scene()->resolution());
                break;
            case 19:
                n = new BlurNode(activeTab->scene(), activeTab->scene()->resolution());
                break;
            case 20:
                n = new InverseNode(activeTab->scene(), activeTab->scene()->resolution());
                break;
            case 21:
                n = new BrightnessContrastNode(activeTab->scene(), activeTab->scene()->resolution());
                break;
            case 22:
                n = new ThresholdNode(activeTab->scene(), activeTab->scene()->resolution());
                break;
            case 23:
                n = new EmissionNode(activeTab->scene(), activeTab->scene()->resolution());
                break;
            case 24:
                n = new GrayscaleNode(activeTab->scene(), activeTab->scene()->resolution());
                break;
            case 25:
                n = new GradientNode(activeTab->scene(), activeTab->scene()->resolution());
                break;
            case 26:
                n = new DirectionalWarpNode(activeTab->scene(), activeTab->scene()->resolution());
                break;
            case 27:
                n = new DirectionalBlurNode(activeTab->scene(), activeTab->scene()->resolution());
                break;
            case 28:
                n = new SlopeBlurNode(activeTab->scene(), activeTab->scene()->resolution());
                break;
            case 29:
                n = new BevelNode(activeTab->scene(), activeTab->scene()->resolution());
                break;
            case 30:
                n = new PolarTransformNode(activeTab->scene(), activeTab->scene()->resolution());
                break;
            case 31:
                n = new BricksNode(activeTab->scene(), activeTab->scene()->resolution());
                break;
            case 32:
                n = new HexagonsNode(activeTab->scene(), activeTab->scene()->resolution());
                break;
            default:
                break;
        }
        if(!n) return;
        activeTab->scene()->addNode(n);
        QVector2D pan = activeTab->scene()->background()->viewPan();
        float scale = activeTab->scene()->background()->viewScale();
        n->setBaseX((x + pan.x())/scale);
        n->setBaseY((y + pan.y())/scale);
        n->setPan(pan);
        activeTab->scene()->addedNode(n);
    }
}

void MainWindow::createFrame(float x, float y) {
    if(activeTab) {
        Frame *frame = new Frame(activeTab->scene());
        activeTab->scene()->addFrame(frame);
        QVector2D pan = activeTab->scene()->background()->viewPan();
        float scale = activeTab->scene()->background()->viewScale();
        frame->setBaseX((x + pan.x())/scale);
        frame->setBaseY((y + pan.y())/scale);
        activeTab->scene()->addedFrame(frame);
    }
}

void MainWindow::newDocument() {
    Tab *tab = new Tab(contentItem());
    tab->setParent(this);
    tab->setZ(1);
    connect(tab, &Tab::changeActiveTab, this, &MainWindow::setActiveTab);
    connect(tab, &Tab::closedTab, this, &MainWindow::tabClosing);
    connect(tab->scene(), &Scene::activeItemChanged, this, &MainWindow::activeItemChanged);
    setActiveTab(tab);
    tabs.append(tab);
    emit addTab(tab);
}

void MainWindow::copy() {
    if(activeTab) {
       m_clipboard->copy(activeTab->scene());
    }
}

void MainWindow::paste() {
    if(activeTab) {
        float sceneX = QCursor::pos().x() - x();
        float sceneY = QCursor::pos().y() - y();
        m_clipboard->paste(sceneX, sceneY, activeTab->scene());
    }
}

void MainWindow::cut() {
    if(activeTab) {
      m_clipboard->cut(activeTab->scene());
    }
}

void MainWindow::deleteItems() {
    if(activeTab) {
        activeTab->scene()->deleteItems();
    }
}

bool MainWindow::saveScene() {
    if(activeTab) {
        QString fileName = activeTab->scene()->fileName();
        return activeTab->scene()->saveScene(fileName);
    }
    return false;
}

void MainWindow::saveSceneAs() {
    if(activeTab) {
        QString fileName = QFileDialog::getSaveFileName(nullptr,
                tr("Save Node Scene"), "",
                tr("Node Scene (*.sne);"));
        if(fileName.isEmpty()) return;
        activeTab->scene()->saveScene(fileName);
    }
}

void MainWindow::loadScene() {
    QString fileName = QFileDialog::getOpenFileName(nullptr,
            tr("Open Node Scene"), "",
            tr("Node Scene (*.sne);"));
    if(fileName.isEmpty()) return;
    for(auto tab: tabs) {
        if(tab->scene()->fileName() == fileName) {
            setActiveTab(tab);
            tab->setSelected(true);
            return;
        }
    }
    newDocument();
    if(activeTab) {
        activeTab->scene()->loadScene(fileName);
        resolutionChanged(activeTab->scene()->resolution());
    }
}

void MainWindow::exportTextures() {
    if(activeTab) {
        QString folder = QFileDialog::getExistingDirectory(nullptr, tr("Open Directory"),
                                                           "/home",
                                                           QFileDialog::ShowDirsOnly
                                                           | QFileDialog::DontResolveSymlinks);
        if(folder.isEmpty()) return;

        activeTab->scene()->outputsSave(folder);
    }
}

void MainWindow::saveCurrentTexture() {
    if(m_pinnedNode || m_activeNode) {
        QString fileName = QFileDialog::getSaveFileName(nullptr,
                tr("Save Node Texture"), "",
                tr("Node Texture (*.png);"));
        if(fileName.isEmpty()) return;
        if(m_pinnedNode) m_pinnedNode->saveTexture(fileName);
        else if(m_activeNode) m_activeNode->saveTexture(fileName);
    }
}

void MainWindow::changeResolution(QVector2D res) {
    if(activeTab) {
        activeTab->scene()->setResolution(res);
    }
}

void MainWindow::changePrimitive(int id) {
    if(activeTab) {
        activeTab->scene()->preview3d()->setPrimitivesType(id);
    }
}

void MainWindow::changeTilePreview3D(int id) {
    if(activeTab) {
        activeTab->scene()->preview3d()->setTilesSize(id);
    }
}

void MainWindow::changeSelfShadow(bool enable) {
    if(activeTab) {
        activeTab->scene()->preview3d()->setSelfShadow(enable);
    }
}

void MainWindow::changeHeightScale(qreal scale) {
    if(activeTab) {
        activeTab->scene()->preview3d()->setHeightScale(0.1f*scale);
    }
}

void MainWindow::changeEmissiveStrenght(qreal strenght) {
    if(activeTab) {
        activeTab->scene()->preview3d()->setEmissiveStrenght(strenght);
    }
}

void MainWindow::changeBloomRadius(qreal radius) {
    if(activeTab) {
        activeTab->scene()->preview3d()->setBloomRadius(radius);
    }
}

void MainWindow::changeBloomIntensity(qreal intensity) {
    if(activeTab) {
        activeTab->scene()->preview3d()->setBloomIntensity(intensity);
    }
}

void MainWindow::changeBloomThreshold(qreal threshold) {
    if(activeTab) {
        activeTab->scene()->preview3d()->setBloomThreshold(threshold);
    }
}

void MainWindow::changeBloom(bool enable) {
    if(activeTab) {
        activeTab->scene()->preview3d()->setBloom(enable);
    }
}

void MainWindow::undo() {
    if(activeTab) {
        activeTab->scene()->undo();
    }
}

void MainWindow::redo() {
    if(activeTab) {
        activeTab->scene()->redo();
    }
}

void MainWindow::pin(bool pinned) {
    if(pinned) {
       m_pinnedNode = m_activeNode;
    }
    else {
        if(m_pinnedNode) {
            disconnect(m_pinnedNode, &Node::updatePreview, this, &MainWindow::previewUpdate);
        }
        m_pinnedNode = nullptr;
        if(m_activeNode) {
            connect(m_activeNode, &Node::updatePreview, this, &MainWindow::previewUpdate);
            previewUpdate(m_activeNode->getPreviewTexture());
        }
        else {
            previewUpdate(0);
        }
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_C && event->modifiers() == Qt::ControlModifier) {
        if(activeFocusItem() && activeFocusItem() != contentItem()) {
            QApplication::sendEvent(activeFocusItem(), event);
        }
        else copy();
    }
    else if(event->key() == Qt::Key_V && event->modifiers() == Qt::ControlModifier) {
        if(activeFocusItem() && activeFocusItem() != contentItem()) {
            QApplication::sendEvent(activeFocusItem(), event);
        }
        else paste();
    }
    else if(event->key() == Qt::Key_D && event->modifiers() == Qt::ControlModifier) {
        duplicate();
    }
    else if(event->key() == Qt::Key_X && event->modifiers() == Qt::ControlModifier) {
        cut();
    }
    else if(event->key() == Qt::Key_Delete) {
        if(activeFocusItem() && activeFocusItem() != contentItem()) {
            QApplication::sendEvent(activeFocusItem(), event);
        }
        else deleteItems();
    }
    else if(event->key() == Qt::Key_Z && event->modifiers() == Qt::ControlModifier) {        
        if(activeFocusItem() && activeFocusItem() != contentItem()) {
            activeFocusItem()->setFocus(false);
            contentItem()->setFocus(true);
        }
        undo();
    }
    else if(event->key() == Qt::Key_Y && event->modifiers() == Qt::ControlModifier) {
        if(activeFocusItem() && activeFocusItem() != contentItem()) {
            activeFocusItem()->setFocus(false);
            contentItem()->setFocus(true);
        }
        redo();
    }
    else if(event->key() == Qt::Key_F && event->modifiers() == Qt::ControlModifier) {
        addToFrame();
    }
    else if(event->key() == Qt::Key_F && event->modifiers() == Qt::AltModifier) {
        removeFromFrame();
    }    
    else {
        QApplication::sendEvent(activeFocusItem(), event);
    }
}
void MainWindow::duplicate() {
    if(activeTab) {
      m_clipboard->duplicate(activeTab->scene());
    }
}

void MainWindow::removeFromFrame() {
    if(activeTab) {
        activeTab->scene()->removeFromFrame();
    }
}

void MainWindow::addToFrame() {
    if(activeTab) {
        activeTab->scene()->addToFrame();
    }
}

void MainWindow::setActiveTab(Tab *tab) {
    QQuickItem *oldPreview = nullptr;
    if(activeTab) {
       activeTab->scene()->setVisible(false);
       activeTab->setSelected(false);
       disconnect(this, &MainWindow::widthChanged, activeTab->scene(), &Scene::setWidth);
       disconnect(this, &MainWindow::heightChanged, activeTab->scene(), &Scene::setHeight);
       disconnect(this, &MainWindow::widthChanged, tab->scene()->background(), &BackgroundObject::setWidth);
       disconnect(this, &MainWindow::heightChanged, tab->scene()->background(), &BackgroundObject::setHeight);
       oldPreview = activeTab->scene()->preview3d();
    }

    activeTab = tab;
    tab->scene()->setVisible(true);

    tab->scene()->setWidth(width());
    tab->scene()->setHeight(height());
    tab->scene()->background()->setWidth(width());
    tab->scene()->background()->setHeight(height());
    connect(this, &MainWindow::widthChanged, tab->scene(), &Scene::setWidth);
    connect(this, &MainWindow::heightChanged, tab->scene(), &Scene::setHeight);
    connect(this, &MainWindow::widthChanged, tab->scene()->background(), &BackgroundObject::setWidth);
    connect(this, &MainWindow::heightChanged, tab->scene()->background(), &BackgroundObject::setHeight);

    preview3DChanged(oldPreview, tab->scene()->preview3d());    
    activeItemChanged();
    resolutionChanged(tab->scene()->resolution());
}

void MainWindow::closeTab(Tab *tab) {
    int index = tabs.indexOf(activeTab);
    tabs.removeOne(tab);

    if(activeTab == tab && tabs.size() > 0) {
        index = std::min(index, tabs.size() - 1);
        setActiveTab(tabs[index]);
        tabs[index]->setSelected(true);
    }
    else if(tabs.size() == 0) {
       activeTab = nullptr;
       m_activeItem = nullptr;
       previewUpdate(0);
    }
    tab->deleteLater();
}

int MainWindow::tabsCount() {
    return tabs.count();
}

Tab *MainWindow::tab(int index) {
    if(index >= 0 && index < tabs.size()) {
        return tabs[index];
    }
    return nullptr;
}

Node *MainWindow::pinnedNode() {
    return m_pinnedNode;
}

Node *MainWindow::activeNode() {
    return m_activeNode;
}

void MainWindow::activeItemChanged() {
    QQuickItem *oldPanel = nullptr;
    if(qobject_cast<Node*>(m_activeItem)) {
        oldPanel = qobject_cast<Node*>(m_activeItem)->getPropertyPanel();
        if(!m_pinnedNode) disconnect(m_activeNode, &Node::updatePreview, this, &MainWindow::previewUpdate);
    }
    else if(qobject_cast<Frame*>(m_activeItem)) {
        oldPanel = qobject_cast<Frame*>(m_activeItem)->getPropertyPanel();
    }
    m_activeItem = activeTab->scene()->activeItem();
    m_activeNode = nullptr;
    QQuickItem *newPanel = nullptr;
    if(qobject_cast<Node*>(m_activeItem)) {
        Node* node = qobject_cast<Node*>(m_activeItem);
        m_activeNode = node;
        newPanel = node->getPropertyPanel();
        if(!m_pinnedNode) {
            connect(node, &Node::updatePreview, this, &MainWindow::previewUpdate);
            previewUpdate(node->getPreviewTexture());
        }
    }
    else if(qobject_cast<Frame*>(m_activeItem)) {        
        newPanel = qobject_cast<Frame*>(m_activeItem)->getPropertyPanel();
    }
    else if(!m_pinnedNode) {
        previewUpdate(0);
    }
    propertiesPanelChanged(oldPanel, newPanel);
}

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

#include "clipboard.h"
#include "scene.h"
#include "noisenode.h"
#include "mixnode.h"
#include "normalmapnode.h"
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
#include "albedonode.h"
#include "metalnode.h"
#include "roughnode.h"
#include "normalnode.h"
#include "heightnode.h"
#include "emissionnode.h"
#include "grayscalenode.h"
#include "gradientnode.h"
#include "directionalwarpnode.h"
#include "directionalblurnode.h"
#include "slopeblurnode.h"
#include "bevelnode.h"
#include "polartransformnode.h"
#include "bricksnode.h"
#include "hexagonsnode.h"
#include <iostream>

Clipboard::Clipboard()
{
}

Clipboard::~Clipboard() {
    for(auto node: clipboard_nodes) {
        delete node;
    }
    for(auto edge: clipboard_edges) {
        delete edge;
    }
    for(auto frame: clipboard_frames) {
        delete frame;
    }
}

void Clipboard::cut(Scene *scene) {
    copy(scene);
    scene->cut();
}

void Clipboard::copy(Scene *scene) {
    clear();
    QList<QQuickItem*> selected = scene->selectedList();
    QList<Frame*> sel_frames;
    QList<Node*> sel_nodes;
    QList<Edge*> sel_edges;
    float maxX = std::numeric_limits<float>::min();
    float maxY = maxX;
    float minX = std::numeric_limits<float>::max();
    float minY = minX;
    for(auto item: selected) {
        if(!(qobject_cast<AlbedoNode*>(item) || qobject_cast<MetalNode*>(item) || qobject_cast<RoughNode*>(item) || qobject_cast<NormalNode*>(item) || qobject_cast<HeightNode*>(item) || qobject_cast<EmissionNode*>(item))) {
            if(qobject_cast<Node*>(item)) {
                Node *node = qobject_cast<Node*>(item);
                if(!node->attachedFrame() || !node->attachedFrame()->selected()) sel_nodes.append(node);
                maxX = std::max(maxX, (float)(node->x() + node->width()));
                maxY = std::max(maxY, (float)(node->y() + node->height()));
                minX = std::min(minX, (float)node->x());
                minY = std::min(minY, (float)node->y());
                QList<Edge*> edges = node->getEdges();
                for(auto edge: edges) {
                    if(sel_edges.contains(edge)) continue;
                    Node *startNode = qobject_cast<Node*>(edge->startSocket()->parentItem());
                    Node *endNode = qobject_cast<Node*>(edge->endSocket()->parentItem());
                    if((startNode && startNode->selected()) && (!(qobject_cast<AlbedoNode*>(endNode) || qobject_cast<MetalNode*>(endNode) || qobject_cast<RoughNode*>(endNode) || qobject_cast<NormalNode*>(endNode) || qobject_cast<HeightNode*>(endNode) || qobject_cast<EmissionNode*>(endNode)) && endNode->selected())) {
                        sel_edges.append(edge);
                    }
                }
            }
            else if(qobject_cast<Frame*>(item)) {
                Frame *frame = qobject_cast<Frame*>(item);
                sel_frames.append(frame);
                maxX = std::max(maxX, (float)(frame->x() + frame->width()));
                maxY = std::max(maxY, (float)(frame->y() + frame->height()));
                minX = std::min(minX, (float)frame->x());
                minY = std::min(minY, (float)frame->y());
            }
        }
    }

    center.setX(((maxX - minX)*0.5 + minX + scene->background()->viewPan().x())/scene->background()->viewScale());
    center.setY(((maxY - minY)*0.5 + minY + scene->background()->viewPan().y())/scene->background()->viewScale());

    for(auto frame: sel_frames) {
        Frame *f = new Frame();
        f->setBaseX(frame->baseX());
        f->setBaseY(frame->baseY());
        f->setBaseWidth(frame->baseWidth());
        f->setBaseHeight(frame->baseHeight());
        f->setTitle(frame->title());
        f->setColor(frame->color());
        clipboard_frames.append(f);
        QList<QQuickItem*> copiedContent;
        for(auto item: frame->contentList()) {
            Node *baseNode = qobject_cast<Node*>(item);
            if(baseNode && !(qobject_cast<AlbedoNode*>(item) || qobject_cast<MetalNode*>(item) || qobject_cast<RoughNode*>(item) || qobject_cast<NormalNode*>(item) || qobject_cast<HeightNode*>(item) || qobject_cast<EmissionNode*>(item)) && baseNode->selected()) {
                Node *copiedContentNode = nodeCopy(baseNode, scene, nullptr);
                copiedContentNode->setBaseX(baseNode->baseX());
                copiedContentNode->setBaseY(baseNode->baseY());
                copiedContent.append(copiedContentNode);
            }
        }
        if(copiedContent.size() > 0) f->addNodes(copiedContent);
    }
    for(auto node: sel_nodes) {
        Node *copiedNode = nodeCopy(node, scene, nullptr);
        copiedNode->setBaseX(node->baseX());
        copiedNode->setBaseY(node->baseY());
        clipboard_nodes.append(copiedNode);
    }
    for(auto edge: sel_edges) {
        Edge *e = new Edge();
        e->setStartPosition((edge->startPosition() + scene->background()->viewPan())/scene->background()->viewScale());
        e->setEndPosition((edge->endPosition() + scene->background()->viewPan())/scene->background()->viewScale());
        clipboard_edges.append(e);
    }
}

void Clipboard::paste(float posX, float posY, Scene *scene) {
    QList<QQuickItem*> pastedItem;
    if(clipboard_nodes.empty() && clipboard_frames.empty()) return;
    std::cout << posX << " " << posY << std::endl;
    float viewScale = scene->background()->viewScale();
    QVector2D viewPan = scene->background()->viewPan();
    QVector2D currentCenter = center*viewScale - viewPan;

    scene->clearSelected();
    for(auto f: clipboard_frames) {
        Frame *frame = new Frame(scene);
        float x = posX - (currentCenter.x() - (f->baseX()*viewScale - viewPan.x()));
        float y = posY - (currentCenter.y() - (f->baseY()*viewScale - viewPan.y()));
        frame->setPan(viewPan);
        frame->setScaleView(viewScale);
        frame->setBaseX((viewPan.x() + x)/viewScale);
        frame->setBaseY((viewPan.y() + y)/viewScale);
        frame->setBaseWidth(f->baseWidth());
        frame->setBaseHeight(f->baseHeight());
        frame->setTitle(f->title());
        frame->setColor(f->color());
        frame->setSelected(true);
        scene->addFrame(frame);
        scene->addSelected(frame);
        pastedItem.append(frame);
        QList<QQuickItem*> pastedContent;
        for(auto item: f->contentList()) {
            if(qobject_cast<Node*>(item)) {
                Node *baseNode = qobject_cast<Node*>(item);
                Node *pastedNode = nodeCopy(baseNode, scene, scene);
                float x = posX - (currentCenter.x() - (baseNode->baseX()*viewScale - viewPan.x()));
                float y = posY - (currentCenter.y() - (baseNode->baseY()*viewScale - viewPan.y()));
                pastedNode->setBaseX((viewPan.x() + x)/viewScale);
                pastedNode->setBaseY((viewPan.y() + y)/viewScale);
                pastedNode->setSelected(true);
                scene->addNode(pastedNode);
                scene->addSelected(pastedNode);
                pastedContent.append(pastedNode);
                pastedItem.append(pastedNode);
            }
        }
        if(pastedContent.size() > 0) frame->addNodes(pastedContent);
    }
    for(auto n: clipboard_nodes) {
        Node *pastedNode = nodeCopy(n, scene, scene);
        float x = posX - (currentCenter.x() - (n->baseX()*viewScale - viewPan.x()));
        float y = posY - (currentCenter.y() - (n->baseY()*viewScale - viewPan.y()));
        pastedNode->setBaseX((viewPan.x() + x)/viewScale);
        pastedNode->setBaseY((viewPan.y() + y)/viewScale);
        pastedNode->setSelected(true);
        scene->addNode(pastedNode);
        scene->addSelected(pastedNode);
        pastedItem.append(pastedNode);
    }
    for(auto e: clipboard_edges) {
        Edge *edge = new Edge(scene);
        float startX = e->startPosition().x()*viewScale - viewPan.x();
        float startY = e->startPosition().y()*viewScale - viewPan.y();
        float endX = e->endPosition().x()*viewScale - viewPan.x();
        float endY = e->endPosition().y()*viewScale - viewPan.y();
        edge->setStartPosition(QVector2D(posX - (currentCenter.x() - startX), posY - (currentCenter.y() - startY)));
        edge->setEndPosition(QVector2D(posX - (currentCenter.x() - endX), posY - (currentCenter.y() - endY)));
        Socket *startSock = edge->findSockets(scene, edge->startPosition().x(), edge->startPosition().y());
        if(startSock) {
            edge->setStartSocket(startSock);
            startSock->addEdge(edge);
        }

        Socket *endSock = edge->findSockets(scene, edge->endPosition().x(), edge->endPosition().y());
        if(endSock) {
            edge->setEndSocket(endSock);
            endSock->addEdge(edge);
        }
        edge->updateScale(scene->background()->viewScale());
        scene->addEdge(edge);
        pastedItem.append(edge);
    }
    scene->pastedItems(pastedItem);
}

void Clipboard::duplicate(Scene *scene) {
    QList<QQuickItem*> selected = scene->selectedList();
    float viewScale = scene->background()->viewScale();
    QList<Frame*> sel_frames;
    QList<Node*> sel_nodes;
    QList<Edge*> sel_edges;
    QList<QQuickItem*> pastedItem;
    for(auto item: selected) {
        if(!(qobject_cast<AlbedoNode*>(item) || qobject_cast<MetalNode*>(item) || qobject_cast<RoughNode*>(item) || qobject_cast<NormalNode*>(item) || qobject_cast<HeightNode*>(item) || qobject_cast<EmissionNode*>(item))) {
            if(qobject_cast<Node*>(item)) {
                Node *node = qobject_cast<Node*>(item);
                if(!node->attachedFrame() || !node->attachedFrame()->selected()) sel_nodes.append(node);
                QList<Edge*> edges = node->getEdges();
                for(auto edge: edges) {
                    if(sel_edges.contains(edge)) continue;
                    Node *startNode = qobject_cast<Node*>(edge->startSocket()->parentItem());
                    Node *endNode = qobject_cast<Node*>(edge->endSocket()->parentItem());
                    if((startNode && startNode->selected()) && (!(qobject_cast<AlbedoNode*>(endNode) || qobject_cast<MetalNode*>(endNode) || qobject_cast<RoughNode*>(endNode) || qobject_cast<NormalNode*>(endNode) || qobject_cast<HeightNode*>(endNode) || qobject_cast<EmissionNode*>(endNode)) && endNode->selected())) {
                        sel_edges.append(edge);
                    }
                }
            }
            else if(qobject_cast<Frame*>(item)) {
                Frame *frame = qobject_cast<Frame*>(item);
                sel_frames.append(frame);
            }
        }
    }

    for(auto frame: sel_frames) {
        Frame *f = new Frame(scene);
        f->setBaseX(frame->baseX() + 50);
        f->setBaseY(frame->baseY() + 50);
        f->setBaseWidth(frame->baseWidth());
        f->setBaseHeight(frame->baseHeight());
        f->setTitle(frame->title());
        f->setColor(frame->color());
        scene->addFrame(f);
        scene->addSelected(f);
        pastedItem.append(f);
        QList<QQuickItem*> copiedContent;
        for(auto item: frame->contentList()) {
            Node *baseNode = qobject_cast<Node*>(item);
            if(baseNode && !(qobject_cast<AlbedoNode*>(item) || qobject_cast<MetalNode*>(item) || qobject_cast<RoughNode*>(item) || qobject_cast<NormalNode*>(item) || qobject_cast<HeightNode*>(item) || qobject_cast<EmissionNode*>(item)) && baseNode->selected()) {
                Node *copiedContentNode = nodeCopy(baseNode, scene, scene);
                copiedContentNode->setBaseX(baseNode->baseX() + 50);
                copiedContentNode->setBaseY(baseNode->baseY() + 50);
                copiedContent.append(copiedContentNode);
                scene->addNode(copiedContentNode);
                scene->addSelected(copiedContentNode);
                pastedItem.append(copiedContentNode);
            }
        }
        if(copiedContent.size() > 0) f->addNodes(copiedContent);
    }
    for(auto node: sel_nodes) {
        Node *duplicatedNode = nodeCopy(node, scene, scene);
        duplicatedNode->setBaseX(node->baseX() + 50);
        duplicatedNode->setBaseY(node->baseY() + 50);
        scene->addNode(duplicatedNode);
        scene->addSelected(duplicatedNode);
        pastedItem.append(duplicatedNode);
    }
    for(auto edge: sel_edges) {
        Edge *e = new Edge(scene);
        e->setStartPosition(edge->startPosition() + QVector2D(50, 50)*viewScale);
        e->setEndPosition(edge->endPosition() + QVector2D(50, 50)*viewScale);
        Socket *startSock = e->findSockets(scene, e->startPosition().x(), e->startPosition().y());
        if(startSock) {
            e->setStartSocket(startSock);
            startSock->addEdge(e);
        }

        Socket *endSock = e->findSockets(scene, e->endPosition().x(), e->endPosition().y());
        if(endSock) {
            e->setEndSocket(endSock);
            endSock->addEdge(e);
        }
        e->updateScale(scene->background()->viewScale());
        scene->addEdge(e);
        pastedItem.append(e);
    }
    scene->pastedItems(pastedItem);
}

void Clipboard::clear() {
    for(auto frame: clipboard_frames) {
        if(frame) delete frame;
    }
    clipboard_frames.clear();
    for(auto node: clipboard_nodes){
        if(node) delete node;
    }
    clipboard_nodes.clear();
    for(auto edge: clipboard_edges) {
        if(edge) delete edge;
    }
    clipboard_edges.clear();
}

Node *Clipboard::nodeCopy(Node *node, Scene *scene, QQuickItem *parent) {
    if(qobject_cast<NoiseNode*>(node)) {
        NoiseNode *baseNode = qobject_cast<NoiseNode*>(node);
        NoiseNode *noiseNode = new NoiseNode(parent, scene->resolution(), baseNode->bpc(),
                                             baseNode->perlinParams(), baseNode->simpleParams(),
                                             baseNode->noiseType());
        return noiseNode;
    }
    else if(qobject_cast<MixNode*>(node)) {
        MixNode *baseNode = qobject_cast<MixNode*>(node);
        MixNode *mixNode = new MixNode(parent, scene->resolution(), baseNode->bpc(), baseNode->factor(),
                                       baseNode->foregroundOpacity(), baseNode->backgroundOpacity(),
                                       baseNode->mode(), baseNode->includingAlpha());
        return mixNode;
    }
    else if(qobject_cast<NormalMapNode*>(node)) {
        NormalMapNode *baseNode = qobject_cast<NormalMapNode*>(node);
        NormalMapNode *normalMapNode = new NormalMapNode(parent, scene->resolution(), baseNode->bpc(),
                                                         baseNode->strenght());
        return normalMapNode;
    }
    else if(qobject_cast<VoronoiNode*>(node)) {
        VoronoiNode *baseNode = qobject_cast<VoronoiNode*>(node);
        VoronoiNode *voronoiNode = new VoronoiNode(parent, scene->resolution(), baseNode->bpc(),
                                                   baseNode->crystalsParam(), baseNode->bordersParam(),
                                                   baseNode->solidParam(), baseNode->worleyParam(),
                                                   baseNode->voronoiType());
        return voronoiNode;
    }
    else if(qobject_cast<PolygonNode*>(node)) {
        PolygonNode *baseNode = qobject_cast<PolygonNode*>(node);
        PolygonNode *polygonNode = new PolygonNode(parent, scene->resolution(), baseNode->bpc(),
                                                   baseNode->sides(), baseNode->polygonScale(),
                                                   baseNode->smooth(), baseNode->useAlpha());
        return polygonNode;
    }
    else if(qobject_cast<CircleNode*>(node)) {
        CircleNode *baseNode = qobject_cast<CircleNode*>(node);
        CircleNode *circleNode = new CircleNode(parent, scene->resolution(), baseNode->bpc(),
                                                baseNode->interpolation(), baseNode->radius(),
                                                baseNode->smooth(), baseNode->useAlpha());
        return circleNode;
    }
    else if(qobject_cast<TransformNode*>(node)) {
        TransformNode *baseNode = qobject_cast<TransformNode*>(node);
        TransformNode *transformNode = new TransformNode(parent, scene->resolution(), baseNode->bpc(),
                                                         baseNode->translationX(), baseNode->translationY(),
                                                         baseNode->scaleX(), baseNode->scaleY(),
                                                         baseNode->rotation(), baseNode->clampCoords());
        return transformNode;
    }
    else if(qobject_cast<TileNode*>(node)) {
        TileNode *baseNode = qobject_cast<TileNode*>(node);
        TileNode *tileNode = new TileNode(parent, scene->resolution(), baseNode->bpc(), baseNode->offsetX(),
                                          baseNode->offsetY(), baseNode->columns(), baseNode->rows(),
                                          baseNode->tileScale(), baseNode->scaleX(), baseNode->scaleY(),
                                          baseNode->rotationAngle(), baseNode->randPosition(),
                                          baseNode->randRotation(), baseNode->randScale(),
                                          baseNode->maskStrength(), baseNode->inputsCount(),
                                          baseNode->seed(), baseNode->keepProportion(),
                                          baseNode->useAlpha(), baseNode->depthMask());
        return tileNode;
    }
    else if(qobject_cast<WarpNode*>(node)) {
        WarpNode *baseNode = qobject_cast<WarpNode*>(node);
        WarpNode *warpNode = new WarpNode(parent, scene->resolution(), baseNode->bpc(), baseNode->intensity());
        return warpNode;
    }
    else if(qobject_cast<BlurNode*>(node)) {
        BlurNode *baseNode = qobject_cast<BlurNode*>(node);
        BlurNode *blurNode = new BlurNode(parent, scene->resolution(), baseNode->bpc(), baseNode->intensity());
        return blurNode;
    }
    else if(qobject_cast<InverseNode*>(node)) {
        InverseNode *inverseNode = new InverseNode(parent, scene->resolution(), node->bpc());
        return inverseNode;
    }
    else if(qobject_cast<ColorRampNode*>(node)) {
        ColorRampNode *baseNode = qobject_cast<ColorRampNode*>(node);
        ColorRampNode *colorRampNode = new ColorRampNode(parent, scene->resolution(), baseNode->bpc(),
                                                         baseNode->stops());
        return colorRampNode;
    }
    else if(qobject_cast<ColorNode*>(node)) {
        ColorNode *baseNode = qobject_cast<ColorNode*>(node);
        ColorNode *colorNode = new ColorNode(parent, scene->resolution(), baseNode->color());
        return colorNode;
    }
    else if(qobject_cast<ColoringNode*>(node)) {
        ColoringNode *baseNode = qobject_cast<ColoringNode*>(node);
        ColoringNode *coloringNode = new ColoringNode(parent, scene->resolution(), baseNode->bpc(),
                                                      baseNode->color());
        return coloringNode;
    }
    else if(qobject_cast<MappingNode*>(node)) {
        MappingNode *baseNode = qobject_cast<MappingNode*>(node);
        MappingNode *mappingNode = new MappingNode(parent, scene->resolution(), baseNode->bpc(),
                                                    baseNode->inputMin(), baseNode->inputMax(),
                                                    baseNode->outputMin(), baseNode->outputMax());
        return mappingNode;
    }
    else if(qobject_cast<MirrorNode*>(node)) {
        MirrorNode *baseNode = qobject_cast<MirrorNode*>(node);
        MirrorNode *mirrorNode = new MirrorNode(parent, scene->resolution(), baseNode->bpc(),
                                                    baseNode->direction());
        return mirrorNode;
    }
    else if(qobject_cast<BrightnessContrastNode*>(node)) {
        BrightnessContrastNode *baseNode = qobject_cast<BrightnessContrastNode*>(node);
        BrightnessContrastNode *brightnessContrastNode = new BrightnessContrastNode(parent, scene->resolution(),
                                                    baseNode->bpc(), baseNode->brightness(),
                                                    baseNode->contrast());
        return brightnessContrastNode;
    }
    else if(qobject_cast<ThresholdNode*>(node)) {
        ThresholdNode *baseNode = qobject_cast<ThresholdNode*>(node);
        ThresholdNode *thresholdNode = new ThresholdNode(parent, scene->resolution(), baseNode->bpc(),
                                                    baseNode->threshold());
        return thresholdNode;
    }
    else if (qobject_cast<GrayscaleNode*>(node)) {
        GrayscaleNode *grayscaleNode = new GrayscaleNode(parent, scene->resolution(), node->bpc());
        return  grayscaleNode;
    }
    else if (qobject_cast<GradientNode*>(node)) {
        GradientNode *baseNode = qobject_cast<GradientNode*>(node);
        GradientNode *gradientNode = new GradientNode(parent, scene->resolution(), baseNode->bpc(),
                                                      baseNode->linearParam(), baseNode->reflectedParam(),
                                                      baseNode->angularParam(), baseNode->radialParam(),
                                                      baseNode->gradientType());
        return gradientNode;
    }
    else if(qobject_cast<DirectionalWarpNode*>(node)) {
        DirectionalWarpNode *baseNode = qobject_cast<DirectionalWarpNode*>(node);
        DirectionalWarpNode *directionalWarpNode = new DirectionalWarpNode(parent, scene->resolution(),
                                                                           baseNode->bpc(),
                                                                           baseNode->intensity(),
                                                                           baseNode->angle());
        return directionalWarpNode;
    }
    else if (qobject_cast<DirectionalBlurNode*>(node)) {
        DirectionalBlurNode *baseNode = qobject_cast<DirectionalBlurNode*>(node);
        DirectionalBlurNode *directionalBlurNode = new DirectionalBlurNode(parent, scene->resolution(),
                                                                           baseNode->bpc(),
                                                                           baseNode->intensity(),
                                                                           baseNode->angle());
        return directionalBlurNode;
    }
    else if(qobject_cast<SlopeBlurNode*>(node)) {
        SlopeBlurNode *baseNode = qobject_cast<SlopeBlurNode*>(node);
        SlopeBlurNode *slopeBlurNode = new SlopeBlurNode(parent, scene->resolution(), baseNode->bpc(),
                                                         baseNode->mode(), baseNode->intensity(),
                                                         baseNode->samples());
        return slopeBlurNode;
    }
    else if (qobject_cast<BevelNode*>(node)) {
        BevelNode *baseNode = qobject_cast<BevelNode*>(node);
        BevelNode *bevelNode = new BevelNode(parent, scene->resolution(), baseNode->bpc(),
                                             baseNode->distance(), baseNode->smooth(), baseNode->useAlpha());
        return bevelNode;
    }
    else if(qobject_cast<PolarTransformNode*>(node)) {
        PolarTransformNode *baseNode = qobject_cast<PolarTransformNode*>(node);
        PolarTransformNode *polarNode = new PolarTransformNode(parent, scene->resolution(), baseNode->bpc(),
                                                               baseNode->radius(), baseNode->clamp(),
                                                               baseNode->angle());
        return polarNode;
    }
    else if (qobject_cast<BricksNode*>(node)) {
        BricksNode *baseNode = qobject_cast<BricksNode*>(node);
        BricksNode *bricksNode = new BricksNode(parent, scene->resolution(), baseNode->bpc(),
                                                baseNode->columns(), baseNode->rows(), baseNode->offset(),
                                                baseNode->bricksWidth(), baseNode->bricksHeight(),
                                                baseNode->smoothX(), baseNode->smoothY(), baseNode->mask(),
                                                baseNode->seed());
        return bricksNode;
    }
    else if(qobject_cast<HexagonsNode*>(node)) {
        HexagonsNode *baseNode = qobject_cast<HexagonsNode*>(node);
        HexagonsNode *hexagonsNode = new HexagonsNode(parent, scene->resolution(), baseNode->bpc(),
                                                      baseNode->columns(), baseNode->rows(),
                                                      baseNode->hexSize(), baseNode->hexSmooth(),
                                                      baseNode->mask(), baseNode->seed());
        return hexagonsNode;
    }
    return nullptr;
}

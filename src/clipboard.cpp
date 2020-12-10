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
}

void Clipboard::cut(Scene *scene) {
    copy(scene);
    scene->cut();
}

void Clipboard::copy(Scene *scene) {
    clear();
    QList<QQuickItem*> selected = scene->selectedList();
    QList<Node*> sel_nodes;
    QList<Edge*> sel_edges;
    float maxX = std::numeric_limits<float>::min();
    float maxY = maxX;
    float minX = std::numeric_limits<float>::max();
    float minY = minX;
    for(auto item: selected) {
        if(!(qobject_cast<AlbedoNode*>(item) || qobject_cast<MetalNode*>(item) || qobject_cast<RoughNode*>(item) || qobject_cast<NormalNode*>(item))) {
            Node *node = qobject_cast<Node*>(item);
            sel_nodes.append(node);
            maxX = std::max(maxX, (float)(node->x() + node->width()));
            maxY = std::max(maxY, (float)(node->y() + node->height()));
            minX = std::min(minX, (float)node->x());
            minY = std::min(minY, (float)node->y());
            QList<Edge*> edges = node->getEdges();
            for(auto edge: edges) {
                if(sel_edges.contains(edge)) continue;
                Node *startNode = qobject_cast<Node*>(edge->startSocket()->parentItem());
                Node *endNode = qobject_cast<Node*>(edge->endSocket()->parentItem());
                if((startNode && startNode->selected()) && (!(qobject_cast<AlbedoNode*>(endNode) || qobject_cast<MetalNode*>(endNode) || qobject_cast<RoughNode*>(endNode) || qobject_cast<NormalNode*>(endNode)) && endNode->selected())) {
                    sel_edges.append(edge);
                }
            }
        }
    }

    center.setX(((maxX - minX)*0.5 + minX + scene->background()->viewPan().x())/scene->background()->viewScale());
    center.setY(((maxY - minY)*0.5 + minY + scene->background()->viewPan().y())/scene->background()->viewScale());

    for(auto node: sel_nodes) {
        if(qobject_cast<NoiseNode*>(node)) {
            NoiseNode *baseNode = qobject_cast<NoiseNode*>(node);
            NoiseNode *noiseNode = new NoiseNode(nullptr, scene->resolution(), baseNode->perlinParams(),
                                                 baseNode->simpleParams(), baseNode->noiseType());
            noiseNode->setBaseX(baseNode->baseX());
            noiseNode->setBaseY(baseNode->baseY());
            clipboard_nodes.append(noiseNode);
        }
        else if(qobject_cast<MixNode*>(node)) {
            MixNode *baseNode = qobject_cast<MixNode*>(node);
            MixNode *mixNode = new MixNode(nullptr, scene->resolution(), baseNode->factor(), baseNode->mode());
            mixNode->setBaseX(baseNode->baseX());
            mixNode->setBaseY(baseNode->baseY());
            clipboard_nodes.append(mixNode);
        }
        else if(qobject_cast<NormalMapNode*>(node)) {
            NormalMapNode *baseNode = qobject_cast<NormalMapNode*>(node);
            NormalMapNode *normalMapNode = new NormalMapNode(nullptr, scene->resolution(), baseNode->strenght());
            normalMapNode->setBaseX(baseNode->baseX());
            normalMapNode->setBaseY(baseNode->baseY());
            clipboard_nodes.append(normalMapNode);
        }
        else if(qobject_cast<VoronoiNode*>(node)) {
            VoronoiNode *baseNode = qobject_cast<VoronoiNode*>(node);
            VoronoiNode *voronoiNode = new VoronoiNode(nullptr, scene->resolution(), baseNode->crystalsParam(),
                                                       baseNode->bordersParam(), baseNode->solidParam(),
                                                       baseNode->worleyParam(), baseNode->voronoiType());
            voronoiNode->setBaseX(baseNode->baseX());
            voronoiNode->setBaseY(baseNode->baseY());
            clipboard_nodes.append(voronoiNode);
        }
        else if(qobject_cast<PolygonNode*>(node)) {
            PolygonNode *baseNode = qobject_cast<PolygonNode*>(node);
            PolygonNode *polygonNode = new PolygonNode(nullptr, scene->resolution(), baseNode->sides(),
                                                       baseNode->polygonScale(), baseNode->smooth());
            polygonNode->setBaseX(baseNode->baseX());
            polygonNode->setBaseY(baseNode->baseY());
            clipboard_nodes.append(polygonNode);
        }
        else if(qobject_cast<CircleNode*>(node)) {
            CircleNode *baseNode = qobject_cast<CircleNode*>(node);
            CircleNode *circleNode = new CircleNode(nullptr, scene->resolution(), baseNode->interpolation(),
                                                       baseNode->radius(), baseNode->smooth());
            circleNode->setBaseX(baseNode->baseX());
            circleNode->setBaseY(baseNode->baseY());
            clipboard_nodes.append(circleNode);
        }
        else if(qobject_cast<TransformNode*>(node)) {
            TransformNode *baseNode = qobject_cast<TransformNode*>(node);
            TransformNode *transformNode = new TransformNode(nullptr, scene->resolution(),
                                                             baseNode->translationX(), baseNode->translationY(),
                                                             baseNode->scaleX(), baseNode->scaleY(),
                                                             baseNode->rotation(), baseNode->clampCoords());
            transformNode->setBaseX(baseNode->baseX());
            transformNode->setBaseY(baseNode->baseY());
            clipboard_nodes.append(transformNode);
        }
        else if(qobject_cast<TileNode*>(node)) {
            TileNode *baseNode = qobject_cast<TileNode*>(node);
            TileNode *tileNode = new TileNode(nullptr, scene->resolution(), baseNode->offsetX(),
                                              baseNode->offsetY(), baseNode->columns(), baseNode->rows(),
                                              baseNode->scaleX(), baseNode->scaleY(),
                                              baseNode->rotationAngle(), baseNode->randPosition(),
                                              baseNode->randRotation(), baseNode->randScale(),
                                              baseNode->maskStrength(), baseNode->inputsCount(),
                                              baseNode->keepProportion());
            tileNode->setBaseX(baseNode->baseX());
            tileNode->setBaseY(baseNode->baseY());
            clipboard_nodes.append(tileNode);
        }
        else if(qobject_cast<WarpNode*>(node)) {
            WarpNode *baseNode = qobject_cast<WarpNode*>(node);
            WarpNode *warpNode = new WarpNode(nullptr, scene->resolution(), baseNode->intensity());
            warpNode->setBaseX(baseNode->baseX());
            warpNode->setBaseY(baseNode->baseY());
            clipboard_nodes.append(warpNode);
        }
        else if(qobject_cast<BlurNode*>(node)) {
            BlurNode *baseNode = qobject_cast<BlurNode*>(node);
            BlurNode *blurNode = new BlurNode(nullptr, scene->resolution(), baseNode->intensity());
            blurNode->setBaseX(baseNode->baseX());
            blurNode->setBaseY(baseNode->baseY());
            clipboard_nodes.append(blurNode);
        }
        else if(qobject_cast<InverseNode*>(node)) {
            InverseNode *baseNode = qobject_cast<InverseNode*>(node);
            InverseNode *inverseNode = new InverseNode(nullptr, scene->resolution());
            inverseNode->setBaseX(baseNode->baseX());
            inverseNode->setBaseY(baseNode->baseY());
            clipboard_nodes.append(inverseNode);
        }
        else if(qobject_cast<ColorRampNode*>(node)) {
            ColorRampNode *baseNode = qobject_cast<ColorRampNode*>(node);
            ColorRampNode *colorRampNode = new ColorRampNode(nullptr, scene->resolution(), baseNode->stops());
            colorRampNode->setBaseX(baseNode->baseX());
            colorRampNode->setBaseY(baseNode->baseY());
            clipboard_nodes.append(colorRampNode);
        }
        else if(qobject_cast<ColorNode*>(node)) {
            ColorNode *baseNode = qobject_cast<ColorNode*>(node);
            ColorNode *colorNode = new ColorNode(nullptr, scene->resolution(), baseNode->color());
            colorNode->setBaseX(baseNode->baseX());
            colorNode->setBaseY(baseNode->baseY());
            clipboard_nodes.append(colorNode);
        }
        else if(qobject_cast<ColoringNode*>(node)) {
            ColoringNode *baseNode = qobject_cast<ColoringNode*>(node);
            ColoringNode *coloringNode = new ColoringNode(nullptr, scene->resolution(), baseNode->color());
            coloringNode->setBaseX(baseNode->baseX());
            coloringNode->setBaseY(baseNode->baseY());
            clipboard_nodes.append(coloringNode);
        }
        else if(qobject_cast<MappingNode*>(node)) {
            MappingNode *baseNode = qobject_cast<MappingNode*>(node);
            MappingNode *mappingNode = new MappingNode(nullptr, scene->resolution(),
                                                        baseNode->inputMin(), baseNode->inputMax(),
                                                        baseNode->outputMin(), baseNode->outputMax());
            mappingNode->setBaseX(baseNode->baseX());
            mappingNode->setBaseY(baseNode->baseY());
            clipboard_nodes.append(mappingNode);
        }
        else if(qobject_cast<MirrorNode*>(node)) {
            MirrorNode *baseNode = qobject_cast<MirrorNode*>(node);
            MirrorNode *mirrorNode = new MirrorNode(nullptr, scene->resolution(),
                                                        baseNode->direction());
            mirrorNode->setBaseX(baseNode->baseX());
            mirrorNode->setBaseY(baseNode->baseY());
            clipboard_nodes.append(mirrorNode);
        }
        else if(qobject_cast<BrightnessContrastNode*>(node)) {
            BrightnessContrastNode *baseNode = qobject_cast<BrightnessContrastNode*>(node);
            BrightnessContrastNode *brightnessContrastNode = new BrightnessContrastNode(nullptr, scene->resolution(),
                                                        baseNode->brightness(), baseNode->contrast());
            brightnessContrastNode->setBaseX(baseNode->baseX());
            brightnessContrastNode->setBaseY(baseNode->baseY());
            clipboard_nodes.append(brightnessContrastNode);
        }
        else if(qobject_cast<ThresholdNode*>(node)) {
            ThresholdNode *baseNode = qobject_cast<ThresholdNode*>(node);
            ThresholdNode *thresholdNode = new ThresholdNode(nullptr, scene->resolution(),
                                                        baseNode->threshold());
            thresholdNode->setBaseX(baseNode->baseX());
            thresholdNode->setBaseY(baseNode->baseY());
            clipboard_nodes.append(thresholdNode);
        }
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
    if(clipboard_nodes.empty()) return;
    std::cout << posX << " " << posY << std::endl;
    float viewScale = scene->background()->viewScale();
    QVector2D viewPan = scene->background()->viewPan();
    QVector2D currentCenter = center*viewScale - viewPan;

    scene->clearSelected();
    for(auto n: clipboard_nodes) {
        if(qobject_cast<NoiseNode*>(n)) {
            NoiseNode *baseNode = qobject_cast<NoiseNode*>(n);
            NoiseNode *noiseNode = new NoiseNode(scene, scene->resolution(), baseNode->perlinParams(),
                                                 baseNode->simpleParams(), baseNode->noiseType());
            float x = posX - (currentCenter.x() - (baseNode->baseX()*viewScale - viewPan.x()));
            float y = posY - (currentCenter.y() - (baseNode->baseY()*viewScale - viewPan.y()));
            noiseNode->setPan(viewPan);
            noiseNode->updateScale(viewScale);
            noiseNode->setBaseX((noiseNode->pan().x() + x)/noiseNode->scaleView());
            noiseNode->setBaseY((noiseNode->pan().y() + y)/noiseNode->scaleView());
            noiseNode->setSelected(true);
            scene->addNode(noiseNode);
            scene->addSelected(noiseNode);
            pastedItem.append(noiseNode);
        }
        else if(qobject_cast<MixNode*>(n)) {
            MixNode *baseNode = qobject_cast<MixNode*>(n);
            MixNode *mixNode = new MixNode(scene, scene->resolution(), baseNode->factor(), baseNode->mode());
            float x = posX - (currentCenter.x() - (baseNode->baseX()*viewScale - viewPan.x()));
            float y = posY - (currentCenter.y() - (baseNode->baseY()*viewScale - viewPan.y()));
            mixNode->setPan(viewPan);
            mixNode->updateScale(viewScale);
            mixNode->setBaseX((mixNode->pan().x() + x)/mixNode->scaleView());
            mixNode->setBaseY((mixNode->pan().y() + y)/mixNode->scaleView());
            mixNode->setSelected(true);
            scene->addNode(mixNode);
            scene->addSelected(mixNode);
            pastedItem.append(mixNode);
        }
        else if(qobject_cast<NormalMapNode*>(n)) {
            NormalMapNode *baseNode = qobject_cast<NormalMapNode*>(n);
            NormalMapNode *normalMapNode = new NormalMapNode(scene, scene->resolution(), baseNode->strenght());
            float x = posX - (currentCenter.x() - (baseNode->baseX()*viewScale - viewPan.x()));
            float y = posY - (currentCenter.y() - (baseNode->baseY()*viewScale - viewPan.y()));
            normalMapNode->setPan(viewPan);
            normalMapNode->updateScale(viewScale);
            normalMapNode->setBaseX((normalMapNode->pan().x() + x)/normalMapNode->scaleView());
            normalMapNode->setBaseY((normalMapNode->pan().y() + y)/normalMapNode->scaleView());
            normalMapNode->setSelected(true);
            scene->addNode(normalMapNode);
            scene->addSelected(normalMapNode);
            pastedItem.append(normalMapNode);
        }
        else if(qobject_cast<VoronoiNode*>(n)) {
            VoronoiNode *baseNode = qobject_cast<VoronoiNode*>(n);
            VoronoiNode *voronoiNode = new VoronoiNode(scene, scene->resolution(), baseNode->crystalsParam(),
                                                       baseNode->bordersParam(), baseNode->solidParam(),
                                                       baseNode->worleyParam(), baseNode->voronoiType());
            float x = posX - (currentCenter.x() - (baseNode->baseX()*viewScale - viewPan.x()));
            float y = posY - (currentCenter.y() - (baseNode->baseY()*viewScale - viewPan.y()));
            voronoiNode->setPan(viewPan);
            voronoiNode->updateScale(viewScale);
            voronoiNode->setBaseX((voronoiNode->pan().x() + x)/voronoiNode->scaleView());
            voronoiNode->setBaseY((voronoiNode->pan().y() + y)/voronoiNode->scaleView());
            voronoiNode->setSelected(true);
            scene->addNode(voronoiNode);
            scene->addSelected(voronoiNode);
            pastedItem.append(voronoiNode);
        }
        else if(qobject_cast<PolygonNode*>(n)) {
            PolygonNode *baseNode = qobject_cast<PolygonNode*>(n);
            PolygonNode *polygonNode = new PolygonNode(scene, scene->resolution(), baseNode->sides(),
                                                       baseNode->polygonScale(), baseNode->smooth());
            float x = posX - (currentCenter.x() - (baseNode->baseX()*viewScale - viewPan.x()));
            float y = posY - (currentCenter.y() - (baseNode->baseY()*viewScale - viewPan.y()));
            polygonNode->setPan(viewPan);
            polygonNode->updateScale(viewScale);
            polygonNode->setBaseX((polygonNode->pan().x() + x)/polygonNode->scaleView());
            polygonNode->setBaseY((polygonNode->pan().y() + y)/polygonNode->scaleView());
            polygonNode->setSelected(true);
            scene->addNode(polygonNode);
            scene->addSelected(polygonNode);
            pastedItem.append(polygonNode);
        }
        else if(qobject_cast<CircleNode*>(n)) {
            CircleNode *baseNode = qobject_cast<CircleNode*>(n);
            CircleNode *circleNode = new CircleNode(scene, scene->resolution(), baseNode->interpolation(),
                                                       baseNode->radius(), baseNode->smooth());
            float x = posX - (currentCenter.x() - (baseNode->baseX()*viewScale - viewPan.x()));
            float y = posY - (currentCenter.y() - (baseNode->baseY()*viewScale - viewPan.y()));
            circleNode->setPan(viewPan);
            circleNode->updateScale(viewScale);
            circleNode->setBaseX((circleNode->pan().x() + x)/circleNode->scaleView());
            circleNode->setBaseY((circleNode->pan().y() + y)/circleNode->scaleView());
            circleNode->setSelected(true);
            scene->addNode(circleNode);
            scene->addSelected(circleNode);
            pastedItem.append(circleNode);
        }
        else if(qobject_cast<TransformNode*>(n)) {
            TransformNode *baseNode = qobject_cast<TransformNode*>(n);
            TransformNode *transformNode = new TransformNode(scene, scene->resolution(),
                                                             baseNode->translationX(), baseNode->translationY(),
                                                             baseNode->scaleX(), baseNode->scaleY(),
                                                             baseNode->rotation(), baseNode->clampCoords());
            float x = posX - (currentCenter.x() - (baseNode->baseX()*viewScale - viewPan.x()));
            float y = posY - (currentCenter.y() - (baseNode->baseY()*viewScale - viewPan.y()));
            transformNode->setPan(viewPan);
            transformNode->updateScale(viewScale);
            transformNode->setBaseX((transformNode->pan().x() + x)/transformNode->scaleView());
            transformNode->setBaseY((transformNode->pan().y() + y)/transformNode->scaleView());
            transformNode->setSelected(true);
            scene->addNode(transformNode);
            scene->addSelected(transformNode);
            pastedItem.append(transformNode);
        }
        else if(qobject_cast<TileNode*>(n)) {
            TileNode *baseNode = qobject_cast<TileNode*>(n);
            TileNode *tileNode = new TileNode(scene, scene->resolution(), baseNode->offsetX(),
                                              baseNode->offsetY(), baseNode->columns(), baseNode->rows(),
                                              baseNode->scaleX(), baseNode->scaleY(),
                                              baseNode->rotationAngle(), baseNode->randPosition(),
                                              baseNode->randRotation(), baseNode->randScale(),
                                              baseNode->maskStrength(), baseNode->inputsCount(),
                                              baseNode->keepProportion());
            float x = posX - (currentCenter.x() - (baseNode->baseX()*viewScale - viewPan.x()));
            float y = posY - (currentCenter.y() - (baseNode->baseY()*viewScale - viewPan.y()));
            tileNode->setPan(viewPan);
            tileNode->updateScale(viewScale);
            tileNode->setBaseX((tileNode->pan().x() + x)/tileNode->scaleView());
            tileNode->setBaseY((tileNode->pan().y() + y)/tileNode->scaleView());
            tileNode->setSelected(true);
            scene->addNode(tileNode);
            scene->addSelected(tileNode);
            pastedItem.append(tileNode);
        }
        else if(qobject_cast<WarpNode*>(n)) {
            WarpNode *baseNode = qobject_cast<WarpNode*>(n);
            WarpNode *warpNode = new WarpNode(scene, scene->resolution(), baseNode->intensity());
            float x = posX - (currentCenter.x() - (baseNode->baseX()*viewScale - viewPan.x()));
            float y = posY - (currentCenter.y() - (baseNode->baseY()*viewScale - viewPan.y()));
            warpNode->setPan(viewPan);
            warpNode->updateScale(viewScale);
            warpNode->setBaseX((warpNode->pan().x() + x)/warpNode->scaleView());
            warpNode->setBaseY((warpNode->pan().y() + y)/warpNode->scaleView());
            warpNode->setSelected(true);
            scene->addNode(warpNode);
            scene->addSelected(warpNode);
            pastedItem.append(warpNode);
        }
        else if(qobject_cast<BlurNode*>(n)) {
            BlurNode *baseNode = qobject_cast<BlurNode*>(n);
            BlurNode *blurNode = new BlurNode(scene, scene->resolution(), baseNode->intensity());
            float x = posX - (currentCenter.x() - (baseNode->baseX()*viewScale - viewPan.x()));
            float y = posY - (currentCenter.y() - (baseNode->baseY()*viewScale - viewPan.y()));
            blurNode->setPan(viewPan);
            blurNode->updateScale(viewScale);
            blurNode->setBaseX((blurNode->pan().x() + x)/blurNode->scaleView());
            blurNode->setBaseY((blurNode->pan().y() + y)/blurNode->scaleView());
            blurNode->setSelected(true);
            scene->addNode(blurNode);
            scene->addSelected(blurNode);
            pastedItem.append(blurNode);
        }
        else if(qobject_cast<InverseNode*>(n)) {
            InverseNode *baseNode = qobject_cast<InverseNode*>(n);
            InverseNode *inverseNode = new InverseNode(scene, scene->resolution());
            float x = posX - (currentCenter.x() - (baseNode->baseX()*viewScale - viewPan.x()));
            float y = posY - (currentCenter.y() - (baseNode->baseY()*viewScale - viewPan.y()));
            inverseNode->setPan(viewPan);
            inverseNode->updateScale(viewScale);
            inverseNode->setBaseX((inverseNode->pan().x() + x)/inverseNode->scaleView());
            inverseNode->setBaseY((inverseNode->pan().y() + y)/inverseNode->scaleView());
            inverseNode->setSelected(true);
            scene->addNode(inverseNode);
            scene->addSelected(inverseNode);
            pastedItem.append(inverseNode);
        }
        else if(qobject_cast<ColorRampNode*>(n)) {
            ColorRampNode *baseNode = qobject_cast<ColorRampNode*>(n);
            ColorRampNode *colorRampNode = new ColorRampNode(scene, scene->resolution(), baseNode->stops());
            float x = posX - (currentCenter.x() - (baseNode->baseX()*viewScale - viewPan.x()));
            float y = posY - (currentCenter.y() - (baseNode->baseY()*viewScale - viewPan.y()));
            colorRampNode->setPan(viewPan);
            colorRampNode->updateScale(viewScale);
            colorRampNode->setBaseX((colorRampNode->pan().x() + x)/colorRampNode->scaleView());
            colorRampNode->setBaseY((colorRampNode->pan().y() + y)/colorRampNode->scaleView());
            colorRampNode->setSelected(true);
            scene->addNode(colorRampNode);
            scene->addSelected(colorRampNode);
            pastedItem.append(colorRampNode);
        }
        else if(qobject_cast<ColorNode*>(n)) {
            ColorNode *baseNode = qobject_cast<ColorNode*>(n);
            ColorNode *colorNode = new ColorNode(scene, scene->resolution(), baseNode->color());
            float x = posX - (currentCenter.x() - (baseNode->baseX()*viewScale - viewPan.x()));
            float y = posY - (currentCenter.y() - (baseNode->baseY()*viewScale - viewPan.y()));
            colorNode->setPan(viewPan);
            colorNode->updateScale(viewScale);
            colorNode->setBaseX((colorNode->pan().x() + x)/colorNode->scaleView());
            colorNode->setBaseY((colorNode->pan().y() + y)/colorNode->scaleView());
            colorNode->setSelected(true);
            scene->addNode(colorNode);
            scene->addSelected(colorNode);
            pastedItem.append(colorNode);
        }
        else if(qobject_cast<ColoringNode*>(n)) {
            ColoringNode *baseNode = qobject_cast<ColoringNode*>(n);
            ColoringNode *coloringNode = new ColoringNode(scene, scene->resolution(), baseNode->color());
            float x = posX - (currentCenter.x() - (baseNode->baseX()*viewScale - viewPan.x()));
            float y = posY - (currentCenter.y() - (baseNode->baseY()*viewScale - viewPan.y()));
            coloringNode->setPan(viewPan);
            coloringNode->updateScale(viewScale);
            coloringNode->setBaseX((coloringNode->pan().x() + x)/coloringNode->scaleView());
            coloringNode->setBaseY((coloringNode->pan().y() + y)/coloringNode->scaleView());
            coloringNode->setSelected(true);
            scene->addNode(coloringNode);
            scene->addSelected(coloringNode);
            pastedItem.append(coloringNode);
        }
        else if(qobject_cast<MappingNode*>(n)) {
            MappingNode *baseNode = qobject_cast<MappingNode*>(n);
            MappingNode *mappingNode = new MappingNode(scene, scene->resolution(),
                                                        baseNode->inputMin(), baseNode->inputMax(),
                                                        baseNode->outputMin(), baseNode->outputMax());
            float x = posX - (currentCenter.x() - (baseNode->baseX()*viewScale - viewPan.x()));
            float y = posY - (currentCenter.y() - (baseNode->baseY()*viewScale - viewPan.y()));
            mappingNode->setPan(viewPan);
            mappingNode->updateScale(viewScale);
            mappingNode->setBaseX((mappingNode->pan().x() + x)/mappingNode->scaleView());
            mappingNode->setBaseY((mappingNode->pan().y() + y)/mappingNode->scaleView());
            mappingNode->setSelected(true);
            scene->addNode(mappingNode);
            scene->addSelected(mappingNode);
            pastedItem.append(mappingNode);
        }
        else if(qobject_cast<MirrorNode*>(n)) {
            MirrorNode *baseNode = qobject_cast<MirrorNode*>(n);
            MirrorNode *mirrorNode = new MirrorNode(scene, scene->resolution(),
                                                        baseNode->direction());
            float x = posX - (currentCenter.x() - (baseNode->baseX()*viewScale - viewPan.x()));
            float y = posY - (currentCenter.y() - (baseNode->baseY()*viewScale - viewPan.y()));
            mirrorNode->setPan(viewPan);
            mirrorNode->updateScale(viewScale);
            mirrorNode->setBaseX((mirrorNode->pan().x() + x)/mirrorNode->scaleView());
            mirrorNode->setBaseY((mirrorNode->pan().y() + y)/mirrorNode->scaleView());
            mirrorNode->setSelected(true);
            scene->addNode(mirrorNode);
            scene->addSelected(mirrorNode);
            pastedItem.append(mirrorNode);
        }
        else if(qobject_cast<BrightnessContrastNode*>(n)) {
            BrightnessContrastNode *baseNode = qobject_cast<BrightnessContrastNode*>(n);
            BrightnessContrastNode *brightnessContrastNode = new BrightnessContrastNode(scene, scene->resolution(),
                                                        baseNode->brightness(), baseNode->contrast());
            float x = posX - (currentCenter.x() - (baseNode->baseX()*viewScale - viewPan.x()));
            float y = posY - (currentCenter.y() - (baseNode->baseY()*viewScale - viewPan.y()));
            brightnessContrastNode->setPan(viewPan);
            brightnessContrastNode->updateScale(viewScale);
            brightnessContrastNode->setBaseX((brightnessContrastNode->pan().x() + x)/brightnessContrastNode->scaleView());
            brightnessContrastNode->setBaseY((brightnessContrastNode->pan().y() + y)/brightnessContrastNode->scaleView());
            brightnessContrastNode->setSelected(true);
            scene->addNode(brightnessContrastNode);
            scene->addSelected(brightnessContrastNode);
            pastedItem.append(brightnessContrastNode);
        }
        else if(qobject_cast<ThresholdNode*>(n)) {
            ThresholdNode *baseNode = qobject_cast<ThresholdNode*>(n);
            ThresholdNode *thresholdNode = new ThresholdNode(scene, scene->resolution(),
                                                        baseNode->threshold());
            float x = posX - (currentCenter.x() - (baseNode->baseX()*viewScale - viewPan.x()));
            float y = posY - (currentCenter.y() - (baseNode->baseY()*viewScale - viewPan.y()));
            thresholdNode->setPan(viewPan);
            thresholdNode->updateScale(viewScale);
            thresholdNode->setBaseX((thresholdNode->pan().x() + x)/thresholdNode->scaleView());
            thresholdNode->setBaseY((thresholdNode->pan().y() + y)/thresholdNode->scaleView());
            thresholdNode->setSelected(true);
            scene->addNode(thresholdNode);
            scene->addSelected(thresholdNode);
            pastedItem.append(thresholdNode);
        }
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
    QList<Node*> sel_nodes;
    QList<Edge*> sel_edges;
    QList<QQuickItem*> pastedItem;
    float maxX = std::numeric_limits<float>::min();
    float maxY = maxX;
    float minX = std::numeric_limits<float>::max();
    float minY = minX;
    for(auto item: selected) {
        if(!(qobject_cast<AlbedoNode*>(item) || qobject_cast<MetalNode*>(item) || qobject_cast<RoughNode*>(item) || qobject_cast<NormalNode*>(item))) {
            Node *node = qobject_cast<Node*>(item);
            sel_nodes.append(node);
            maxX = std::max(maxX, (float)(node->x() + node->width()));
            maxY = std::max(maxY, (float)(node->y() + node->height()));
            minX = std::min(minX, (float)node->x());
            minY = std::min(minY, (float)node->y());
            QList<Edge*> edges = node->getEdges();
            for(auto edge: edges) {
                if(sel_edges.contains(edge)) continue;
                Node *startNode = qobject_cast<Node*>(edge->startSocket()->parentItem());
                Node *endNode = qobject_cast<Node*>(edge->endSocket()->parentItem());
                if((startNode && startNode->selected()) && (!(qobject_cast<AlbedoNode*>(endNode) || qobject_cast<MetalNode*>(endNode) || qobject_cast<RoughNode*>(endNode) || qobject_cast<NormalNode*>(endNode)) && endNode->selected())) {
                    sel_edges.append(edge);
                }
            }
        }
    }

    for(auto node: sel_nodes) {
        if(qobject_cast<NoiseNode*>(node)) {
            NoiseNode *baseNode = qobject_cast<NoiseNode*>(node);
            NoiseNode *noiseNode = new NoiseNode(scene, scene->resolution(), baseNode->perlinParams(),
                                                 baseNode->simpleParams(), baseNode->noiseType());
            noiseNode->setBaseX(baseNode->baseX());
            noiseNode->setBaseY(baseNode->baseY());
            scene->addNode(noiseNode);
            scene->addSelected(noiseNode);
            pastedItem.append(noiseNode);
        }
        else if(qobject_cast<MixNode*>(node)) {
            MixNode *baseNode = qobject_cast<MixNode*>(node);
            MixNode *mixNode = new MixNode(scene, scene->resolution(), baseNode->factor(), baseNode->mode());
            mixNode->setBaseX(baseNode->baseX());
            mixNode->setBaseY(baseNode->baseY());
            scene->addNode(mixNode);
            scene->addSelected(mixNode);
            pastedItem.append(mixNode);
        }
        else if(qobject_cast<NormalMapNode*>(node)) {
            NormalMapNode *baseNode = qobject_cast<NormalMapNode*>(node);
            NormalMapNode *normalMapNode = new NormalMapNode(scene, scene->resolution(), baseNode->strenght());
            normalMapNode->setBaseX(baseNode->baseX());
            normalMapNode->setBaseY(baseNode->baseY());
            scene->addNode(normalMapNode);
            scene->addSelected(normalMapNode);
            pastedItem.append(normalMapNode);
        }
        else if(qobject_cast<VoronoiNode*>(node)) {
            VoronoiNode *baseNode = qobject_cast<VoronoiNode*>(node);
            VoronoiNode *voronoiNode = new VoronoiNode(scene, scene->resolution(), baseNode->crystalsParam(),
                                                       baseNode->bordersParam(), baseNode->solidParam(),
                                                       baseNode->worleyParam(), baseNode->voronoiType());
            voronoiNode->setBaseX(baseNode->baseX());
            voronoiNode->setBaseY(baseNode->baseY());
            scene->addNode(voronoiNode);
            scene->addSelected(voronoiNode);
            pastedItem.append(voronoiNode);
        }
        else if(qobject_cast<PolygonNode*>(node)) {
            PolygonNode *baseNode = qobject_cast<PolygonNode*>(node);
            PolygonNode *polygonNode = new PolygonNode(scene, scene->resolution(), baseNode->sides(),
                                                       baseNode->polygonScale(), baseNode->smooth());
            polygonNode->setBaseX(baseNode->baseX());
            polygonNode->setBaseY(baseNode->baseY());
            scene->addNode(polygonNode);
            scene->addSelected(polygonNode);
            pastedItem.append(polygonNode);
        }
        else if(qobject_cast<CircleNode*>(node)) {
            CircleNode *baseNode = qobject_cast<CircleNode*>(node);
            CircleNode *circleNode = new CircleNode(scene, scene->resolution(), baseNode->interpolation(),
                                                       baseNode->radius(), baseNode->smooth());
            circleNode->setBaseX(baseNode->baseX());
            circleNode->setBaseY(baseNode->baseY());
            scene->addNode(circleNode);
            scene->addSelected(circleNode);
            pastedItem.append(circleNode);
        }
        else if(qobject_cast<TransformNode*>(node)) {
            TransformNode *baseNode = qobject_cast<TransformNode*>(node);
            TransformNode *transformNode = new TransformNode(scene, scene->resolution(),
                                                             baseNode->translationX(), baseNode->translationY(),
                                                             baseNode->scaleX(), baseNode->scaleY(),
                                                             baseNode->rotation(), baseNode->clampCoords());
            transformNode->setBaseX(baseNode->baseX());
            transformNode->setBaseY(baseNode->baseY());
            scene->addNode(transformNode);
            scene->addSelected(transformNode);
            pastedItem.append(transformNode);
        }
        else if(qobject_cast<TileNode*>(node)) {
            TileNode *baseNode = qobject_cast<TileNode*>(node);
            TileNode *tileNode = new TileNode(scene, scene->resolution(), baseNode->offsetX(),
                                              baseNode->offsetY(), baseNode->columns(), baseNode->rows(),
                                              baseNode->scaleX(), baseNode->scaleY(),
                                              baseNode->rotationAngle(), baseNode->randPosition(),
                                              baseNode->randRotation(), baseNode->randScale(),
                                              baseNode->maskStrength(), baseNode->inputsCount(),
                                              baseNode->keepProportion());
            tileNode->setBaseX(baseNode->baseX());
            tileNode->setBaseY(baseNode->baseY());
            scene->addNode(tileNode);
            scene->addSelected(tileNode);
            pastedItem.append(tileNode);
        }
        else if(qobject_cast<WarpNode*>(node)) {
            WarpNode *baseNode = qobject_cast<WarpNode*>(node);
            WarpNode *warpNode = new WarpNode(scene, scene->resolution(), baseNode->intensity());
            warpNode->setBaseX(baseNode->baseX());
            warpNode->setBaseY(baseNode->baseY());
            scene->addNode(warpNode);
            scene->addSelected(warpNode);
            pastedItem.append(warpNode);
        }
        else if(qobject_cast<BlurNode*>(node)) {
            BlurNode *baseNode = qobject_cast<BlurNode*>(node);
            BlurNode *blurNode = new BlurNode(scene, scene->resolution(), baseNode->intensity());
            blurNode->setBaseX(baseNode->baseX());
            blurNode->setBaseY(baseNode->baseY());
            scene->addNode(blurNode);
            scene->addSelected(blurNode);
            pastedItem.append(blurNode);
        }
        else if(qobject_cast<InverseNode*>(node)) {
            InverseNode *baseNode = qobject_cast<InverseNode*>(node);
            InverseNode *inverseNode = new InverseNode(scene, scene->resolution());
            inverseNode->setBaseX(baseNode->baseX());
            inverseNode->setBaseY(baseNode->baseY());
            scene->addNode(inverseNode);
            scene->addSelected(inverseNode);
            pastedItem.append(inverseNode);
        }
        else if(qobject_cast<ColorRampNode*>(node)) {
            ColorRampNode *baseNode = qobject_cast<ColorRampNode*>(node);
            ColorRampNode *colorRampNode = new ColorRampNode(scene, scene->resolution(), baseNode->stops());
            colorRampNode->setBaseX(baseNode->baseX());
            colorRampNode->setBaseY(baseNode->baseY());
            scene->addNode(colorRampNode);
            scene->addSelected(colorRampNode);
            pastedItem.append(colorRampNode);
        }
        else if(qobject_cast<ColorNode*>(node)) {
            ColorNode *baseNode = qobject_cast<ColorNode*>(node);
            ColorNode *colorNode = new ColorNode(scene, scene->resolution(), baseNode->color());
            colorNode->setBaseX(baseNode->baseX());
            colorNode->setBaseY(baseNode->baseY());
            scene->addNode(colorNode);
            scene->addSelected(colorNode);
            pastedItem.append(colorNode);
        }
        else if(qobject_cast<ColoringNode*>(node)) {
            ColoringNode *baseNode = qobject_cast<ColoringNode*>(node);
            ColoringNode *coloringNode = new ColoringNode(scene, scene->resolution(), baseNode->color());
            coloringNode->setBaseX(baseNode->baseX());
            coloringNode->setBaseY(baseNode->baseY());
            scene->addNode(coloringNode);
            scene->addSelected(coloringNode);
            pastedItem.append(coloringNode);
        }
        else if(qobject_cast<MappingNode*>(node)) {
            MappingNode *baseNode = qobject_cast<MappingNode*>(node);
            MappingNode *mappingNode = new MappingNode(scene, scene->resolution(),
                                                        baseNode->inputMin(), baseNode->inputMax(),
                                                        baseNode->outputMin(), baseNode->outputMax());
            mappingNode->setBaseX(baseNode->baseX());
            mappingNode->setBaseY(baseNode->baseY());
            scene->addNode(mappingNode);
            scene->addSelected(mappingNode);
            pastedItem.append(mappingNode);
        }
        else if(qobject_cast<MirrorNode*>(node)) {
            MirrorNode *baseNode = qobject_cast<MirrorNode*>(node);
            MirrorNode *mirrorNode = new MirrorNode(scene, scene->resolution(),
                                                        baseNode->direction());
            mirrorNode->setBaseX(baseNode->baseX());
            mirrorNode->setBaseY(baseNode->baseY());
            scene->addNode(mirrorNode);
            scene->addSelected(mirrorNode);
            pastedItem.append(mirrorNode);
        }
        else if(qobject_cast<BrightnessContrastNode*>(node)) {
            BrightnessContrastNode *baseNode = qobject_cast<BrightnessContrastNode*>(node);
            BrightnessContrastNode *brightnessContrastNode = new BrightnessContrastNode(scene, scene->resolution(),
                                                        baseNode->brightness(), baseNode->contrast());
            brightnessContrastNode->setBaseX(baseNode->baseX());
            brightnessContrastNode->setBaseY(baseNode->baseY());
            scene->addNode(brightnessContrastNode);
            scene->addSelected(brightnessContrastNode);
            pastedItem.append(brightnessContrastNode);
        }
        else if(qobject_cast<ThresholdNode*>(node)) {
            ThresholdNode *baseNode = qobject_cast<ThresholdNode*>(node);
            ThresholdNode *thresholdNode = new ThresholdNode(scene, scene->resolution(),
                                                        baseNode->threshold());
            thresholdNode->setBaseX(baseNode->baseX());
            thresholdNode->setBaseY(baseNode->baseY());
            scene->addNode(thresholdNode);
            scene->addSelected(thresholdNode);
            pastedItem.append(thresholdNode);
        }
    }
    for(auto edge: sel_edges) {
        Edge *e = new Edge(scene);
        e->setStartPosition(edge->startPosition());
        e->setEndPosition(edge->endPosition());
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
    for(auto node: clipboard_nodes){
        if(node) delete node;
    }
    clipboard_nodes.clear();
    for(auto edge: clipboard_edges) {
        if(edge) delete edge;
    }
    clipboard_edges.clear();
}

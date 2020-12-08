QT += quick
QT += gui
QT += widgets
CONFIG += c++11

TARGET = symbinode

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

RC_ICONS = icons/symbinode.ico

INCLUDEPATH += libs/FreeImage
LIBS += -L$$_PRO_FILE_PWD_/libs/FreeImage -lFreeImage

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
    backgroundobject.cpp \
    backgroundrenderer.cpp \
    scene.cpp \
    node.cpp \
    edge.cpp \
    socket.cpp \
    commands.cpp \
    clipboard.cpp \
    mainwindow.cpp \
    tab.cpp \
    preview.cpp \
    noisenode.cpp \
    noise.cpp \
    mixnode.cpp \
    mix.cpp \
    preview3d.cpp \
    albedonode.cpp \
    albedo.cpp \
    metalnode.cpp \
    onechanel.cpp \
    roughnode.cpp \
    normalmapnode.cpp \
    normalmap.cpp \
    normalnode.cpp \
    normal.cpp \
    voronoinode.cpp \
    voronoi.cpp \
    polygonnode.cpp \
    polygon.cpp \
    circlenode.cpp \
    circle.cpp \
    transformnode.cpp \
    transform.cpp \
    tilenode.cpp \
    tile.cpp \
    warpnode.cpp \
    warp.cpp \
    blurnode.cpp \
    blur.cpp \
    inversenode.cpp \
    inverse.cpp \
    colorrampnode.cpp \
    colorramp.cpp \
    colornode.cpp \
    color.cpp \
    coloring.cpp \
    coloringnode.cpp \
    mappingnode.cpp \
    mapping.cpp \
    splatnode.cpp \
    splat.cpp \
    mirrornode.cpp \
    mirror.cpp \
    brightnesscontrastnode.cpp \
    brightnesscontrast.cpp \
    thresholdnode.cpp \
    threshold.cpp \
    cubicbezier.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    backgroundobject.h \
    backgroundrenderer.h \
    scene.h \
    node.h \
    edge.h \
    socket.h \
    commands.h \
    clipboard.h \
    mainwindow.h \
    tab.h \
    preview.h \
    noisenode.h \
    noise.h \
    mixnode.h \
    mix.h \
    preview3d.h \
    albedonode.h \
    albedo.h \
    metalnode.h \
    onechanel.h \
    roughnode.h \
    normalmapnode.h \
    normalmap.h \
    normalnode.h \
    normal.h \
    voronoinode.h \
    voronoi.h \
    polygonnode.h \
    polygon.h \
    circlenode.h \
    circle.h \
    transformnode.h \
    transform.h \
    tilenode.h \
    tile.h \
    warpnode.h \
    warp.h \
    blurnode.h \
    blur.h \
    inversenode.h \
    inverse.h \
    colorrampnode.h \
    colorramp.h \
    colornode.h \
    color.h \
    coloring.h \
    coloringnode.h \
    mappingnode.h \
    mapping.h \
    splatnode.h \
    splat.h \
    mirrornode.h \
    mirror.h \
    brightnesscontrastnode.h \
    brightnesscontrast.h \
    thresholdnode.h \
    threshold.h \
    cubicbezier.h

DISTFILES += \
    shaders/noise.vert \
    shaders/noise.frag \
    shaders/texture.vert \
    shaders/texture.frag \
    shaders/texmatrix.vert \
    shaders/mix.frag \
    shaders/pbr.vert \
    shaders/pbr.frag \
    shaders/cubemap.vert \
    shaders/equirectangular.frag \
    shaders/irradiance.frag \
    shaders/prefiltered.frag \
    shaders/brdf.vert \
    shaders/brdf.frag \
    shaders/background.vert \
    shaders/background.frag \
    shaders/albedo.frag \
    shaders/onechanel.frag \
    shaders/normalmap.frag \
    shaders/voronoi.frag \
    shaders/polygon.frag \
    shaders/polygon.frag \
    shaders/circle.frag \
    shaders/transform.frag \
    shaders/tile.frag \
    shaders/warp.frag \
    shaders/blur.frag \
    shaders/inverse.frag \
    shaders/colorramp.frag \
    shaders/color.frag \
    shaders/coloring.frag \
    shaders/mapping.frag \
    shaders/bombing.frag \
    shaders/random.frag \
    shaders/mirror.frag \
    shaders/brightnesscontrast.frag \
    shaders/threshold.frag

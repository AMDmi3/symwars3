cmake_minimum_required (VERSION 3.0)
project (corpstatesc) 

set (CORP_VERSION_MAJOR 0)
set (CORP_VERSION_MINOR 0)
set (CORP_VERSION_PATCH 1)

ADD_DEFINITIONS("-std=c++0x"
    -lGL
    -lGLU
    -lGLEW)

ADD_EXECUTABLE(${PROJECT_NAME}
        common/ai/ai.cpp
        common/ai/astarpath.cpp
        common/ai/binheap.cpp
        common/ai/collidertile.cpp
        common/ai/jpsexpansion.cpp
        common/ai/jpspartpath.cpp
        common/ai/jpspath.cpp
        common/ai/localavoid.cpp
        common/ai/mempool.cpp
        common/ai/partialpath.cpp
        common/ai/pathdebug.cpp
        common/ai/pathjob.cpp
        common/ai/pathnode.cpp
        common/ai/reconstructpath.cpp
        common/debug.cpp
        common/econ/balance.cpp
        common/econ/demand.cpp
        common/gui/cursor.cpp
        common/gui/draw2d.cpp
        common/gui/font.cpp
        common/gui/gui.cpp
        common/gui/icon.cpp
        common/gui/inevent.cpp
        common/gui/richtext.cpp
        common/gui/sprite.cpp
        common/gui/viewlayer.cpp
        common/gui/widget.cpp
        common/gui/widgets/barbutton.cpp
        common/gui/widgets/button.cpp
        common/gui/widgets/checkbox.cpp
        common/gui/widgets/dropdowns.cpp
        common/gui/widgets/editbox.cpp
        common/gui/widgets/frame.cpp
        common/gui/widgets/hscrollbar.cpp
        common/gui/widgets/image.cpp
        common/gui/widgets/insdraw.cpp
        common/gui/widgets/link.cpp
        common/gui/widgets/listbox.cpp
        common/gui/widgets/spez/bottompanel.cpp
        common/gui/widgets/spez/buildpreview.cpp
        common/gui/widgets/spez/constructionview.cpp
        common/gui/widgets/spez/resticker.cpp
        common/gui/widgets/text.cpp
        common/gui/widgets/textarea.cpp
        common/gui/widgets/textblock.cpp
        common/gui/widgets/touchlistener.cpp
        common/gui/widgets/viewportw.cpp
        common/gui/widgets/vscrollbar.cpp
        common/gui/widgets/windoww.cpp
        common/math/3dmath.cpp
        common/math/barycentric.cpp
        common/math/brush.cpp
        common/math/camera.cpp
        common/math/edge.cpp
        common/math/frustum.cpp
        common/math/hmapmath.cpp
        common/math/line.cpp
        common/math/matrix.cpp
        common/math/physics.cpp
        common/math/plane2i.cpp
        common/math/plane3f.cpp
        common/math/polygon.cpp
        common/math/quaternion.cpp
        common/math/vec3f.cpp
        common/math/vec3i.cpp
        common/math/vec4f.cpp
        common/phys/collision.cpp
        common/phys/trace.cpp
        common/platform.cpp
        common/render/billboard.cpp
        common/render/dmdmodel.cpp
        common/render/foliage.cpp
        common/render/heightmap.cpp
        common/render/model.cpp
        common/render/ms3d.cpp
        common/render/particle.cpp
        common/render/projectile.cpp
        common/render/screenshot.cpp
        common/render/shader.cpp
        common/render/shadow.cpp
        common/render/skybox.cpp
        common/render/transaction.cpp
        common/render/vertexarray.cpp
        common/render/water.cpp
        common/save/savemap.cpp
        common/script/console.cpp
        common/script/script.cpp
        common/sim/build.cpp
        common/sim/building.cpp
        common/sim/buildingtype.cpp
        common/sim/connectable.cpp
        common/sim/crpipe.cpp
        common/sim/deposit.cpp
        common/sim/infrastructure.cpp
        common/sim/labourer.cpp
        common/sim/order.cpp
        common/sim/player.cpp
        common/sim/powl.cpp
        common/sim/resources.cpp
        common/sim/road.cpp
        common/sim/selection.cpp
        common/sim/sim.cpp
        common/sim/unit.cpp
        common/sim/unitmove.cpp
        common/sim/unittype.cpp
        common/sound/sound.cpp
        common/sys/unicode.cpp
        common/texture.cpp
        common/ustring.cpp
        common/utils.cpp
        common/window.cpp
        game/gmain.cpp
        game/gui/editorgui.cpp
        game/gui/ggui.cpp
        game/gui/gviewport.cpp
        game/gui/playgui.cpp
        game/keymap.cpp
        )

INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(GLEW REQUIRED glew)
PKG_SEARCH_MODULE(LIBPNG REQUIRED libpng)
FIND_PACKAGE(JPEG)
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(GLEW REQUIRED)

INCLUDE_DIRECTORIES( ${SDL2_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(
        ${PROJECT_NAME}
        ${SDL2_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${GL_LIBRARIES}
        ${GLU_LIBRARIES}
        ${LIBPNG_LIBRARIES}
        ${JPEG_LIBRARIES}
	${OPENGL_LIBRARIES}
        )

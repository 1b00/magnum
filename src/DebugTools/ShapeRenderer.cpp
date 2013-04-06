/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include "ShapeRenderer.h"

#include "ResourceManager.h"
#include "Physics/AbstractShape.h"
#include "Physics/AxisAlignedBox.h"
#include "Physics/Box.h"
#include "Physics/ObjectShape.h"
#include "Physics/LineSegment.h"
#include "Physics/Point.h"
#include "Physics/ShapeGroup.h"
#include "Physics/Sphere.h"
#include "SceneGraph/AbstractCamera.h"

#include "Implementation/AxisAlignedBoxRenderer.h"
#include "Implementation/BoxRenderer.h"
#include "Implementation/LineSegmentRenderer.h"
#include "Implementation/PointRenderer.h"
#include "Implementation/SphereRenderer.h"

namespace Magnum { namespace DebugTools {

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {

template<> void createDebugMesh(ShapeRenderer<2>* renderer, Physics::AbstractShape<2>* shape) {
    switch(shape->type()) {
        case Physics::AbstractShape2D::Type::AxisAlignedBox:
            renderer->renderers.push_back(new Implementation::AxisAlignedBoxRenderer<2>(*static_cast<Physics::AxisAlignedBox2D*>(shape)));
            break;
        case Physics::AbstractShape2D::Type::Box:
            renderer->renderers.push_back(new Implementation::BoxRenderer<2>(*static_cast<Physics::Box2D*>(shape)));
            break;
        case Physics::AbstractShape2D::Type::LineSegment:
            renderer->renderers.push_back(new Implementation::LineSegmentRenderer<2>(*static_cast<Physics::LineSegment2D*>(shape)));
            break;
        case Physics::AbstractShape2D::Type::Point:
            renderer->renderers.push_back(new Implementation::PointRenderer<2>(*static_cast<Physics::Point2D*>(shape)));
            break;
        case Physics::AbstractShape2D::Type::ShapeGroup: {
            Physics::ShapeGroup2D* group = static_cast<Physics::ShapeGroup2D*>(shape);
            if(group->first()) createDebugMesh(renderer, group->first());
            if(group->second()) createDebugMesh(renderer, group->second());
        } break;
        case Physics::AbstractShape2D::Type::Sphere:
            renderer->renderers.push_back(new Implementation::SphereRenderer<2>(*static_cast<Physics::Sphere2D*>(shape)));
            break;
        default:
            Warning() << "DebugTools::ShapeRenderer2D::createShapeRenderer(): type" << shape->type() << "not implemented";
    }
}

template<> void createDebugMesh(ShapeRenderer<3>* renderer, Physics::AbstractShape<3>* shape) {
    switch(shape->type()) {
        case Physics::AbstractShape3D::Type::AxisAlignedBox:
            renderer->renderers.push_back(new Implementation::AxisAlignedBoxRenderer<3>(*static_cast<Physics::AxisAlignedBox3D*>(shape)));
            break;
        case Physics::AbstractShape3D::Type::Box:
            renderer->renderers.push_back(new Implementation::BoxRenderer<3>(*static_cast<Physics::Box3D*>(shape)));
            break;
        case Physics::AbstractShape3D::Type::LineSegment:
            renderer->renderers.push_back(new Implementation::LineSegmentRenderer<3>(*static_cast<Physics::LineSegment3D*>(shape)));
            break;
        case Physics::AbstractShape3D::Type::Point:
            renderer->renderers.push_back(new Implementation::PointRenderer<3>(*static_cast<Physics::Point3D*>(shape)));
            break;
        case Physics::AbstractShape3D::Type::ShapeGroup: {
            Physics::ShapeGroup3D* group = static_cast<Physics::ShapeGroup3D*>(shape);
            if(group->first()) createDebugMesh(renderer, group->first());
            if(group->second()) createDebugMesh(renderer, group->second());
        } break;
        default:
            Warning() << "DebugTools::ShapeRenderer3D::createShapeRenderer(): type" << shape->type() << "not implemented";
    }
}

}
#endif

template<UnsignedInt dimensions> ShapeRenderer<dimensions>::ShapeRenderer(Physics::ObjectShape<dimensions>* shape, ResourceKey options, SceneGraph::DrawableGroup<dimensions>* drawables): SceneGraph::Drawable<dimensions>(shape->object(), drawables), options(ResourceManager::instance()->get<ShapeRendererOptions>(options)) {
    CORRADE_ASSERT(shape->shape() != nullptr, "DebugTools::ShapeRenderer: cannot create renderer for empty shape", );
    Implementation::createDebugMesh(this, shape->shape());
}

template<UnsignedInt dimensions> ShapeRenderer<dimensions>::~ShapeRenderer() {
    for(auto it = renderers.begin(); it != renderers.end(); ++it)
        delete *it;
}

template<UnsignedInt dimensions> void ShapeRenderer<dimensions>::draw(const typename DimensionTraits<dimensions>::MatrixType&, SceneGraph::AbstractCamera<dimensions>* camera) {
    typename DimensionTraits<dimensions>::MatrixType projectionMatrix = camera->projectionMatrix()*camera->cameraMatrix();
    for(auto it = renderers.begin(); it != renderers.end(); ++it)
        (*it)->draw(options, projectionMatrix);
}

template class ShapeRenderer<2>;
template class ShapeRenderer<3>;

}}

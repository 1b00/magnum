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

#include "Plane.h"

#include <limits>

#include "Math/Matrix4.h"
#include "Math/Geometry/Intersection.h"
#include "LineSegment.h"

using namespace Magnum::Math::Geometry;

namespace Magnum { namespace Physics {

void Plane::applyTransformationMatrix(const Matrix4& matrix) {
    _transformedPosition = matrix.transformPoint(_position);
    _transformedNormal = matrix.rotation()*_normal;
}

bool Plane::collides(const AbstractShape<3>* other) const {
    if(other->type() == Type::Line)
        return *this % *static_cast<const Line3D*>(other);
    if(other->type() == Type::LineSegment)
        return *this % *static_cast<const LineSegment3D*>(other);

    return AbstractShape<3>::collides(other);
}

bool Plane::operator%(const Line3D& other) const {
    Float t = Intersection::planeLine(transformedPosition(), transformedNormal(), other.transformedA(), other.transformedB());
    return t != t || (t != std::numeric_limits<Float>::infinity() && t != -std::numeric_limits<Float>::infinity());
}

bool Plane::operator%(const LineSegment3D& other) const {
    Float t = Intersection::planeLine(transformedPosition(), transformedNormal(), other.transformedA(), other.transformedB());
    return t > 0.0f && t < 1.0f;
}

}}

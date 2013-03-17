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

layout(location = 0) uniform int radius;
layout(location = 1) uniform vec2 scaling;
layout(binding = 8) uniform sampler2D texture;

layout(pixel_center_integer) in vec4 gl_FragCoord;

out float value;

ivec2 rotate(const ivec2 vec) {
    return ivec2(-vec.y, vec.x);
}

bool hasValue(const ivec2 position, const ivec2 offset) {
    return texelFetch(texture, position+offset, 0).r > 0.5;
}

void main() {
    const ivec2 position = ivec2(gl_FragCoord.xy*scaling);

    /* If pixel at the position is inside (1), we are looking for nearest pixel
       outside and the value will be positive (> 0.5). If it is outside (0), we
       are looking for nearest pixel inside and the value will be negative
       (< 0.5). */
    const bool isInside = hasValue(position, ivec2(0, 0));
    const float sign = isInside ? 1.0 : -1.0;

    /* Minimal found distance is just out of the radius (i.e. infinity) */
    float minDistanceSquared = float((radius+1)*(radius+1));

    /* Go in circles around the point and find nearest value */
    int radiusLimit = radius;
    for(int i = 1; i <= radiusLimit; ++i) {
        for(int j = 0, jmax = i*2; j != jmax; ++j) {
            const ivec2 offset = {-i+j, i};

            /* If any of the four values is opposite of what is on the pixel,
               we found nearest value */
            if(hasValue(position, offset) == !isInside ||
               hasValue(position, rotate(offset)) == !isInside ||
               hasValue(position, rotate(rotate(offset))) == !isInside ||
               hasValue(position, rotate(rotate(rotate(offset)))) == !isInside) {
                const float distanceSquared = dot(vec2(offset), vec2(offset));

                /* Set smaller distance, if found, or continue with lookup for
                   smaller */
                if(minDistanceSquared < distanceSquared) continue;
                else minDistanceSquared = distanceSquared;

                /* Set radius limit to max radius which can contain smaller
                   value, e.g. for distance 3.5 we can find smaller value even
                   in radius 3 */
                radiusLimit = min(radius, int(floor(length(vec2(offset)))));
            }
        }
    }

    /* Final signed distance, normalized from [-radius-1, radius+1] to [0, 1] */
    value = sign*sqrt(minDistanceSquared)/float(radius*2+2)+0.5;
}

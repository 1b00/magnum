/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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

#include "SceneData.h"

#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/Triple.h>
#include <Corrade/Utility/Algorithms.h>

#include "Magnum/Math/FunctionsBatch.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/Math/DualComplex.h"
#include "Magnum/Math/DualQuaternion.h"
#include "Magnum/Math/PackingBatch.h"
#include "Magnum/Trade/Implementation/arrayUtilities.h"

namespace Magnum { namespace Trade {

Debug& operator<<(Debug& debug, const SceneObjectType value) {
    debug << "Trade::SceneObjectType" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case SceneObjectType::value: return debug << "::" #value;
        _c(UnsignedByte)
        _c(UnsignedInt)
        _c(UnsignedShort)
        _c(UnsignedLong)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

UnsignedInt sceneObjectTypeSize(const SceneObjectType type) {
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic error "-Wswitch"
    #endif
    switch(type) {
        case SceneObjectType::UnsignedByte: return 1;
        case SceneObjectType::UnsignedShort: return 2;
        case SceneObjectType::UnsignedInt: return 4;
        case SceneObjectType::UnsignedLong: return 8;
    }
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic pop
    #endif

    CORRADE_ASSERT_UNREACHABLE("Trade::sceneObjectTypeSize(): invalid type" << type, {});
}

Debug& operator<<(Debug& debug, const SceneField value) {
    debug << "Trade::SceneField" << Debug::nospace;

    if(UnsignedInt(value) >= UnsignedInt(SceneField::Custom))
        return debug << "::Custom(" << Debug::nospace << (UnsignedInt(value) - UnsignedInt(SceneField::Custom)) << Debug::nospace << ")";

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case SceneField::value: return debug << "::" #value;
        _c(Parent)
        _c(Transformation)
        _c(Translation)
        _c(Rotation)
        _c(Scaling)
        _c(Mesh)
        _c(MeshMaterial)
        _c(Light)
        _c(Camera)
        _c(Skin)
        _c(ImporterState)
        #undef _c
        /* LCOV_EXCL_STOP */

        /* To silence compiler warning about unhandled values */
        case SceneField::Custom: CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedInt(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const SceneFieldType value) {
    debug << "Trade::SceneFieldType" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case SceneFieldType::value: return debug << "::" #value;
        _c(Float)
        _c(Half)
        _c(Double)
        _c(UnsignedByte)
        _c(Byte)
        _c(UnsignedShort)
        _c(Short)
        _c(UnsignedInt)
        _c(Int)
        _c(UnsignedLong)
        _c(Long)
        _c(Vector2)
        _c(Vector2h)
        _c(Vector2d)
        _c(Vector2ub)
        _c(Vector2b)
        _c(Vector2us)
        _c(Vector2s)
        _c(Vector2ui)
        _c(Vector2i)
        _c(Vector3)
        _c(Vector3h)
        _c(Vector3d)
        _c(Vector3ub)
        _c(Vector3b)
        _c(Vector3us)
        _c(Vector3s)
        _c(Vector3ui)
        _c(Vector3i)
        _c(Vector4)
        _c(Vector4h)
        _c(Vector4d)
        _c(Vector4ub)
        _c(Vector4b)
        _c(Vector4us)
        _c(Vector4s)
        _c(Vector4ui)
        _c(Vector4i)
        _c(Matrix2x2)
        _c(Matrix2x2h)
        _c(Matrix2x2d)
        _c(Matrix2x3)
        _c(Matrix2x3h)
        _c(Matrix2x3d)
        _c(Matrix2x4)
        _c(Matrix2x4h)
        _c(Matrix2x4d)
        _c(Matrix3x2)
        _c(Matrix3x2h)
        _c(Matrix3x2d)
        _c(Matrix3x3)
        _c(Matrix3x3h)
        _c(Matrix3x3d)
        _c(Matrix3x4)
        _c(Matrix3x4h)
        _c(Matrix3x4d)
        _c(Matrix4x2)
        _c(Matrix4x2h)
        _c(Matrix4x2d)
        _c(Matrix4x3)
        _c(Matrix4x3h)
        _c(Matrix4x3d)
        _c(Matrix4x4)
        _c(Matrix4x4h)
        _c(Matrix4x4d)
        _c(Range1D)
        _c(Range1Dh)
        _c(Range1Dd)
        _c(Range1Di)
        _c(Range2D)
        _c(Range2Dh)
        _c(Range2Dd)
        _c(Range2Di)
        _c(Range3D)
        _c(Range3Dh)
        _c(Range3Dd)
        _c(Range3Di)
        _c(Complex)
        _c(Complexd)
        _c(DualComplex)
        _c(DualComplexd)
        _c(Quaternion)
        _c(Quaterniond)
        _c(DualQuaternion)
        _c(DualQuaterniond)
        _c(Deg)
        _c(Degh)
        _c(Degd)
        _c(Rad)
        _c(Radh)
        _c(Radd)
        _c(Pointer)
        _c(MutablePointer)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedShort(value)) << Debug::nospace << ")";
}

UnsignedInt sceneFieldTypeSize(const SceneFieldType type) {
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic error "-Wswitch"
    #endif
    switch(type) {
        case SceneFieldType::UnsignedByte:
        case SceneFieldType::Byte:
            return 1;
        case SceneFieldType::UnsignedShort:
        case SceneFieldType::Short:
        case SceneFieldType::Half:
        case SceneFieldType::Vector2ub:
        case SceneFieldType::Vector2b:
        case SceneFieldType::Degh:
        case SceneFieldType::Radh:
            return 2;
        case SceneFieldType::Vector3ub:
        case SceneFieldType::Vector3b:
            return 3;
        case SceneFieldType::UnsignedInt:
        case SceneFieldType::Int:
        case SceneFieldType::Float:
        case SceneFieldType::Vector2us:
        case SceneFieldType::Vector2s:
        case SceneFieldType::Vector2h:
        case SceneFieldType::Vector4ub:
        case SceneFieldType::Vector4b:
        case SceneFieldType::Range1Dh:
        case SceneFieldType::Deg:
        case SceneFieldType::Rad:
            return 4;
        case SceneFieldType::Vector3us:
        case SceneFieldType::Vector3s:
        case SceneFieldType::Vector3h:
            return 6;
        case SceneFieldType::UnsignedLong:
        case SceneFieldType::Long:
        case SceneFieldType::Double:
        case SceneFieldType::Vector2:
        case SceneFieldType::Vector2ui:
        case SceneFieldType::Vector2i:
        case SceneFieldType::Vector4us:
        case SceneFieldType::Vector4s:
        case SceneFieldType::Vector4h:
        case SceneFieldType::Matrix2x2h:
        case SceneFieldType::Range1D:
        case SceneFieldType::Range1Di:
        case SceneFieldType::Range2Dh:
        case SceneFieldType::Complex:
        case SceneFieldType::Degd:
        case SceneFieldType::Radd:
            return 8;
        case SceneFieldType::Vector3:
        case SceneFieldType::Vector3ui:
        case SceneFieldType::Vector3i:
        case SceneFieldType::Matrix2x3h:
        case SceneFieldType::Matrix3x2h:
        case SceneFieldType::Range3Dh:
            return 12;
        case SceneFieldType::Vector2d:
        case SceneFieldType::Vector4:
        case SceneFieldType::Vector4ui:
        case SceneFieldType::Vector4i:
        case SceneFieldType::Matrix2x2:
        case SceneFieldType::Matrix2x4h:
        case SceneFieldType::Matrix4x2h:
        case SceneFieldType::Range1Dd:
        case SceneFieldType::Range2D:
        case SceneFieldType::Range2Di:
        case SceneFieldType::Complexd:
        case SceneFieldType::DualComplex:
        case SceneFieldType::Quaternion:
            return 16;
        case SceneFieldType::Matrix3x3h:
            return 18;
        case SceneFieldType::Vector3d:
        case SceneFieldType::Matrix2x3:
        case SceneFieldType::Matrix3x4h:
        case SceneFieldType::Matrix3x2:
        case SceneFieldType::Matrix4x3h:
        case SceneFieldType::Range3D:
        case SceneFieldType::Range3Di:
            return 24;
        case SceneFieldType::Vector4d:
        case SceneFieldType::Matrix2x2d:
        case SceneFieldType::Matrix2x4:
        case SceneFieldType::Matrix4x2:
        case SceneFieldType::Matrix4x4h:
        case SceneFieldType::Range2Dd:
        case SceneFieldType::DualComplexd:
        case SceneFieldType::Quaterniond:
        case SceneFieldType::DualQuaternion:
            return 32;
        case SceneFieldType::Matrix3x3:
            return 36;
        case SceneFieldType::Matrix2x3d:
        case SceneFieldType::Matrix3x4:
        case SceneFieldType::Matrix3x2d:
        case SceneFieldType::Matrix4x3:
        case SceneFieldType::Range3Dd:
            return 48;
        case SceneFieldType::Matrix2x4d:
        case SceneFieldType::Matrix4x2d:
        case SceneFieldType::Matrix4x4:
        case SceneFieldType::DualQuaterniond:
            return 64;
        case SceneFieldType::Matrix3x3d:
            return 72;
        case SceneFieldType::Matrix3x4d:
        case SceneFieldType::Matrix4x3d:
            return 96;
        case SceneFieldType::Matrix4x4d:
            return 128;
        case SceneFieldType::Pointer:
        case SceneFieldType::MutablePointer:
            return sizeof(void*);
    }
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic pop
    #endif

    CORRADE_ASSERT_UNREACHABLE("Trade::sceneFieldTypeSize(): invalid type" << type, {});
}

SceneFieldData::SceneFieldData(const SceneField name, const Containers::StridedArrayView2D<const char>& objectData, const SceneFieldType fieldType, const Containers::StridedArrayView2D<const char>& fieldData, const UnsignedShort fieldArraySize) noexcept: SceneFieldData{name, {}, Containers::StridedArrayView1D<const void>{{objectData.data(), ~std::size_t{}}, objectData.size()[0], objectData.stride()[0]}, fieldType, Containers::StridedArrayView1D<const void>{{fieldData.data(), ~std::size_t{}}, fieldData.size()[0], fieldData.stride()[0]}, fieldArraySize} {
    /* Yes, this calls into a constexpr function defined in the header --
       because I feel that makes more sense than duplicating the full assert
       logic */
    #ifndef CORRADE_NO_ASSERT
    if(fieldArraySize) CORRADE_ASSERT(fieldData.empty()[0] || fieldData.size()[1] == sceneFieldTypeSize(fieldType)*fieldArraySize,
        "Trade::SceneFieldData: second field view dimension size" << fieldData.size()[1] << "doesn't match" << fieldType << "and field array size" << fieldArraySize, );
    else CORRADE_ASSERT(fieldData.empty()[0] || fieldData.size()[1] == sceneFieldTypeSize(fieldType),
        "Trade::SceneFieldData: second field view dimension size" << fieldData.size()[1] << "doesn't match" << fieldType, );
    #endif

    if(objectData.size()[1] == 8) _objectType = SceneObjectType::UnsignedLong;
    else if(objectData.size()[1] == 4) _objectType = SceneObjectType::UnsignedInt;
    else if(objectData.size()[1] == 2) _objectType = SceneObjectType::UnsignedShort;
    else if(objectData.size()[1] == 1) _objectType = SceneObjectType::UnsignedByte;
    else CORRADE_ASSERT_UNREACHABLE("Trade::SceneFieldData: expected second object view dimension size 1, 2, 4 or 8 but got" << objectData.size()[1], );

    CORRADE_ASSERT(fieldData.isContiguous<1>(), "Trade::SceneFieldData: second field view dimension is not contiguous", );
    CORRADE_ASSERT(objectData.isContiguous<1>(), "Trade::SceneFieldData: second object view dimension is not contiguous", );
}

Containers::Array<SceneFieldData> sceneFieldDataNonOwningArray(const Containers::ArrayView<const SceneFieldData> view) {
    /* Ugly, eh? */
    return Containers::Array<SceneFieldData>{const_cast<SceneFieldData*>(view.data()), view.size(), reinterpret_cast<void(*)(SceneFieldData*, std::size_t)>(Implementation::nonOwnedArrayDeleter)};
}

SceneData::SceneData(const SceneObjectType objectType, const UnsignedLong objectCount, Containers::Array<char>&& data, Containers::Array<SceneFieldData>&& fields, const void* const importerState) noexcept: _dataFlags{DataFlag::Owned|DataFlag::Mutable}, _objectType{objectType}, _dimensions{}, _objectCount{objectCount}, _importerState{importerState}, _fields{std::move(fields)}, _data{std::move(data)} {
    /* Check that object type is large enough */
    CORRADE_ASSERT(
        (objectType == SceneObjectType::UnsignedByte && objectCount <= 0xffull) ||
        (objectType == SceneObjectType::UnsignedShort && objectCount <= 0xffffull) ||
        (objectType == SceneObjectType::UnsignedInt && objectCount <= 0xffffffffull) ||
        objectType == SceneObjectType::UnsignedLong,
        "Trade::SceneData:" << objectType << "is too small for" << objectCount << "objects", );

    #ifndef CORRADE_NO_ASSERT
    /* Check various assumptions about field data */
    Math::BoolVector<12> fieldsPresent; /** @todo some constant for this */
    const UnsignedInt objectTypeSize = sceneObjectTypeSize(_objectType);
    UnsignedInt transformationField = ~UnsignedInt{};
    UnsignedInt translationField = ~UnsignedInt{};
    UnsignedInt rotationField = ~UnsignedInt{};
    UnsignedInt scalingField = ~UnsignedInt{};
    UnsignedInt meshField = ~UnsignedInt{};
    UnsignedInt meshMaterialField = ~UnsignedInt{};
    for(std::size_t i = 0; i != _fields.size(); ++i) {
        const SceneFieldData& field = _fields[i];

        /* The object type has to be the same among all fields. Technically it
           wouldn't need to be, but if there's 60k objects then using a 8bit
           type for certain fields would mean only the first 256 objects can be
           referenced, which makes no practical sense, and to improve the
           situation there would need to be some additional per-field object
           offset and ... it's simpler to just require the object type to be
           large enough to reference all objects (checked outside of the loop
           above) and that it's the same for all fields. This also makes it
           more convenient for the user. */
        CORRADE_ASSERT(field._objectType == _objectType,
            "Trade::SceneData: inconsistent object type, got" << field._objectType << "for field" << i << "but expected" << _objectType, );

        /* We could check that object indices are in bounds, but that's rather
           expensive. OTOH it's fine if field size is larger than object count,
           as a single object can have multiple instances of the same field
           attached, so checking that would be wrong. */

        /* Check that there are only unique fields. To avoid a O(n^2) operation
           always (or allocating a sorted field map), builtin fields are
           checked against a map and only custom fields are checked in an
           O(n^2) way with the assumption there isn't many of them (and that
           they'll gradually become builtin). */
        if(!isSceneFieldCustom(_fields[i]._name)) {
            CORRADE_INTERNAL_ASSERT(UnsignedInt(_fields[i]._name) < fieldsPresent.Size);
            CORRADE_ASSERT(!fieldsPresent[UnsignedInt(_fields[i]._name)],
                "Trade::SceneData: duplicate field" << _fields[i]._name, );
            fieldsPresent.set(UnsignedInt(_fields[i]._name), true);
        } else for(std::size_t j = 0; j != i; ++j) {
            CORRADE_ASSERT(_fields[j]._name != _fields[i]._name,
                "Trade::SceneData: duplicate field" << _fields[i]._name, );
        }

        /* Check that both the object and field view fits into the provided
           data array. If the field is empty, we don't check anything --
           accessing the memory would be invalid anyway and enforcing this
           would lead to unnecessary friction with optional fields. */
        if(field._size) {
            const UnsignedInt fieldTypeSize = sceneFieldTypeSize(field._fieldType)*
                (field._fieldArraySize ? field._fieldArraySize : 1);
            if(field._isOffsetOnly) {
                const std::size_t objectSize = field._objectData.offset + (field._size - 1)*field._objectStride + objectTypeSize;
                const std::size_t fieldSize = field._fieldData.offset + (field._size - 1)*field._fieldStride + fieldTypeSize;
                CORRADE_ASSERT(objectSize <= _data.size(),
                    "Trade::SceneData: offset-only object data of field" << i << "span" << objectSize << "bytes but passed data array has only" << _data.size(), );
                CORRADE_ASSERT(fieldSize <= _data.size(),
                    "Trade::SceneData: offset-only field data of field" << i << "span" << fieldSize << "bytes but passed data array has only" << _data.size(), );
            } else {
                const void* const objectBegin = field._objectData.pointer;
                const void* const fieldBegin = field._fieldData.pointer;
                const void* const objectEnd = static_cast<const char*>(field._objectData.pointer) + (field._size - 1)*field._objectStride + objectTypeSize;
                const void* const fieldEnd = static_cast<const char*>(field._fieldData.pointer) + (field._size - 1)*field._fieldStride + fieldTypeSize;
                CORRADE_ASSERT(objectBegin >= _data.begin() && objectEnd <= _data.end(),
                    "Trade::SceneData: object data [" << Debug::nospace << objectBegin << Debug::nospace << ":" << Debug::nospace << objectEnd << Debug::nospace << "] of field" << i << "are not contained in passed data array [" << Debug::nospace << static_cast<const void*>(_data.begin()) << Debug::nospace << ":" << Debug::nospace << static_cast<const void*>(_data.end()) << Debug::nospace << "]", );
                CORRADE_ASSERT(fieldBegin >= _data.begin() && fieldEnd <= _data.end(),
                    "Trade::SceneData: field data [" << Debug::nospace << fieldBegin << Debug::nospace << ":" << Debug::nospace << fieldEnd << Debug::nospace << "] of field" << i << "are not contained in passed data array [" << Debug::nospace << static_cast<const void*>(_data.begin()) << Debug::nospace << ":" << Debug::nospace << static_cast<const void*>(_data.end()) << Debug::nospace << "]", );
            }
        }

        /* Remember TRS and mesh/material fields to check their object mapping
           consistency outside of the loop below */
        if(_fields[i]._name == SceneField::Transformation) {
            transformationField = i;
        } else if(_fields[i]._name == SceneField::Translation) {
            translationField = i;
        } else if(_fields[i]._name == SceneField::Rotation) {
            rotationField = i;
        } else if(_fields[i]._name == SceneField::Scaling) {
            scalingField = i;
        } else if(_fields[i]._name == SceneField::Mesh) {
            meshField = i;
        } else if(_fields[i]._name == SceneField::MeshMaterial) {
            meshMaterialField = i;
        }
    }

    /* Check that certain fields share the same object mapping. Printing as if
       all would be pointers (and not offset-only), it's not worth the extra
       effort just for an assert message. Also, compared to above, where
       "begin" was always zero, here we're always comparing four values, so the
       message for offset-only wouldn't be simpler either. */
    const auto checkFieldObjectDataMatch = [](const SceneFieldData& a, const SceneFieldData& b) {
        const std::size_t objectTypeSize = sceneObjectTypeSize(a._objectType);
        const void* const aBegin = a._objectData.pointer;
        const void* const bBegin = b._objectData.pointer;
        const void* const aEnd = static_cast<const char*>(a._objectData.pointer) + a._size*objectTypeSize;
        const void* const bEnd = static_cast<const char*>(b._objectData.pointer) + b._size*objectTypeSize;
        CORRADE_ASSERT(aBegin == bBegin && aEnd == bEnd,
            "Trade::SceneData:" << b._name << "object data [" << Debug::nospace << bBegin << Debug::nospace << ":" << Debug::nospace << bEnd << Debug::nospace << "] is different from" << a._name << "object data [" << Debug::nospace << aBegin << Debug::nospace << ":" << Debug::nospace << aEnd << Debug::nospace << "]", );
    };

    /* All present TRS fields should share the same object mapping */
    if(translationField != ~UnsignedInt{}) {
        if(rotationField != ~UnsignedInt{})
            checkFieldObjectDataMatch(_fields[translationField], _fields[rotationField]);
        if(scalingField != ~UnsignedInt{})
            checkFieldObjectDataMatch(_fields[translationField], _fields[scalingField]);
    }
    if(rotationField != ~UnsignedInt{} && scalingField != ~UnsignedInt{})
        checkFieldObjectDataMatch(_fields[rotationField], _fields[scalingField]);

    /* Mesh and materials also */
    if(meshField != ~UnsignedInt{} && meshMaterialField != ~UnsignedInt{})
        checkFieldObjectDataMatch(_fields[meshField], _fields[meshMaterialField]);

    /* Decide about dimensionality based on transformation type, if present */
    if(transformationField != ~UnsignedInt{}) {
        const SceneFieldType fieldType = _fields[transformationField]._fieldType;
        if(fieldType == SceneFieldType::Matrix3x3 ||
           fieldType == SceneFieldType::Matrix3x3d ||
           fieldType == SceneFieldType::DualComplex ||
           fieldType == SceneFieldType::DualComplexd)
            _dimensions = 2;
        else if(fieldType == SceneFieldType::Matrix4x4 ||
                fieldType == SceneFieldType::Matrix4x4d ||
                fieldType == SceneFieldType::DualQuaternion ||
                fieldType == SceneFieldType::DualQuaterniond)
            _dimensions = 3;
        else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    }

    /* Use TRS fields to decide about dimensionality, if the transformation
       field is not present. If it is, verify that they match it. */
    if(translationField != ~UnsignedInt{}) {
        const SceneFieldType fieldType = _fields[translationField]._fieldType;
        if(fieldType == SceneFieldType::Vector2 ||
           fieldType == SceneFieldType::Vector2d) {
            CORRADE_ASSERT(!_dimensions || _dimensions == 2,
                "Trade::SceneData: expected a 3D translation field but got" << fieldType, );
            _dimensions = 2;
        } else if(fieldType == SceneFieldType::Vector3 ||
                  fieldType == SceneFieldType::Vector3d) {
            CORRADE_ASSERT(!_dimensions || _dimensions == 3,
                "Trade::SceneData: expected a 2D translation field but got" << fieldType, );
            _dimensions = 3;
        } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    }
    if(rotationField != ~UnsignedInt{}) {
        const SceneFieldType fieldType = _fields[rotationField]._fieldType;
        if(fieldType == SceneFieldType::Complex ||
           fieldType == SceneFieldType::Complexd) {
            CORRADE_ASSERT(!_dimensions || _dimensions == 2,
                "Trade::SceneData: expected a 3D rotation field but got" << fieldType, );
            _dimensions = 2;
        } else if(fieldType == SceneFieldType::Quaternion ||
                  fieldType == SceneFieldType::Quaterniond) {
            CORRADE_ASSERT(!_dimensions || _dimensions == 3,
                "Trade::SceneData: expected a 2D rotation field but got" << fieldType, );
            _dimensions = 3;
        } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    }
    if(scalingField != ~UnsignedInt{}) {
        const SceneFieldType fieldType = _fields[scalingField]._fieldType;
        if(fieldType == SceneFieldType::Vector2 ||
           fieldType == SceneFieldType::Vector2d) {
            CORRADE_ASSERT(!_dimensions || _dimensions == 2,
                "Trade::SceneData: expected a 3D scaling field but got" << fieldType, );
            _dimensions = 2;
        } else if(fieldType == SceneFieldType::Vector3 ||
                  fieldType == SceneFieldType::Vector3d) {
            CORRADE_ASSERT(!_dimensions || _dimensions == 3,
                "Trade::SceneData: expected a 2D scaling field but got" << fieldType, );
            _dimensions = 3;
        } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    }
    #endif
}

SceneData::SceneData(const SceneObjectType objectType, const UnsignedLong objectCount, Containers::Array<char>&& data, const std::initializer_list<SceneFieldData> fields, const void* const importerState): SceneData{objectType, objectCount, std::move(data), Implementation::initializerListToArrayWithDefaultDeleter(fields), importerState} {}

SceneData::SceneData(const SceneObjectType objectType, const UnsignedLong objectCount, const DataFlags dataFlags, const Containers::ArrayView<const void> data, Containers::Array<SceneFieldData>&& fields, const void* const importerState) noexcept: SceneData{objectType, objectCount, Containers::Array<char>{const_cast<char*>(static_cast<const char*>(data.data())), data.size(), Implementation::nonOwnedArrayDeleter}, std::move(fields), importerState} {
    CORRADE_ASSERT(!(dataFlags & DataFlag::Owned),
        "Trade::SceneData: can't construct with non-owned data but" << dataFlags, );
    _dataFlags = dataFlags;
}

SceneData::SceneData(const SceneObjectType objectType, const UnsignedLong objectCount, const DataFlags dataFlags, const Containers::ArrayView<const void> data, const std::initializer_list<SceneFieldData> fields, const void* const importerState): SceneData{objectType, objectCount, dataFlags, data, Implementation::initializerListToArrayWithDefaultDeleter(fields), importerState} {}

SceneData::SceneData(SceneData&&) noexcept = default;

SceneData::~SceneData() = default;

SceneData& SceneData::operator=(SceneData&&) noexcept = default;

Containers::ArrayView<char> SceneData::mutableData() & {
    CORRADE_ASSERT(_dataFlags & DataFlag::Mutable,
        "Trade::SceneData::mutableData(): data not mutable", {});
    return _data;
}

Containers::StridedArrayView1D<const void> SceneData::fieldDataObjectViewInternal(const SceneFieldData& field, const std::size_t offset, const std::size_t size) const {
    CORRADE_INTERNAL_ASSERT(offset + size <= field._size);
    return Containers::StridedArrayView1D<const void>{
        /* We're *sure* the view is correct, so faking the view size */
        {static_cast<const char*>(field._isOffsetOnly ?
            _data.data() + field._objectData.offset : field._objectData.pointer)
            + field._fieldStride*offset, ~std::size_t{}},
        size, field._objectStride};
}

Containers::StridedArrayView1D<const void> SceneData::fieldDataObjectViewInternal(const SceneFieldData& field) const {
    return fieldDataObjectViewInternal(field, 0, field._size);
}

Containers::StridedArrayView1D<const void> SceneData::fieldDataFieldViewInternal(const SceneFieldData& field, const std::size_t offset, const std::size_t size) const {
    CORRADE_INTERNAL_ASSERT(offset + size <= field._size);
    return Containers::StridedArrayView1D<const void>{
        /* We're *sure* the view is correct, so faking the view size */
        {static_cast<const char*>(field._isOffsetOnly ?
            _data.data() + field._fieldData.offset : field._fieldData.pointer)
            + field._fieldStride*offset, ~std::size_t{}},
        size, field._fieldStride};
}

Containers::StridedArrayView1D<const void> SceneData::fieldDataFieldViewInternal(const SceneFieldData& field) const {
    return fieldDataFieldViewInternal(field, 0, field._size);
}

SceneFieldData SceneData::fieldData(const UnsignedInt id) const {
    CORRADE_ASSERT(id < _fields.size(),
        "Trade::SceneData::fieldData(): index" << id << "out of range for" << _fields.size() << "fields", SceneFieldData{});
    const SceneFieldData& field = _fields[id];
    return SceneFieldData{field._name, field._objectType, fieldDataObjectViewInternal(field), field._fieldType, fieldDataFieldViewInternal(field), field._fieldArraySize};
}

SceneField SceneData::fieldName(const UnsignedInt id) const {
    CORRADE_ASSERT(id < _fields.size(),
        "Trade::SceneData::fieldName(): index" << id << "out of range for" << _fields.size() << "fields", {});
    return _fields[id]._name;
}

SceneFieldType SceneData::fieldType(const UnsignedInt id) const {
    CORRADE_ASSERT(id < _fields.size(),
        "Trade::SceneData::fieldType(): index" << id << "out of range for" << _fields.size() << "fields", {});
    return _fields[id]._fieldType;
}

std::size_t SceneData::fieldSize(const UnsignedInt id) const {
    CORRADE_ASSERT(id < _fields.size(),
        "Trade::SceneData::fieldSize(): index" << id << "out of range for" << _fields.size() << "fields", {});
    return _fields[id]._size;
}

UnsignedShort SceneData::fieldArraySize(const UnsignedInt id) const {
    CORRADE_ASSERT(id < _fields.size(),
        "Trade::SceneData::fieldArraySize(): index" << id << "out of range for" << _fields.size() << "fields", {});
    return _fields[id]._fieldArraySize;
}

UnsignedInt SceneData::fieldFor(const SceneField name) const {
    for(std::size_t i = 0; i != _fields.size(); ++i)
        if(_fields[i]._name == name) return i;
    return ~UnsignedInt{};
}

bool SceneData::hasField(const SceneField name) const {
    return fieldFor(name) != ~UnsignedInt{};
}

UnsignedInt SceneData::fieldId(const SceneField name) const {
    const UnsignedInt fieldId = fieldFor(name);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{}, "Trade::SceneData::fieldId(): field" << name << "not found", {});
    return fieldId;
}

SceneFieldType SceneData::fieldType(const SceneField name) const {
    const UnsignedInt fieldId = fieldFor(name);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{}, "Trade::SceneData::fieldType(): field" << name << "not found", {});
    return _fields[fieldId]._fieldType;
}

std::size_t SceneData::fieldSize(const SceneField name) const {
    const UnsignedInt fieldId = fieldFor(name);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{}, "Trade::SceneData::fieldSize(): field" << name << "not found", {});
    return _fields[fieldId]._size;
}

UnsignedShort SceneData::fieldArraySize(const SceneField name) const {
    const UnsignedInt fieldId = fieldFor(name);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{}, "Trade::SceneData::fieldArraySize(): field" << name << "not found", {});
    return _fields[fieldId]._fieldArraySize;
}

Containers::StridedArrayView2D<const char> SceneData::objects(const UnsignedInt fieldId) const {
    CORRADE_ASSERT(fieldId < _fields.size(),
        "Trade::SceneData::objects(): index" << fieldId << "out of range for" << _fields.size() << "fields", {});
    const SceneFieldData& field = _fields[fieldId];
    /* Build a 2D view using information about object type size */
    return Containers::arrayCast<2, const char>(
        fieldDataObjectViewInternal(field),
        sceneObjectTypeSize(field._objectType));
}

Containers::StridedArrayView2D<char> SceneData::mutableObjects(const UnsignedInt fieldId) {
    CORRADE_ASSERT(_dataFlags & DataFlag::Mutable,
        "Trade::SceneData::mutableObjects(): data not mutable", {});
    CORRADE_ASSERT(fieldId < _fields.size(),
        "Trade::SceneData::mutableObjects(): index" << fieldId << "out of range for" << _fields.size() << "fields", {});
    const SceneFieldData& field = _fields[fieldId];
    /* Build a 2D view using information about attribute type size */
    const auto out = Containers::arrayCast<2, const char>(
        fieldDataObjectViewInternal(field),
        sceneObjectTypeSize(field._objectType));
    /** @todo some arrayConstCast? UGH */
    return Containers::StridedArrayView2D<char>{
        /* The view size is there only for a size assert, we're pretty sure the
           view is valid */
        {static_cast<char*>(const_cast<void*>(out.data())), ~std::size_t{}},
        out.size(), out.stride()};
}

Containers::StridedArrayView2D<const char> SceneData::objects(const SceneField fieldName) const {
    const UnsignedInt fieldId = fieldFor(fieldName);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{}, "Trade::SceneData::objects(): field" << fieldName << "not found", {});
    return objects(fieldId);
}

Containers::StridedArrayView2D<char> SceneData::mutableObjects(const SceneField fieldName) {
    CORRADE_ASSERT(_dataFlags & DataFlag::Mutable,
        "Trade::SceneData::mutableObjects(): data not mutable", {});
    const UnsignedInt fieldId = fieldFor(fieldName);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{}, "Trade::SceneData::mutableObjects(): field" << fieldName << "not found", {});
    return mutableObjects(fieldId);
}

Containers::StridedArrayView2D<const char> SceneData::field(const UnsignedInt id) const {
    CORRADE_ASSERT(id < _fields.size(),
        "Trade::SceneData::field(): index" << id << "out of range for" << _fields.size() << "fields", {});
    const SceneFieldData& field = _fields[id];
    /* Build a 2D view using information about object type size */
    return Containers::arrayCast<2, const char>(
        fieldDataFieldViewInternal(field),
        sceneFieldTypeSize(field._fieldType)*(field._fieldArraySize ? field._fieldArraySize : 1));
}

Containers::StridedArrayView2D<char> SceneData::mutableField(const UnsignedInt id) {
    CORRADE_ASSERT(_dataFlags & DataFlag::Mutable,
        "Trade::SceneData::mutableField(): data not mutable", {});
    CORRADE_ASSERT(id < _fields.size(),
        "Trade::SceneData::mutableField(): index" << id << "out of range for" << _fields.size() << "fields", {});
    const SceneFieldData& field = _fields[id];
    /* Build a 2D view using information about attribute type size */
    const auto out = Containers::arrayCast<2, const char>(
        fieldDataFieldViewInternal(field),
        sceneFieldTypeSize(field._fieldType)*(field._fieldArraySize ? field._fieldArraySize : 1));
    /** @todo some arrayConstCast? UGH */
    return Containers::StridedArrayView2D<char>{
        /* The view size is there only for a size assert, we're pretty sure the
           view is valid */
        {static_cast<char*>(const_cast<void*>(out.data())), ~std::size_t{}},
        out.size(), out.stride()};
}

Containers::StridedArrayView2D<const char> SceneData::field(const SceneField name) const {
    const UnsignedInt fieldId = fieldFor(name);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::field(): field" << name << "not found", {});
    return field(fieldId);
}

Containers::StridedArrayView2D<char> SceneData::mutableField(const SceneField name) {
    CORRADE_ASSERT(_dataFlags & DataFlag::Mutable,
        "Trade::SceneData::mutableField(): data not mutable", {});
    const UnsignedInt fieldId = fieldFor(name);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::mutableField(): field" << name << "not found", {});
    return mutableField(fieldId);
}

void SceneData::objectsIntoInternal(const UnsignedInt fieldId, const std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& destination) const {
    /* fieldId, offset and destination.size() is assumed to be in bounds,
       checked by the callers */

    const SceneFieldData& field = _fields[fieldId];
    const Containers::StridedArrayView1D<const void> objectData = fieldDataObjectViewInternal(field, offset, destination.size());
    const auto destination1ui = Containers::arrayCast<2, UnsignedInt>(destination);

    if(field._objectType == SceneObjectType::UnsignedInt)
        Utility::copy(Containers::arrayCast<const UnsignedInt>(objectData), destination);
    else if(field._objectType == SceneObjectType::UnsignedShort)
        Math::castInto(Containers::arrayCast<2, const UnsignedShort>(objectData, 1), destination1ui);
    else if(field._objectType == SceneObjectType::UnsignedByte)
        Math::castInto(Containers::arrayCast<2, const UnsignedByte>(objectData, 1), destination1ui);
    else if(field._objectType == SceneObjectType::UnsignedLong) {
        CORRADE_ASSERT(_objectCount <= 0xffffffffull, "Trade::SceneData::objectsInto(): indices for up to" << _objectCount << "objects can't fit into a 32-bit type, access them directly via objects() instead", );
        Math::castInto(Containers::arrayCast<2, const UnsignedLong>(objectData, 1), destination1ui);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

void SceneData::objectsInto(const UnsignedInt fieldId, const Containers::StridedArrayView1D<UnsignedInt>& destination) const {
    CORRADE_ASSERT(fieldId < _fields.size(),
        "Trade::SceneData::objectsInto(): index" << fieldId << "out of range for" << _fields.size() << "fields", );
    CORRADE_ASSERT(destination.size() == _fields[fieldId]._size,
        "Trade::SceneData::objectsInto(): expected a view with" << _fields[fieldId]._size << "elements but got" << destination.size(), );
    objectsIntoInternal(fieldId, 0, destination);
}

std::size_t SceneData::objectsInto(const UnsignedInt fieldId, const std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& destination) const {
    CORRADE_ASSERT(fieldId < _fields.size(),
        "Trade::SceneData::objectsInto(): index" << fieldId << "out of range for" << _fields.size() << "fields", {});
    CORRADE_ASSERT(offset <= _fields[fieldId]._size,
        "Trade::SceneData::objectsInto(): offset" << offset << "out of bounds for a field of size" << _fields[fieldId]._size, {});
    const std::size_t size = Math::min(destination.size(), std::size_t(_fields[fieldId]._size) - offset);
    objectsIntoInternal(fieldId, offset, destination.prefix(size));
    return size;
}

Containers::Array<UnsignedInt> SceneData::objectsAsArray(const UnsignedInt fieldId) const {
    CORRADE_ASSERT(fieldId < _fields.size(),
        /* Using the same message as in Into() to avoid too many redundant
           strings in the binary */
        "Trade::SceneData::objectsInto(): index" << fieldId << "out of range for" << _fields.size() << "fields", {});
    Containers::Array<UnsignedInt> out{NoInit, std::size_t(_fields[fieldId]._size)};
    objectsIntoInternal(fieldId, 0, out);
    return out;
}

void SceneData::objectsInto(const SceneField name, const Containers::StridedArrayView1D<UnsignedInt>& destination) const {
    const UnsignedInt fieldId = fieldFor(name);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::objectsInto(): field" << name << "not found", );
    objectsInto(fieldId, destination);
}

std::size_t SceneData::objectsInto(const SceneField name, std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& destination) const {
    const UnsignedInt fieldId = fieldFor(name);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::objectsInto(): field" << name << "not found", {});
    return objectsInto(fieldId, offset, destination);
}

Containers::Array<UnsignedInt> SceneData::objectsAsArray(const SceneField name) const {
    const UnsignedInt fieldId = fieldFor(name);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        /* Using the same message as in Into() to avoid too many redundant
           strings in the binary */
        "Trade::SceneData::objectsInto(): field" << name << "not found", {});
    return objectsAsArray(fieldId);
}

void SceneData::parentsIntoInternal(const UnsignedInt fieldId, const std::size_t offset, const Containers::StridedArrayView1D<Int>& destination) const {
    /* fieldId, offset and destination.size() is assumed to be in bounds,
       checked by the callers */

    const SceneFieldData& field = _fields[fieldId];
    const Containers::StridedArrayView1D<const void> fieldData = fieldDataFieldViewInternal(field, offset, destination.size());
    const auto destination1i = Containers::arrayCast<2, Int>(destination);

    if(field._fieldType == SceneFieldType::Int)
        Utility::copy(Containers::arrayCast<const Int>(fieldData), destination);
    else if(field._fieldType == SceneFieldType::Short)
        Math::castInto(Containers::arrayCast<2, const Short>(fieldData, 1), destination1i);
    else if(field._fieldType == SceneFieldType::Byte)
        Math::castInto(Containers::arrayCast<2, const Byte>(fieldData, 1), destination1i);
    else if(field._fieldType == SceneFieldType::Long) {
        CORRADE_ASSERT(field._size <= 0xffffffffull, "Trade::SceneData::parentsInto(): parent indices for up to"  << field._size << "objects can't fit into a 32-bit type, access them directly via field() instead", );
        Math::castInto(Containers::arrayCast<2, const Long>(fieldData, 1), destination1i);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

void SceneData::parentsInto(const Containers::StridedArrayView1D<Int>& destination) const {
    const UnsignedInt fieldId = fieldFor(SceneField::Parent);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::parentsInto(): field not found", );
    CORRADE_ASSERT(destination.size() == _fields[fieldId]._size,
        "Trade::SceneData::parentsInto(): expected a view with" << _fields[fieldId]._size << "elements but got" << destination.size(), );
    parentsIntoInternal(fieldId, 0, destination);
}

std::size_t SceneData::parentsInto(const std::size_t offset, const Containers::StridedArrayView1D<Int>& destination) const {
    const UnsignedInt fieldId = fieldFor(SceneField::Parent);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::parentsInto(): field not found", {});
    CORRADE_ASSERT(offset <= _fields[fieldId]._size,
        "Trade::SceneData::parentsInto(): offset" << offset << "out of bounds for a field of size" << _fields[fieldId]._size, {});
    const std::size_t size = Math::min(destination.size(), std::size_t(_fields[fieldId]._size) - offset);
    parentsIntoInternal(fieldId, offset, destination.prefix(size));
    return size;
}

Containers::Array<Int> SceneData::parentsAsArray() const {
    const UnsignedInt fieldId = fieldFor(SceneField::Parent);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        /* Using the same message as in Into() to avoid too many redundant
           strings in the binary */
        "Trade::SceneData::parentsInto(): field not found", {});
    Containers::Array<Int> out{NoInit, std::size_t(_fields[fieldId]._size)};
    parentsIntoInternal(fieldId, 0, out);
    return out;
}

namespace {

template<class Source, class Destination> void convertTransformation(const Containers::StridedArrayView1D<const void>& source, const Containers::StridedArrayView1D<Destination>& destination) {
    CORRADE_INTERNAL_ASSERT(source.size() == destination.size());
    const auto sourceT = Containers::arrayCast<const Source>(source);
    for(std::size_t i = 0; i != sourceT.size(); ++i)
        destination[i] = Destination{sourceT[i].toMatrix()};
}

/** @todo these (or the float variants at least) should eventually be replaced
    with optimized batched APIs (applyTranslationsInto() updating just the
    last matrix column etc.) */

template<class Source, class Destination> void applyTranslation(const Containers::StridedArrayView1D<const void>& source, const Containers::StridedArrayView1D<Destination>& destination) {
    CORRADE_INTERNAL_ASSERT(source.size() == destination.size());
    const auto sourceT = Containers::arrayCast<const Source>(source);
    for(std::size_t i = 0; i != sourceT.size(); ++i)
        destination[i] = Destination::translation(Math::Vector<Destination::Size - 1, Float>{sourceT[i]})*destination[i];
}

template<class Source, class Destination> void applyRotation(const Containers::StridedArrayView1D<const void>& source, const Containers::StridedArrayView1D<Destination>& destination) {
    CORRADE_INTERNAL_ASSERT(source.size() == destination.size());
    const auto sourceT = Containers::arrayCast<const Source>(source);
    for(std::size_t i = 0; i != sourceT.size(); ++i)
        destination[i] = Destination{Math::Matrix<Destination::Size - 1, Float>{ sourceT[i].toMatrix()}}*destination[i];
}

template<class Source, class Destination> void applyScaling(const Containers::StridedArrayView1D<const void>& source, const Containers::StridedArrayView1D<Destination>& destination) {
    CORRADE_INTERNAL_ASSERT(source.size() == destination.size());
    const auto sourceT = Containers::arrayCast<const Source>(source);
    for(std::size_t i = 0; i != sourceT.size(); ++i)
        destination[i] = Destination::scaling(Math::Vector<Destination::Size - 1, Float>{sourceT[i]})*destination[i];
}

}

std::size_t SceneData::findTransformFields(UnsignedInt& transformationFieldId, UnsignedInt& translationFieldId, UnsignedInt& rotationFieldId, UnsignedInt& scalingFieldId, UnsignedInt* const fieldWithObjectMappingDestination) const {
    UnsignedInt fieldWithObjectMapping = ~UnsignedInt{};
    transformationFieldId = ~UnsignedInt{};
    translationFieldId = ~UnsignedInt{};
    rotationFieldId = ~UnsignedInt{};
    scalingFieldId = ~UnsignedInt{};
    for(std::size_t i = 0; i != _fields.size(); ++i) {
        /* If we find a transformation field, we don't need to look any
           further */
        if(_fields[i]._name == SceneField::Transformation) {
            fieldWithObjectMapping = transformationFieldId = i;
            break;
        } else if(_fields[i]._name == SceneField::Translation) {
            fieldWithObjectMapping = translationFieldId = i;
        } else if(_fields[i]._name == SceneField::Rotation) {
            fieldWithObjectMapping = rotationFieldId = i;
        } else if(_fields[i]._name == SceneField::Scaling) {
            fieldWithObjectMapping = scalingFieldId = i;
        }
    }

    if(fieldWithObjectMappingDestination)
        *fieldWithObjectMappingDestination = fieldWithObjectMapping;

    /* Assuming the caller fires an appropriate assertion */
    return fieldWithObjectMapping == ~UnsignedInt{} ?
        ~std::size_t{} : _fields[fieldWithObjectMapping]._size;
}

std::size_t SceneData::findTranslationRotationScalingFields(UnsignedInt& translationFieldId, UnsignedInt& rotationFieldId, UnsignedInt& scalingFieldId, UnsignedInt* const fieldWithObjectMappingDestination) const {
    UnsignedInt fieldWithObjectMapping = ~UnsignedInt{};
    translationFieldId = ~UnsignedInt{};
    rotationFieldId = ~UnsignedInt{};
    scalingFieldId = ~UnsignedInt{};
    for(std::size_t i = 0; i != _fields.size(); ++i) {
        if(_fields[i]._name == SceneField::Translation) {
            fieldWithObjectMapping = translationFieldId = i;
        } else if(_fields[i]._name == SceneField::Rotation) {
            fieldWithObjectMapping = rotationFieldId = i;
        } else if(_fields[i]._name == SceneField::Scaling) {
            fieldWithObjectMapping = scalingFieldId = i;
        }
    }

    if(fieldWithObjectMappingDestination)
        *fieldWithObjectMappingDestination = fieldWithObjectMapping;

    /* Assuming the caller fires an appropriate assertion */
    return fieldWithObjectMapping == ~UnsignedInt{} ?
        ~std::size_t{} : _fields[fieldWithObjectMapping]._size;
}

void SceneData::transformations2DIntoInternal(const UnsignedInt transformationFieldId, const UnsignedInt translationFieldId, const UnsignedInt rotationFieldId, const UnsignedInt scalingFieldId, std::size_t offset, const Containers::StridedArrayView1D<Matrix3>& destination) const {
    /* *FieldId, offset and destination.size() is assumed to be in bounds (or
       an invalid field ID), checked by the callers */

    /* If is2D() returned false as well, all *FieldId would be invalid, which
       the caller is assumed to check. */
    CORRADE_ASSERT(!is3D(), "Trade::SceneData::transformations2DInto(): scene has a 3D transformation type", );

    /** @todo apply scalings as well if dual complex? */

    /* Prefer the transformation field, if present */
    if(transformationFieldId != ~UnsignedInt{}) {
        const SceneFieldData& field = _fields[transformationFieldId];
        const Containers::StridedArrayView1D<const void> fieldData = fieldDataFieldViewInternal(field, offset, destination.size());
        const auto destination1f = Containers::arrayCast<2, Float>(destination);

        if(field._fieldType == SceneFieldType::Matrix3x3) {
            Utility::copy(Containers::arrayCast<const Matrix3>(fieldData), destination);
        } else if(field._fieldType == SceneFieldType::Matrix3x3d) {
            Math::castInto(Containers::arrayCast<2, const Double>(fieldData, 9), destination1f);
        } else if(field._fieldType == SceneFieldType::DualComplex) {
            convertTransformation<DualComplex>(fieldData, destination);
        } else if(field._fieldType == SceneFieldType::DualComplexd) {
            convertTransformation<DualComplexd>(fieldData, destination);
        } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */

    /* If not, combine from TRS components */
    } else if(translationFieldId != ~UnsignedInt{} || rotationFieldId != ~UnsignedInt{} || scalingFieldId != ~UnsignedInt{}) {
        /* First fill the destination with identity matrices */
        const Matrix3 identity[1]{Matrix3{Math::IdentityInit}};
        Utility::copy(Containers::stridedArrayView(identity).broadcasted<0>(destination.size()), destination);

        /* Apply scaling first, if present */
        if(scalingFieldId != ~UnsignedInt{}) {
            const SceneFieldData& field = _fields[scalingFieldId];
            const Containers::StridedArrayView1D<const void> fieldData = fieldDataFieldViewInternal(field, offset, destination.size());

            if(field._fieldType == SceneFieldType::Vector2) {
                applyScaling<Vector2>(fieldData, destination);
            } else if(field._fieldType == SceneFieldType::Vector2d) {
                applyScaling<Vector2d>(fieldData, destination);
            } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
        }

        /* Apply rotation second, if present */
        if(rotationFieldId != ~UnsignedInt{}) {
            const SceneFieldData& field = _fields[rotationFieldId];
            const Containers::StridedArrayView1D<const void> fieldData = fieldDataFieldViewInternal(field, offset, destination.size());

            if(field._fieldType == SceneFieldType::Complex) {
                applyRotation<Complex>(fieldData, destination);
            } else if(field._fieldType == SceneFieldType::Complexd) {
                applyRotation<Complexd>(fieldData, destination);
            } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
        }

        /* Apply translation last, if present */
        if(translationFieldId != ~UnsignedInt{}) {
            const SceneFieldData& field = _fields[translationFieldId];
            const Containers::StridedArrayView1D<const void> fieldData = fieldDataFieldViewInternal(field, offset, destination.size());

            if(field._fieldType == SceneFieldType::Vector2) {
                applyTranslation<Vector2>(fieldData, destination);
            } else if(field._fieldType == SceneFieldType::Vector2d) {
                applyTranslation<Vector2d>(fieldData, destination);
            } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
        }

    /* Checked in the caller */
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

void SceneData::transformations2DInto(const Containers::StridedArrayView1D<Matrix3>& destination) const {
    UnsignedInt transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId;
    #ifndef CORRADE_NO_ASSERT
    const std::size_t expectedSize =
    #endif
        findTransformFields(transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId);
    CORRADE_ASSERT(expectedSize != ~std::size_t{},
        "Trade::SceneData::transformations2DInto(): no transformation-related field found", );
    CORRADE_ASSERT(expectedSize == destination.size(),
        "Trade::SceneData::transformations2DInto(): expected a view with" << expectedSize << "elements but got" << destination.size(), );
    transformations2DIntoInternal(transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId, 0, destination);
}

std::size_t SceneData::transformations2DInto(const std::size_t offset, const Containers::StridedArrayView1D<Matrix3>& destination) const {
    UnsignedInt transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId;
    #ifndef CORRADE_NO_ASSERT
    const std::size_t expectedSize =
    #endif
        findTransformFields(transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId);
    CORRADE_ASSERT(expectedSize != ~std::size_t{},
        "Trade::SceneData::transformations2DInto(): no transformation-related field found", {});
    CORRADE_ASSERT(offset <= expectedSize,
        "Trade::SceneData::transformations2DInto(): offset" << offset << "out of bounds for a field of size" << expectedSize, {});
    const std::size_t size = Math::min(destination.size(), expectedSize - offset);
    transformations2DIntoInternal(transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId, offset, destination.prefix(size));
    return size;
}

Containers::Array<Matrix3> SceneData::transformations2DAsArray() const {
    UnsignedInt transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId;
    const std::size_t expectedSize = findTransformFields(transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId);
    CORRADE_ASSERT(expectedSize != ~std::size_t{},
        /* Using the same message as in Into() to avoid too many redundant
           strings in the binary */
        "Trade::SceneData::transformations2DInto(): no transformation-related field found", {});
    Containers::Array<Matrix3> out{NoInit, expectedSize};
    transformations2DIntoInternal(transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId, 0, out);
    return out;
}

void SceneData::translationsRotationsScalings2DIntoInternal(const UnsignedInt translationFieldId, const UnsignedInt rotationFieldId, const UnsignedInt scalingFieldId, const std::size_t offset, const Containers::StridedArrayView1D<Vector2>& translationDestination, const Containers::StridedArrayView1D<Complex>& rotationDestination, const Containers::StridedArrayView1D<Vector2>& scalingDestination) const {
    /* *FieldId, offset and *Destination.size() is assumed to be in bounds (or
       an invalid field ID), checked by the callers */

    /* If is2D() returned false as well, all *FieldId would be invalid, which
       the caller is assumed to check. */
    CORRADE_ASSERT(!is3D(), "Trade::SceneData::translationsRotationsScalings2DInto(): scene has a 3D transformation type", );

    /* Retrieve translation, if desired. If no field is present, output a zero
       vector for all objects. */
    if(translationDestination) {
        if(translationFieldId == ~UnsignedInt{}) {
            constexpr Vector2 identity[]{Vector2{0.0f}};
            Utility::copy(Containers::stridedArrayView(identity).broadcasted<0>(translationDestination.size()), translationDestination);
        } else {
            const SceneFieldData& field = _fields[translationFieldId];
            const Containers::StridedArrayView1D<const void> fieldData = fieldDataFieldViewInternal(field, offset, translationDestination.size());

            if(field._fieldType == SceneFieldType::Vector2) {
                Utility::copy(Containers::arrayCast<const Vector2>(fieldData), translationDestination);
            } else if(field._fieldType == SceneFieldType::Vector2d) {
                Math::castInto(Containers::arrayCast<2, const Double>(fieldData, 2), Containers::arrayCast<2, Float>(translationDestination));
            } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
        }
    }

    /* Retrieve rotation, if desired. If no field is present, output an
       identity rotation for all objects. */
    if(rotationDestination) {
        if(rotationFieldId == ~UnsignedInt{}) {
            constexpr Complex identity[]{Complex{Math::IdentityInit}};
            Utility::copy(Containers::stridedArrayView(identity).broadcasted<0>(rotationDestination.size()), rotationDestination);
        } else {
            const SceneFieldData& field = _fields[rotationFieldId];
            const Containers::StridedArrayView1D<const void> fieldData = fieldDataFieldViewInternal(field, offset, rotationDestination.size());

            if(field._fieldType == SceneFieldType::Complex) {
                Utility::copy(Containers::arrayCast<const Complex>(fieldData), rotationDestination);
            } else if(field._fieldType == SceneFieldType::Complexd) {
                Math::castInto(Containers::arrayCast<2, const Double>(fieldData, 2), Containers::arrayCast<2, Float>(rotationDestination));
            } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
        }
    }

    /* Retrieve scaling, if desired. If no field is present, output an identity
       scaling for all objects. */
    if(scalingDestination) {
        if(scalingFieldId == ~UnsignedInt{}) {
            constexpr Vector2 identity[]{Vector2{1.0f}};
            Utility::copy(Containers::stridedArrayView(identity).broadcasted<0>(scalingDestination.size()), scalingDestination);
        } else {
            const SceneFieldData& field = _fields[scalingFieldId];
            const Containers::StridedArrayView1D<const void> fieldData = fieldDataFieldViewInternal(field, offset, scalingDestination.size());

            if(field._fieldType == SceneFieldType::Vector2) {
                Utility::copy(Containers::arrayCast<const Vector2>(fieldData), scalingDestination);
            } else if(field._fieldType == SceneFieldType::Vector2d) {
                Math::castInto(Containers::arrayCast<2, const Double>(fieldData, 2), Containers::arrayCast<2, Float>(scalingDestination));
            } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
        }
    }
}

void SceneData::translationsRotationsScalings2DInto(const Containers::StridedArrayView1D<Vector2>& translationDestination, const Containers::StridedArrayView1D<Complex>& rotationDestination, const Containers::StridedArrayView1D<Vector2>& scalingDestination) const {
    UnsignedInt translationFieldId, rotationFieldId, scalingFieldId;
    #ifndef CORRADE_NO_ASSERT
    const std::size_t expectedSize =
    #endif
        findTranslationRotationScalingFields(translationFieldId, rotationFieldId, scalingFieldId);
    CORRADE_ASSERT(expectedSize != ~std::size_t{},
        "Trade::SceneData::translationsRotationsScalings2DInto(): no transformation-related field found", );
    CORRADE_ASSERT(!translationDestination || translationDestination.size() == expectedSize,
        "Trade::SceneData::translationsRotationsScalings2DInto(): expected translation destination view either empty or with" << expectedSize << "elements but got" << translationDestination.size(), );
    CORRADE_ASSERT(!rotationDestination || rotationDestination.size() == expectedSize,
        "Trade::SceneData::translationsRotationsScalings2DInto(): expected rotation destination view either empty or with" << expectedSize << "elements but got" << rotationDestination.size(), );
    CORRADE_ASSERT(!scalingDestination || scalingDestination.size() == expectedSize,
        "Trade::SceneData::translationsRotationsScalings2DInto(): expected scaling destination view either empty or with" << expectedSize << "elements but got" << scalingDestination.size(), );
    translationsRotationsScalings2DIntoInternal(translationFieldId, rotationFieldId, scalingFieldId, 0, translationDestination, rotationDestination, scalingDestination);
}

std::size_t SceneData::translationsRotationsScalings2DInto(const std::size_t offset, const Containers::StridedArrayView1D<Vector2>& translationDestination, const Containers::StridedArrayView1D<Complex>& rotationDestination, const Containers::StridedArrayView1D<Vector2>& scalingDestination) const {
    UnsignedInt translationFieldId, rotationFieldId, scalingFieldId;
    #ifndef CORRADE_NO_ASSERT
    const std::size_t expectedSize =
    #endif
        findTranslationRotationScalingFields(translationFieldId, rotationFieldId, scalingFieldId);
    CORRADE_ASSERT(expectedSize != ~std::size_t{},
        "Trade::SceneData::translationsRotationsScalings2DInto(): no transformation-related field found", {});
    CORRADE_ASSERT(offset <= expectedSize,
        "Trade::SceneData::translationsRotationsScalings2DInto(): offset" << offset << "out of bounds for a field of size" << expectedSize, {});
    CORRADE_ASSERT(!translationDestination != !rotationDestination || translationDestination.size() == rotationDestination.size(),
        "Trade::SceneData::translationsRotationsScalings2DInto(): translation and rotation destination views have different size," << translationDestination.size() << "vs" << rotationDestination.size(), {});
    CORRADE_ASSERT(!translationDestination != !scalingDestination || translationDestination.size() == scalingDestination.size(),
        "Trade::SceneData::translationsRotationsScalings2DInto(): translation and scaling destination views have different size," << translationDestination.size() << "vs" << scalingDestination.size(), {});
    CORRADE_ASSERT(!rotationDestination != !scalingDestination || rotationDestination.size() == scalingDestination.size(),
        "Trade::SceneData::translationsRotationsScalings2DInto(): rotation and scaling destination views have different size," << rotationDestination.size() << "vs" << scalingDestination.size(), {});
    const std::size_t size = Math::min(Math::max({translationDestination.size(), rotationDestination.size(), scalingDestination.size()}), expectedSize - offset);
    translationsRotationsScalings2DIntoInternal(translationFieldId, rotationFieldId, scalingFieldId, offset,
        translationDestination ? translationDestination.prefix(size) : nullptr,
        rotationDestination ? rotationDestination.prefix(size) : nullptr,
        scalingDestination ? scalingDestination.prefix(size) : nullptr);
    return size;
}

Containers::Array<Containers::Triple<Vector2, Complex, Vector2>> SceneData::translationsRotationsScalings2DAsArray() const {
    UnsignedInt translationFieldId, rotationFieldId, scalingFieldId;
    const std::size_t expectedSize = findTranslationRotationScalingFields(translationFieldId, rotationFieldId, scalingFieldId);
    CORRADE_ASSERT(expectedSize != ~std::size_t{},
        /* Using the same message as in Into() to avoid too many redundant
           strings in the binary */
        "Trade::SceneData::translationsRotationsScalings2DInto(): no transformation-related field found", {});
    Containers::Array<Containers::Triple<Vector2, Complex, Vector2>> out{NoInit, expectedSize};
    /** @todo use slicing once Triple exposes members somehow */
    Containers::StridedArrayView1D<Vector2> translationsOut{out, reinterpret_cast<Vector2*>(out.data()), out.size(), sizeof(decltype(out)::Type)};
    Containers::StridedArrayView1D<Complex> rotationsOut{out, reinterpret_cast<Complex*>(reinterpret_cast<char*>(out.data()) + sizeof(Vector2)), out.size(), sizeof(decltype(out)::Type)};
    Containers::StridedArrayView1D<Vector2> scalingsOut{out, reinterpret_cast<Vector2*>(reinterpret_cast<char*>(out.data()) + sizeof(Vector2) + sizeof(Complex)), out.size(), sizeof(decltype(out)::Type)};
    translationsRotationsScalings2DIntoInternal(translationFieldId, rotationFieldId, scalingFieldId, 0, translationsOut, rotationsOut, scalingsOut);
    return out;
}

void SceneData::transformations3DIntoInternal(const UnsignedInt transformationFieldId, const UnsignedInt translationFieldId, const UnsignedInt rotationFieldId, const UnsignedInt scalingFieldId, const std::size_t offset, const Containers::StridedArrayView1D<Matrix4>& destination) const {
    /* *FieldId, offset and destination.size() is assumed to be in bounds (or
       an invalid field ID), checked by the callers */

    /* If is3D() returned false as well, all *FieldId would be invalid, which
       the caller is assumed to check. */
    CORRADE_ASSERT(!is2D(), "Trade::SceneData::transformations3DInto(): scene has a 2D transformation type", );

    /** @todo apply scalings as well if dual quat? */

    /* Prefer the transformation field, if present */
    if(transformationFieldId != ~UnsignedInt{}) {
        const SceneFieldData& field = _fields[transformationFieldId];
        const Containers::StridedArrayView1D<const void> fieldData = fieldDataFieldViewInternal(field, offset, destination.size());
        const auto destination1f = Containers::arrayCast<2, Float>(destination);

        if(field._fieldType == SceneFieldType::Matrix4x4) {
            Utility::copy(Containers::arrayCast<const Matrix4>(fieldData), destination);
        } else if(field._fieldType == SceneFieldType::Matrix4x4d) {
            Math::castInto(Containers::arrayCast<2, const Double>(fieldData, 16), destination1f);
        } else if(field._fieldType == SceneFieldType::DualQuaternion) {
            convertTransformation<DualQuaternion>(fieldData, destination);
        } else if(field._fieldType == SceneFieldType::DualQuaterniond) {
            convertTransformation<DualQuaterniond>(fieldData, destination);
        } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */

    /* If not, combine from TRS components */
    } else if(translationFieldId != ~UnsignedInt{} || rotationFieldId != ~UnsignedInt{} || scalingFieldId != ~UnsignedInt{}) {
        /* First fill the destination with identity matrices */
        const Matrix4 identity[1]{Matrix4{Math::IdentityInit}};
        Utility::copy(Containers::stridedArrayView(identity).broadcasted<0>(destination.size()), destination);

        /* Apply scaling first, if present */
        if(scalingFieldId != ~UnsignedInt{}) {
            const SceneFieldData& field = _fields[scalingFieldId];
            const Containers::StridedArrayView1D<const void> fieldData = fieldDataFieldViewInternal(field, offset, destination.size());

            if(field._fieldType == SceneFieldType::Vector3) {
                applyScaling<Vector3>(fieldData, destination);
            } else if(field._fieldType == SceneFieldType::Vector3d) {
                applyScaling<Vector3d>(fieldData, destination);
            } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
        }

        /* Apply rotation second, if present */
        if(rotationFieldId != ~UnsignedInt{}) {
            const SceneFieldData& field = _fields[rotationFieldId];
            const Containers::StridedArrayView1D<const void> fieldData = fieldDataFieldViewInternal(field, offset, destination.size());

            if(field._fieldType == SceneFieldType::Quaternion) {
                applyRotation<Quaternion>(fieldData, destination);
            } else if(field._fieldType == SceneFieldType::Quaterniond) {
                applyRotation<Quaterniond>(fieldData, destination);
            } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
        }

        /* Apply translation last, if present */
        if(translationFieldId != ~UnsignedInt{}) {
            const SceneFieldData& field = _fields[translationFieldId];
            const Containers::StridedArrayView1D<const void> fieldData = fieldDataFieldViewInternal(field, offset, destination.size());

            if(field._fieldType == SceneFieldType::Vector3) {
                applyTranslation<Vector3>(fieldData, destination);
            } else if(field._fieldType == SceneFieldType::Vector3d) {
                applyTranslation<Vector3d>(fieldData, destination);
            } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
        }

    /* Checked in the caller */
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

void SceneData::transformations3DInto(const Containers::StridedArrayView1D<Matrix4>& destination) const {
    UnsignedInt transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId;
    #ifndef CORRADE_NO_ASSERT
    const std::size_t expectedSize =
    #endif
        findTransformFields(transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId);
    CORRADE_ASSERT(expectedSize != ~std::size_t{},
        "Trade::SceneData::transformations3DInto(): no transformation-related field found", );
    CORRADE_ASSERT(expectedSize == destination.size(),
        "Trade::SceneData::transformations3DInto(): expected a view with" << expectedSize << "elements but got" << destination.size(), );
    transformations3DIntoInternal(transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId, 0, destination);
}

std::size_t SceneData::transformations3DInto(const std::size_t offset, const Containers::StridedArrayView1D<Matrix4>& destination) const {
    UnsignedInt transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId;
    #ifndef CORRADE_NO_ASSERT
    const std::size_t expectedSize =
    #endif
        findTransformFields(transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId);
    CORRADE_ASSERT(expectedSize != ~std::size_t{},
        "Trade::SceneData::transformations3DInto(): no transformation-related field found", {});
    CORRADE_ASSERT(offset <= expectedSize,
        "Trade::SceneData::transformations3DInto(): offset" << offset << "out of bounds for a field of size" << expectedSize, {});
    const std::size_t size = Math::min(destination.size(), expectedSize - offset);
    transformations3DIntoInternal(transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId, offset, destination.prefix(size));
    return size;
}

Containers::Array<Matrix4> SceneData::transformations3DAsArray() const {
    UnsignedInt transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId;
    const std::size_t expectedSize = findTransformFields(transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId);
    CORRADE_ASSERT(expectedSize != ~std::size_t{},
        /* Using the same message as in Into() to avoid too many redundant
           strings in the binary */
        "Trade::SceneData::transformations3DInto(): no transformation-related field found", {});
    Containers::Array<Matrix4> out{NoInit, expectedSize};
    transformations3DIntoInternal(transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId, 0, out);
    return out;
}

void SceneData::translationsRotationsScalings3DIntoInternal(const UnsignedInt translationFieldId, const UnsignedInt rotationFieldId, const UnsignedInt scalingFieldId, const std::size_t offset, const Containers::StridedArrayView1D<Vector3>& translationDestination, const Containers::StridedArrayView1D<Quaternion>& rotationDestination, const Containers::StridedArrayView1D<Vector3>& scalingDestination) const {
    /* *FieldId, offset and *Destination.size() is assumed to be in bounds (or
       an invalid field ID), checked by the callers */

    /* If is3D() returned false as well, all *FieldId would be invalid, which
       the caller is assumed to check. */
    CORRADE_ASSERT(!is2D(), "Trade::SceneData::translationsRotationsScalings3DInto(): scene has a 2D transformation type", );

    /* Retrieve translation, if desired. If no field is present, output a zero
       vector for all objects. */
    if(translationDestination) {
        if(translationFieldId == ~UnsignedInt{}) {
            constexpr Vector3 identity[]{Vector3{0.0f}};
            Utility::copy(Containers::stridedArrayView(identity).broadcasted<0>(translationDestination.size()), translationDestination);
        } else {
            const SceneFieldData& field = _fields[translationFieldId];
            const Containers::StridedArrayView1D<const void> fieldData = fieldDataFieldViewInternal(field, offset, translationDestination.size());

            if(field._fieldType == SceneFieldType::Vector3) {
                Utility::copy(Containers::arrayCast<const Vector3>(fieldData), translationDestination);
            } else if(field._fieldType == SceneFieldType::Vector3d) {
                Math::castInto(Containers::arrayCast<2, const Double>(fieldData, 3), Containers::arrayCast<2, Float>(translationDestination));
            } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
        }
    }

    /* Retrieve rotation, if desired. If no field is present, output an
       identity rotation for all objects. */
    if(rotationDestination) {
        if(rotationFieldId == ~UnsignedInt{}) {
            constexpr Quaternion identity[]{Quaternion{Math::IdentityInit}};
            Utility::copy(Containers::stridedArrayView(identity).broadcasted<0>(rotationDestination.size()), rotationDestination);
        } else {
            const SceneFieldData& field = _fields[rotationFieldId];
            const Containers::StridedArrayView1D<const void> fieldData = fieldDataFieldViewInternal(field, offset, rotationDestination.size());

            if(field._fieldType == SceneFieldType::Quaternion) {
                Utility::copy(Containers::arrayCast<const Quaternion>(fieldData), rotationDestination);
            } else if(field._fieldType == SceneFieldType::Quaterniond) {
                Math::castInto(Containers::arrayCast<2, const Double>(fieldData, 4), Containers::arrayCast<2, Float>(rotationDestination));
            } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
        }
    }

    /* Retrieve scaling, if desired. If no field is present, output an identity
       scaling for all objects. */
    if(scalingDestination) {
        if(scalingFieldId == ~UnsignedInt{}) {
            constexpr Vector3 identity[]{Vector3{1.0f}};
            Utility::copy(Containers::stridedArrayView(identity).broadcasted<0>(scalingDestination.size()), scalingDestination);
        } else {
            const SceneFieldData& field = _fields[scalingFieldId];
            const Containers::StridedArrayView1D<const void> fieldData = fieldDataFieldViewInternal(field, offset, scalingDestination.size());

            if(field._fieldType == SceneFieldType::Vector3) {
                Utility::copy(Containers::arrayCast<const Vector3>(fieldData), scalingDestination);
            } else if(field._fieldType == SceneFieldType::Vector3d) {
                Math::castInto(Containers::arrayCast<2, const Double>(fieldData, 3), Containers::arrayCast<2, Float>(scalingDestination));
            } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
        }
    }
}

void SceneData::translationsRotationsScalings3DInto(const Containers::StridedArrayView1D<Vector3>& translationDestination, const Containers::StridedArrayView1D<Quaternion>& rotationDestination, const Containers::StridedArrayView1D<Vector3>& scalingDestination) const {
    UnsignedInt translationFieldId, rotationFieldId, scalingFieldId;
    #ifndef CORRADE_NO_ASSERT
    const std::size_t expectedSize =
    #endif
        findTranslationRotationScalingFields(translationFieldId, rotationFieldId, scalingFieldId);
    CORRADE_ASSERT(expectedSize != ~std::size_t{},
        "Trade::SceneData::translationsRotationsScalings3DInto(): no transformation-related field found", );
    CORRADE_ASSERT(!translationDestination || translationDestination.size() == expectedSize,
        "Trade::SceneData::translationsRotationsScalings3DInto(): expected translation destination view either empty or with" << expectedSize << "elements but got" << translationDestination.size(), );
    CORRADE_ASSERT(!rotationDestination || rotationDestination.size() == expectedSize,
        "Trade::SceneData::translationsRotationsScalings3DInto(): expected rotation destination view either empty or with" << expectedSize << "elements but got" << rotationDestination.size(), );
    CORRADE_ASSERT(!scalingDestination || scalingDestination.size() == expectedSize,
        "Trade::SceneData::translationsRotationsScalings3DInto(): expected scaling destination view either empty or with" << expectedSize << "elements but got" << scalingDestination.size(), );
    translationsRotationsScalings3DIntoInternal(translationFieldId, rotationFieldId, scalingFieldId, 0, translationDestination, rotationDestination, scalingDestination);
}

std::size_t SceneData::translationsRotationsScalings3DInto(const std::size_t offset, const Containers::StridedArrayView1D<Vector3>& translationDestination, const Containers::StridedArrayView1D<Quaternion>& rotationDestination, const Containers::StridedArrayView1D<Vector3>& scalingDestination) const {
    UnsignedInt translationFieldId, rotationFieldId, scalingFieldId;
    #ifndef CORRADE_NO_ASSERT
    const std::size_t expectedSize =
    #endif
        findTranslationRotationScalingFields(translationFieldId, rotationFieldId, scalingFieldId);
    CORRADE_ASSERT(expectedSize != ~std::size_t{},
        "Trade::SceneData::translationsRotationsScalings3DInto(): no transformation-related field found", {});
    CORRADE_ASSERT(offset <= expectedSize,
        "Trade::SceneData::translationsRotationsScalings3DInto(): offset" << offset << "out of bounds for a field of size" << expectedSize, {});
    CORRADE_ASSERT(!translationDestination != !rotationDestination || translationDestination.size() == rotationDestination.size(),
        "Trade::SceneData::translationsRotationsScalings3DInto(): translation and rotation destination views have different size," << translationDestination.size() << "vs" << rotationDestination.size(), {});
    CORRADE_ASSERT(!translationDestination != !scalingDestination || translationDestination.size() == scalingDestination.size(),
        "Trade::SceneData::translationsRotationsScalings3DInto(): translation and scaling destination views have different size," << translationDestination.size() << "vs" << scalingDestination.size(), {});
    CORRADE_ASSERT(!rotationDestination != !scalingDestination || rotationDestination.size() == scalingDestination.size(),
        "Trade::SceneData::translationsRotationsScalings3DInto(): rotation and scaling destination views have different size," << rotationDestination.size() << "vs" << scalingDestination.size(), {});
    const std::size_t size = Math::min(Math::max({translationDestination.size(), rotationDestination.size(), scalingDestination.size()}), expectedSize - offset);
    translationsRotationsScalings3DIntoInternal(translationFieldId, rotationFieldId, scalingFieldId, offset,
        translationDestination ? translationDestination.prefix(size) : nullptr,
        rotationDestination ? rotationDestination.prefix(size) : nullptr,
        scalingDestination ? scalingDestination.prefix(size) : nullptr);
    return size;
}

Containers::Array<Containers::Triple<Vector3, Quaternion, Vector3>> SceneData::translationsRotationsScalings3DAsArray() const {
    UnsignedInt translationFieldId, rotationFieldId, scalingFieldId;
    const std::size_t expectedSize = findTranslationRotationScalingFields(translationFieldId, rotationFieldId, scalingFieldId);
    CORRADE_ASSERT(expectedSize != ~std::size_t{},
        /* Using the same message as in Into() to avoid too many redundant
           strings in the binary */
        "Trade::SceneData::translationsRotationsScalings3DInto(): no transformation-related field found", {});
    Containers::Array<Containers::Triple<Vector3, Quaternion, Vector3>> out{NoInit, expectedSize};
    /** @todo use slicing once Triple exposes members somehow */
    Containers::StridedArrayView1D<Vector3> translationsOut{out, reinterpret_cast<Vector3*>(out.data()), out.size(), sizeof(decltype(out)::Type)};
    Containers::StridedArrayView1D<Quaternion> rotationsOut{out, reinterpret_cast<Quaternion*>(reinterpret_cast<char*>(out.data()) + sizeof(Vector3)), out.size(), sizeof(decltype(out)::Type)};
    Containers::StridedArrayView1D<Vector3> scalingsOut{out, reinterpret_cast<Vector3*>(reinterpret_cast<char*>(out.data()) + sizeof(Vector3) + sizeof(Quaternion)), out.size(), sizeof(decltype(out)::Type)};
    translationsRotationsScalings3DIntoInternal(translationFieldId, rotationFieldId, scalingFieldId, 0, translationsOut, rotationsOut, scalingsOut);
    return out;
}

void SceneData::unsignedIndexFieldIntoInternal(const UnsignedInt fieldId, const std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& destination) const {
    /* fieldId, offset and destination.size() is assumed to be in bounds,
       checked by the callers */

    const SceneFieldData& field = _fields[fieldId];
    const Containers::StridedArrayView1D<const void> fieldData = fieldDataFieldViewInternal(field, offset, destination.size());
    const auto destination1ui = Containers::arrayCast<2, UnsignedInt>(destination);

    if(field._fieldType == SceneFieldType::UnsignedInt)
        Utility::copy(Containers::arrayCast<const UnsignedInt>(fieldData), destination);
    else if(field._fieldType == SceneFieldType::UnsignedShort)
        Math::castInto(Containers::arrayCast<2, const UnsignedShort>(fieldData, 1), destination1ui);
    else if(field._fieldType == SceneFieldType::UnsignedByte)
        Math::castInto(Containers::arrayCast<2, const UnsignedByte>(fieldData, 1), destination1ui);
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

void SceneData::indexFieldIntoInternal(const UnsignedInt fieldId, const std::size_t offset, const Containers::StridedArrayView1D<Int>& destination) const {
    /* fieldId, offset and destination.size() is assumed to be in bounds,
       checked by the callers */

    const SceneFieldData& field = _fields[fieldId];
    const Containers::StridedArrayView1D<const void> fieldData = fieldDataFieldViewInternal(field, offset, destination.size());
    const auto destination1ui = Containers::arrayCast<2, Int>(destination);

    if(field._fieldType == SceneFieldType::Int)
        Utility::copy(Containers::arrayCast<const Int>(fieldData), destination);
    else if(field._fieldType == SceneFieldType::Short)
        Math::castInto(Containers::arrayCast<2, const Short>(fieldData, 1), destination1ui);
    else if(field._fieldType == SceneFieldType::Byte)
        Math::castInto(Containers::arrayCast<2, const Byte>(fieldData, 1), destination1ui);
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

Containers::Array<UnsignedInt> SceneData::unsignedIndexFieldAsArrayInternal(const UnsignedInt fieldId) const {
    Containers::Array<UnsignedInt> out{NoInit, std::size_t(_fields[fieldId]._size)};
    unsignedIndexFieldIntoInternal(fieldId, 0, out);
    return out;
}

void SceneData::meshesMaterialsIntoInternal(const UnsignedInt fieldId, const std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& meshDestination, const Containers::StridedArrayView1D<Int>& meshMaterialDestination) const {
    /* fieldId, offset, meshDestination.size() and
       meshMaterialDestination.size() is assumed to be in bounds, checked by
       the callers */

    if(meshDestination)
        unsignedIndexFieldIntoInternal(fieldId, offset, meshDestination);

    /* Copy also the material, if desired. If no such field is present, output
       -1 for all meshes. */
    if(meshMaterialDestination) {
        const UnsignedInt materialFieldId = fieldFor(SceneField::MeshMaterial);
        if(materialFieldId == ~UnsignedInt{}) {
            constexpr Int invalid[]{-1};
            Utility::copy(Containers::stridedArrayView(invalid).broadcasted<0>(meshMaterialDestination.size()), meshMaterialDestination);
        } else indexFieldIntoInternal(materialFieldId, offset, meshMaterialDestination);
    }
}

void SceneData::meshesMaterialsInto(const Containers::StridedArrayView1D<UnsignedInt>& meshDestination, const Containers::StridedArrayView1D<Int>& meshMaterialDestination) const {
    const UnsignedInt fieldId = fieldFor(SceneField::Mesh);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::meshesMaterialsInto(): field" << SceneField::Mesh << "not found", );
    CORRADE_ASSERT(!meshDestination || meshDestination.size() == _fields[fieldId]._size,
        "Trade::SceneData::meshesMaterialsInto(): expected mesh destination view either empty or with" << _fields[fieldId]._size << "elements but got" << meshDestination.size(), );
    CORRADE_ASSERT(!meshMaterialDestination || meshMaterialDestination.size() == _fields[fieldId]._size,
        "Trade::SceneData::meshesMaterialsInto(): expected mesh material destination view either empty or with" << _fields[fieldId]._size << "elements but got" << meshMaterialDestination.size(), );
    meshesMaterialsIntoInternal(fieldId, 0, meshDestination, meshMaterialDestination);
}

std::size_t SceneData::meshesMaterialsInto(const std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& meshDestination, const Containers::StridedArrayView1D<Int>& meshMaterialDestination) const {
    const UnsignedInt fieldId = fieldFor(SceneField::Mesh);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::meshesMaterialsInto(): field" << SceneField::Mesh << "not found", {});
    CORRADE_ASSERT(offset <= _fields[fieldId]._size,
        "Trade::SceneData::meshesMaterialsInto(): offset" << offset << "out of bounds for a field of size" << _fields[fieldId]._size, {});
    CORRADE_ASSERT(!meshDestination != !meshMaterialDestination || meshMaterialDestination.size() == meshDestination.size(),
        "Trade::SceneData::meshesMaterialsInto(): mesh and mesh material destination views have different size," << meshDestination.size() << "vs" << meshMaterialDestination.size(), {});
    const std::size_t size = Math::min(Math::max(meshDestination.size(), meshMaterialDestination.size()), std::size_t(_fields[fieldId]._size) - offset);
    meshesMaterialsIntoInternal(fieldId, offset,
        meshDestination ? meshDestination.prefix(size) : nullptr,
        meshMaterialDestination ? meshMaterialDestination.prefix(size) : nullptr);
    return size;
}

Containers::Array<Containers::Pair<UnsignedInt, Int>> SceneData::meshesMaterialsAsArray() const {
    const UnsignedInt fieldId = fieldFor(SceneField::Mesh);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        /* Using the same message as in Into() to avoid too many redundant
           strings in the binary */
        "Trade::SceneData::meshesMaterialsInto(): field" << SceneField::Mesh << "not found", {});
    Containers::Array<Containers::Pair<UnsignedInt, Int>> out{NoInit, std::size_t(_fields[fieldId]._size)};
    /** @todo use slicing once Pair exposes members somehow */
    Containers::StridedArrayView1D<UnsignedInt> meshesOut{out, reinterpret_cast<UnsignedInt*>(out.data()), out.size(), sizeof(decltype(out)::Type)};
    Containers::StridedArrayView1D<Int> meshMaterialsOut{out, reinterpret_cast<Int*>(reinterpret_cast<char*>(out.data()) + sizeof(UnsignedInt)), out.size(), sizeof(decltype(out)::Type)};
    meshesMaterialsIntoInternal(fieldId, 0, meshesOut, meshMaterialsOut);
    return out;
}

void SceneData::lightsInto(const Containers::StridedArrayView1D<UnsignedInt>& destination) const {
    const UnsignedInt fieldId = fieldFor(SceneField::Light);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::lightsInto(): field not found", );
    CORRADE_ASSERT(destination.size() == _fields[fieldId]._size,
        "Trade::SceneData::lightsInto(): expected a view with" << _fields[fieldId]._size << "elements but got" << destination.size(), );
    unsignedIndexFieldIntoInternal(fieldId, 0, destination);
}

std::size_t SceneData::lightsInto(const std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& destination) const {
    const UnsignedInt fieldId = fieldFor(SceneField::Light);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::lightsInto(): field not found", {});
    CORRADE_ASSERT(offset <= _fields[fieldId]._size,
        "Trade::SceneData::lightsInto(): offset" << offset << "out of bounds for a field of size" << _fields[fieldId]._size, {});
    const std::size_t size = Math::min(destination.size(), std::size_t(_fields[fieldId]._size) - offset);
    unsignedIndexFieldIntoInternal(fieldId, offset, destination.prefix(size));
    return size;
}

Containers::Array<UnsignedInt> SceneData::lightsAsArray() const {
    const UnsignedInt fieldId = fieldFor(SceneField::Light);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        /* Using the same message as in Into() to avoid too many redundant
           strings in the binary */
        "Trade::SceneData::lightsInto(): field not found", {});
    return unsignedIndexFieldAsArrayInternal(fieldId);
}

void SceneData::camerasInto(const Containers::StridedArrayView1D<UnsignedInt>& destination) const {
    const UnsignedInt fieldId = fieldFor(SceneField::Camera);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::camerasInto(): field not found", );
    CORRADE_ASSERT(destination.size() == _fields[fieldId]._size,
        "Trade::SceneData::camerasInto(): expected a view with" << _fields[fieldId]._size << "elements but got" << destination.size(), );
    unsignedIndexFieldIntoInternal(fieldId, 0, destination);
}

std::size_t SceneData::camerasInto(const std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& destination) const {
    const UnsignedInt fieldId = fieldFor(SceneField::Camera);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::camerasInto(): field not found", {});
    CORRADE_ASSERT(offset <= _fields[fieldId]._size,
        "Trade::SceneData::camerasInto(): offset" << offset << "out of bounds for a field of size" << _fields[fieldId]._size, {});
    const std::size_t size = Math::min(destination.size(), std::size_t(_fields[fieldId]._size) - offset);
    unsignedIndexFieldIntoInternal(fieldId, offset, destination.prefix(size));
    return size;
}

Containers::Array<UnsignedInt> SceneData::camerasAsArray() const {
    const UnsignedInt fieldId = fieldFor(SceneField::Camera);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        /* Using the same message as in Into() to avoid too many redundant
           strings in the binary */
        "Trade::SceneData::camerasInto(): field not found", {});
    return unsignedIndexFieldAsArrayInternal(fieldId);
}

void SceneData::skinsInto(const Containers::StridedArrayView1D<UnsignedInt>& destination) const {
    const UnsignedInt fieldId = fieldFor(SceneField::Skin);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::skinsInto(): field not found", );
    CORRADE_ASSERT(destination.size() == _fields[fieldId]._size,
        "Trade::SceneData::skinsInto(): expected a view with" << _fields[fieldId]._size << "elements but got" << destination.size(), );
    unsignedIndexFieldIntoInternal(fieldId, 0, destination);
}

std::size_t SceneData::skinsInto(const std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& destination) const {
    const UnsignedInt fieldId = fieldFor(SceneField::Skin);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::skinsInto(): field not found", {});
    CORRADE_ASSERT(offset <= _fields[fieldId]._size,
        "Trade::SceneData::skinsInto(): offset" << offset << "out of bounds for a field of size" << _fields[fieldId]._size, {});
    const std::size_t size = Math::min(destination.size(), std::size_t(_fields[fieldId]._size) - offset);
    unsignedIndexFieldIntoInternal(fieldId, offset, destination.prefix(size));
    return size;
}

Containers::Array<UnsignedInt> SceneData::skinsAsArray() const {
    const UnsignedInt fieldId = fieldFor(SceneField::Skin);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        /* Using the same message as in Into() to avoid too many redundant
           strings in the binary */
        "Trade::SceneData::skinsInto(): field not found", {});
    return unsignedIndexFieldAsArrayInternal(fieldId);
}

void SceneData::importerStateIntoInternal(const UnsignedInt fieldId, const std::size_t offset, const Containers::StridedArrayView1D<const void*>& destination) const {
    /* fieldId, offset and destination.size() is assumed to be in bounds,
       checked by the callers */

    const SceneFieldData& field = _fields[fieldId];
    CORRADE_INTERNAL_ASSERT(field._fieldType == SceneFieldType::Pointer ||
                            field._fieldType == SceneFieldType::MutablePointer);
    Utility::copy(Containers::arrayCast<const void* const>(fieldDataFieldViewInternal(field, offset, destination.size())), destination);
}

void SceneData::importerStateInto(const Containers::StridedArrayView1D<const void*>& destination) const {
    const UnsignedInt fieldId = fieldFor(SceneField::ImporterState);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::importerStateInto(): field not found", );
    CORRADE_ASSERT(destination.size() == _fields[fieldId]._size,
        "Trade::SceneData::importerStateInto(): expected a view with" << _fields[fieldId]._size << "elements but got" << destination.size(), );
    importerStateIntoInternal(fieldId, 0, destination);
}

std::size_t SceneData::importerStateInto(const std::size_t offset, const Containers::StridedArrayView1D<const void*>& destination) const {
    const UnsignedInt fieldId = fieldFor(SceneField::ImporterState);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::importerStateInto(): field not found", {});
    CORRADE_ASSERT(offset <= _fields[fieldId]._size,
        "Trade::SceneData::importerStateInto(): offset" << offset << "out of bounds for a field of size" << _fields[fieldId]._size, {});
    const std::size_t size = Math::min(destination.size(), std::size_t(_fields[fieldId]._size) - offset);
    importerStateIntoInternal(fieldId, offset, destination.prefix(size));
    return size;
}

Containers::Array<const void*> SceneData::importerStateAsArray() const {
    const UnsignedInt fieldId = fieldFor(SceneField::ImporterState);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        /* Using the same message as in Into() to avoid too many redundant
           strings in the binary */
        "Trade::SceneData::importerStateInto(): field not found", {});
    Containers::Array<const void*> out{NoInit, std::size_t(_fields[fieldId]._size)};
    importerStateIntoInternal(fieldId, 0, out);
    return out;
}

namespace {

template<class T> std::size_t findObject(const Containers::StridedArrayView1D<const void>& objects, const UnsignedInt object) {
    const Containers::StridedArrayView1D<const T> objectsT = Containers::arrayCast<const T>(objects);
    const std::size_t max = objectsT.size();
    /** @todo implement something faster than O(n) when field-specific flags
        can annotate how the object mapping is done */
    for(std::size_t i = 0; i != max; ++i)
        if(objectsT[i] == object) return i;
    return max;
}

}

std::size_t SceneData::fieldFor(const SceneFieldData& field, const std::size_t offset, const UnsignedInt object) const {
    const Containers::StridedArrayView1D<const void> objects = fieldDataObjectViewInternal(field, offset, field._size - offset);
    if(field._objectType == SceneObjectType::UnsignedInt)
        return offset + findObject<UnsignedInt>(objects, object);
    else if(field._objectType == SceneObjectType::UnsignedShort)
        return offset + findObject<UnsignedShort>(objects, object);
    else if(field._objectType == SceneObjectType::UnsignedByte)
        return offset + findObject<UnsignedByte>(objects, object);
    else if(field._objectType == SceneObjectType::UnsignedLong)
        return offset + findObject<UnsignedLong>(objects, object);
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

Containers::Optional<Int> SceneData::parentFor(const UnsignedInt object) const {
    CORRADE_ASSERT(object < _objectCount,
        "Trade::SceneData::parentFor(): object" << object << "out of bounds for" << _objectCount << "objects", {});

    const UnsignedInt fieldId = fieldFor(SceneField::Parent);
    if(fieldId == ~UnsignedInt{}) return {};

    const SceneFieldData& field = _fields[fieldId];
    const std::size_t offset = fieldFor(field, 0, object);
    if(offset == field._size) return {};

    Int index[1];
    indexFieldIntoInternal(fieldId, offset, index);
    if(*index == -1) return -1;

    UnsignedInt parent[1];
    objectsIntoInternal(fieldId, *index, parent);
    return Int(*parent);
}

Containers::Array<UnsignedInt> SceneData::childrenFor(const Int object) const {
    CORRADE_ASSERT(object >= -1 && object < Long(_objectCount),
        "Trade::SceneData::childrenFor(): object" << object << "out of bounds for" << _objectCount << "objects", {});

    const UnsignedInt parentFieldId = fieldFor(SceneField::Parent);
    if(parentFieldId == ~UnsignedInt{}) return {};

    const SceneFieldData& parentField = _fields[parentFieldId];

    /* Figure out the parent object index to look for or -1 if we want
       top-level objects */
    Int parentIndexToLookFor;
    if(object == -1) parentIndexToLookFor = -1;
    else {
        const std::size_t parentObjectIndex = fieldFor(parentField, 0, object);
        if(parentObjectIndex == parentField._size) return {};
        parentIndexToLookFor = parentObjectIndex;
    }

    /* Collect IDs of all objects that reference this index */
    Containers::Array<UnsignedInt> out;
    for(std::size_t offset = 0; offset != parentField.size(); ++offset) {
        Int parentIndex[1];
        parentsIntoInternal(parentFieldId, offset, parentIndex);
        if(*parentIndex == parentIndexToLookFor) {
            UnsignedInt child[1];
            /** @todo bleh slow, use the children <-> parent field proxying
                when implemented */
            objectsIntoInternal(parentFieldId, offset, child);
            arrayAppend(out, *child);
        }
    }

    return out;
}

Containers::Optional<Matrix3> SceneData::transformation2DFor(const UnsignedInt object) const {
    CORRADE_ASSERT(object < _objectCount,
        "Trade::SceneData::transformation2DFor(): object" << object << "out of bounds for" << _objectCount << "objects", {});

    UnsignedInt transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId, fieldWithObjectMapping;
    if(findTransformFields(transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId, &fieldWithObjectMapping) == ~std::size_t{}) return {};

    /* If is2D() returned false as well, all *FieldId would be invalid, which
       is handled above. */
    CORRADE_ASSERT(!is3D(), "Trade::SceneData::transformation2DFor(): scene has a 3D transformation type", {});

    const std::size_t offset = fieldFor(_fields[fieldWithObjectMapping], 0, object);
    if(offset == _fields[fieldWithObjectMapping]._size) return {};

    Matrix3 transformation[1];
    transformations2DIntoInternal(transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId, offset, transformation);
    return *transformation;
}

Containers::Optional<Containers::Triple<Vector2, Complex, Vector2>> SceneData::translationRotationScaling2DFor(const UnsignedInt object) const {
    CORRADE_ASSERT(object < _objectCount,
        "Trade::SceneData::translationRotationScaling2DFor(): object" << object << "out of bounds for" << _objectCount << "objects", {});

    UnsignedInt translationFieldId, rotationFieldId, scalingFieldId, fieldWithObjectMapping;
    if(findTranslationRotationScalingFields(translationFieldId, rotationFieldId, scalingFieldId, &fieldWithObjectMapping) == ~std::size_t{}) return {};

    /* If is2D() returned false as well, all *FieldId would be invalid, which
       is handled above. */
    CORRADE_ASSERT(!is3D(), "Trade::SceneData::translationRotationScaling2DFor(): scene has a 3D transformation type", {});

    const std::size_t offset = fieldFor(_fields[fieldWithObjectMapping], 0, object);
    if(offset == _fields[fieldWithObjectMapping]._size) return {};

    Vector2 translation[1];
    Complex rotation[1];
    Vector2 scaling[1];
    translationsRotationsScalings2DIntoInternal(translationFieldId, rotationFieldId, scalingFieldId, offset, translation, rotation, scaling);
    return {InPlaceInit, *translation, *rotation, *scaling};
}

Containers::Optional<Matrix4> SceneData::transformation3DFor(const UnsignedInt object) const {
    CORRADE_ASSERT(object < _objectCount,
        "Trade::SceneData::transformation3DFor(): object" << object << "out of bounds for" << _objectCount << "objects", {});

    UnsignedInt transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId, fieldWithObjectMapping;
    if(findTransformFields(transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId, &fieldWithObjectMapping) == ~std::size_t{}) return {};

    /* If is3D() returned false as well, all *FieldId would be invalid, which
       is handled above. */
    CORRADE_ASSERT(!is2D(), "Trade::SceneData::transformation3DFor(): scene has a 2D transformation type", {});

    const std::size_t offset = fieldFor(_fields[fieldWithObjectMapping], 0, object);
    if(offset == _fields[fieldWithObjectMapping]._size) return {};

    Matrix4 transformation[1];
    transformations3DIntoInternal(transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId, offset, transformation);
    return *transformation;
}

Containers::Optional<Containers::Triple<Vector3, Quaternion, Vector3>> SceneData::translationRotationScaling3DFor(const UnsignedInt object) const {
    CORRADE_ASSERT(object < _objectCount,
        "Trade::SceneData::translationRotationScaling3DFor(): object" << object << "out of bounds for" << _objectCount << "objects", {});

    UnsignedInt translationFieldId, rotationFieldId, scalingFieldId, fieldWithObjectMapping;
    if(findTranslationRotationScalingFields(translationFieldId, rotationFieldId, scalingFieldId, &fieldWithObjectMapping) == ~std::size_t{}) return {};

    /* If is3D() returned false as well, all *FieldId would be invalid, which
       is handled above. */
    CORRADE_ASSERT(!is2D(), "Trade::SceneData::translationRotationScaling3DFor(): scene has a 2D transformation type", {});

    const std::size_t offset = fieldFor(_fields[fieldWithObjectMapping], 0, object);
    if(offset == _fields[fieldWithObjectMapping]._size) return {};

    Vector3 translation[1];
    Quaternion rotation[1];
    Vector3 scaling[1];
    translationsRotationsScalings3DIntoInternal(translationFieldId, rotationFieldId, scalingFieldId, offset, translation, rotation, scaling);
    return {InPlaceInit, *translation, *rotation, *scaling};
}

Containers::Array<Containers::Pair<UnsignedInt, Int>> SceneData::meshesMaterialsFor(const UnsignedInt object) const {
    CORRADE_ASSERT(object < _objectCount,
        "Trade::SceneData::meshesMaterialsFor(): object" << object << "out of bounds for" << _objectCount << "objects", {});

    const UnsignedInt meshFieldId = fieldFor(SceneField::Mesh);
    if(meshFieldId == ~UnsignedInt{}) return {};

    const SceneFieldData& field = _fields[meshFieldId];
    Containers::Array<Containers::Pair<UnsignedInt, Int>> out;
    std::size_t offset = 0;
    for(;;) {
        offset = fieldFor(field, offset, object);
        if(offset == field._size) break;

        UnsignedInt mesh[1];
        Int material[1];
        meshesMaterialsIntoInternal(meshFieldId, offset, mesh, material);
        arrayAppend(out, InPlaceInit, *mesh, *material);
        ++offset;
    }

    return out;
}

Containers::Array<UnsignedInt> SceneData::lightsFor(const UnsignedInt object) const {
    CORRADE_ASSERT(object < _objectCount,
        "Trade::SceneData::lightsFor(): object" << object << "out of bounds for" << _objectCount << "objects", {});

    const UnsignedInt fieldId = fieldFor(SceneField::Light);
    if(fieldId == ~UnsignedInt{}) return {};

    const SceneFieldData& field = _fields[fieldId];
    Containers::Array<UnsignedInt> out;
    std::size_t offset = 0;
    for(;;) {
        offset = fieldFor(field, offset, object);
        if(offset == field._size) break;

        UnsignedInt index[1];
        unsignedIndexFieldIntoInternal(fieldId, offset, index);
        arrayAppend(out, InPlaceInit, *index);
        ++offset;
    }

    return out;
}

Containers::Array<UnsignedInt> SceneData::camerasFor(const UnsignedInt object) const {
    CORRADE_ASSERT(object < _objectCount,
        "Trade::SceneData::camerasFor(): object" << object << "out of bounds for" << _objectCount << "objects", {});

    const UnsignedInt fieldId = fieldFor(SceneField::Camera);
    if(fieldId == ~UnsignedInt{}) return {};

    const SceneFieldData& field = _fields[fieldId];
    Containers::Array<UnsignedInt> out;
    std::size_t offset = 0;
    for(;;) {
        offset = fieldFor(field, offset, object);
        if(offset == field._size) break;

        UnsignedInt index[1];
        unsignedIndexFieldIntoInternal(fieldId, offset, index);
        arrayAppend(out, InPlaceInit, *index);
        ++offset;
    }

    return out;
}

Containers::Array<UnsignedInt> SceneData::skinsFor(const UnsignedInt object) const {
    CORRADE_ASSERT(object < _objectCount,
        "Trade::SceneData::skinsFor(): object" << object << "out of bounds for" << _objectCount << "objects", {});

    const UnsignedInt fieldId = fieldFor(SceneField::Skin);
    if(fieldId == ~UnsignedInt{}) return {};

    const SceneFieldData& field = _fields[fieldId];
    Containers::Array<UnsignedInt> out;
    std::size_t offset = 0;
    for(;;) {
        offset = fieldFor(field, offset, object);
        if(offset == field._size) break;

        UnsignedInt index[1];
        unsignedIndexFieldIntoInternal(fieldId, offset, index);
        arrayAppend(out, InPlaceInit, *index);
        ++offset;
    }

    return out;
}

Containers::Optional<const void*> SceneData::importerStateFor(const UnsignedInt object) const {
    CORRADE_ASSERT(object < _objectCount,
        "Trade::SceneData::importerStateFor(): object" << object << "out of bounds for" << _objectCount << "objects", {});

    const UnsignedInt fieldId = fieldFor(SceneField::ImporterState);
    if(fieldId == ~UnsignedInt{}) return {};

    const SceneFieldData& field = _fields[fieldId];
    const std::size_t offset = fieldFor(field, 0, object);
    if(offset == field._size) return {};

    const void* importerState[1];
    importerStateIntoInternal(fieldId, offset, importerState);
    return *importerState;
}

Containers::Array<SceneFieldData> SceneData::releaseFieldData() {
    Containers::Array<SceneFieldData> out = std::move(_fields);
    _fields = {};
    return out;
}

Containers::Array<char> SceneData::releaseData() {
    _fields = {};
    Containers::Array<char> out = std::move(_data);
    _data = {};
    return out;
}

}}

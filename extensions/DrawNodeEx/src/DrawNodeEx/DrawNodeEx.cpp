/* Copyright (c) 2012 Scott Lembcke and Howling Moon Software
* Copyright (c) 2012 cocos2d-x.org
* Copyright (c) 2013-2016 Chukong Technologies Inc.
* Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
* Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#include "DrawNodeEx/DrawNodeEx.h"
#include <stddef.h>  // offsetof
#include "base/Types.h"
#include "base/EventType.h"
#include "base/Configuration.h"
#include "renderer/Renderer.h"
#include "base/Director.h"
#include "base/EventListenerCustom.h"
#include "base/EventDispatcher.h"
#include "2d/ActionCatmullRom.h"
#include "base/Utils.h"
#include "renderer/Shaders.h"
#include "renderer/backend/ProgramState.h"
#include "poly2tri/poly2tri.h"

#if defined(_WIN32)
#    pragma push_macro("TRANSPARENT")
#    undef TRANSPARENT
#endif

NS_AX_EXT_BEGIN

/** Is a polygon convex?
* @param verts A pointer to point coordinates.
* @param count The number of verts measured in points.
*/
static bool isConvex(const Vec2* verts, int count)
{
    bool isPositive = false, isNegative = false;
    for (unsigned int i = 0; i < count; i++)
    {
        Vec2 A = verts[i];
        Vec2 B = verts[(i + 1) % count];
        Vec2 C = verts[(i + 2) % count];

        double crossProduct = (B.x - A.x) * (C.y - B.y) - (B.y - A.y) * (C.x - B.x);

        if (crossProduct > 0) isPositive = true;
        else if (crossProduct < 0) isNegative = true;

        if (isPositive && isNegative)
            return false;  // is concave
    }
    return true;  // is convex
}

Vec2* DrawNodeEx::transform(const Vec2* vertices, unsigned int& count, bool closedPolygon)
{
    Vec2 vert0 = vertices[0];
    int closedCounter = 0;

    if (closedPolygon && vertices[0] != vertices[count - 1])
    {
        closedCounter = 1;
    }

    Vec2* vert = new Vec2[count + closedCounter];
    AXASSERT(vert != nullptr, "DrawNode::transform: NO MEMORY");

    if (_dnTransform == false)
    {
        memcpy(vert, vertices, count * sizeof(Vec2));
        if (closedCounter)
        {
            vert[count] = vert0;
            count++;
        }
        return vert;
    }

    const float sinRot = sin(_dnRotation);
    const float cosRot = cos(_dnRotation);

    for (unsigned int i = 0; i < count; i++)
    {
        if (_dnRotation == 0.0f)
        {
            vert[i].x = vertices[i].x * _dnScale.x + _dnPosition.x;
            vert[i].y = vertices[i].y * _dnScale.y + _dnPosition.y;
        }
        else  // https://stackoverflow.com/questions/2259476/rotating-a-point-about-another-point-2d
        {

            // translate point back to origin:
            vert[i].x = vertices[i].x - _dnCenter.x;
            vert[i].y = vertices[i].y - _dnCenter.y;

            // rotate point
            float xnew = vert[i].x * cosRot - vert[i].y * sinRot;
            float ynew = vert[i].x * sinRot + vert[i].y * cosRot;

            // translate point back:
            vert[i].x = xnew + _dnCenter.x;
            vert[i].y = ynew + _dnCenter.y;

            // scale and position
            vert[i].x = vert[i].x * _dnScale.x + _dnPosition.x;
            vert[i].y = vert[i].y * _dnScale.y + _dnPosition.y;
        }
    }

    if (closedCounter)
    {
        vert[count] = vert[0];
        count++;
    }

    return vert;
}

DrawNodeEx::DrawNodeEx(float lineWidth)
    : _lineWidth(lineWidth)
    , _defaultLineWidth(lineWidth)
    , _isConvex(false)
    , _dnPosition(Vec2::ZERO)
    , _dnRotation(0.0f)
    , _dnScale(Vec2::ONE)
    , _dnTransform(_dnTransform)
    , _drawOrder(false)
{
    _blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
#if AX_ENABLE_CACHE_TEXTURE_DATA
    // TODO new-renderer: interface setupBuffer removal

    // Need to listen the event only when not use batchnode, because it will use VBO
    //    auto listener = EventListenerCustom::create(EVENT_RENDERER_RECREATED, [this](EventCustom* event){
    //        /** listen the event that renderer was recreated on Android/WP8 */
    //        this->setupBuffer();
    //    });

    //    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
#endif
}

DrawNodeEx::~DrawNodeEx()
{
    AX_SAFE_FREE(_bufferTriangle);
    freeShaderInternal(_customCommandTriangle);

    AX_SAFE_FREE(_bufferPoint);
    freeShaderInternal(_customCommandPoint);

    AX_SAFE_FREE(_bufferLine);
    freeShaderInternal(_customCommandLine);
}

DrawNodeEx* DrawNodeEx::create(float defaultLineWidth)
{
    DrawNodeEx* ret = new DrawNodeEx(defaultLineWidth);
    if (ret->init())
    {
        ret->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ret);
    }
    return ret;
}

void DrawNodeEx::ensureCapacityTriangle(int count)
{
    AXASSERT(count >= 0, "capacity must be >= 0");

    if (_bufferCountTriangle + count > _bufferCapacityTriangle)
    {
        _bufferCapacityTriangle += MAX(_bufferCapacityTriangle, count);
        _bufferTriangle = (V2F_C4B_T2F*)realloc(_bufferTriangle, _bufferCapacityTriangle * sizeof(V2F_C4B_T2F));

        _customCommandTriangle.createVertexBuffer(sizeof(V2F_C4B_T2F), _bufferCapacityTriangle,
            CustomCommand::BufferUsage::STATIC);
        _customCommandTriangle.updateVertexBuffer(_bufferTriangle, _bufferCapacityTriangle * sizeof(V2F_C4B_T2F));
    }
}

void DrawNodeEx::ensureCapacityPoint(int count)
{
    AXASSERT(count >= 0, "capacity must be >= 0");

    if (_bufferCountPoint + count > _bufferCapacityPoint)
    {
        _bufferCapacityPoint += MAX(_bufferCapacityPoint, count);
        _bufferPoint = (V2F_C4B_T2F*)realloc(_bufferPoint, _bufferCapacityPoint * sizeof(V2F_C4B_T2F));

        _customCommandPoint.createVertexBuffer(sizeof(V2F_C4B_T2F), _bufferCapacityPoint,
            CustomCommand::BufferUsage::STATIC);
        _customCommandPoint.updateVertexBuffer(_bufferPoint, _bufferCapacityPoint * sizeof(V2F_C4B_T2F));
    }
}

void DrawNodeEx::ensureCapacityLine(int count)
{
    AXASSERT(count >= 0, "capacity must be >= 0");

    if (_bufferCountLine + count > _bufferCapacityLine)
    {
        _bufferCapacityLine += MAX(_bufferCapacityLine, count);
        _bufferLine = (V2F_C4B_T2F*)realloc(_bufferLine, _bufferCapacityLine * sizeof(V2F_C4B_T2F));

        _customCommandLine.createVertexBuffer(sizeof(V2F_C4B_T2F), _bufferCapacityLine,
            CustomCommand::BufferUsage::STATIC);
        _customCommandLine.updateVertexBuffer(_bufferLine, _bufferCapacityLine * sizeof(V2F_C4B_T2F));
    }
}

bool DrawNodeEx::init()
{
    _blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
    updateShader();
    ensureCapacityTriangle(512);
    _dirtyTriangle = true;

    ensureCapacityPoint(64);
    ensureCapacityLine(256);
    _dirtyLine = true;
    _dirtyPoint = true;

    return true;
}

void DrawNodeEx::updateShader()
{
    updateShaderInternal(_customCommandTriangle, backend::ProgramType::POSITION_COLOR_LENGTH_TEXTURE,
        CustomCommand::DrawType::ARRAY, CustomCommand::PrimitiveType::TRIANGLE);

    updateShaderInternal(_customCommandPoint, backend::ProgramType::POSITION_COLOR_TEXTURE_AS_POINTSIZE,
        CustomCommand::DrawType::ARRAY, CustomCommand::PrimitiveType::POINT);

    updateShaderInternal(_customCommandLine, backend::ProgramType::POSITION_COLOR_LENGTH_TEXTURE,
        CustomCommand::DrawType::ARRAY, CustomCommand::PrimitiveType::LINE);
}

void DrawNodeEx::updateShaderInternal(CustomCommand& cmd,
    uint32_t programType,
    CustomCommand::DrawType drawType,
    CustomCommand::PrimitiveType primitiveType)
{
    auto& pipelinePS = cmd.getPipelineDescriptor().programState;
    AX_SAFE_RELEASE(pipelinePS);

    auto program = backend::Program::getBuiltinProgram(programType);
    pipelinePS = new backend::ProgramState(program);
    setVertexLayout(cmd);
    cmd.setPrimitiveType(primitiveType);
    cmd.setDrawType(drawType);
}

void DrawNodeEx::setVertexLayout(CustomCommand& cmd)
{
    auto* programState = cmd.getPipelineDescriptor().programState;
    programState->validateSharedVertexLayout(backend::VertexLayoutType::DrawNode);
}

void DrawNodeEx::freeShaderInternal(CustomCommand& cmd)
{
    auto& pipelinePS = cmd.getPipelineDescriptor().programState;
    AX_SAFE_RELEASE_NULL(pipelinePS);
}

void DrawNodeEx::updateBlendState(CustomCommand& cmd)
{
    backend::BlendDescriptor& blendDescriptor = cmd.getPipelineDescriptor().blendDescriptor;
    blendDescriptor.blendEnabled = true;
    if (_blendFunc == BlendFunc::ALPHA_NON_PREMULTIPLIED)
    {
        blendDescriptor.sourceRGBBlendFactor = backend::BlendFactor::SRC_ALPHA;
        blendDescriptor.destinationRGBBlendFactor = backend::BlendFactor::ONE_MINUS_SRC_ALPHA;
        blendDescriptor.sourceAlphaBlendFactor = backend::BlendFactor::SRC_ALPHA;
        blendDescriptor.destinationAlphaBlendFactor = backend::BlendFactor::ONE_MINUS_SRC_ALPHA;
        setOpacityModifyRGB(false);
    }
    else
    {
        blendDescriptor.sourceRGBBlendFactor = backend::BlendFactor::ONE;
        blendDescriptor.destinationRGBBlendFactor = backend::BlendFactor::ONE_MINUS_SRC_ALPHA;
        blendDescriptor.sourceAlphaBlendFactor = backend::BlendFactor::ONE;
        blendDescriptor.destinationAlphaBlendFactor = backend::BlendFactor::ONE_MINUS_SRC_ALPHA;
        setOpacityModifyRGB(true);
    }
}

void DrawNodeEx::updateUniforms(const Mat4& transform, CustomCommand& cmd)
{
    auto& pipelineDescriptor = cmd.getPipelineDescriptor();
    const auto& matrixP = _director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
    Mat4 matrixMVP = matrixP * transform;
    auto mvpLocation = pipelineDescriptor.programState->getUniformLocation("u_MVPMatrix");
    pipelineDescriptor.programState->setUniform(mvpLocation, matrixMVP.m, sizeof(matrixMVP.m));

    float alpha = _displayedOpacity / 255.0f;
    auto alphaUniformLocation = pipelineDescriptor.programState->getUniformLocation("u_alpha");
    pipelineDescriptor.programState->setUniform(alphaUniformLocation, &alpha, sizeof(alpha));
}

void DrawNodeEx::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
    if (_bufferCountTriangle)
    {
        updateBlendState(_customCommandTriangle);
        updateUniforms(transform, _customCommandTriangle);
        _customCommandTriangle.init(_globalZOrder);
        renderer->addCommand(&_customCommandTriangle);
    }

    if (_bufferCountPoint)
    {
        updateBlendState(_customCommandPoint);
        updateUniforms(transform, _customCommandPoint);
        _customCommandPoint.init(_globalZOrder);
        renderer->addCommand(&_customCommandPoint);
    }

    if (_bufferCountLine)
    {
        updateBlendState(_customCommandLine);
        updateUniforms(transform, _customCommandLine);
        _customCommandLine.init(_globalZOrder);
        renderer->addCommand(&_customCommandLine);
    }
}

void DrawNodeEx::drawPoint(const Vec2& position, const float pointSize, const Color4B& color,
    const DrawNodeEx::PointType pointType)
{
    if (pointSize == 0) return;

    _drawPoint(position, pointSize, color, pointType);
}

void DrawNodeEx::drawPoints(const Vec2* position, unsigned int numberOfPoints, const Color4B& color,
    const DrawNodeEx::PointType pointType)
{
    drawPoints(position, numberOfPoints, 1.f, color);
}

void DrawNodeEx::drawPoints(const Vec2* position,
    unsigned int numberOfPoints,
    const float pointSize,
    const Color4B& color,
    const DrawNodeEx::PointType pointType)
{
    if (pointSize == 0) return;
    _drawPoints(position, numberOfPoints, pointSize, color, pointType);
}

void DrawNodeEx::drawLine(const Vec2& origin, const Vec2& destination, const Color4B& color, float thickness, DrawNodeEx::EndType etStart, DrawNodeEx::EndType etEnd)
{
    if (thickness == 0) return;
    _drawSegment(origin, destination, color, thickness, etStart, etEnd);
}

void DrawNodeEx::drawPoly(const Vec2* poli,
    unsigned int numberOfPoints,
    bool closedPolygon,
    const Color4B& color,
    float thickness)
{
    if (thickness == 0) return;
    _drawPoly(poli, numberOfPoints, closedPolygon, color, thickness);
}

void DrawNodeEx::drawCircle(const Vec2& center,
    float radius,
    float angle,
    unsigned int segments,
    bool drawLineToCenter,
    float scaleX,
    float scaleY,
    const Color4B& color,
    float thickness)
{
    if (thickness == 0) return;
    _drawCircle(center, radius, angle, segments, drawLineToCenter, scaleX, scaleY, color, Color4B(), false, thickness);
}

void DrawNodeEx::drawCircle(const Vec2& center,
    float radius,
    float angle,
    unsigned int segments,
    bool drawLineToCenter,
    const Color4B& color,
    float thickness)
{
    if (thickness == 0) return;
    _drawCircle(center,  radius, angle,  segments, drawLineToCenter, 1.0f, 1.0f, color, color, false, thickness);
}

void DrawNodeEx::drawStar(const Vec2& center,
    float radiusI,
    float radiusO,
    unsigned int segments,
    const Color4B& color,
    float thickness)
{
    if (thickness == 0) return;
    _drawAStar(center, radiusI, radiusO, segments, color, color, thickness, false);
}

void DrawNodeEx::drawSolidStar(const Vec2& center,
    float radiusI, // inner
    float radiusO, // outer
    unsigned int segments,
    const Color4B& color,
    const Color4B& filledColor,
    float thickness)
{
    _drawAStar(center, radiusI, radiusO, segments, color, filledColor, thickness, true);
}

void DrawNodeEx::drawQuadBezier(const Vec2& origin,
    const Vec2& control,
    const Vec2& destination,
    unsigned int segments,
    const Color4B& color,
    float thickness)
{
    if (thickness == 0) return;

    Vec2* vertices = _abuf.get<Vec2>(segments + 1);

    float t = 0.0f;
    for (unsigned int i = 0; i < segments; i++)
    {
        vertices[i].x = powf(1.0f - t, 2.0f) * origin.x + 2.0f * (1.0f - t) * t * control.x + t * t * destination.x;
        vertices[i].y = powf(1.0f - t, 2.0f) * origin.y + 2.0f * (1.0f - t) * t * control.y + t * t * destination.y;
        t += 1.0f / segments;
    }
    vertices[segments].x = destination.x;
    vertices[segments].y = destination.y;

    swapIsConvex(false);
    _drawPoly(vertices, segments + 1, false, color, thickness);
    _isConvex = _isConvexTmp;
}

void DrawNodeEx::drawCubicBezier(const Vec2& origin,
    const Vec2& control1,
    const Vec2& control2,
    const Vec2& destination,
    unsigned int segments,
    const Color4B& color,
    float thickness)
{
    if (thickness == 0) return;

    Vec2* vertices = _abuf.get<Vec2>(segments + 1);

    float t = 0.0f;
    for (unsigned int i = 0; i < segments; i++)
    {
        vertices[i].x = powf(1.0f - t, 3.0f) * origin.x + 3.0f * powf(1.0f - t, 2.0f) * t * control1.x +
            3.0f * (1 - t) * t * t * control2.x + t * t * t * destination.x;
        vertices[i].y = powf(1.0f - t, 3.0f) * origin.y + 3.0f * powf(1.0f - t, 2.0f) * t * control1.y +
            3.0f * (1 - t) * t * t * control2.y + t * t * t * destination.y;
        t += 1.0f / segments;
    }
    vertices[segments].x = destination.x;
    vertices[segments].y = destination.y;

    swapIsConvex(true);
    _drawPoly(vertices, segments + 1, false, color, thickness);
    _isConvex = _isConvexTmp;
}

void DrawNodeEx::drawCardinalSpline(ax::PointArray* config,
    float tension,
    unsigned int segments,
    const Color4B& color,
    float thickness)
{
    if (thickness == 0.0f) return;

    Vec2* vertices = _abuf.get<Vec2>(segments);

    ssize_t p;
    float lt;
    float deltaT = 1.0f / config->count();

    for (unsigned int i = 0; i < segments; i++)
    {
        float dt = (float)i / segments;

        // border
        if (dt == 1)
        {
            p = config->count() - 1;
            lt = 1;
        }
        else
        {
            p = static_cast<ssize_t>(dt / deltaT);
            lt = (dt - deltaT * (float)p) / deltaT;
        }

        // Interpolate
        Vec2 pp0 = config->getControlPointAtIndex(p - 1);
        Vec2 pp1 = config->getControlPointAtIndex(p + 0);
        Vec2 pp2 = config->getControlPointAtIndex(p + 1);
        Vec2 pp3 = config->getControlPointAtIndex(p + 2);

        Vec2 newPos = ccCardinalSplineAt(pp0, pp1, pp2, pp3, tension, lt);
        vertices[i].x = newPos.x;
        vertices[i].y = newPos.y;
        if (newPos == config->getControlPointAtIndex(config->count() - 1) && i > 0)
        {
            segments = i + 1;
            break;
        }
    }

    swapIsConvex(true);
    _drawPoly(vertices, segments, false, color, thickness);
    _isConvex = _isConvexTmp;
}

void DrawNodeEx::drawCatmullRom(ax::PointArray* points, unsigned int segments, const Color4B& color, float thickness)
{
    if (thickness == 0.0f) return;
    drawCardinalSpline(points, 0.5f, segments, color, thickness);
}


void DrawNodeEx::drawDot(const Vec2& pos, float radius, const Color4B& color)
{
    if (radius == 0.0f) return;
    _drawDot(pos, radius, color);
}

void DrawNodeEx::drawRect(const Vec2& p1,
    const Vec2& p2,
    const Vec2& p3,
    const Vec2& p4,
    const Color4B& color,
    float thickness)
{
    if (thickness == 0.0f) return;

    Vec2 line[5] = { {p1}, {p2}, {p3}, {p4}, {p1} };
    swapIsConvex(true);
    _drawPoly(line, 5, false, color, thickness);
    _isConvex = _isConvexTmp;
}

void DrawNodeEx::drawRect(const Vec2& origin, const Vec2& destination, const Color4B& color, float thickness)
{
    if (thickness == 0.0f) return;

    Vec2 line[5] = { origin, Vec2(destination.x, origin.y), destination, Vec2(origin.x, destination.y), origin };
    swapIsConvex(true);
    _drawPoly(line, 5, false, color, thickness);
    _isConvex = _isConvexTmp;
}

void DrawNodeEx::drawSegment(const Vec2& from, const Vec2& to, float thickness, const Color4B& color, DrawNodeEx::EndType etStart, DrawNodeEx::EndType etEnd)
{
    if (thickness == 0.0f) return;
    _drawSegment(from, to, color, thickness, etStart, etEnd);
}

void DrawNodeEx::drawPolygon(Vec2* verts,
    int count,
    const Color4B& fillColor,
    float thickness,
    const Color4B& borderColor)
{
    _drawPolygon(verts, count, fillColor, borderColor, true, thickness);
}

void DrawNodeEx::drawPolygon(Vec2* verts,
    int count,
    float thickness,
    const Color4B& borderColor)
{
    _drawPolygon(verts, count, Color4B::TRANSPARENT, borderColor, true,thickness);
}

void DrawNodeEx::drawSolidPolygon(Vec2* verts,
    int count,
    const Color4B& fillColor,
    float thickness,
    const Color4B& borderColor)
{
    _drawPolygon(verts, count, fillColor, borderColor, true, thickness);
}

void DrawNodeEx::drawSolidRect(const Vec2& origin, const Vec2& destination, const Color4B& fillColor, float thickness,
    const Color4B& borderColor)
{
    Vec2 vertices[] = { origin, Vec2(destination.x, origin.y), destination, Vec2(origin.x, destination.y) };
    swapIsConvex(true);
    _drawPolygon(vertices, 4, fillColor, borderColor, false, thickness);
    _isConvex = _isConvexTmp;
}

void DrawNodeEx::drawSolidPoly(const Vec2* poli, unsigned int numberOfPoints, const Color4B& color, float thickness,
    const Color4B& borderColor)
{
    _drawPolygon(poli, numberOfPoints, color, borderColor, true, thickness);
}

void DrawNodeEx::drawPie(const Vec2& center,
    float radius,
    float rotation,
    int startAngle,
    int endAngle,
    float scaleX,
    float scaleY,
    const Color4B& fillColor,
    const Color4B& borderColor,
    DrawMode drawMode,
    float thickness)
{
  _drawPie(center, radius, rotation, startAngle, endAngle, scaleX, scaleY, fillColor, borderColor, drawMode, thickness);
}

void DrawNodeEx::drawSolidCircle(const Vec2& center,
    float radius,
    float angle,
    unsigned int segments,
    float scaleX,
    float scaleY,
    const Color4B& fillColor,
    float thickness,
    const Color4B& borderColor)
{
    _drawCircle(center,  radius, angle,  segments, false, scaleX, scaleY, borderColor, fillColor,true, thickness);
}

void DrawNodeEx::drawSolidCircle(const Vec2& center,
    float radius,
    float angle,
    unsigned int segments,
    float scaleX,
    float scaleY,
    const Color4B& color)
{
    _drawCircle(center,  radius, angle,  segments, false, scaleX, scaleY, Color4B(), color,true);
}

void DrawNodeEx::drawSolidCircle(const Vec2& center,
    float radius,
    float angle,
    unsigned int segments,
    const Color4B& color)
{
    _drawCircle(center,  radius, angle,  segments, false, 1.0f, 1.0f, Color4B(), color,true);
}

void DrawNodeEx::drawTriangle(const Vec2* vertices3,
    const ax::Color4B& fillColor,
    const ax::Color4B& borderColor,
    float thickness)
{
    _drawTriangle(vertices3, fillColor, borderColor, false, thickness);
}

void DrawNodeEx::drawTriangle(const Vec2& p1, const Vec2& p2, const Vec2& p3, const Color4B& fillColor, float thickness)
{
    Vec2 vertices3[3] = { p1, p2, p3 };
    _drawTriangle(vertices3, fillColor, fillColor/*Color4B::TRANSPARENT*/, false, thickness);
}

void DrawNodeEx::drawSolidTriangle(const Vec2* vertices3,
    const ax::Color4B& fillColor,
    const ax::Color4B& borderColor,
    float thickness)
{
    _drawTriangle(vertices3, fillColor, borderColor, true, thickness);
}

void DrawNodeEx::clear()
{
    _bufferCountTriangle = 0;
    _dirtyTriangle = true;
    _bufferCountLine = 0;
    _dirtyLine = true;
    _bufferCountPoint = 0;
    _dirtyPoint = true;

    _lineWidth = _defaultLineWidth;
}

const BlendFunc& DrawNodeEx::getBlendFunc() const
{
    return _blendFunc;
}

void DrawNodeEx::setBlendFunc(const BlendFunc& blendFunc)
{
    _blendFunc = blendFunc;
}

void DrawNodeEx::setLineWidth(float lineWidth)
{
    _defaultLineWidth = lineWidth;
}

float DrawNodeEx::getLineWidth()
{
    return this->_defaultLineWidth;
}

void DrawNodeEx::visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
    if (_isolated)
    {
        // ignore `parentTransform` from parent
        Node::visit(renderer, Mat4::IDENTITY, parentFlags);
    }
    else
    {
        Node::visit(renderer, parentTransform, parentFlags);
    }
}

void DrawNodeEx::_drawPolygon(const Vec2* verts,
    unsigned int count,
    const Color4B& fillColor,
    const Color4B& borderColor,
    bool closedPolygon,
    float thickness
    )
{
    AXASSERT(count >= 0, "invalid count value");

    bool outline = (thickness != 0.0f);

    Vec2* _vertices = transform(verts, count, closedPolygon);

    std::vector<V2F_C4B_T2F_Triangle> triangleList;

    int vertex_count = 0;

    // calculate the memory (important for correct drawing stuff)
    if (closedPolygon && !_isConvex && fillColor.a > 0.0f && !isConvex(_vertices, count) && count >= 3)
    {
        std::vector<p2t::Point> p2pointsStorage;
        p2pointsStorage.reserve(count);
        std::vector<p2t::Point*> p2points;
        p2points.reserve(count);

        for (unsigned int i = 0; i < count - 1; i++)  // count-1 is needed because of: _vertices[0] = _vertices[i < count]
        {
            p2points.emplace_back(&p2pointsStorage.emplace_back((float)_vertices[i].x, (float)_vertices[i].y));
        }
        p2t::CDT cdt(p2points);
        cdt.Triangulate();
        std::vector<p2t::Triangle*> tris = cdt.GetTriangles();

        vertex_count += tris.size();
        for (auto&& t : tris)  // use it later; only one calculate!!!
        {
            p2t::Point* vec1 = t->GetPoint(0);
            p2t::Point* vec2 = t->GetPoint(1);
            p2t::Point* vec3 = t->GetPoint(2);

            V2F_C4B_T2F_Triangle triangle = {
                {Vec2(vec1->x, vec1->y), fillColor, Tex2F::ZERO},
                {Vec2(vec2->x, vec2->y), fillColor, Tex2F::ZERO},
                {Vec2(vec3->x, vec3->y), fillColor, Tex2F::ZERO},
            };
            triangleList.emplace_back(triangle); // use it for drawing later
        }
    }
    else if (fillColor.a > 0.0f)
    {
        vertex_count += count - 2;
    }

    if (outline)
    {

        if (thickness != 1.0f || _drawOrder)
        {
            vertex_count += 6 * (count - 1);
        }
        else
        {
            vertex_count += 2 * count;
        }

    }

    vertex_count *= 3;
    ensureCapacityTriangle(vertex_count);
    V2F_C4B_T2F_Triangle* triangles = (V2F_C4B_T2F_Triangle*)(_bufferTriangle + _bufferCountTriangle);

    // start drawing...
    int ii = 0;
    if (closedPolygon && !_isConvex && fillColor.a > 0.0f && !isConvex(_vertices, count) && count >= 3)
    {
        ;
        for (auto&& t : triangleList)
        {
            triangles[ii++] = t;
        }
    }
    else if (fillColor.a > 0.0f)
    {
        for (unsigned int i = 0; i < count - 2; i++)
        {
            triangles[ii++] = {
                {_vertices[0], fillColor, Tex2F::ZERO},
                {_vertices[i + 1], fillColor, Tex2F::ZERO},
                {_vertices[i + 2], fillColor, Tex2F::ZERO},
            };
        }
    }
    if (outline)
    {
        if (thickness != 1.0f || _drawOrder)
        {
            thickness *= _dnFactor;  // thickness 1 is the same for all

            Vec2 vo0, vo1, vo2, vo3, vo4, vo5, vo6, vo7;

            for (unsigned int i = 1; i < (count); i++)
            {
                Vec2 a = _vertices[i - 1];
                Vec2 b = _vertices[i];
                Vec2 n = ((b - a).getPerp()).getNormalized();
                Vec2 t = n.getPerp();
                Vec2 nw = n * thickness;
                Vec2 tw = t * thickness;
                Vec2 v0 = b - (nw + tw);
                Vec2 v1 = b + (nw - tw);
                Vec2 v2 = b - nw;
                Vec2 v3 = b + nw;
                Vec2 v4 = a - nw;
                Vec2 v5 = a + nw;
                Vec2 v6 = a - (nw - tw);
                Vec2 v7 = a + (nw + tw);

                {
                    triangles[ii++] = {
                        {v0, borderColor, Tex2F(-(n + t))},
                        {v1, borderColor, Tex2F(n - t)},
                        {v2, borderColor, Tex2F(-n)},
                    };
                    //          debugColor = Color4B::BLUE;//borderColor;
                    triangles[ii++] = {
                        {v3, borderColor, Tex2F(n)},
                        {v1, borderColor, Tex2F(n - t)},
                        {v2, borderColor, Tex2F(-n)},
                    };
                }

                triangles[ii++] = {
                    {v3, borderColor, Tex2F(n)},
                    {v4, borderColor, Tex2F(-n)},
                    {v2, borderColor, Tex2F(-n)},
                };
                triangles[ii++] = {
                    {v3, borderColor, Tex2F(n)},
                    {v4, borderColor, Tex2F(-n)},
                    {v5, borderColor, Tex2F(n)},
                };

                {
                    triangles[ii++] = {
                        {v6, borderColor, Tex2F(t - n)},
                        {v4, borderColor, Tex2F(-n)},
                        {v5, borderColor, Tex2F(n)},
                    };
                    triangles[ii++] = {
                        {v6, borderColor, Tex2F(t - n)},
                        {v7, borderColor, Tex2F(t + n)},
                        {v5, borderColor, Tex2F(n)},
                    };
                }
            }
        }
        else
        {
            struct ExtrudeVerts
            {
                Vec2 offset, n;
            };
            struct ExtrudeVerts* extrude = (struct ExtrudeVerts*)malloc(sizeof(struct ExtrudeVerts) * count);

            int ccount = count - ((closedPolygon) ? 0 : 1);
            for (unsigned int i = 0; i < count; i++)
            {
                Vec2 v0 = _vertices[(i - 1 + count) % count];
                Vec2 v1 = _vertices[i];
                Vec2 v2 = _vertices[(i + 1) % count];

                Vec2 n1 = ((v1 - v0).getPerp()).getNormalized();
                Vec2 n2 = ((v2 - v1).getPerp()).getNormalized();

                Vec2 offset = (n1 + n2) * (1.0f / (Vec2::dot(n1, n2) + 1.0f));
                extrude[i] = { offset, n2 };
            }


            for (int i = 0; i < ccount; i++)
            {
                int j = (i + 1) % count;
                Vec2 v0 = _vertices[i];
                Vec2 v1 = _vertices[j];

                Vec2 n0 = extrude[i].n;

                Vec2 offset0 = extrude[i].offset;
                Vec2 offset1 = extrude[j].offset;

                Vec2 inner0 = v0 - offset0 * thickness;
                Vec2 inner1 = v1 - offset1 * thickness;
                Vec2 outer0 = v0 + offset0 * thickness;
                Vec2 outer1 = v1 + offset1 * thickness;

                triangles[ii++] = {
                    {inner0, borderColor, Tex2F(-n0)},
                    {inner1, borderColor, Tex2F(-n0)},
                    {outer1, borderColor, Tex2F(n0)} };

                triangles[ii++] = {
                    {inner0, borderColor, Tex2F(-n0)},
                    {outer0, borderColor, Tex2F(n0)},
                    {outer1, borderColor, Tex2F(n0)} };
            }
            free(extrude);
        }
    }

    _customCommandTriangle.updateVertexBuffer(triangles, _bufferCountTriangle * sizeof(V2F_C4B_T2F), vertex_count * sizeof(V2F_C4B_T2F));
    _bufferCountTriangle += vertex_count;
    _customCommandTriangle.setVertexDrawInfo(0, _bufferCountTriangle);
    _dirtyTriangle = true;

    AX_SAFE_DELETE_ARRAY(_vertices);
}

void DrawNodeEx::_drawPoly(const Vec2* verts,
    unsigned int count,
    bool closedPolygon,
    const Color4B& color,
    float thickness)
{
    if (thickness == 0.0f) return;
    if (thickness == 1.0f && !_drawOrder)  // usefull for a DrawNode:::_drawPoly thickness = 1.0 only ?
    {
        Vec2* _vertices = transform(verts, count);

        unsigned int vertex_count = (closedPolygon) ? 2 * count : 2 * (count - 1);

        ensureCapacityLine(vertex_count);
        V2F_C4B_T2F* line = _bufferLine + _bufferCountLine;

        int ii = 0;
        for (unsigned int i = 0; i < count - 1; i++)
        {
            line[ii++] = { _vertices[i], color, Tex2F::ZERO };
            line[ii++] = { _vertices[i + 1], color, Tex2F::ZERO };
        }
        if (closedPolygon)
        {
            line[ii++] = { _vertices[count - 1], color, Tex2F::ZERO };
            line[ii++] = line[0];
        }


        _customCommandLine.updateVertexBuffer(line, _bufferCountLine * sizeof(V2F_C4B_T2F),
            vertex_count * sizeof(V2F_C4B_T2F));
        _bufferCountLine += vertex_count;
        _customCommandLine.setVertexDrawInfo(0, _bufferCountLine);

        AX_SAFE_DELETE_ARRAY(_vertices);
    }
    else
    {
        _drawPolygon(verts, count, Color4B::TRANSPARENT, color, closedPolygon, thickness);
    }
}

void DrawNodeEx::_drawSegment(const Vec2& from, const Vec2& to, const Color4B& color, float thickness, DrawNodeEx::EndType etStart, DrawNodeEx::EndType etEnd)
{
    if (thickness == 0.0f) return;
    if (thickness == 1.0f && !_drawOrder)  // usefull for a DrawNode::drawline thickness = 1.0 only ?
    {
        unsigned int count = 2;
        Vec2 aLine[] = { from, to };

        Vec2* _vertices = transform(aLine, count, false);

        ensureCapacityLine(count);

        V2F_C4B_T2F* line = _bufferLine + _bufferCountLine;

        line[0] = { _vertices[0], color, Tex2F::ZERO };
        line[1] = { _vertices[1], color, Tex2F::ZERO };

        _customCommandLine.updateVertexBuffer(line, _bufferCountLine * sizeof(V2F_C4B_T2F), 2 * sizeof(V2F_C4B_T2F));
        _bufferCountLine += count;
        _dirtyLine = true;
        _customCommandLine.setVertexDrawInfo(0, _bufferCountLine);

        AX_SAFE_DELETE_ARRAY(_vertices);
        return;
    }

    unsigned int count = 2;
    Vec2 line[] = { from, to };

    Vec2* _vertices = transform(line, count, false);

    Vec2 a = _vertices[0];
    Vec2 b = _vertices[1];
    Vec2 n = ((b - a).getPerp()).getNormalized();
    Vec2 t = n.getPerp();
    Vec2 nw = n * thickness;
    Vec2 tw = t * thickness;
    Vec2 v0 = b - (nw + tw);
    Vec2 v1 = b + (nw - tw);
    Vec2 v2 = b - nw;
    Vec2 v3 = b + nw;
    Vec2 v4 = a - nw;
    Vec2 v5 = a + nw;
    Vec2 v6 = a - (nw - tw);
    Vec2 v7 = a + (nw + tw);

    unsigned int vertex_count = 3 * ((etStart != DrawNodeEx::EndType::Butt) ? 2 : 0) + 3 * 2 + 3 * ((etEnd != DrawNodeEx::EndType::Butt) ? 2 : 0);
    ensureCapacityTriangle(vertex_count);
    V2F_C4B_T2F_Triangle* triangles = (V2F_C4B_T2F_Triangle*)(_bufferTriangle + _bufferCountTriangle);

    int ii = 0;
    switch (etEnd)
    {
    case DrawNodeEx::EndType::Butt:
        break;

    case DrawNodeEx::EndType::Square:
        triangles[ii++] = {
            {v0, color, Tex2F::ZERO},
            {v1, color, Tex2F(-n)},
            {v2, color, Tex2F(n)},
        };

        triangles[ii++] = {
            {v3, color, Tex2F(n)},
            {v1, color, Tex2F::ZERO},
            {v2, color, Tex2F(-n)},
        };

        break;
    case DrawNodeEx::EndType::Round:
        triangles[ii++] = {
            {v0, color, Tex2F(-(n + t))},
            {v1, color, Tex2F(n - t)},
            {v2, color, Tex2F(-n)},
        };

        triangles[ii++] = {
            {v3, color, Tex2F(n)},
            {v1, color, Tex2F(n - t)},
            {v2, color, Tex2F(-n)},
        };
        break;

    default:
        break;
    }

    // BODY
    triangles[ii++] = {
        {v3, color, Tex2F(n)},
        {v4, color, Tex2F(-n)},
        {v2, color, Tex2F(-n)},
    };

    triangles[ii++] = {
        {v3, color, Tex2F(n)},
        {v4, color, Tex2F(-n)},
        {v5, color, Tex2F(n)},
    };

    switch (etStart)
    {
    case DrawNodeEx::EndType::Butt:
        break;

    case DrawNodeEx::EndType::Square:
        triangles[ii++] = {
            {v6, color, Tex2F::ZERO},
            {v4, color, Tex2F(-n)},
            {v5, color, Tex2F(n)},
        };

        triangles[ii++] = {
            {v6, color, Tex2F(-n)},
            {v7, color, Tex2F::ZERO},
            {v5, color, Tex2F(n)},
        };
        break;

    case DrawNodeEx::EndType::Round:
        triangles[ii++] = {
            {v6, color, Tex2F(t - n)},
            {v4, color, Tex2F(-n)},
            {v5, color, Tex2F(n)},
        };

        triangles[ii++] = {
            {v6, color, Tex2F(t - n)},
            {v7, color, Tex2F(t + n)},
            {v5, color, Tex2F(n)},
        };
        break;

    default:
        break;
    }

    _customCommandTriangle.updateVertexBuffer(triangles, _bufferCountTriangle * sizeof(V2F_C4B_T2F),
        vertex_count * sizeof(V2F_C4B_T2F));
    _bufferCountTriangle += vertex_count; //ii * 3;
    _dirtyTriangle = true;
    _customCommandTriangle.setVertexDrawInfo(0, _bufferCountTriangle);

    AX_SAFE_DELETE_ARRAY(_vertices);
}


void DrawNodeEx::_drawDot(const Vec2& pos, float radius, const Color4B& color)
{
    if (radius == 0.0f) return;

    unsigned int vertex_count = 2 * 3;
    ensureCapacityTriangle(vertex_count);

    V2F_C4B_T2F a = { Vec2(pos.x - radius, pos.y - radius), color, Tex2F(-1.0f, -1.0f) };
    V2F_C4B_T2F b = { Vec2(pos.x - radius, pos.y + radius), color, Tex2F(-1.0f, 1.0f) };
    V2F_C4B_T2F c = { Vec2(pos.x + radius, pos.y + radius), color, Tex2F(1.0f, 1.0f) };
    V2F_C4B_T2F d = { Vec2(pos.x + radius, pos.y - radius), color, Tex2F(1.0f, -1.0f) };

    V2F_C4B_T2F_Triangle* triangles = (V2F_C4B_T2F_Triangle*)(_bufferTriangle + _bufferCountTriangle);
    triangles[0] = { a, b, c };
    triangles[1] = { a, c, d };

    _customCommandTriangle.updateVertexBuffer(triangles, _bufferCountTriangle * sizeof(V2F_C4B_T2F),
        vertex_count * sizeof(V2F_C4B_T2F));
    _bufferCountTriangle += vertex_count;
    _dirtyTriangle = true;
    _customCommandTriangle.setVertexDrawInfo(0, _bufferCountTriangle);
}

void DrawNodeEx::_drawCircle(const Vec2& center,
    float radius,
    float angle,
    unsigned int segments,
    bool drawLineToCenter,
    float scaleX,
    float scaleY,
    const Color4B& borderColor,
    const Color4B& fillColor,
    bool solid,
    float thickness)
{
    const float coef = 2.0f * (float)M_PI / segments;

    Vec2* _vertices = new Vec2[segments + 2]; //_abuf.get<Vec2>(segments + 2);

    for (unsigned int i = 0; i < segments; i++)
    {
        float rads = i * coef;
        float j = radius * cosf(rads + angle) * scaleX + center.x;
        float k = radius * sinf(rads + angle) * scaleY + center.y;

        _vertices[i].x = j;
        _vertices[i].y = k;
    }
    _vertices[segments] = _vertices[0];

    swapIsConvex(true);
    if (solid)
    {
        _drawPolygon(_vertices, segments, fillColor, borderColor, false, thickness);
    }
    else
    {
        if (drawLineToCenter)  _vertices[++segments] = center;
        _drawPoly(_vertices, segments + 1, false, borderColor, thickness);
    }
    _isConvex = _isConvexTmp;
}

void DrawNodeEx::_drawTriangle(const Vec2* vertices3, const Color4B& borderColor, const Color4B& fillColor, bool solid, float thickness)
{
    if (thickness == 0.0f) return;

    unsigned int vertex_count = 3;

    if (thickness != 1.0f)
    {
        swapIsConvex(true);
        _drawPolygon(vertices3, vertex_count, Color4B::BLUE, Color4B::BLUE, true, thickness);
        _isConvex = _isConvexTmp;
    }
    else
    {
        Vec2* _vertices = transform(vertices3, vertex_count, false);

        ensureCapacityTriangle(vertex_count);

        V2F_C4B_T2F_Triangle* triangles = (V2F_C4B_T2F_Triangle*)(_bufferTriangle + _bufferCountTriangle);
        triangles[0] = { { _vertices[0], fillColor, Tex2F::ZERO },
            { _vertices[1], fillColor, Tex2F::ZERO },
            { _vertices[2], fillColor, Tex2F::ZERO } };

        _customCommandTriangle.updateVertexBuffer(triangles, _bufferCountTriangle * sizeof(V2F_C4B_T2F),
            vertex_count * sizeof(V2F_C4B_T2F));
        _bufferCountTriangle += vertex_count;
        _dirtyTriangle = true;
        _customCommandTriangle.setVertexDrawInfo(0, _bufferCountTriangle);

        AX_SAFE_DELETE_ARRAY(_vertices);
    }
}

void DrawNodeEx::_drawAStar(const Vec2& center,
    float radiusI, // inner
    float radiusO, // outer
    unsigned int segments,
    const Color4B& color,
    const Color4B& filledColor,
    float thickness,
    bool solid)
{
    const float coef = 2.0f * (float)M_PI / segments;
    float halfAngle = coef / 2.0f;

    auto vertices = _abuf.get<Vec2>(segments * 2 + 1);

    int i = 0;
    for (unsigned int a = 0; a < segments; a++)
    {
        float rads = a * coef;
        vertices[i++] = Vec2(center.x + cos(rads) * radiusO, center.y + sin(rads) * radiusO);
        vertices[i++] = Vec2(center.x + cos(rads + halfAngle) * radiusI, center.y + sin(rads + halfAngle) * radiusI);
    }

    if (solid)
    {
        swapIsConvex(false);
        _drawPolygon(vertices, i, filledColor, color, true, thickness);
        _isConvex = _isConvexTmp;
    }
    else
    {
        vertices[i++] = vertices[0];
        _drawPoly(vertices, i, true, color, thickness);
    }
}

void DrawNodeEx::_drawPoints(const Vec2* position, unsigned int numberOfPoints, const float pointSize, const Color4B& color, const DrawNodeEx::PointType pointType)
{
    if (pointSize == 0) return;

    if (_drawOrder == true)
    {
        Vec2 pointSize4 = Vec2(pointSize / 4, pointSize / 4);
        for (unsigned int i = 0; i < numberOfPoints; i++)
        {
            switch (pointType)
            {
            case PointType::Circle:
                drawSolidCircle(position[i], pointSize, 90, 8, color);
                break;
            case PointType::Rect:
                drawSolidRect(position[i] - pointSize4, position[i] + pointSize4, color);
                break;
            case PointType::Triangle:
                //   drawTriangle(position[i] - pointSize4, position[i] + pointSize4, color);
                break;

            default:
                break;
            }
        }
        return;
    }
    ensureCapacityPoint(numberOfPoints);

    V2F_C4B_T2F* point = _bufferPoint + _bufferCountPoint;
    for (unsigned int i = 0; i < numberOfPoints; i++)
    {
        *(point + i) = { position[i], color, Tex2F(pointSize, 0.0f) };
    }

    _customCommandPoint.updateVertexBuffer(point, _bufferCountPoint * sizeof(V2F_C4B_T2F),
        numberOfPoints * sizeof(V2F_C4B_T2F));
    _bufferCountPoint += numberOfPoints;
    _dirtyPoint = true;
    _customCommandPoint.setVertexDrawInfo(0, _bufferCountPoint);
}

void DrawNodeEx::_drawPoint(const Vec2& position, const float pointSize, const Color4B& color, const DrawNodeEx::PointType pointType)
{
    if (pointSize == 0) return;

    if (_drawOrder == true)
    {
        float pointSize4 = pointSize / 4;
        drawSolidRect(position - Vec2(pointSize4, pointSize4), position + Vec2(pointSize4, pointSize4), color);

    }
    else
    {
        ensureCapacityPoint(1);

        V2F_C4B_T2F* point = _bufferPoint + _bufferCountPoint;
        *point = { position, color, Tex2F(pointSize, 0.0f) };

        _customCommandPoint.updateVertexBuffer(point, _bufferCountPoint * sizeof(V2F_C4B_T2F), sizeof(V2F_C4B_T2F));
        _bufferCountPoint += 1;
        _dirtyPoint = true;
        _customCommandPoint.setVertexDrawInfo(0, _bufferCountPoint);
    }
}


void DrawNodeEx::_drawPie(const Vec2& center,
    float radius,
    float rotation,
    int startAngle,
    int endAngle,
    float scaleX,
    float scaleY,
    const Color4B& fillColor,
    const Color4B& borderColor,
    DrawMode drawMode,
    float thickness)
{
#define DEGREES 360
    bool _circle = false;

    // Not a real line!
    if (startAngle == endAngle)
        return;

    // Its a circle?
    if (MAX((startAngle - endAngle), (endAngle - startAngle)) == DEGREES)
    {
        _circle = true;
    }

    const float coef = 2.0f * (float)M_PI / DEGREES;
    Vec2* vertices = new Vec2[DEGREES + 2]; //_abuf.get<Vec2>(DEGREES + 2);

    int n = 0;
    float rads = 0.0f;
    float _angle = AX_DEGREES_TO_RADIANS(rotation);

    if (startAngle > endAngle)
    {
        std::swap(endAngle, startAngle);
    }

    for (int i = 0; i <= DEGREES; i++)
    {
        if (startAngle <= i && endAngle >= i)
        {
            rads = i * coef;

            float j = radius * cosf(rads + _angle) * scaleX + center.x;
            float k = radius * sinf(rads + _angle) * scaleY + center.y;

            vertices[n].x = j;
            vertices[n].y = k;
            n++;
        }
    }
    switch (drawMode)
    {
    case DrawMode::Fill:
        vertices[n++] = center;
        vertices[n++] = vertices[0];
        if (_circle)
        {
            _drawCircle(center,  radius, 0.0f,  360, false, scaleX, scaleY, borderColor, fillColor,true, thickness);
            break;
        }
        _drawPolygon(vertices, n, fillColor, borderColor, true, thickness);
        break;
    case DrawMode::Outline:
        vertices[n++] = center;
        vertices[n++] = vertices[0];
        if (_circle)
        {
            _drawCircle(center,  radius, 0.0f,  360, false, scaleX, scaleY, borderColor, Color4B::TRANSPARENT, true, thickness);
            break;
        }
        _drawPolygon(vertices, n, Color4B::TRANSPARENT, borderColor, false, thickness);
        break;
    case DrawMode::Line:
        if (_circle)
        {
            _drawCircle(center,  radius, 0.0f,  360, false, scaleX, scaleY, borderColor, Color4B::TRANSPARENT, true, thickness);
            break;
        }
        _drawPolygon(vertices, n - 1, Color4B::TRANSPARENT, borderColor, false, thickness);
        break;
    case DrawMode::Semi:
        if (_circle)
        {
            _drawCircle(center,  radius, 0.0f,  360, false, scaleX, scaleY, borderColor, fillColor,true, thickness);
            break;
        }
        _drawPolygon(vertices, n - 1, fillColor, borderColor, true, thickness);
        break;

    default:
        break;
    }
}

NS_AX_EXT_END

#if defined(_WIN32)
#    pragma pop_macro("TRANSPARENT")
#endif

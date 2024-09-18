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

/*
* Code copied & pasted from SpacePatrol game https://github.com/slembcke/SpacePatrol
*
* Renamed and added some changes for cocos2d
*
*/

#ifndef __DRAW_NODE_EX_H__
#define __DRAW_NODE_EX_H__

#include "2d/Node.h"
#include "base/Types.h"
#include "renderer/CustomCommand.h"
#include "math/Math.h"
#include "base/any_buffer.h"

#include "extensions/ExtensionMacros.h"
#include "extensions/ExtensionExport.h"
#include "axmol.h"

NS_AX_EXT_BEGIN


// New "parameter" rule:
// returnValue func(position, dimension1, dimensionN, colors1, colorsN, stettings1, settingsN, stettings1withDefault, stettingsNwithDefault)


static const int DEFAULT_LINEWIDTH = 2;


/** @class DrawNodeExt
* @brief Node that draws dots, segments and polygons.
* Faster than the "drawing primitives" since they draws everything in one single batch.
* @since v2.1
*/
class AX_EX_DLL DrawNodeEx : public ax::Node
{
public:
    /** Different draw modus types.
    *
    *.
    */
    enum DrawMode
    {
        Fill,
        Outline,
        Line,
        Semi,
    };

    enum PointType
    {
        Circle,
        Rect,
        Triangle,
    };

    // See also example on https://www.angusj.com/clipper2/Docs/Units/Clipper/Types/EndType.htm
    enum EndType
    {
        Square,
        Round,
        Butt,
    };

    enum class Version
    {
        v0,  // DrawNode cocos2dx/axmol 1.0
        v1,  // DrawNode 1.0
        v2,  // DrawNode 2.0 (aka DrawNodeEx)
        v3,
        v4,
    };


    // Internal function _drawPoint
    void _drawPoint(const Vec2& position, const float pointSize, const Color4B& color, const DrawNodeEx::PointType pointType);

    // Internal function _drawPoints
    void _drawPoints(const Vec2* position, unsigned int numberOfPoints, const float pointSize, const Color4B& color, const DrawNodeEx::PointType pointType);

    // Internal function _drawDot
    void _drawDot(const Vec2& pos, float radius, const Color4B& color);

    // Internal function _drawTriangle
    void _drawTriangle(const Vec2* vertices3, const Color4B& borderColor, const Color4B& fillColor, bool solid = true, float thickness = 0.0f);

    // Internal function _drawAStar
    void _drawAStar(const Vec2& center,
        float radiusI,
        float radiusO,
        unsigned int segments,
        const Color4B& color,
        const Color4B& filledColor,
        float thickness = 1.0f,
        bool solid = false);

    // Internal function _drawPoly
    void _drawPoly(const ax::Vec2* poli,
        unsigned int numberOfPoints,
        bool closedPolygon,
        const ax::Color4B& color,
        float thickness = 1.0f);

    // Internal function _drawPolygon
    void _drawPolygon(const ax::Vec2* verts,
        unsigned int count,
        const ax::Color4B& fillColor,
        const ax::Color4B& borderColor,
        bool closedPolygon = true,
        float thickness = 1.0f);

    // Internal function _drawSegment
    void _drawSegment(const ax::Vec2& origin,
        const ax::Vec2& destination,
        const ax::Color4B& color,
        float thickness = 1.0f,
        DrawNodeEx::EndType etStart = DrawNodeEx::EndType::Square,
        DrawNodeEx::EndType etEnd = DrawNodeEx::EndType::Square);

    // Internal function _drawCircle
    void _drawCircle(const Vec2& center,
        float radius,
        float angle,
        unsigned int segments,
        bool drawLineToCenter,
        float scaleX,
        float scaleY,
        const Color4B& borderColor,
        const Color4B& fillColor,
        bool solid,
        float thickness = 1.0f);

    // Internal function _drawPie
    void _drawPie(const Vec2& center,
        float radius,
        float rotation,
        int startAngle,
        int endAngle,
        float scaleX,
        float scaleY,
        const Color4B& fillColor,
        const Color4B& borderColor,
        DrawMode drawMode,
        float thickness);

    /** Set backwards compatible with cocos2dx/axmol 2.0  
    *
    * @param isConvex swap the '_isConvex' flag.
    * @js NA
    */
    bool swapIsConvex(bool isConvex) {
        _isConvexTmp = _isConvex; _isConvex = isConvex; return _isConvexTmp;
    };
    /** Set backwards compatible with cocos2dx/axmol 2.0
    *
    * @param isConvex Polygons be convex (true).
    * @js NA
    */
    void setIsConvex(bool isConvex) {
        _isConvex = isConvex;
    };

    /** Get the actual DrawNode version
    * @js NA
    */
    const ax::extension::DrawNodeEx::Version getDNVersion() {
        return _dnVersion;
    };

    /** Set the DrawNode draw order
    *
    * @param drawOrder. true/false = On/Off

    * @js NA
    */
    void setDNDrawOrder(bool drawOrder) {
        _drawOrder = drawOrder;
    };

    /** Swap the _drawOrder flag.
    *
    * @js NA
    */
    bool swapDNDrawOrder() {
        return _drawOrder = (_drawOrder) ? false : true;
    };

    /** Set the DrawNode transformation
    *
    * @param transform. true/false = On/Off
    *
    * @js NA
    */
    void setDNTransform(bool transform) {
        _dnTransform = transform;
    };

    /** Swap the _dnTransform flag.
    *
    * @js NA
    */
    bool swapDNTransform() {
        return _dnTransform = (_dnTransform) ? false: true;
    };

    /** Set the DrawNode scale for each drawing primitive after this.

    * @js NA
    */
    void setDNScale(ax::Vec2 scale) {
        _dnScale = scale;
    };

    /** Set the DrawNode scaleX for each drawing primitive after this.

    * @js NA
    */
    void setDNScaleX(float scaleX) {
        _dnScale.x = scaleX;
    };

    /** Set the DrawNode scaleY for each drawing primitive after this.

    * @js NA
    */
    void setDNScaleY(float scaleY) {
        _dnScale.y = scaleY;
    };

    /** Set the DrawNode rotation for each drawing primitive after this.

    * @js NA
    */
    void setDNRotation(float rotation) {
        _dnRotation = rotation;
    };

    /** Get the DrawNode rotation for each drawing primitive after this.

    * @js NA
    */
    float getDNRotation() {
        return _dnRotation;
    };

    /** Set the DrawNode center of rotation for each drawing primitive after this.

    * @js NA
    */
    void setDNCenter(ax::Vec2 center) {
        _dnCenter = center;
    };

    /** Get the DrawNode center of rotation for each drawing primitive after this.

    * @js NA
    */
    ax::Vec2 getDNCenter() {
        return _dnCenter;
    };

    /** Set the DrawNode position for each drawing primitive after this.

    * @js NA
    */
    void setDNPosition(ax::Vec2 position) {
        _dnPosition = position;
    };

    /** Get the DrawNode position for drawing primitive.

    * @js NA
    */
    ax::Vec2 getDNPosition() {
        return _dnPosition;
    };

    /** Set the DrawNode line width for each drawing primitive after this.

    * @js NA
    */
    void setDNLineWidth(float lineWidth) {
        _dnLineWidth = lineWidth;
    };

    /** Get the DrawNode line width for each drawing primitive after this.

    * @js NA
    */
    float getDNLineWidth() {
        return _dnLineWidth;
    };


    /** DrawNode transform method.
    *
    * @param vertices A Vec2 vertices list.
    * @param count The number of vertices.
    * @param closedPolygon The closedPolygon flag.
    * @js NA
    */
    ax::Vec2* transform(const ax::Vec2* vertices, unsigned int& count, bool closedPolygon = false);

    /** Reset all DrawNode properties.

    * @js NA
    */
    void resetDNValues()
    {
        _dnScale = Vec2(1.f, 1.f);
        _dnCenter = Vec2(0.f, 0.f);
        _dnRotation = 0.0f;
        _dnPosition = Vec2(0.f, 0.f);
        _dnLineWidth = 1.0f;
    };

    /** Ensure the DrawNode properties.

    * @js NA
    */
    void ensureDNValues()
    {
        _dnScaleTmp = _dnScale;
        _dnCenterTmp = _dnCenter;
        _dnRotationTmp = _dnRotation;
        _dnPositionTmp = _dnPosition;
        _dnLineWidthTmp = _dnLineWidth;
    };

    /** Restore the DrawNode properties from last ensureDNValues() call.

    * @js NA
    */
    void restoreDNValues()
    {
        _dnTransform = true;
        _dnScale = _dnScaleTmp;
        _dnCenter = _dnCenterTmp;
        _dnRotation = _dnRotationTmp;
        _dnPosition = _dnPositionTmp;
        _dnLineWidth = _dnLineWidthTmp;
    };

    /** creates and initialize a DrawNodeExt node.
    *
    * @return Return an autorelease object.
    */
    static DrawNodeEx* create(float defaultLineWidth = DEFAULT_LINEWIDTH);
    // DrawNodeEx();

     /** Draw a point.
     *
     * @param point A Vec2 used to point.
     * @param pointSize The point size.
     * @param color The point color.
     * @js NA
     */
    void drawPoint(const ax::Vec2& point, const float pointSize, const ax::Color4B& color, DrawNodeEx::PointType pointType = DrawNodeEx::PointType::Rect);

    /** Draw a group point.
    *
    * @param position A Vec2 pointer.
    * @param numberOfPoints The number of points.
    * @param color The point color.
    * @js NA
    */
    void drawPoints(const ax::Vec2* position, unsigned int numberOfPoints, const ax::Color4B& color, DrawNodeEx::PointType pointType = DrawNodeEx::PointType::Rect);

    /** Draw a group point.
    *
    * @param position A Vec2 pointer.
    * @param numberOfPoints The number of points.
    * @param pointSize The point size.
    * @param color The point color.
    * @js NA
    */
    void drawPoints(const ax::Vec2* position,
        unsigned int numberOfPoints,
        const float pointSize,
        const ax::Color4B& color, DrawNodeEx::PointType pointType = DrawNodeEx::PointType::Rect);

    /** Draw an line from origin to destination with color.
    *
    * @param origin The line origin.
    * @param destination The line destination.
    * @param color The line color.
    * @js NA
    */
    void drawLine(const ax::Vec2& origin,
        const ax::Vec2& destination,
        const ax::Color4B& color,
        float thickness = 1.0f,
        DrawNodeEx::EndType etStart = DrawNodeEx::EndType::Round,
        DrawNodeEx::EndType etEnd = DrawNodeEx::EndType::Round);

    /** Draws a rectangle given the origin and destination point measured in points.
    * The origin and the destination can not have the same x and y coordinate.
    *
    * @param origin The rectangle origin.
    * @param destination The rectangle destination.
    * @param color The rectangle color.
    */
    void drawRect(const ax::Vec2& origin, const ax::Vec2& destination, const ax::Color4B& color, float thickness = 1.0f);

    /** Draws a polygon given a pointer to point coordinates and the number of vertices measured in points.
    * The polygon can be closed or open.
    *
    * @param poli A pointer to point coordinates.
    * @param numberOfPoints The number of vertices measured in points.
    * @param closePolygon The polygon can be closed or open.
    * @param color The polygon color.
    */
    void drawPoly(const ax::Vec2* poli,
        unsigned int numberOfPoints,
        bool closedPolygon,
        const ax::Color4B& color,
        float thickness = 1.0f);



    /** Draws a circle given the center, radius and number of segments.
    *
    * @param center The circle center point.
    * @param radius The circle rotate of radius.
    * @param angle  The circle angle.
    * @param segments The number of segments.
    * @param drawLineToCenter Whether or not draw the line from the origin to center.
    * @param scaleX The scale value in x.
    * @param scaleY The scale value in y.
    * @param color Set the circle color.
    * @param thickness  (default 1.0f)
    */
    void drawCircle(const ax::Vec2& center,
        float radius,
        float angle,
        unsigned int segments,
        bool drawLineToCenter,
        float scaleX,
        float scaleY,
        const ax::Color4B& color,
        float thickness = 1.0f);  // 500 should "simulate/save" the backwards compatibility

    /** Draws a circle given the center, radius and number of segments.
    *
    * @param center The circle center point.
    * @param radius The circle rotate of radius.
    * @param angle  The circle angle.
    * @param segments The number of segments.
    * @param drawLineToCenter Whether or not draw the line from the origin to center.
    * @param color Set the circle color.
    * @param thickness  (default 1.0f)
    */
    void drawCircle(const Vec2& center,
        float radius,
        float angle,
        unsigned int segments,
        bool drawLineToCenter,
        const Color4B& color,
        float thickness = 1.0f);  // 500 should "simulate/save" the backwards compatibility

    /** Draws a star given the center, radiusI, radiusO and number of segments.
    *
    * @param center The circle center point.
    * @param radiusI The inner radius.
    * @param radiusO The outer radius.
    * @param segments The number of segments.
    * @param color Set the star color.
    * @param thickness  (default = 1.0f)
    */
    void drawStar(const Vec2& center,
        float radiusI,
        float radiusO,
        unsigned int segments,
        const Color4B& color,
        float thickness = 1.0f);

    /** Draws a solid star given the center, radiusI, radiusO and number of segments.
    *
    * @param center The circle center point.
    * @param radiusI The inner radius.
    * @param radiusO The outer radius.
    * @param segments The number of segments.
    * @param color Set the star color.
    * @param thickness  (default = 1.0f)
    */
    void drawSolidStar(const Vec2& center,
        float radiusI,
        float radiusO,
        unsigned int segments,
        const Color4B& color,
        const Color4B& filledColor,
        float thickness = 1.0f);



    /** Draws a quad bezier path.
    *
    * @param origin The origin of the bezier path.
    * @param control The control of the bezier path.
    * @param destination The destination of the bezier path.
    * @param segments The number of segments.
    * @param color Set the quad bezier color.
    */
    void drawQuadBezier(const ax::Vec2& origin,
        const ax::Vec2& control,
        const ax::Vec2& destination,
        unsigned int segments,
        const ax::Color4B& color,
        float thickness = 1.0f);

    /** Draw a cubic bezier curve with color and number of segments
    *
    * @param origin The origin of the bezier path.
    * @param control1 The first control of the bezier path.
    * @param control2 The second control of the bezier path.
    * @param destination The destination of the bezier path.
    * @param segments The number of segments.
    * @param color Set the cubic bezier color.
    */
    void drawCubicBezier(const ax::Vec2& origin,
        const ax::Vec2& control1,
        const ax::Vec2& control2,
        const ax::Vec2& destination,
        unsigned int segments,
        const ax::Color4B& color,
        float thickness = 1.0f);

    /** Draws a Cardinal Spline path.
    *
    * @param config A array point.
    * @param tension The tension of the spline.
    * @param segments The number of segments.
    * @param color Set the Spline color.
    */
    void drawCardinalSpline(ax::PointArray* config,
        float tension,
        unsigned int segments,
        const ax::Color4B& color,
        float thickness = 1.0f);

    /** Draws a Catmull Rom path.
    *
    * @param points A point array  of control point.
    * @param segments The number of segments.
    * @param color The Catmull Rom color.
    */
    void drawCatmullRom(ax::PointArray* points,
        unsigned int segments,
        const ax::Color4B& color,
        float thickness = 1.0f);

    /** draw a dot at a position, with a given radius and color.
    *
    * @param pos The dot center.
    * @param radius The dot radius.
    * @param color The dot color.
    */
    void drawDot(const ax::Vec2& pos, float radius, const ax::Color4B& color);

    /** Draws a rectangle with 4 points.
    *
    * @param p1 The rectangle vertex point.
    * @param p2 The rectangle vertex point.
    * @param p3 The rectangle vertex point.
    * @param p4 The rectangle vertex point.
    * @param color The rectangle color.
    */
    void drawRect(const ax::Vec2& p1,
        const ax::Vec2& p2,
        const ax::Vec2& p3,
        const ax::Vec2& p4,
        const ax::Color4B& color,
        float thickness = 1.0f);

    /** Draws a solid rectangle given the origin and destination point measured in points.
    * The origin and the destination can not have the same x and y coordinate.
    *
    * @param origin The rectangle origin.
    * @param destination The rectangle destination.
    * @param color The rectangle color.
    * @js NA
    */
    void drawSolidRect(const ax::Vec2& origin, const ax::Vec2& destination, const ax::Color4B& color, float thickness = 0, const ax::Color4B& borderColor = ax::Color4B(0, 0, 0, 0));

    /** Draws a solid polygon given a pointer to CGPoint coordinates, the number of vertices measured in points, and a
    * color.
    *
    * @param poli A solid polygon given a pointer to CGPoint coordinates.
    * @param numberOfPoints The number of vertices measured in points.
    * @param color The solid polygon color.
    * @js NA
    */
    void drawSolidPoly(const ax::Vec2* poli, unsigned int numberOfPoints, const ax::Color4B& color, float thickness = 0, const ax::Color4B& borderColor = ax::Color4B(0, 0, 0, 0));

    /** Draws a solid circle given the center, radius and number of segments.
    * @param center The circle center point.
    * @param radius The circle rotate of radius.
    * @param angle  The circle angle.
    * @param segments The number of segments.
    * @param scaleX The scale value in x.
    * @param scaleY The scale value in y.
    * @param fillColor The color will fill in polygon.
    * @param thickness The border of line width.
    * @param borderColor The border of line color.
    * @js NA
    */
    void drawSolidCircle(const ax::Vec2& center,
        float radius,
        float angle,
        unsigned int segments,
        float scaleX,
        float scaleY,
        const ax::Color4B& fillColor,
        float thickness,
        const ax::Color4B& borderColor);

    /** Draws a solid circle given the center, radius and number of segments.
    * @param center The circle center point.
    * @param radius The circle rotate of radius.
    * @param angle  The circle angle.
    * @param segments The number of segments.
    * @param scaleX The scale value in x.
    * @param scaleY The scale value in y.
    * @param color The solid circle color.
    * @js NA
    */
    void drawSolidCircle(const ax::Vec2& center,
        float radius,
        float angle,
        unsigned int segments,
        float scaleX,
        float scaleY,
        const ax::Color4B& color);

    /** Draws a solid circle given the center, radius and number of segments.
    * @param center The circle center point.
    * @param radius The circle rotate of radius.
    * @param angle  The circle angle.
    * @param segments The number of segments.
    * @param color The solid circle color.
    * @js NA
    */
    void drawSolidCircle(const ax::Vec2& center,
        float radius,
        float angle,
        unsigned int segments,
        const ax::Color4B& color);

    /** Draws a pie given the center, radius, angle, start angle, end angle  and number of segments.
    * @param center The circle center point.
    * @param radius The circle rotate of radius.
    * @param angle  The circle angle.
    * @param startAngle The start angle.
    * @param endAngle The end angle.
    * @param scaleX The scale value in x.
    * @param scaleY The scale value in y.
    * @param color The solid circle color.
    * @param DrawMode The draw mode
    * @js NA
    */
    void drawPie(const ax::Vec2& center,
        float radius,
        float rotation,
        int startAngle,
        int endAngle,
        float scaleX,
        float scaleY,
        const ax::Color4B& fillColor,
        const ax::Color4B& borderColor,
        DrawMode drawMode,
        float thickness = 1.0f);

    /** draw a segment with a radius and color.
    *
    * @param from The segment origin.
    * @param to The segment destination.
    * @param radius The segment radius.
    * @param color The segment color.
    * @param etStart The segment first DrawNodeEx::EndType.
    * @param etEnd The segment last DrawNodeEx::EndType.
    */
    void drawSegment(const ax::Vec2& from, const ax::Vec2& to, float radius, const ax::Color4B& color,
        DrawNodeEx::EndType etStart = DrawNodeEx::EndType::Round, DrawNodeEx::EndType etEnd = DrawNodeEx::EndType::Round);

    /** draw a polygon with a fill color and line color
    * @code
    * When this function bound into js or lua,the parameter will be changed
    * In js: var drawPolygon(var Arrayofpoints, var fillColor, var width, var borderColor)
    * In lua:local drawPolygon(local pointTable,local tableCount,local fillColor,local width,local borderColor)
    * @endcode
    * @param verts A pointer to point coordinates.
    * @param count The number of verts measured in points.
    * @param fillColor The color will fill in polygon.
    * @param thickness The border of line width.
    * @param borderColor The border of line color.
    * @js NA
    */
    void drawPolygon(ax::Vec2* verts,
        int count,
        const ax::Color4B& fillColor,
        float thickness,
        const ax::Color4B& borderColor);

    void drawPolygon(ax::Vec2* verts, int count, float thickness, const ax::Color4B& borderColor);
    void drawSolidPolygon(ax::Vec2* verts,
        int count,
        const ax::Color4B& fillColor,
        float thickness,
        const ax::Color4B& borderColor);



    /** draw a triangle with color.
    *
    * @param p1 The triangle vertex point.
    * @param p2 The triangle vertex point.
    * @param p3 The triangle vertex point.
    * @param color The triangle color.
    * @js NA
    */

    void drawTriangle(const Vec2* vertices3,
        const ax::Color4B& fillColor,
        const ax::Color4B& borderColor,
        float thickness = 1.0f);

    void drawTriangle(const ax::Vec2& p1,
        const ax::Vec2& p2,
        const ax::Vec2& p3,
        const ax::Color4B& color,
        float thickness = 1.0f);

    void drawSolidTriangle(const Vec2* vertices3,
        const ax::Color4B& fillColor,
        const ax::Color4B& borderColor,
        float thickness = 1.0f);

    void drawSolidTriangle(const ax::Vec2& p1,
        const ax::Vec2& p2,
        const ax::Vec2& p3,
        const ax::Color4B& fillColor,
        const ax::Color4B& borderColor,
        float thickness = 1.0f);

    /** Clear the geometry in the node's buffer. */
    void clear();
    /** Get the color mixed mode.
    * @lua NA
    */
    const ax::BlendFunc& getBlendFunc() const;
    /** Set the color mixed mode.
    * @code
    * When this function bound into js or lua,the parameter will be changed
    * In js: var setBlendFunc(var src, var dst)
    * @endcode
    * @lua NA
    */
    void setBlendFunc(const ax::BlendFunc& blendFunc);

    // Overrides
    virtual void draw(ax::Renderer* renderer, const ax::Mat4& transform, uint32_t flags) override;

    virtual void visit(ax::Renderer* renderer, const ax::Mat4& parentTransform, uint32_t parentFlags) override;

    void setLineWidth(float lineWidth);
    // Get CocosStudio guide lines width.
    float getLineWidth();

    /**
    * When isolated is set, the position of the node is no longer affected by parent nodes.
    * Which means it will be drawn just like a root node.
    */
    void setIsolated(bool isolated) {
        _isolated = isolated;
    }

    bool isIsolated() const {
        return _isolated;
    }

    DrawNodeEx(float lineWidth = DEFAULT_LINEWIDTH);
    virtual ~DrawNodeEx();
    virtual bool init() override;

protected:
    void ensureCapacityTriangle(int count);
    void ensureCapacityPoint(int count);
    void ensureCapacityLine(int count);

    void updateShader();
    void updateShaderInternal(ax::CustomCommand& cmd,
        uint32_t programType,
        ax::CustomCommand::DrawType drawType,
        ax::CustomCommand::PrimitiveType primitiveType);
    void freeShaderInternal(ax::CustomCommand& cmd);

    void setVertexLayout(ax::CustomCommand& cmd);

    void updateBlendState(ax::CustomCommand& cmd);
    void updateUniforms(const ax::Mat4& transform, ax::CustomCommand& cmd);

    int _bufferCapacityTriangle = 0;
    int _bufferCountTriangle = 0;
    ax::V2F_C4B_T2F* _bufferTriangle = nullptr;
    ax::CustomCommand _customCommandTriangle;
    bool _dirtyTriangle = false;

    int _bufferCapacityPoint = 0;
    int _bufferCountPoint = 0;
    ax::V2F_C4B_T2F* _bufferPoint = nullptr;
    ax::Color4F _pointColor;
    int _pointSize = 0;

    int _bufferCapacityLine = 0;
    int _bufferCountLine = 0;
    ax::V2F_C4B_T2F* _bufferLine = nullptr;


    ax::CustomCommand _customCommandPoint;
    ax::CustomCommand _customCommandLine;
    bool _dirtyPoint = false;
    bool _dirtyLine = false;

    ax::BlendFunc _blendFunc;

    bool _isolated = false;
    float _lineWidth = 0.0f;
    float _defaultLineWidth = 0.0f;

    ax::any_buffer _abuf;
    ax::any_buffer _abufTransform;


    // DrawNode V2
    const Version _dnVersion = Version::v2;
    float _dnFactor = 0.5f;  /// set the lineWith like Axmol 1.0

    // transforming stuff
    ax::Vec2 _dnScale;
    ax::Vec2 _dnScaleTmp = _dnScale;

    ax::Vec2 _dnCenter;
    ax::Vec2 _dnCenterTmp = _dnCenter;

    float _dnRotation = 0.0f;
    float _dnRotationTmp = _dnRotation;

    ax::Vec2 _dnPosition;
    ax::Vec2 _dnPositionTmp = _dnPosition;

    // Thickness stuff
    float _dnLineWidth = 0.0f;
    float _dnLineWidthTmp = _dnLineWidth;
    float _dnDefaultLineWidth = 0.0f;


    // Filling stuff
    bool _isConvex = true;
    bool _isConvexTmp = _isConvex;

    // Drawing flags
    bool  _dnTransform = true;
    bool  _dnTransformTemp = _dnTransform;

    bool _drawOrder = false;
    bool _drawOrderTemp = _drawOrder;

private:
    AX_DISALLOW_COPY_AND_ASSIGN(DrawNodeEx);
};
/** @} */
NS_AX_EXT_END

#endif  // __DRAW_NODE_EX_H__

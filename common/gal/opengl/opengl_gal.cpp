/*
 * This program source code file is part of KICAD, a free EDA CAD application.
 *
 * Copyright (C) 2012 Torsten Hueter, torstenhtr <at> gmx.de
 * Copyright (C) 2012-2018 Kicad Developers, see AUTHORS.txt for contributors.
 * Copyright (C) 2013-2017 CERN
 * @author Maciej Suminski <maciej.suminski@cern.ch>
 *
 * Graphics Abstraction Layer (GAL) for OpenGL
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include <gal/opengl/opengl_gal.h>
#include <gal/opengl/utils.h>
#include <gal/definitions.h>
#include <gl_context_mgr.h>
#include <geometry/shape_poly_set.h>
#include <text_utils.h>
#include <bitmap_base.h>

#include <macros.h>

#ifdef __WXDEBUG__
#include <profile.h>
#include <wx/log.h>
#endif /* __WXDEBUG__ */

#include <limits>
#include <functional>
using namespace std::placeholders;


using namespace KIGFX;


// The current font is "Ubuntu Mono" available under Ubuntu Font Licence 1.0
// (see ubuntu-font-licence-1.0.txt for details)
#include "gl_resources.h"
#include "gl_builtin_shaders.h"
using namespace KIGFX::BUILTIN_FONT;

static void InitTesselatorCallbacks( GLUtesselator* aTesselator );
static const int glAttributes[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 8, 0 };

wxGLContext* OPENGL_GAL::glMainContext = NULL;
int OPENGL_GAL::instanceCounter = 0;
GLuint OPENGL_GAL::fontTexture = 0;
bool OPENGL_GAL::isBitmapFontLoaded = false;
SHADER* OPENGL_GAL::shader = NULL;

namespace KIGFX {
class GL_BITMAP_CACHE
{
public:
    GL_BITMAP_CACHE()
    {
    }

    ~GL_BITMAP_CACHE();

    GLuint RequestBitmap( const BITMAP_BASE* aBitmap );

private:

    struct CACHED_BITMAP
    {
        GLuint id;
        int w, h;
    };

    GLuint cacheBitmap( const BITMAP_BASE* aBitmap );

    std::map<const BITMAP_BASE*, CACHED_BITMAP> m_bitmaps;
};

};

GL_BITMAP_CACHE::~GL_BITMAP_CACHE()
{
    for ( auto b = m_bitmaps.begin(); b != m_bitmaps.end(); ++b )
    {
        glDeleteTextures( 1, &b->second.id );
    }
}

GLuint GL_BITMAP_CACHE::RequestBitmap( const BITMAP_BASE* aBitmap )
{
    auto it = m_bitmaps.find( aBitmap) ;

    if ( it != m_bitmaps.end() )
    {
        return it->second.id;
    }
    else
    {
        return cacheBitmap( aBitmap );
    }
}


GLuint GL_BITMAP_CACHE::cacheBitmap( const BITMAP_BASE* aBitmap )
{
    CACHED_BITMAP bmp;

    bmp.w = aBitmap->GetSizePixels().x;
    bmp.h = aBitmap->GetSizePixels().y;

    // There are draw issues (incorrect rendering) with some w values.
    // It happens when the w value is not a multiple of 4
    // so we use only a sub image with a modified width
    bmp.w -= bmp.w % 4;

    GLuint textureID;
    glGenTextures(1, &textureID);

    uint8_t *buf = new uint8_t [ bmp.w * bmp.h * 3];
    auto imgData = const_cast<BITMAP_BASE*>( aBitmap )->GetImageData();

    for( int y = 0; y < bmp.h; y++ )
    {
        for( int x = 0; x < bmp.w; x++ )
        {
            uint8_t *p = buf + ( bmp.w * y + x ) * 3;

            p[0] = imgData->GetRed( x, y );
            p[1] = imgData->GetGreen( x, y );
            p[2] = imgData->GetBlue( x, y );
        }
    }

    glBindTexture( GL_TEXTURE_2D, textureID );

    glTexImage2D( GL_TEXTURE_2D, 0,GL_RGB, bmp.w, bmp.h, 0, GL_RGB, GL_UNSIGNED_BYTE, buf );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

    delete [] buf;

    bmp.id = textureID;

    m_bitmaps[ aBitmap ] = bmp;

    return textureID;
}

OPENGL_GAL::OPENGL_GAL( GAL_DISPLAY_OPTIONS& aDisplayOptions, wxWindow* aParent,
                        wxEvtHandler* aMouseListener, wxEvtHandler* aPaintListener,
                        const wxString& aName ) :
    GAL( aDisplayOptions ),
    HIDPI_GL_CANVAS( aParent, wxID_ANY, (int*) glAttributes, wxDefaultPosition, wxDefaultSize,
                wxEXPAND, aName ),
    mouseListener( aMouseListener ), paintListener( aPaintListener ), currentManager( nullptr ),
    cachedManager( nullptr ), nonCachedManager( nullptr ), overlayManager( nullptr ), mainBuffer( 0 ), overlayBuffer( 0 )
{
// IsDisplayAttr() handles WX_GL_{MAJOR,MINOR}_VERSION correctly only in 3.0.4
// starting with 3.1.0 one should use wxGLContext::IsOk() (done by GL_CONTEXT_MANAGER)
#if wxCHECK_VERSION( 3, 0, 3 ) and !wxCHECK_VERSION( 3, 1, 0 )
    const int attr[] = { WX_GL_MAJOR_VERSION, 2, WX_GL_MINOR_VERSION, 1, 0 };

    if( !IsDisplaySupported( attr ) )
        throw std::runtime_error( "OpenGL 2.1 or higher is required!" );
#endif /* wxCHECK_VERSION( 3, 0, 3 ) */

    if( glMainContext == NULL )
    {
        glMainContext = GL_CONTEXT_MANAGER::Get().CreateCtx( this );

        if( !glMainContext )
            throw std::runtime_error( "Could not create the main OpenGL context" );

        glPrivContext = glMainContext;
        shader = new SHADER();
    }
    else
    {
        glPrivContext = GL_CONTEXT_MANAGER::Get().CreateCtx( this, glMainContext );

        if( !glPrivContext )
            throw std::runtime_error( "Could not create a private OpenGL context" );
    }

    ++instanceCounter;

    bitmapCache.reset( new GL_BITMAP_CACHE );

    compositor = new OPENGL_COMPOSITOR;
    compositor->SetAntialiasingMode( options.gl_antialiasing_mode );

    // Initialize the flags
    isFramebufferInitialized = false;
    isBitmapFontInitialized  = false;
    isInitialized            = false;
    isGrouping               = false;
    groupCounter             = 0;

    // Connecting the event handlers
    Connect( wxEVT_PAINT,           wxPaintEventHandler( OPENGL_GAL::onPaint ) );

    // Mouse events are skipped to the parent
    Connect( wxEVT_MOTION,          wxMouseEventHandler( OPENGL_GAL::skipMouseEvent ) );
    Connect( wxEVT_LEFT_DOWN,       wxMouseEventHandler( OPENGL_GAL::skipMouseEvent ) );
    Connect( wxEVT_LEFT_UP,         wxMouseEventHandler( OPENGL_GAL::skipMouseEvent ) );
    Connect( wxEVT_LEFT_DCLICK,     wxMouseEventHandler( OPENGL_GAL::skipMouseEvent ) );
    Connect( wxEVT_MIDDLE_DOWN,     wxMouseEventHandler( OPENGL_GAL::skipMouseEvent ) );
    Connect( wxEVT_MIDDLE_UP,       wxMouseEventHandler( OPENGL_GAL::skipMouseEvent ) );
    Connect( wxEVT_MIDDLE_DCLICK,   wxMouseEventHandler( OPENGL_GAL::skipMouseEvent ) );
    Connect( wxEVT_RIGHT_DOWN,      wxMouseEventHandler( OPENGL_GAL::skipMouseEvent ) );
    Connect( wxEVT_RIGHT_UP,        wxMouseEventHandler( OPENGL_GAL::skipMouseEvent ) );
    Connect( wxEVT_RIGHT_DCLICK,    wxMouseEventHandler( OPENGL_GAL::skipMouseEvent ) );
    Connect( wxEVT_MOUSEWHEEL,      wxMouseEventHandler( OPENGL_GAL::skipMouseEvent ) );
#if wxCHECK_VERSION( 3, 1, 0 ) || defined( USE_OSX_MAGNIFY_EVENT )
    Connect( wxEVT_MAGNIFY,         wxMouseEventHandler( OPENGL_GAL::skipMouseEvent ) );
#endif
#if defined _WIN32 || defined _WIN64
    Connect( wxEVT_ENTER_WINDOW,    wxMouseEventHandler( OPENGL_GAL::skipMouseEvent ) );
#endif

    SetSize( aParent->GetClientSize() );
    screenSize = VECTOR2I( aParent->GetClientSize() );

    // Grid color settings are different in Cairo and OpenGL
    SetGridColor( COLOR4D( 0.8, 0.8, 0.8, 0.1 ) );
    SetAxesColor( COLOR4D( BLUE ) );

    // Tesselator initialization
    tesselator = gluNewTess();
    InitTesselatorCallbacks( tesselator );

    if( tesselator == NULL )
        throw std::runtime_error( "Could not create the tesselator" );

    gluTessProperty( tesselator, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_POSITIVE );

    SetTarget( TARGET_NONCACHED );
}


OPENGL_GAL::~OPENGL_GAL()
{
    GL_CONTEXT_MANAGER::Get().LockCtx( glPrivContext, this );

    --instanceCounter;
    glFlush();
    gluDeleteTess( tesselator );
    ClearCache();

    delete compositor;

    if( isInitialized )
    {
        delete cachedManager;
        delete nonCachedManager;
        delete overlayManager;
    }

    GL_CONTEXT_MANAGER::Get().UnlockCtx( glPrivContext );

    // If it was the main context, then it will be deleted
    // when the last OpenGL GAL instance is destroyed (a few lines below)
    if( glPrivContext != glMainContext )
        GL_CONTEXT_MANAGER::Get().DestroyCtx( glPrivContext );

    // Are we destroying the last GAL instance?
    if( instanceCounter == 0 )
    {
        GL_CONTEXT_MANAGER::Get().LockCtx( glMainContext, this );

        if( isBitmapFontLoaded )
        {
            glDeleteTextures( 1, &fontTexture );
            isBitmapFontLoaded = false;
        }

        delete shader;

        GL_CONTEXT_MANAGER::Get().UnlockCtx( glMainContext );
        GL_CONTEXT_MANAGER::Get().DestroyCtx( glMainContext );
        glMainContext = NULL;
    }
}


bool OPENGL_GAL::updatedGalDisplayOptions( const GAL_DISPLAY_OPTIONS& aOptions )
{
    bool refresh = false;

    if( options.gl_antialiasing_mode != compositor->GetAntialiasingMode() )
    {
        compositor->SetAntialiasingMode( options.gl_antialiasing_mode );
        isFramebufferInitialized = false;
        refresh = true;
    }

    if( super::updatedGalDisplayOptions( aOptions ) || refresh )
    {
        Refresh();
        refresh = true;
    }

    return refresh;
}


void OPENGL_GAL::BeginDrawing()
{
    if( !IsShownOnScreen() || GetClientRect().IsEmpty() )
        return;

#ifdef __WXDEBUG__
    PROF_COUNTER totalRealTime( "OPENGL_GAL::BeginDrawing()", true );
#endif /* __WXDEBUG__ */

    if( !isInitialized )
        init();

    GL_CONTEXT_MANAGER::Get().LockCtx( glPrivContext, this );

    // Set up the view port
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    // Create the screen transformation (Do the RH-LH conversion here)
    glOrtho( 0, (GLint) screenSize.x, (GLsizei) screenSize.y, 0, -depthRange.x, -depthRange.y );

    if( !isFramebufferInitialized )
    {
        try
        {
            // Prepare rendering target buffers
            compositor->Initialize();
            mainBuffer = compositor->CreateBuffer();
            overlayBuffer = compositor->CreateBuffer();
        }
        catch( std::runtime_error& )
        {
            GL_CONTEXT_MANAGER::Get().UnlockCtx( glPrivContext );
            throw;      // DRAW_PANEL_GAL will handle it
        }

        isFramebufferInitialized = true;
    }

    compositor->Begin();

    // Disable 2D Textures
    glDisable( GL_TEXTURE_2D );

    glShadeModel( GL_FLAT );

    // Enable the depth buffer
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LESS );

    // Setup blending, required for transparent objects
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glMatrixMode( GL_MODELVIEW );

    // Set up the world <-> screen transformation
    ComputeWorldScreenMatrix();
    GLdouble matrixData[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
    matrixData[0]   = worldScreenMatrix.m_data[0][0];
    matrixData[1]   = worldScreenMatrix.m_data[1][0];
    matrixData[2]   = worldScreenMatrix.m_data[2][0];
    matrixData[4]   = worldScreenMatrix.m_data[0][1];
    matrixData[5]   = worldScreenMatrix.m_data[1][1];
    matrixData[6]   = worldScreenMatrix.m_data[2][1];
    matrixData[12]  = worldScreenMatrix.m_data[0][2];
    matrixData[13]  = worldScreenMatrix.m_data[1][2];
    matrixData[14]  = worldScreenMatrix.m_data[2][2];
    glLoadMatrixd( matrixData );

    // Set defaults
    SetFillColor( fillColor );
    SetStrokeColor( strokeColor );

    // Remove all previously stored items
    nonCachedManager->Clear();
    overlayManager->Clear();

    cachedManager->BeginDrawing();
    nonCachedManager->BeginDrawing();
    overlayManager->BeginDrawing();

    if( !isBitmapFontInitialized )
    {
        // Keep bitmap font texture always bound to the second texturing unit
        const GLint FONT_TEXTURE_UNIT = 2;

        // Either load the font atlas to video memory, or simply bind it to a texture unit
        if( !isBitmapFontLoaded )
        {
            glActiveTexture( GL_TEXTURE0 + FONT_TEXTURE_UNIT );
            glGenTextures( 1, &fontTexture );
            glBindTexture( GL_TEXTURE_2D, fontTexture );
            glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8, font_image.width, font_image.height,
                          0, GL_RGB, GL_UNSIGNED_BYTE, font_image.pixels );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
            checkGlError( "loading bitmap font" );

            glActiveTexture( GL_TEXTURE0 );

            isBitmapFontLoaded = true;
        }
        else
        {
            glActiveTexture( GL_TEXTURE0 + FONT_TEXTURE_UNIT );
            glBindTexture( GL_TEXTURE_2D, fontTexture );
            glActiveTexture( GL_TEXTURE0 );
        }

        // Set shader parameter
        GLint ufm_fontTexture       = shader->AddParameter( "fontTexture" );
        GLint ufm_fontTextureWidth  = shader->AddParameter( "fontTextureWidth" );
        shader->Use();
        shader->SetParameter( ufm_fontTexture,       (int) FONT_TEXTURE_UNIT  );
        shader->SetParameter( ufm_fontTextureWidth,  (int) font_image.width  );
        shader->Deactivate();
        checkGlError( "setting bitmap font sampler as shader parameter" );

        isBitmapFontInitialized = true;
    }

    // Something betreen BeginDrawing and EndDrawing seems to depend on
    // this texture unit being active, but it does not assure it itself.
    glActiveTexture( GL_TEXTURE0 );

    // Unbind buffers - set compositor for direct drawing
    compositor->SetBuffer( OPENGL_COMPOSITOR::DIRECT_RENDERING );

#ifdef __WXDEBUG__
    totalRealTime.Stop();
    wxLogTrace( "GAL_PROFILE",
                wxT( "OPENGL_GAL::BeginDrawing(): %.1f ms" ), totalRealTime.msecs() );
#endif /* __WXDEBUG__ */
}


void OPENGL_GAL::EndDrawing()
{
#ifdef __WXDEBUG__
    PROF_COUNTER totalRealTime( "OPENGL_GAL::EndDrawing()", true );
#endif /* __WXDEBUG__ */

    // Cached & non-cached containers are rendered to the same buffer
    compositor->SetBuffer( mainBuffer );
    nonCachedManager->EndDrawing();
    cachedManager->EndDrawing();

    // Overlay container is rendered to a different buffer
    compositor->SetBuffer( overlayBuffer );
    overlayManager->EndDrawing();

    // Be sure that the framebuffer is not colorized (happens on specific GPU&drivers combinations)
    glColor4d( 1.0, 1.0, 1.0, 1.0 );

    // Draw the remaining contents, blit the rendering targets to the screen, swap the buffers
    compositor->DrawBuffer( mainBuffer );
    compositor->DrawBuffer( overlayBuffer );
    compositor->Present();
    blitCursor();

    SwapBuffers();
    GL_CONTEXT_MANAGER::Get().UnlockCtx( glPrivContext );

#ifdef __WXDEBUG__
    totalRealTime.Stop();
    wxLogTrace( "GAL_PROFILE", wxT( "OPENGL_GAL::EndDrawing(): %.1f ms" ), totalRealTime.msecs() );
#endif /* __WXDEBUG__ */
}


void OPENGL_GAL::BeginUpdate()
{
    if( !IsShownOnScreen() || GetClientRect().IsEmpty() )
        return;

    if( !isInitialized )
        init();

    GL_CONTEXT_MANAGER::Get().LockCtx( glPrivContext, this );
    cachedManager->Map();
}


void OPENGL_GAL::EndUpdate()
{
    if( !isInitialized )
        return;

    cachedManager->Unmap();
    GL_CONTEXT_MANAGER::Get().UnlockCtx( glPrivContext );
}


void OPENGL_GAL::DrawLine( const VECTOR2D& aStartPoint, const VECTOR2D& aEndPoint )
{
    const VECTOR2D  startEndVector = aEndPoint - aStartPoint;
    double          lineAngle = startEndVector.Angle();

    currentManager->Color( strokeColor.r, strokeColor.g, strokeColor.b, strokeColor.a );

    drawLineQuad( aStartPoint, aEndPoint );

    // Line caps
    if( lineWidth > 1.0 )
    {
        drawFilledSemiCircle( aStartPoint, lineWidth / 2, lineAngle + M_PI / 2 );
        drawFilledSemiCircle( aEndPoint,   lineWidth / 2, lineAngle - M_PI / 2 );
    }
}


void OPENGL_GAL::DrawSegment( const VECTOR2D& aStartPoint, const VECTOR2D& aEndPoint,
                              double aWidth )
{
    VECTOR2D startEndVector = aEndPoint - aStartPoint;
    double   lineAngle      = startEndVector.Angle();

    // Width must be nonzero for anything to appear
    if( aWidth <= 0 )
        aWidth = 1.0;

    if( isFillEnabled )
    {
        // Filled tracks
        currentManager->Color( fillColor.r, fillColor.g, fillColor.b, fillColor.a );

        SetLineWidth( aWidth );
        drawLineQuad( aStartPoint, aEndPoint );

        // Draw line caps
        drawFilledSemiCircle( aStartPoint, aWidth / 2, lineAngle + M_PI / 2 );
        drawFilledSemiCircle( aEndPoint,   aWidth / 2, lineAngle - M_PI / 2 );
    }
    else
    {
        // Outlined tracks
        double lineLength = startEndVector.EuclideanNorm();

        currentManager->Color( strokeColor.r, strokeColor.g, strokeColor.b, strokeColor.a );

        Save();

        currentManager->Translate( aStartPoint.x, aStartPoint.y, 0.0 );
        currentManager->Rotate( lineAngle, 0.0f, 0.0f, 1.0f );

        drawLineQuad( VECTOR2D( 0.0,         aWidth / 2.0 ),
                      VECTOR2D( lineLength,  aWidth / 2.0 ) );

        drawLineQuad( VECTOR2D( 0.0,        -aWidth / 2.0 ),
                      VECTOR2D( lineLength, -aWidth / 2.0 ) );

        // Draw line caps
        drawStrokedSemiCircle( VECTOR2D( 0.0, 0.0 ), aWidth / 2, M_PI / 2 );
        drawStrokedSemiCircle( VECTOR2D( lineLength, 0.0 ), aWidth / 2, -M_PI / 2 );

        Restore();
    }
}


void OPENGL_GAL::DrawCircle( const VECTOR2D& aCenterPoint, double aRadius )
{
    if( isFillEnabled )
    {
        currentManager->Reserve( 3 );
        currentManager->Color( fillColor.r, fillColor.g, fillColor.b, fillColor.a );

        /* Draw a triangle that contains the circle, then shade it leaving only the circle.
         *  Parameters given to Shader() are indices of the triangle's vertices
         *  (if you want to understand more, check the vertex shader source [shader.vert]).
         *  Shader uses this coordinates to determine if fragments are inside the circle or not.
         *       v2
         *       /\
         *      //\\
         *  v0 /_\/_\ v1
         */
        currentManager->Shader( SHADER_FILLED_CIRCLE, 1.0 );
        currentManager->Vertex( aCenterPoint.x - aRadius * sqrt( 3.0f ),            // v0
                                aCenterPoint.y - aRadius, layerDepth );

        currentManager->Shader( SHADER_FILLED_CIRCLE, 2.0 );
        currentManager->Vertex( aCenterPoint.x + aRadius * sqrt( 3.0f),             // v1
                                aCenterPoint.y - aRadius, layerDepth );

        currentManager->Shader( SHADER_FILLED_CIRCLE, 3.0 );
        currentManager->Vertex( aCenterPoint.x, aCenterPoint.y + aRadius * 2.0f,    // v2
                                layerDepth );
    }

    if( isStrokeEnabled )
    {
        currentManager->Reserve( 3 );
        currentManager->Color( strokeColor.r, strokeColor.g, strokeColor.b, strokeColor.a );

        /* Draw a triangle that contains the circle, then shade it leaving only the circle.
         *  Parameters given to Shader() are indices of the triangle's vertices
         *  (if you want to understand more, check the vertex shader source [shader.vert]).
         *  and the line width. Shader uses this coordinates to determine if fragments are
         *  inside the circle or not.
         *       v2
         *       /\
         *      //\\
         *  v0 /_\/_\ v1
         */
        double outerRadius = aRadius + ( lineWidth / 2 );
        currentManager->Shader( SHADER_STROKED_CIRCLE, 1.0, aRadius, lineWidth );
        currentManager->Vertex( aCenterPoint.x - outerRadius * sqrt( 3.0f ),            // v0
                                aCenterPoint.y - outerRadius, layerDepth );

        currentManager->Shader( SHADER_STROKED_CIRCLE, 2.0, aRadius, lineWidth );
        currentManager->Vertex( aCenterPoint.x + outerRadius * sqrt( 3.0f ),            // v1
                                aCenterPoint.y - outerRadius, layerDepth );

        currentManager->Shader( SHADER_STROKED_CIRCLE, 3.0, aRadius, lineWidth );
        currentManager->Vertex( aCenterPoint.x, aCenterPoint.y + outerRadius * 2.0f,    // v2
                                layerDepth );
    }
}


void OPENGL_GAL::DrawArc( const VECTOR2D& aCenterPoint, double aRadius, double aStartAngle,
                          double aEndAngle )
{
    if( aRadius <= 0 )
        return;

    // Swap the angles, if start angle is greater than end angle
    SWAP( aStartAngle, >, aEndAngle );

    const double alphaIncrement = calcAngleStep( aRadius );

    Save();
    currentManager->Translate( aCenterPoint.x, aCenterPoint.y, 0.0 );

    if( isStrokeEnabled )
    {
        currentManager->Color( strokeColor.r, strokeColor.g, strokeColor.b, strokeColor.a );

        VECTOR2D p( cos( aStartAngle ) * aRadius, sin( aStartAngle ) * aRadius );
        double alpha;

        for( alpha = aStartAngle + alphaIncrement; alpha <= aEndAngle; alpha += alphaIncrement )
        {
            VECTOR2D p_next( cos( alpha ) * aRadius, sin( alpha ) * aRadius );
            DrawLine( p, p_next );

            p = p_next;
        }

        // Draw the last missing part
        if( alpha != aEndAngle )
        {
            VECTOR2D p_last( cos( aEndAngle ) * aRadius, sin( aEndAngle ) * aRadius );
            DrawLine( p, p_last );
        }
    }

    if( isFillEnabled )
    {
        double alpha;
        currentManager->Color( fillColor.r, fillColor.g, fillColor.b, fillColor.a );
        currentManager->Shader( SHADER_NONE );

        // Triangle fan
        for( alpha = aStartAngle; ( alpha + alphaIncrement ) < aEndAngle; )
        {
            currentManager->Reserve( 3 );
            currentManager->Vertex( 0.0, 0.0, 0.0 );
            currentManager->Vertex( cos( alpha ) * aRadius, sin( alpha ) * aRadius, 0.0 );
            alpha += alphaIncrement;
            currentManager->Vertex( cos( alpha ) * aRadius, sin( alpha ) * aRadius, 0.0 );
        }

        // The last missing triangle
        const VECTOR2D endPoint( cos( aEndAngle ) * aRadius, sin( aEndAngle ) * aRadius );

        currentManager->Reserve( 3 );
        currentManager->Vertex( 0.0, 0.0, 0.0 );
        currentManager->Vertex( cos( alpha ) * aRadius, sin( alpha ) * aRadius, 0.0 );
        currentManager->Vertex( endPoint.x,    endPoint.y,     0.0 );
    }

    Restore();
}


void OPENGL_GAL::DrawArcSegment( const VECTOR2D& aCenterPoint, double aRadius, double aStartAngle,
                                 double aEndAngle, double aWidth )
{
    if( aRadius <= 0 )
    {
        // Arcs of zero radius are a circle of aWidth diameter
        if( aWidth > 0 )
            DrawCircle( aCenterPoint, aWidth / 2.0 );

        return;
    }

    // Swap the angles, if start angle is greater than end angle
    SWAP( aStartAngle, >, aEndAngle );

    const double alphaIncrement = calcAngleStep( aRadius );

    Save();
    currentManager->Translate( aCenterPoint.x, aCenterPoint.y, 0.0 );

    if( isStrokeEnabled )
    {
        currentManager->Color( strokeColor.r, strokeColor.g, strokeColor.b, strokeColor.a );

        double width = aWidth / 2.0;
        VECTOR2D startPoint( cos( aStartAngle ) * aRadius,
                             sin( aStartAngle ) * aRadius );
        VECTOR2D endPoint( cos( aEndAngle ) * aRadius,
                           sin( aEndAngle ) * aRadius );

        drawStrokedSemiCircle( startPoint, width, aStartAngle + M_PI );
        drawStrokedSemiCircle( endPoint, width, aEndAngle );

        VECTOR2D pOuter( cos( aStartAngle ) * ( aRadius + width ),
                         sin( aStartAngle ) * ( aRadius + width ) );

        VECTOR2D pInner( cos( aStartAngle ) * ( aRadius - width ),
                         sin( aStartAngle ) * ( aRadius - width ) );

        double alpha;

        for( alpha = aStartAngle + alphaIncrement; alpha <= aEndAngle; alpha += alphaIncrement )
        {
            VECTOR2D pNextOuter( cos( alpha ) * ( aRadius + width ),
                                 sin( alpha ) * ( aRadius + width ) );
            VECTOR2D pNextInner( cos( alpha ) * ( aRadius - width ),
                                 sin( alpha ) * ( aRadius - width ) );

            DrawLine( pOuter, pNextOuter );
            DrawLine( pInner, pNextInner );

            pOuter = pNextOuter;
            pInner = pNextInner;
        }

        // Draw the last missing part
        if( alpha != aEndAngle )
        {
            VECTOR2D pLastOuter( cos( aEndAngle ) * ( aRadius + width ),
                                 sin( aEndAngle ) * ( aRadius + width ) );
            VECTOR2D pLastInner( cos( aEndAngle ) * ( aRadius - width ),
                                 sin( aEndAngle ) * ( aRadius - width ) );

            DrawLine( pOuter, pLastOuter );
            DrawLine( pInner, pLastInner );
        }
    }

    if( isFillEnabled )
    {
        currentManager->Color( fillColor.r, fillColor.g, fillColor.b, fillColor.a );
        SetLineWidth( aWidth );

        VECTOR2D p( cos( aStartAngle ) * aRadius, sin( aStartAngle ) * aRadius );
        double alpha;

        for( alpha = aStartAngle + alphaIncrement; alpha <= aEndAngle; alpha += alphaIncrement )
        {
            VECTOR2D p_next( cos( alpha ) * aRadius, sin( alpha ) * aRadius );
            DrawLine( p, p_next );

            p = p_next;
        }

        // Draw the last missing part
        if( alpha != aEndAngle )
        {
            VECTOR2D p_last( cos( aEndAngle ) * aRadius, sin( aEndAngle ) * aRadius );
            DrawLine( p, p_last );
        }
    }

    Restore();
}


void OPENGL_GAL::DrawRectangle( const VECTOR2D& aStartPoint, const VECTOR2D& aEndPoint )
{
    // Compute the diagonal points of the rectangle
    VECTOR2D diagonalPointA( aEndPoint.x, aStartPoint.y );
    VECTOR2D diagonalPointB( aStartPoint.x, aEndPoint.y );

    // Stroke the outline
    if( isStrokeEnabled )
    {
        currentManager->Color( strokeColor.r, strokeColor.g, strokeColor.b, strokeColor.a );

        std::deque<VECTOR2D> pointList;
        pointList.push_back( aStartPoint );
        pointList.push_back( diagonalPointA );
        pointList.push_back( aEndPoint );
        pointList.push_back( diagonalPointB );
        pointList.push_back( aStartPoint );
        DrawPolyline( pointList );
    }

    // Fill the rectangle
    if( isFillEnabled )
    {
        currentManager->Reserve( 6 );
        currentManager->Shader( SHADER_NONE );
        currentManager->Color( fillColor.r, fillColor.g, fillColor.b, fillColor.a );

        currentManager->Vertex( aStartPoint.x, aStartPoint.y, layerDepth );
        currentManager->Vertex( diagonalPointA.x, diagonalPointA.y, layerDepth );
        currentManager->Vertex( aEndPoint.x, aEndPoint.y, layerDepth );

        currentManager->Vertex( aStartPoint.x, aStartPoint.y, layerDepth );
        currentManager->Vertex( aEndPoint.x, aEndPoint.y, layerDepth );
        currentManager->Vertex( diagonalPointB.x, diagonalPointB.y, layerDepth );
    }
}


void OPENGL_GAL::DrawPolyline( const std::deque<VECTOR2D>& aPointList )
{
    drawPolyline( [&](int idx) { return aPointList[idx]; }, aPointList.size() );
}


void OPENGL_GAL::DrawPolyline( const VECTOR2D aPointList[], int aListSize )
{
    drawPolyline( [&](int idx) { return aPointList[idx]; }, aListSize );
}


void OPENGL_GAL::DrawPolyline( const SHAPE_LINE_CHAIN& aLineChain )
{
    auto numPoints = aLineChain.PointCount();

    if( aLineChain.IsClosed() )
        numPoints += 1;

    drawPolyline( [&](int idx) { return aLineChain.CPoint(idx); }, numPoints );
}


void OPENGL_GAL::DrawPolygon( const std::deque<VECTOR2D>& aPointList )
{
    auto points = std::unique_ptr<GLdouble[]>( new GLdouble[3 * aPointList.size()] );
    GLdouble* ptr = points.get();

    for( const VECTOR2D& p : aPointList )
    {
        *ptr++ = p.x;
        *ptr++ = p.y;
        *ptr++ = layerDepth;
    }

    drawPolygon( points.get(), aPointList.size() );
}


void OPENGL_GAL::DrawPolygon( const VECTOR2D aPointList[], int aListSize )
{
    auto points = std::unique_ptr<GLdouble[]>( new GLdouble[3 * aListSize] );
    GLdouble* target = points.get();
    const VECTOR2D* src = aPointList;

    for( int i = 0; i < aListSize; ++i )
    {
        *target++ = src->x;
        *target++ = src->y;
        *target++ = layerDepth;
        ++src;
    }

    drawPolygon( points.get(), aListSize );
}


void OPENGL_GAL::drawTriangulatedPolyset( const SHAPE_POLY_SET& aPolySet )
{
    currentManager->Shader( SHADER_NONE );
    currentManager->Color( fillColor.r, fillColor.g, fillColor.b, fillColor.a );

    if( isFillEnabled )
    {
        for( unsigned int j = 0; j < aPolySet.TriangulatedPolyCount(); ++j )
        {
            auto triPoly = aPolySet.TriangulatedPolygon( j );

            for( size_t i = 0; i < triPoly->GetTriangleCount(); i++ )
            {
                VECTOR2I a, b, c;
                triPoly->GetTriangle( i, a, b, c );
                currentManager->Vertex( a.x, a.y, layerDepth );
                currentManager->Vertex( b.x, b.y, layerDepth );
                currentManager->Vertex( c.x, c.y, layerDepth );
            }
        }
    }

    if( isStrokeEnabled )
    {
        for( int j = 0; j < aPolySet.OutlineCount(); ++j )
        {
            const auto& poly = aPolySet.Polygon( j );

            for( const auto& lc : poly )
            {
                DrawPolyline( lc );
            }
        }
    }
}


void OPENGL_GAL::DrawPolygon( const SHAPE_POLY_SET& aPolySet )
{
    if ( aPolySet.IsTriangulationUpToDate() )
    {
        drawTriangulatedPolyset( aPolySet );
        return;
    }

    for( int j = 0; j < aPolySet.OutlineCount(); ++j )
    {
        const SHAPE_LINE_CHAIN& outline = aPolySet.COutline( j );

        if( outline.SegmentCount() == 0 )
            continue;

        const int pointCount = outline.SegmentCount() + 1;
        std::unique_ptr<GLdouble[]> points( new GLdouble[3 * pointCount] );
        GLdouble* ptr = points.get();

        for( int i = 0; i < pointCount; ++i )
        {
            const VECTOR2I& p = outline.CPoint( i );
            *ptr++ = p.x;
            *ptr++ = p.y;
            *ptr++ = layerDepth;
        }

        drawPolygon( points.get(), pointCount );
    }
}


void OPENGL_GAL::DrawCurve( const VECTOR2D& aStartPoint, const VECTOR2D& aControlPointA,
                            const VECTOR2D& aControlPointB, const VECTOR2D& aEndPoint )
{
    // FIXME The drawing quality needs to be improved
    // FIXME Perhaps choose a quad/triangle strip instead?
    // FIXME Brute force method, use a better (recursive?) algorithm

    std::deque<VECTOR2D> pointList;

    double t  = 0.0;
    double dt = 1.0 / (double) CURVE_POINTS;

    for( int i = 0; i <= CURVE_POINTS; i++ )
    {
        double omt  = 1.0 - t;
        double omt2 = omt * omt;
        double omt3 = omt * omt2;
        double t2   = t * t;
        double t3   = t * t2;

        VECTOR2D vertex = omt3 * aStartPoint + 3.0 * t * omt2 * aControlPointA
                          + 3.0 * t2 * omt * aControlPointB + t3 * aEndPoint;

        pointList.push_back( vertex );

        t += dt;
    }

    DrawPolyline( pointList );
}


void OPENGL_GAL::DrawBitmap( const BITMAP_BASE& aBitmap )
{
    int ppi = aBitmap.GetPPI();
    double worldIU_per_mm = 1.0 / ( worldUnitLength / 2.54 )/ 1000;
    double pix_size_iu = worldIU_per_mm * ( 25.4 / ppi );

    double w = aBitmap.GetSizePixels().x * pix_size_iu;
    double h = aBitmap.GetSizePixels().y * pix_size_iu;

    auto xform = currentManager->GetTransformation();

    glm::vec4 v0 = xform * glm::vec4( -w/2, -h/2, 0.0, 0.0 );
    glm::vec4 v1 = xform * glm::vec4( w/2, h/2, 0.0, 0.0 );
    glm::vec4 trans = xform[3];

    auto id = bitmapCache->RequestBitmap( &aBitmap );

    auto oldTarget = GetTarget();

    glPushMatrix();
    glTranslated( trans.x, trans.y, trans.z );

    SetTarget( TARGET_NONCACHED );
    glEnable(GL_TEXTURE_2D);
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, id );

    glBegin( GL_QUADS );
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glTexCoord2f(0.0, 0.0);
    glVertex3f( v0.x, v0.y, layerDepth );
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glTexCoord2f(1.0, 0.0);
    glVertex3f( v1.x, v0.y, layerDepth );
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glTexCoord2f(1.0, 1.0);
    glVertex3f( v1.x, v1.y, layerDepth );
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glTexCoord2f(0.0, 1.0);
    glVertex3f( v0.x, v1.y, layerDepth );
    glEnd();

    SetTarget( oldTarget );
    glBindTexture( GL_TEXTURE_2D, 0 );

    glPopMatrix();
}


void OPENGL_GAL::BitmapText( const wxString& aText, const VECTOR2D& aPosition,
                             double aRotationAngle )
{
    wxASSERT_MSG( !IsTextMirrored(), "No support for mirrored text using bitmap fonts." );

    auto processedText = ProcessOverbars( aText );
    const auto& text = processedText.first;
    const auto& overbars = processedText.second;

    // Compute text size, so it can be properly justified
    VECTOR2D textSize;
    float commonOffset;
    std::tie( textSize, commonOffset ) = computeBitmapTextSize( text );

    const double SCALE = 1.4 * GetGlyphSize().y / textSize.y;
    bool overbar = false;

    int overbarLength = 0;
    double overbarHeight = textSize.y;

    Save();

    currentManager->Color( strokeColor.r, strokeColor.g, strokeColor.b, strokeColor.a );
    currentManager->Translate( aPosition.x, aPosition.y, layerDepth );
    currentManager->Rotate( aRotationAngle, 0.0f, 0.0f, -1.0f );

    double sx = SCALE * ( globalFlipX ? -1.0 : 1.0 );
    double sy = SCALE * ( globalFlipY ? -1.0 : 1.0 );

    currentManager->Scale( sx, sy, 0 );
    currentManager->Translate( 0, -commonOffset, 0 );

    switch( GetHorizontalJustify() )
    {
    case GR_TEXT_HJUSTIFY_CENTER:
        Translate( VECTOR2D( -textSize.x / 2.0, 0 ) );
        break;

    case GR_TEXT_HJUSTIFY_RIGHT:
        //if( !IsTextMirrored() )
            Translate( VECTOR2D( -textSize.x, 0 ) );
        break;

    case GR_TEXT_HJUSTIFY_LEFT:
        //if( IsTextMirrored() )
            //Translate( VECTOR2D( -textSize.x, 0 ) );
        break;
    }

    switch( GetVerticalJustify() )
    {
    case GR_TEXT_VJUSTIFY_TOP:
        Translate( VECTOR2D( 0, -textSize.y ) );
        overbarHeight = -textSize.y / 2.0;
        break;

    case GR_TEXT_VJUSTIFY_CENTER:
        Translate( VECTOR2D( 0, -textSize.y / 2.0 ) );
        overbarHeight = 0;
        break;

    case GR_TEXT_VJUSTIFY_BOTTOM:
        break;
    }

    int i = 0;

    for( UTF8::uni_iter chIt = text.ubegin(), end = text.uend(); chIt < end; ++chIt )
    {
        unsigned int c = *chIt;
        wxASSERT_MSG( c != '\n' && c != '\r', wxT( "No support for multiline bitmap text yet" ) );

        // Handle overbar
        if( overbars[i] && !overbar )
        {
            overbar = true;     // beginning of an overbar
        }
        else if( overbar && !overbars[i] )
        {
            overbar = false;    // end of an overbar
            drawBitmapOverbar( overbarLength, overbarHeight );
            overbarLength = 0;
        }

        if( overbar )
            overbarLength += drawBitmapChar( c );
        else
            drawBitmapChar( c );

        ++i;
    }

    // Handle the case when overbar is active till the end of the drawn text
    currentManager->Translate( 0, commonOffset, 0 );

    if( overbar && overbarLength > 0 )
        drawBitmapOverbar( overbarLength, overbarHeight );

    Restore();
}


void OPENGL_GAL::DrawGrid()
{
    SetTarget( TARGET_NONCACHED );
    compositor->SetBuffer( mainBuffer );

    // sub-pixel lines all render the same
    double minorLineWidth = std::max( 1.0, gridLineWidth );
    double majorLineWidth = minorLineWidth * 2.0;

    // Draw the axis and grid
    // For the drawing the start points, end points and increments have
    // to be calculated in world coordinates
    VECTOR2D worldStartPoint = screenWorldMatrix * VECTOR2D( 0.0, 0.0 );
    VECTOR2D worldEndPoint = screenWorldMatrix * VECTOR2D( screenSize );

    // Draw axes if desired
    if( axesEnabled )
    {
        glLineWidth( minorLineWidth );
        glColor4d( axesColor.r, axesColor.g, axesColor.b, axesColor.a );

        glBegin( GL_LINES );
        glVertex2d( worldStartPoint.x, 0 );
        glVertex2d( worldEndPoint.x, 0 );
        glEnd();

        glBegin( GL_LINES );
        glVertex2d( 0, worldStartPoint.y );
        glVertex2d( 0, worldEndPoint.y );
        glEnd();
    }

    if( !gridVisibility )
        return;

    int gridScreenSizeDense  = KiROUND( gridSize.x * worldScale );
    int gridScreenSizeCoarse = KiROUND( gridSize.x * static_cast<double>( gridTick ) * worldScale );

    const double gridThreshold = computeMinGridSpacing();

    // Check if the grid would not be too dense
    if( std::max( gridScreenSizeDense, gridScreenSizeCoarse ) < gridThreshold )
        return;

    // Compute grid staring and ending indexes to draw grid points on the
    // visible screen area
    // Note: later any point coordinate will be offsetted by gridOrigin
    int gridStartX = KiROUND( ( worldStartPoint.x - gridOrigin.x ) / gridSize.x );
    int gridEndX = KiROUND( ( worldEndPoint.x - gridOrigin.x ) / gridSize.x );
    int gridStartY = KiROUND( ( worldStartPoint.y - gridOrigin.y ) / gridSize.y );
    int gridEndY = KiROUND( ( worldEndPoint.y - gridOrigin.y ) / gridSize.y );

    // Ensure start coordinate > end coordinate
    if( gridStartX > gridEndX )
        std::swap( gridStartX, gridEndX );

    if( gridStartY > gridEndY )
        std::swap( gridStartY, gridEndY );

    // Ensure the grid fills the screen
    --gridStartX; ++gridEndX;
    --gridStartY; ++gridEndY;

    glDisable( GL_DEPTH_TEST );
    glDisable( GL_TEXTURE_2D );

    if( gridStyle == GRID_STYLE::DOTS )
    {
        glEnable( GL_STENCIL_TEST );
        glStencilFunc( GL_ALWAYS, 1, 1 );
        glStencilOp( GL_KEEP, GL_KEEP, GL_INCR );
        glColor4d( 0.0, 0.0, 0.0, 0.0 );
    }
    else
    {
        glColor4d( gridColor.r, gridColor.g, gridColor.b, gridColor.a );
    }

    if( gridStyle == GRID_STYLE::SMALL_CROSS )
    {
        glLineWidth( minorLineWidth );

        // calculate a line len = 2 minorLineWidth, in internal unit value
        // (in fact the size of cross is lineLen*2)
        int lineLen = KiROUND( minorLineWidth / worldScale * 2 );

        // Vertical positions
        for( int j = gridStartY; j <= gridEndY; j++ )
        {
            if( ( j % gridTick == 0 && gridScreenSizeCoarse > gridThreshold )
                || gridScreenSizeDense > gridThreshold )
            {
                int posY =  j * gridSize.y + gridOrigin.y;

                // Horizontal positions
                for( int i = gridStartX; i <= gridEndX; i++ )
                {
                    if( ( i % gridTick == 0 && gridScreenSizeCoarse > gridThreshold )
                        || gridScreenSizeDense > gridThreshold )
                    {
                        int posX = i * gridSize.x + gridOrigin.x;

                        glBegin( GL_LINES );
                        glVertex2d( posX -lineLen, posY );
                        glVertex2d( posX + lineLen, posY );
                        glVertex2d( posX, posY - lineLen );
                        glVertex2d( posX, posY + lineLen );
                        glEnd();
                    }
                }
            }
        }
    }
    else
    {
        // Vertical lines
        for( int j = gridStartY; j <= gridEndY; j++ )
        {
            const double y = j * gridSize.y + gridOrigin.y;

            // If axes are drawn, skip the lines that would cover them
            if( axesEnabled && y == 0 )
                continue;

            if( j % gridTick == 0 && gridScreenSizeDense > gridThreshold )
                glLineWidth( majorLineWidth );
            else
                glLineWidth( minorLineWidth );

            if( ( j % gridTick == 0 && gridScreenSizeCoarse > gridThreshold )
                || gridScreenSizeDense > gridThreshold )
            {
                glBegin( GL_LINES );
                glVertex2d( gridStartX * gridSize.x + gridOrigin.x, y );
                glVertex2d( gridEndX * gridSize.x + gridOrigin.x, y );
                glEnd();
            }
        }

        if( gridStyle == GRID_STYLE::DOTS )
        {
            glStencilFunc( GL_NOTEQUAL, 0, 1 );
            glColor4d( gridColor.r, gridColor.g, gridColor.b, gridColor.a );
        }

        // Horizontal lines
        for( int i = gridStartX; i <= gridEndX; i++ )
        {
            const double x = i * gridSize.x + gridOrigin.x;

            // If axes are drawn, skip the lines that would cover them
            if( axesEnabled && x == 0 )
                continue;

            if( i % gridTick == 0 && gridScreenSizeDense > gridThreshold )
                glLineWidth( majorLineWidth );
            else
                glLineWidth( minorLineWidth );

            if( ( i % gridTick == 0 && gridScreenSizeCoarse > gridThreshold )
                || gridScreenSizeDense > gridThreshold )
            {
                glBegin( GL_LINES );
                glVertex2d( x, gridStartY * gridSize.y + gridOrigin.y );
                glVertex2d( x, gridEndY * gridSize.y + gridOrigin.y );
                glEnd();
            }
        }

        if( gridStyle == GRID_STYLE::DOTS )
            glDisable( GL_STENCIL_TEST );
    }

    glEnable( GL_DEPTH_TEST );
    glEnable( GL_TEXTURE_2D );
}


void OPENGL_GAL::ResizeScreen( int aWidth, int aHeight )
{
    screenSize = VECTOR2I( aWidth, aHeight );

    // Resize framebuffers
    const float scaleFactor = GetBackingScaleFactor();
    compositor->Resize( aWidth * scaleFactor, aHeight * scaleFactor );
    isFramebufferInitialized = false;

    wxGLCanvas::SetSize( aWidth, aHeight );
}


bool OPENGL_GAL::Show( bool aShow )
{
    bool s = wxGLCanvas::Show( aShow );

    if( aShow )
        wxGLCanvas::Raise();

    return s;
}


void OPENGL_GAL::Flush()
{
    glFlush();
}


void OPENGL_GAL::ClearScreen( )
{
    // Clear screen
    compositor->SetBuffer( OPENGL_COMPOSITOR::DIRECT_RENDERING );
    // NOTE: Black used here instead of m_clearColor; it will be composited later
    glClearColor( 0, 0, 0, 1 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
}


void OPENGL_GAL::Transform( const MATRIX3x3D& aTransformation )
{
    GLdouble matrixData[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

    matrixData[0]   = aTransformation.m_data[0][0];
    matrixData[1]   = aTransformation.m_data[1][0];
    matrixData[2]   = aTransformation.m_data[2][0];
    matrixData[4]   = aTransformation.m_data[0][1];
    matrixData[5]   = aTransformation.m_data[1][1];
    matrixData[6]   = aTransformation.m_data[2][1];
    matrixData[12]  = aTransformation.m_data[0][2];
    matrixData[13]  = aTransformation.m_data[1][2];
    matrixData[14]  = aTransformation.m_data[2][2];

    glMultMatrixd( matrixData );
}


void OPENGL_GAL::Rotate( double aAngle )
{
    currentManager->Rotate( aAngle, 0.0f, 0.0f, 1.0f );
}


void OPENGL_GAL::Translate( const VECTOR2D& aVector )
{
    currentManager->Translate( aVector.x, aVector.y, 0.0f );
}


void OPENGL_GAL::Scale( const VECTOR2D& aScale )
{
    currentManager->Scale( aScale.x, aScale.y, 0.0f );
}


void OPENGL_GAL::Save()
{
    currentManager->PushMatrix();
}


void OPENGL_GAL::Restore()
{
    currentManager->PopMatrix();
}


int OPENGL_GAL::BeginGroup()
{
    isGrouping = true;

    std::shared_ptr<VERTEX_ITEM> newItem = std::make_shared<VERTEX_ITEM>( *cachedManager );
    int groupNumber = getNewGroupNumber();
    groups.insert( std::make_pair( groupNumber, newItem ) );

    return groupNumber;
}


void OPENGL_GAL::EndGroup()
{
    cachedManager->FinishItem();
    isGrouping = false;
}


void OPENGL_GAL::DrawGroup( int aGroupNumber )
{
    if( groups[aGroupNumber] )
        cachedManager->DrawItem( *groups[aGroupNumber] );
}


void OPENGL_GAL::ChangeGroupColor( int aGroupNumber, const COLOR4D& aNewColor )
{
    if( groups[aGroupNumber] )
        cachedManager->ChangeItemColor( *groups[aGroupNumber], aNewColor );
}


void OPENGL_GAL::ChangeGroupDepth( int aGroupNumber, int aDepth )
{
    if( groups[aGroupNumber] )
        cachedManager->ChangeItemDepth( *groups[aGroupNumber], aDepth );
}


void OPENGL_GAL::DeleteGroup( int aGroupNumber )
{
    // Frees memory in the container as well
    groups.erase( aGroupNumber );
}


void OPENGL_GAL::ClearCache()
{
    bitmapCache.reset( new GL_BITMAP_CACHE );

    groups.clear();

    if( isInitialized )
        cachedManager->Clear();
}


void OPENGL_GAL::SaveScreen()
{
    wxASSERT_MSG( false, wxT( "Not implemented yet" ) );
}


void OPENGL_GAL::RestoreScreen()
{
    wxASSERT_MSG( false, wxT( "Not implemented yet" ) );
}


void OPENGL_GAL::SetTarget( RENDER_TARGET aTarget )
{
    switch( aTarget )
    {
    default:
    case TARGET_CACHED:
        currentManager = cachedManager;
        break;

    case TARGET_NONCACHED:
        currentManager = nonCachedManager;
        break;

    case TARGET_OVERLAY:
        currentManager = overlayManager;
        break;
    }

    currentTarget = aTarget;
}


RENDER_TARGET OPENGL_GAL::GetTarget() const
{
    return currentTarget;
}


void OPENGL_GAL::ClearTarget( RENDER_TARGET aTarget )
{
    // Save the current state
    unsigned int oldTarget = compositor->GetBuffer();

    switch( aTarget )
    {
    // Cached and noncached items are rendered to the same buffer
    default:
    case TARGET_CACHED:
    case TARGET_NONCACHED:
        compositor->SetBuffer( mainBuffer );
        break;

    case TARGET_OVERLAY:
        compositor->SetBuffer( overlayBuffer );
        break;
    }


    if( aTarget != TARGET_OVERLAY )
        compositor->ClearBuffer( m_clearColor );
    else
        compositor->ClearBuffer( COLOR4D::BLACK );

    // Restore the previous state
    compositor->SetBuffer( oldTarget );
}


void OPENGL_GAL::DrawCursor( const VECTOR2D& aCursorPosition )
{
    // Now we should only store the position of the mouse cursor
    // The real drawing routines are in blitCursor()
    //VECTOR2D screenCursor = worldScreenMatrix * aCursorPosition;
    //cursorPosition = screenWorldMatrix * VECTOR2D( screenCursor.x, screenCursor.y );
    cursorPosition = aCursorPosition;
}


void OPENGL_GAL::drawLineQuad( const VECTOR2D& aStartPoint, const VECTOR2D& aEndPoint )
{
    /* Helper drawing:                   ____--- v3       ^
     *                           ____---- ...   \          \
     *                   ____----      ...       \   end    \
     *     v1    ____----           ...    ____----          \ width
     *       ----                ...___----        \          \
     *       \             ___...--                 \          v
     *        \    ____----...                ____---- v2
     *         ----     ...           ____----
     *  start   \    ...      ____----
     *           \... ____----
     *            ----
     *            v0
     * dots mark triangles' hypotenuses
     */

    VECTOR2D startEndVector = aEndPoint - aStartPoint;
    double   lineLength     = startEndVector.EuclideanNorm();

    if( lineLength <= 0.0 )
        return;

    double   scale          = 0.5 * lineWidth / lineLength;

    // The perpendicular vector also needs transformations
    glm::vec4 vector = currentManager->GetTransformation() *
                       glm::vec4( -startEndVector.y * scale, startEndVector.x * scale, 0.0, 0.0 );

    currentManager->Reserve( 6 );

    // Line width is maintained by the vertex shader
    currentManager->Shader( SHADER_LINE, vector.x, vector.y, lineWidth );
    currentManager->Vertex( aStartPoint.x, aStartPoint.y, layerDepth );    // v0

    currentManager->Shader( SHADER_LINE, -vector.x, -vector.y, lineWidth );
    currentManager->Vertex( aStartPoint.x, aStartPoint.y, layerDepth );    // v1

    currentManager->Shader( SHADER_LINE, -vector.x, -vector.y, lineWidth );
    currentManager->Vertex( aEndPoint.x, aEndPoint.y, layerDepth );        // v3

    currentManager->Shader( SHADER_LINE, vector.x, vector.y, lineWidth );
    currentManager->Vertex( aStartPoint.x, aStartPoint.y, layerDepth );    // v0

    currentManager->Shader( SHADER_LINE, -vector.x, -vector.y, lineWidth );
    currentManager->Vertex( aEndPoint.x, aEndPoint.y, layerDepth );        // v3

    currentManager->Shader( SHADER_LINE, vector.x, vector.y, lineWidth );
    currentManager->Vertex( aEndPoint.x, aEndPoint.y, layerDepth );        // v2
}


void OPENGL_GAL::drawSemiCircle( const VECTOR2D& aCenterPoint, double aRadius, double aAngle )
{
    if( isFillEnabled )
    {
        currentManager->Color( fillColor.r, fillColor.g, fillColor.b, fillColor.a );
        drawFilledSemiCircle( aCenterPoint, aRadius, aAngle );
    }

    if( isStrokeEnabled )
    {
        currentManager->Color( strokeColor.r, strokeColor.g, strokeColor.b, strokeColor.a );
        drawStrokedSemiCircle( aCenterPoint, aRadius, aAngle );
    }
}


void OPENGL_GAL::drawFilledSemiCircle( const VECTOR2D& aCenterPoint, double aRadius,
                                       double aAngle )
{
    Save();

    currentManager->Reserve( 3 );
    currentManager->Translate( aCenterPoint.x, aCenterPoint.y, 0.0f );
    currentManager->Rotate( aAngle, 0.0f, 0.0f, 1.0f );

    /* Draw a triangle that contains the semicircle, then shade it to leave only
     * the semicircle. Parameters given to Shader() are indices of the triangle's vertices
     * (if you want to understand more, check the vertex shader source [shader.vert]).
     * Shader uses these coordinates to determine if fragments are inside the semicircle or not.
     *       v2
     *       /\
     *      /__\
     *  v0 //__\\ v1
     */
    currentManager->Shader( SHADER_FILLED_CIRCLE, 4.0f );
    currentManager->Vertex( -aRadius * 3.0f / sqrt( 3.0f ), 0.0f, layerDepth );     // v0

    currentManager->Shader( SHADER_FILLED_CIRCLE, 5.0f );
    currentManager->Vertex( aRadius * 3.0f / sqrt( 3.0f ), 0.0f, layerDepth );      // v1

    currentManager->Shader( SHADER_FILLED_CIRCLE, 6.0f );
    currentManager->Vertex( 0.0f, aRadius * 2.0f, layerDepth );                     // v2

    Restore();
}


void OPENGL_GAL::drawStrokedSemiCircle( const VECTOR2D& aCenterPoint, double aRadius,
                                        double aAngle )
{
    double outerRadius = aRadius + ( lineWidth / 2 );

    Save();

    currentManager->Reserve( 3 );
    currentManager->Translate( aCenterPoint.x, aCenterPoint.y, 0.0f );
    currentManager->Rotate( aAngle, 0.0f, 0.0f, 1.0f );

    /* Draw a triangle that contains the semicircle, then shade it to leave only
     * the semicircle. Parameters given to Shader() are indices of the triangle's vertices
     * (if you want to understand more, check the vertex shader source [shader.vert]), the
     * radius and the line width. Shader uses these coordinates to determine if fragments are
     * inside the semicircle or not.
     *       v2
     *       /\
     *      /__\
     *  v0 //__\\ v1
     */
    currentManager->Shader( SHADER_STROKED_CIRCLE, 4.0f, aRadius, lineWidth );
    currentManager->Vertex( -outerRadius * 3.0f / sqrt( 3.0f ), 0.0f, layerDepth );     // v0

    currentManager->Shader( SHADER_STROKED_CIRCLE, 5.0f, aRadius, lineWidth );
    currentManager->Vertex( outerRadius * 3.0f / sqrt( 3.0f ), 0.0f, layerDepth );      // v1

    currentManager->Shader( SHADER_STROKED_CIRCLE, 6.0f, aRadius, lineWidth );
    currentManager->Vertex( 0.0f, outerRadius * 2.0f, layerDepth );                     // v2

    Restore();
}


void OPENGL_GAL::drawPolygon( GLdouble* aPoints, int aPointCount )
{
    if( isFillEnabled )
    {
        currentManager->Shader( SHADER_NONE );
        currentManager->Color( fillColor.r, fillColor.g, fillColor.b, fillColor.a );

        // Any non convex polygon needs to be tesselated
        // for this purpose the GLU standard functions are used
        TessParams params = { currentManager, tessIntersects };
        gluTessBeginPolygon( tesselator, &params );
        gluTessBeginContour( tesselator );

        GLdouble* point = aPoints;

        for( int i = 0; i < aPointCount; ++i )
        {
            gluTessVertex( tesselator, point, point );
            point += 3;     // 3 coordinates
        }

        gluTessEndContour( tesselator );
        gluTessEndPolygon( tesselator );

        // Free allocated intersecting points
        tessIntersects.clear();
    }

    if( isStrokeEnabled )
    {
        drawPolyline( [&](int idx) { return VECTOR2D( aPoints[idx * 3], aPoints[idx * 3 + 1] ); },
                aPointCount );
    }
}


void OPENGL_GAL::drawPolyline( const std::function<VECTOR2D (int)>& aPointGetter, int aPointCount )
{
    if( aPointCount < 2 )
        return;

    currentManager->Color( strokeColor.r, strokeColor.g, strokeColor.b, strokeColor.a );
    int i;

    for( i = 1; i < aPointCount; ++i )
    {
        auto start = aPointGetter( i - 1 );
        auto end = aPointGetter( i );
        const VECTOR2D startEndVector = ( end - start );
        double lineAngle = startEndVector.Angle();

        drawLineQuad( start, end );

        // There is no need to draw line caps on both ends of polyline's segments
        drawFilledSemiCircle( start, lineWidth / 2, lineAngle + M_PI / 2 );
    }

    // ..and now - draw the ending cap
    auto start = aPointGetter( i - 2 );
    auto end = aPointGetter( i - 1 );
    const VECTOR2D startEndVector = ( end - start );
    double lineAngle = startEndVector.Angle();
    drawFilledSemiCircle( end, lineWidth / 2, lineAngle - M_PI / 2 );
}


int OPENGL_GAL::drawBitmapChar( unsigned long aChar )
{
    const float TEX_X = font_image.width;
    const float TEX_Y = font_image.height;

    // handle space
    if( aChar == ' ' )
    {
        const FONT_GLYPH_TYPE* g = LookupGlyph( 'x' );
        wxASSERT( g );
        Translate( VECTOR2D( g->advance, 0 ) );
        return g->advance;
    }

    const FONT_GLYPH_TYPE* glyph = LookupGlyph( aChar );

    if( !glyph )    // Replace any non existing char shape by '?'
        glyph = LookupGlyph( '?' );

    if( !glyph )    // Should not occur
        return 0;

    const float X = glyph->atlas_x + font_information.smooth_pixels;
    const float Y = glyph->atlas_y + font_information.smooth_pixels;
    const float XOFF =  glyph->minx;

    // adjust for height rounding
    const float round_adjust =   ( glyph->maxy - glyph->miny )
                               - float( glyph->atlas_h - font_information.smooth_pixels * 2 );
    const float top_adjust   = font_information.max_y - glyph->maxy;
    const float YOFF = round_adjust + top_adjust;
    const float W    = glyph->atlas_w  - font_information.smooth_pixels *2;
    const float H    = glyph->atlas_h  - font_information.smooth_pixels *2;
    const float B    = 0;

    currentManager->Reserve( 6 );
    Translate( VECTOR2D( XOFF, YOFF ) );
    /* Glyph:
    * v0    v1
    *   +--+
    *   | /|
    *   |/ |
    *   +--+
    * v2    v3
    */
    currentManager->Shader( SHADER_FONT, X / TEX_X, ( Y + H ) / TEX_Y );
    currentManager->Vertex( -B,      -B, 0 );             // v0

    currentManager->Shader( SHADER_FONT, ( X + W ) / TEX_X, ( Y + H ) / TEX_Y );
    currentManager->Vertex( W + B,   -B, 0 );             // v1

    currentManager->Shader( SHADER_FONT, X / TEX_X, Y / TEX_Y );
    currentManager->Vertex( -B,   H + B, 0 );             // v2


    currentManager->Shader( SHADER_FONT, ( X + W ) / TEX_X, ( Y + H ) / TEX_Y );
    currentManager->Vertex( W + B, -B, 0 );               // v1

    currentManager->Shader( SHADER_FONT, X / TEX_X, Y / TEX_Y );
    currentManager->Vertex( -B,  H + B, 0 );              // v2

    currentManager->Shader( SHADER_FONT, ( X + W ) / TEX_X, Y / TEX_Y );
    currentManager->Vertex( W + B,  H + B, 0 );           // v3

    Translate( VECTOR2D( -XOFF + glyph->advance, -YOFF ) );

    return glyph->advance;
}


void OPENGL_GAL::drawBitmapOverbar( double aLength, double aHeight )
{
    // To draw an overbar, simply draw an overbar
    const FONT_GLYPH_TYPE* glyph = LookupGlyph( '_' );
    wxCHECK( glyph, /* void */ );

    const float H = glyph->maxy - glyph->miny;

    Save();

    Translate( VECTOR2D( -aLength, -aHeight-1.5*H ) );

    currentManager->Reserve( 6 );
    currentManager->Color( strokeColor.r, strokeColor.g, strokeColor.b, 1 );

    currentManager->Shader( 0 );

    currentManager->Vertex( 0, 0, 0 );          // v0
    currentManager->Vertex( aLength, 0, 0 );    // v1
    currentManager->Vertex( 0, H, 0 );          // v2

    currentManager->Vertex( aLength, 0, 0 );    // v1
    currentManager->Vertex( 0, H, 0 );          // v2
    currentManager->Vertex( aLength, H, 0 );    // v3

    Restore();
}


std::pair<VECTOR2D, float> OPENGL_GAL::computeBitmapTextSize( const UTF8& aText ) const
{
    VECTOR2D textSize( 0, 0 );
    float commonOffset = std::numeric_limits<float>::max();
    static const auto defaultGlyph = LookupGlyph( '(' ); // for strange chars

    for( UTF8::uni_iter chIt = aText.ubegin(), end = aText.uend(); chIt < end; ++chIt )
    {
        unsigned int c = *chIt;

        const FONT_GLYPH_TYPE* glyph = LookupGlyph( c );
        // Debug: show not coded char in the atlas
        // Be carefull before allowing the assert: it usually crash kicad
        // when the assert is made during a paint event.
        //wxASSERT( c == ' ' || glyph );    // space is not in the atlas, so not found

        // a few chars are strange in font
        if( !glyph || // Not coded in font
            c == '-' || c == '_' )     // Strange size of these 2 chars
        {
            glyph = defaultGlyph;
        }

        if( glyph )
        {
            textSize.x  += glyph->advance;
        }
    }

    textSize.y   = std::max<float>( textSize.y, font_information.max_y - defaultGlyph->miny );
    commonOffset = std::min<float>( font_information.max_y - defaultGlyph->maxy, commonOffset );
    textSize.y -= commonOffset;

    return std::make_pair( textSize, commonOffset );
}


void OPENGL_GAL::onPaint( wxPaintEvent& WXUNUSED( aEvent ) )
{
    PostPaint();
}


void OPENGL_GAL::skipMouseEvent( wxMouseEvent& aEvent )
{
    // Post the mouse event to the event listener registered in constructor, if any
    if( mouseListener )
        wxPostEvent( mouseListener, aEvent );
}


void OPENGL_GAL::blitCursor()
{
    if( !IsCursorEnabled() )
        return;

    compositor->SetBuffer( OPENGL_COMPOSITOR::DIRECT_RENDERING );

    const int cursorSize = fullscreenCursor ? 8000 : 80;

    VECTOR2D cursorBegin  = cursorPosition - cursorSize / ( 2 * worldScale );
    VECTOR2D cursorEnd    = cursorPosition + cursorSize / ( 2 * worldScale );
    VECTOR2D cursorCenter = ( cursorBegin + cursorEnd ) / 2;

    const COLOR4D cColor = getCursorColor();
    const COLOR4D color( cColor.r * cColor.a, cColor.g * cColor.a,
                         cColor.b * cColor.a, 1.0 );

    glActiveTexture( GL_TEXTURE0 );
    glDisable( GL_TEXTURE_2D );
    glLineWidth( 1.0 );
    glColor4d( color.r, color.g, color.b, color.a );

    glBegin( GL_LINES );
    glVertex2d( cursorCenter.x, cursorBegin.y );
    glVertex2d( cursorCenter.x, cursorEnd.y );

    glVertex2d( cursorBegin.x, cursorCenter.y );
    glVertex2d( cursorEnd.x, cursorCenter.y );
    glEnd();
}


unsigned int OPENGL_GAL::getNewGroupNumber()
{
    wxASSERT_MSG( groups.size() < std::numeric_limits<unsigned int>::max(),
                  wxT( "There are no free slots to store a group" ) );

    while( groups.find( groupCounter ) != groups.end() )
    {
        groupCounter++;
    }

    return groupCounter++;
}


void OPENGL_GAL::init()
{
    wxASSERT( IsShownOnScreen() );

    GL_CONTEXT_MANAGER::Get().LockCtx( glPrivContext, this );

    GLenum err = glewInit();

    try
    {
        if( GLEW_OK != err )
            throw std::runtime_error( (const char*) glewGetErrorString( err ) );

        // Check the OpenGL version (minimum 2.1 is required)
        if( !GLEW_VERSION_2_1 )
            throw std::runtime_error( "OpenGL 2.1 or higher is required!" );

#if defined (__LINUX__)      // calling enableGlDebug crashes opengl on some OS (OSX and some Windows)
#ifdef DEBUG
        if( GLEW_ARB_debug_output )
            enableGlDebug( true );
#endif
#endif

        // Framebuffers have to be supported
        if( !GLEW_EXT_framebuffer_object )
            throw std::runtime_error( "Framebuffer objects are not supported!" );

        // Vertex buffer has to be supported
        if( !GLEW_ARB_vertex_buffer_object )
            throw std::runtime_error( "Vertex buffer objects are not supported!" );

        // Prepare shaders
        if( !shader->IsLinked() && !shader->LoadShaderFromStrings( SHADER_TYPE_VERTEX, BUILTIN_SHADERS::kicad_vertex_shader ) )
            throw std::runtime_error( "Cannot compile vertex shader!" );

        if( !shader->IsLinked() && !shader->LoadShaderFromStrings( SHADER_TYPE_FRAGMENT, BUILTIN_SHADERS::kicad_fragment_shader ) )
            throw std::runtime_error( "Cannot compile fragment shader!" );

        if( !shader->IsLinked() && !shader->Link() )
            throw std::runtime_error( "Cannot link the shaders!" );

        // Check if video card supports textures big enough to fit the font atlas
        int maxTextureSize;
        glGetIntegerv( GL_MAX_TEXTURE_SIZE, &maxTextureSize );

        if( maxTextureSize < (int) font_image.width || maxTextureSize < (int)font_image.height )
        {
            // TODO implement software texture scaling
            // for bitmap fonts and use a higher resolution texture?
            throw std::runtime_error( "Requested texture size is not supported" );
        }
    }
    catch( std::runtime_error& )
    {
        GL_CONTEXT_MANAGER::Get().UnlockCtx( glPrivContext );
        throw;
    }

    cachedManager = new VERTEX_MANAGER( true );
    nonCachedManager = new VERTEX_MANAGER( false );
    overlayManager = new VERTEX_MANAGER( false );

    // Make VBOs use shaders
    cachedManager->SetShader( *shader );
    nonCachedManager->SetShader( *shader );
    overlayManager->SetShader( *shader );

    GL_CONTEXT_MANAGER::Get().UnlockCtx( glPrivContext );
    isInitialized = true;
}


// ------------------------------------- // Callback functions for the tesselator // ------------------------------------- // Compare Redbook Chapter 11
void CALLBACK VertexCallback( GLvoid* aVertexPtr, void* aData )
{
    GLdouble* vertex = static_cast<GLdouble*>( aVertexPtr );
    OPENGL_GAL::TessParams* param = static_cast<OPENGL_GAL::TessParams*>( aData );
    VERTEX_MANAGER* vboManager = param->vboManager;

    assert( vboManager );
    vboManager->Vertex( vertex[0], vertex[1], vertex[2] );
}


void CALLBACK CombineCallback( GLdouble coords[3],
                               GLdouble* vertex_data[4],
                               GLfloat weight[4], GLdouble** dataOut, void* aData )
{
    GLdouble* vertex = new GLdouble[3];
    OPENGL_GAL::TessParams* param = static_cast<OPENGL_GAL::TessParams*>( aData );

    // Save the pointer so we can delete it later
    param->intersectPoints.push_back( boost::shared_array<GLdouble>( vertex ) );

    memcpy( vertex, coords, 3 * sizeof(GLdouble) );

    *dataOut = vertex;
}


void CALLBACK EdgeCallback( GLboolean aEdgeFlag )
{
    // This callback is needed to force GLU tesselator to use triangles only
}


void CALLBACK ErrorCallback( GLenum aErrorCode )
{
    //throw std::runtime_error( std::string( "Tessellation error: " ) +
                              //std::string( (const char*) gluErrorString( aErrorCode ) );
}


static void InitTesselatorCallbacks( GLUtesselator* aTesselator )
{
    gluTessCallback( aTesselator, GLU_TESS_VERTEX_DATA,  ( void (CALLBACK*)() )VertexCallback );
    gluTessCallback( aTesselator, GLU_TESS_COMBINE_DATA, ( void (CALLBACK*)() )CombineCallback );
    gluTessCallback( aTesselator, GLU_TESS_EDGE_FLAG,    ( void (CALLBACK*)() )EdgeCallback );
    gluTessCallback( aTesselator, GLU_TESS_ERROR,        ( void (CALLBACK*)() )ErrorCallback );
}

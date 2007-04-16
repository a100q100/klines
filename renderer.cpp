/*******************************************************************
 *
 * Copyright 2006 Dmitry Suzdalev <dimsuz@gmail.com>
 *
 * This file is part of the KDE project "KLines"
 *
 * KLines is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * KLines is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with KAtomic; see the file COPYING.  If not, write to
 * the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 ********************************************************************/
#include "renderer.h"

#include <KStandardDirs>
#include <KSvgRenderer>
#include <KDebug>

#include <QPainter>

// note: this should be in sync with svg
static inline char color2char( BallColor col )
{
    switch( col )
    {
    case Blue:
        return 'b';
    case Brown:
        return 'f';
    case Cyan:
        return 'g';
    case Green:
        return 'c';
    case Red:
        return 'a';
    case Violet:
        return 'd';
    case Yellow:
        return 'e';
    default:
        return 'x'; // error
    }
}

KLinesRenderer* KLinesRenderer::self()
{
    static KLinesRenderer instance;
    return &instance;
}

KLinesRenderer::KLinesRenderer()
    : m_cellSize(32)
{
    m_renderer = new KSvgRenderer();
    m_renderer->load( KStandardDirs::locate( "appdata", "klines.svgz" ) );

    rerenderPixmaps();
}

KLinesRenderer::~KLinesRenderer()
{
    delete m_renderer;
}

QPixmap KLinesRenderer::ballPixmap(BallColor color) const
{
    // FIXME: it should be separate entry in svg!
    return bornPixmap( color, numBornFrames()-1 );
}

QPixmap KLinesRenderer::diePixmap(BallColor color, int frame) const
{
    QString id = QString( "%1_die_%2" ).arg( color2char( color ) ).arg( frame+1 );
    return m_pixHash.value( id );
}

QPixmap KLinesRenderer::bornPixmap(BallColor color, int frame) const
{
    QString id = QString( "%1_born_%2" ).arg( color2char( color ) ).arg( frame+1 );
    return m_pixHash.value( id );
}

QPixmap KLinesRenderer::selectedPixmap( BallColor color, int frame ) const
{
    QString id = QString( "%1_select_%2" ).arg( color2char( color ) ).arg( frame+1 );
    return m_pixHash.value( id );
}

QPixmap KLinesRenderer::backgroundTilePixmap() const
{
    return m_pixHash.value( "field_cell" );
}

void KLinesRenderer::rerenderPixmaps()
{
    QString items="abcdefg";
    QString id;

    QPainter p;
    for ( int i=0; i<items.size(); ++i )
    {
        // rendering born frames
        for ( int f=0; f<numBornFrames();f++ )
        {
            id = QString( "%1_born_%2" ).arg( items.at(i) ).arg( f+1 );
            QPixmap pix( m_cellSize, m_cellSize );
            pix.fill( Qt::transparent );
            p.begin( &pix );
            m_renderer->render( &p, id );
            p.end();
            m_pixHash[id] = pix;
        }
        // rendering "selected" frames
        for ( int f=0; f<numSelectedFrames();f++ )
        {
            id = QString( "%1_select_%2" ).arg( items.at(i) ).arg( f+1 );
            QPixmap pix( m_cellSize, m_cellSize );
            pix.fill( Qt::transparent );
            p.begin( &pix );
            m_renderer->render( &p, id );
            p.end();
            m_pixHash[id] = pix;
        }
        // rendering "die" frames
        for ( int f=0; f<numDieFrames();f++ )
        {
            id = QString( "%1_die_%2" ).arg( items.at(i) ).arg( f+1 );
            QPixmap pix( m_cellSize, m_cellSize );
            pix.fill( Qt::transparent );
            p.begin( &pix );
            m_renderer->render( &p, id );
            p.end();
            m_pixHash[id] = pix;
        }
    }
    QPixmap pix( m_cellSize, m_cellSize );
    pix.fill( Qt::transparent );
    p.begin( &pix );
    m_renderer->render( &p, "field_cell" );
    p.end();
    m_pixHash["field_cell"] = pix;
}
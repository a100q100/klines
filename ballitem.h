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
#ifndef BALL_ITEM_H
#define BALL_ITEM_H

#include <QGraphicsPixmapItem>
#include "commondefs.h"

class QTimeLine;
class KLinesRenderer;

class BallItem : public QObject, public QGraphicsPixmapItem
{
public:
    BallItem( QGraphicsScene* parent, KLinesRenderer* r );

    void setColor( BallColor c );
    BallColor color() const { return m_color; }
    /**
     *  Starts animation
     *  @param type type of animation sequence to play
     */
    void startAnimation( BallAnimationType type );
    void stopAnimation();
private slots:
    void animFrameChanged(int);
private:
    /**
     *  Renderer used to render ball's pixmaps
     */
    KLinesRenderer* m_renderer;
    /**
     *  Timeline for controlling animations
     */
    QTimeLine *m_timeLine;
    /**
     *  Color of the ball
     */
    BallColor m_color;
};

#endif

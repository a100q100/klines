/*
    Copyright 2000 Roman Merzlyakov <roman@sbrf.barrt.ru>
    Copyright 2000 Roman Razilov <roman@sbrf.barrt.ru>
    Copyright 2006 Dimitry Suzdalev <dimsuz@gmail.com>
    Copyright 2007 Simon Hürlimann <simon.huerlimann@huerlisi.ch>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "klines.h"
#include "renderer.h"
#include "prefs.h"
#include "mwidget.h"
#include "scene.h"

#include <KConfig>
#include <KAction>
#include <KActionCollection>
#include <KStandardAction>
#include <KScoreDialog>
#include <KToggleAction>
#include <KStatusBar>
#include <KLocale>
#include <KConfigDialog>
#include <KMessageBox>

#include <KStandardGameAction>
#include <KGameThemeSelector>

KLinesMainWindow::KLinesMainWindow()
{
    KLinesRenderer::self()->restoreSavedBackground();

    mwidget = new MainWidget(this);
    setCentralWidget( mwidget );

    connect(mwidget->scene(), SIGNAL(scoreChanged(int)), SLOT(updateScore(int)));
    connect(mwidget->scene(), SIGNAL(stateChanged(const QString &)), SLOT(slotStateChanged(const QString &)));
    connect(mwidget->scene(), SIGNAL(gameOver(int)), SLOT(gameOver(int)));

    statusBar()->insertItem(i18n("Score:"), 0);
    updateScore(0);

    setupActions();

    stateChanged("init");
}

KLinesMainWindow::~KLinesMainWindow()
{
}

void KLinesMainWindow::setupActions()
{
  // Game
  KStandardGameAction::gameNew(this, SLOT(startGame()), actionCollection());
  KStandardGameAction::highscores(this, SLOT(viewHighScore()), actionCollection());
  KStandardGameAction::quit(this, SLOT(close()), actionCollection());

  // Move
  KStandardGameAction::undo(mwidget->scene(), SLOT(undo()), actionCollection());
  KStandardGameAction::endTurn(mwidget->scene(), SLOT(endTurn()), actionCollection());

  // Preferences
  KToggleAction *showNext = actionCollection()->add<KToggleAction>("show_next");
  showNext->setText( i18n( "Show next" ) );
  connect(showNext, SIGNAL(triggered(bool) ), SLOT(showNextToggled(bool)));

  showNext->setChecked(Prefs::showNext());
  mwidget->setShowNextColors(Prefs::showNext());

  // Navigation
  actionCollection()->add<QAction>("navi_left", mwidget->scene(), SLOT(moveFocusLeft()));
  actionCollection()->add<QAction>("navi_right", mwidget->scene(), SLOT(moveFocusRight()));
  actionCollection()->add<QAction>("navi_up", mwidget->scene(), SLOT(moveFocusUp()));
  actionCollection()->add<QAction>("navi_down", mwidget->scene(), SLOT(moveFocusDown()));
  actionCollection()->add<QAction>("navi_select", mwidget->scene(), SLOT(cellSelected()));

  KStandardAction::preferences( this, SLOT( configureSettings() ), actionCollection() );
  setupGUI();
}

void KLinesMainWindow::updateScore(int score)
{
    statusBar()->changeItem(i18n("Score: %1", score), 0);
}

void KLinesMainWindow::gameOver(int score)
{
    KScoreDialog d(KScoreDialog::Name | KScoreDialog::Score, this);
    d.addScore(score, KScoreDialog::AskName);
    //   d.exec();
}

void KLinesMainWindow::viewHighScore()
{
   KScoreDialog d(KScoreDialog::Name | KScoreDialog::Score, this);
   d.exec();
}

void KLinesMainWindow::startGame()
{
    updateScore(0);
    mwidget->scene()->startNewGame();
}

void KLinesMainWindow::showNextToggled(bool show)
{
    mwidget->setShowNextColors(show);
    Prefs::setShowNext(show);
    Prefs::self()->writeConfig();
}


// FIXME these are strings from old tutorial
// leave them if I ever want to use them when I'll impelement tutorial mode
/**
       msg = i18n("The goal of the game is to put\n"
       msg = i18n("You can make horizontal, vertical\n"
                  "and diagonal lines.");
       msg = i18n("Each turn, three new balls are placed on the board.");
       msg = i18n("Every turn, you can move one ball.");
       msg = i18n("To move a ball, click on it with the mouse,\n"
                  "then click where you want the ball to go.");
       msg = i18n("You just moved the blue ball!");
       msg = i18n("Balls can be moved to every position on the board,\n"
                  "as long as there are no other balls in their way.");
       msg = i18n("Now we only need one more blue ball.");
       msg = i18n("It seems to be our lucky day!");
       msg = i18n("Hurray! And away they go!\n"
                  "Now lets try the green balls.");
       msg = i18n("Now you try!\n"
                  "Click on the green ball and move it to the others!");
       msg = i18n("Almost, try again!");
       msg = i18n("Very good!");
       msg = i18n("Whenever you complete a line you get an extra turn.");
       msg = i18n("This is the end of this tutorial.\n"
                  "Feel free to finish the game!");
                  */

void KLinesMainWindow::configureSettings()
{
    if ( KConfigDialog::showDialog( "settings" ) )
        return;
    KConfigDialog *dialog = new KConfigDialog( this, "settings", Prefs::self() );
    dialog->addPage( new KGameThemeSelector( dialog, Prefs::self() ), i18n( "Theme" ), "game_theme" );
    connect( dialog, SIGNAL( settingsChanged(const QString&) ), this, SLOT( loadSettings() ) );
    dialog->show();
}

void KLinesMainWindow::loadSettings()
{
    if ( !KLinesRenderer::self()->loadTheme(Prefs::theme()) )
    {
        KMessageBox::error( this,  i18n( "Failed to load \"%1\" theme. Please check your installation.", Prefs::theme() ) );
        return;
    }
    mwidget->scene()->invalidate( mwidget->scene()->sceneRect() ); // trigger complete redraw
}

bool KLinesMainWindow::queryClose()
{
    KLinesRenderer::self()->saveBackground(mwidget->scene()->sceneRect().size().toSize());
    return true;
}

#include "klines.moc"

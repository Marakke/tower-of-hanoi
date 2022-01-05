#include "mainwindow.hh"
#include "ui_mainwindow.h"

#include <QGraphicsRectItem>
#include <cmath>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->graphicsView->setScene(&scene_);
    this->setWindowTitle("Tunin Torni");

    ui->discsSpinBox->setMinimum(3);
    ui->discsSpinBox->setMaximum(8);
    ui->discsSpinBox->setValue(3);

    QColor labelsColor(225, 225, 225);
    labelsColor.setAlphaF(0.9);
    QPalette* labelsPalette = new QPalette();
    labelsPalette->setColor(QPalette::Window, labelsColor);
    ui->aLabel->setVisible(false);
    ui->sLabel->setVisible(false);
    ui->dLabel->setVisible(false);
    ui->aLabel->setAutoFillBackground(true);
    ui->aLabel->setPalette(*labelsPalette);
    ui->sLabel->setAutoFillBackground(true);
    ui->sLabel->setPalette(*labelsPalette);
    ui->dLabel->setAutoFillBackground(true);
    ui->dLabel->setPalette(*labelsPalette);
    delete labelsPalette;

    ui->beginningLabel->setVisible(false);
    ui->auxillairyLabel->setVisible(false);
    ui->targetLabel->setVisible(false);

    QColor backgroundColor(100, 0, 200);
    backgroundColor.setAlphaF(0.3);
    ui->graphicsView->setBackgroundBrush(QBrush(backgroundColor, Qt::SolidPattern));

    QPalette* lcdPalette = new QPalette();
    lcdPalette->setColor(QPalette::Window, backgroundColor);
    ui->movesLcdNumber->setAutoFillBackground(true);
    ui->movesLcdNumber->setPalette(*lcdPalette);
    ui->timeLcdNumber->setAutoFillBackground(true);
    ui->timeLcdNumber->setPalette(*lcdPalette);
    ui->bestTimeLcdNumber->setAutoFillBackground(true);
    ui->bestTimeLcdNumber->setPalette(*lcdPalette);
    delete lcdPalette;

    connect(&timeTimer_, &QTimer::timeout, this, &MainWindow::elapsedTime);
    connect(&flashingLightTimer_, &QTimer::timeout, this, &MainWindow::flashLights);

    ui->aButton->setVisible(false);
    ui->sButton->setVisible(false);
    ui->dButton->setVisible(false);
    // Keybindings activate their respective button.
    // This way we don't need Qt's keyPressEvent.
    ui->aButton->setShortcut(QKeySequence(Qt::Key_A));
    ui->sButton->setShortcut(QKeySequence(Qt::Key_S));
    ui->dButton->setShortcut(QKeySequence(Qt::Key_D));
    connect(ui->aButton, SIGNAL(clicked()), this, SLOT(onAButtonClicked()));
    connect(ui->sButton, SIGNAL(clicked()), this, SLOT(onSButtonClicked()));
    connect(ui->dButton, SIGNAL(clicked()), this, SLOT(onDButtonClicked()));

    connect(ui->playButton, SIGNAL(clicked()),
            this, SLOT(onPlayButtonClicked()));
    connect(ui->restartButton, SIGNAL(clicked()),
            this, SLOT(onRestartButtonClicked()));
    ui->restartButton->setDisabled(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onPlayButtonClicked()
{
    discsAmount_ = ui->discsSpinBox->value();

    ui->aLabel->show();
    ui->sLabel->show();
    ui->dLabel->show();
    ui->beginningLabel->show();
    ui->auxillairyLabel->show();
    ui->targetLabel->show();

    minimumMovesToSolve_ = pow(2,discsAmount_)-1;
    ui->solveNumberLabel->setText(QString::number(minimumMovesToSolve_));

    ui->playButton->setDisabled(true);
    ui->restartButton->setEnabled(true);
    ui->aButton->setVisible(true);
    ui->sButton->setVisible(true);
    ui->dButton->setVisible(true);

    timeTimer_.start(1000);
    flashingLightTimer_.start(101);

    adjustSceneArea(width_, height_);
    setupRods();
    setupDiscs(discsAmount_);
}

void MainWindow::onRestartButtonClicked()
{
    scene_.clear();

    discSelected_ = false;  // If a disc was selected when restarting, unselect it.
    gameEnd_ = false;
    flashMoves_ = false;
    flashTime_ = false;

    ui->aButton->setEnabled(true);
    ui->sButton->setEnabled(true);
    ui->dButton->setEnabled(true);

    // Reset the vectors.
    discs1_.clear();
    discs2_.clear();
    discs3_.clear();
    discWidths1_.clear();
    discWidths2_.clear();
    discWidths3_.clear();

    discTopWidth1_ = 0;
    discTopWidth2_ = 0;
    discTopWidth3_ = 0;
    movesTaken_ = 0;
    ui->movesLcdNumber->display(movesTaken_);
    illegalMovesLeft_ = 2;
    ui->illegalMovesLabel->setText(QString::number(illegalMovesLeft_));
    seconds_ = 1;
    ui->timeLcdNumber->display(0);
    ui->gameEndLabel->setText("");
    ui->solveTextabel->setText("Minimal number of moves left to solve:");

    QColor color(100, 0, 200);
    color.setAlphaF(0.3);

    QPalette* palette = new QPalette();
    palette->setColor(QPalette::Window, color);
    ui->movesLcdNumber->setPalette(*palette);
    ui->timeLcdNumber->setPalette(*palette);
    ui->bestTimeLcdNumber->setPalette(*palette);
    delete palette;

    ui->playButton->setDisabled(true);
    onPlayButtonClicked();
}

void MainWindow::onAButtonClicked()
{
    rod_ =  '1';
    discDirection();
}

void MainWindow::onSButtonClicked()
{
    rod_ = '2';
    discDirection();
}

void MainWindow::onDButtonClicked()
{
    rod_ = '3';
    discDirection();
}

void MainWindow::adjustSceneArea(int width, int height)
{
    const QRectF area(0, 0, width, height);
    scene_.setSceneRect(area);
    ui->graphicsView->fitInView(area);
}

void MainWindow::setupRods()
{
    const QRectF rod1(rod1pos_, 1, 1, rodHeight_);
    const QRectF rod2(rod2pos_, 1, 1, rodHeight_);
    const QRectF rod3(rod3pos_, 1, 1, rodHeight_);
    const QRectF table(0, height_-1, width_, 1);
    const QPen penRod(Qt::black, 0.02);
    const QBrush brushRod(Qt::darkRed);
    rod1_ = scene_.addRect(rod1, penRod, brushRod);
    rod2_ = scene_.addRect(rod2, penRod, brushRod);
    rod3_ = scene_.addRect(rod3, penRod, brushRod);
    table_ = scene_.addRect(table, penRod, brushRod);
}

void MainWindow::setupDiscs(int discsAmount)
{
    int bottomDiscWidth = 2 * discsAmount - 1;
    const QPen penDisc(Qt::black, 0.1);

    // Generate discs.
    for (int i=0; i<discsAmount; i++) {
        const QRectF disc(rod1pos_-discsAmount+1+i, rodHeight_-i,
                          bottomDiscWidth-2*i, 1);
        const QBrush brushDisc(Qt::green); // We change the discs' color later.
        disc_ = scene_.addRect(disc, penDisc, brushDisc);
        // Change disc' color evenly from bottom to top.
        QColor color(255 - ((i + 1) * (255 / discsAmount - 1)), 0, 255);
        disc_->setBrush(color);
        discTopPosY_ = i;
        discs1_.push_back(disc_);
        discWidths1_.push_back(disc.width());
    }
}

void MainWindow::elapsedTime()
{
    ui->timeLcdNumber->display(seconds_);
    if (!gameEnd_) {
        seconds_++;
    }
}

void MainWindow::discDirection()
{
    const QPen penDiscHighlight(Qt::black, 0.2);

    // Choose a disc from a rod.
    if (!discSelected_) {
        discSelected_ = true;
        if (rod_ == '1' && discs1_.size() > 0) {
            startingPos_ = '1';
            discs1_.back()->setPen(penDiscHighlight);
            discTopPosY_ = discs1_.size()-1;
        } else if (rod_ == '2' && discs2_.size() > 0) {
            startingPos_ = '2';
            discs2_.back()->setPen(penDiscHighlight);
            discTopPosY_ = discs2_.size()-1;
        } else if (rod_ == '3' && discs3_.size() > 0) {
            startingPos_ = '3';
            discs3_.back()->setPen(penDiscHighlight);
            discTopPosY_ = discs3_.size()-1;
        } else {
            discSelected_ = false;
        }
    }
    // Figure out where to move the selected disc.
    else {
        discSelected_ = false;
        if (rod_ == '1') {
            if (startingPos_ == '1') {
                moveDiscs(0, 0, rod_);
            } else if (startingPos_ == '2') {
                y_ = discTopPosY_-discs1_.size();
                moveDiscs(-15, y_, rod_);
            } else if (startingPos_ == '3') {
                y_ = discTopPosY_-discs1_.size();
                moveDiscs(-30, y_, rod_);
            }
        } else if (rod_ == '2') {
            if (startingPos_ == '1') {
                y_ = discTopPosY_-discs2_.size();
                moveDiscs(15, y_, rod_);
            } if (startingPos_ == '2') {
                moveDiscs(0, 0, rod_);
            } else if (startingPos_ == '3') {
                y_ = discTopPosY_-discs2_.size();
                moveDiscs(-15, y_, rod_);
            }
        } else if (rod_ == '3') {
            if (startingPos_ == '1') {
                y_ = discTopPosY_-discs3_.size();
                moveDiscs(30, y_, rod_);
            } else if (startingPos_ == '2') {
                y_ = discTopPosY_-discs3_.size();
                moveDiscs(15, y_, rod_);
            } else if (startingPos_ == '3') {
                moveDiscs(0, 0, rod_);
            }
        }
    }
}

void MainWindow::moveDiscs(int x, int y, char destination)
{
    const QPen penDisc(Qt::black, 0.1);

    if (startingPos_ == '1') {
        const QPointF oldPos = discs1_.back()->scenePos();
        const QPointF newPos = oldPos + QPoint(x, y);
        discs1_.back()->setPen(penDisc);

        if (destination == '1') {
            // Push the disc back to the vector, as it is later removed.
            discs1_.push_back(discs1_.back());
            discWidths1_.push_back(discTopWidth1_);
        } else if (destination == '2') {
            if (discTopWidth2_ > discTopWidth1_ || discs2_.size() == 0) {
                discs1_.back()->setPos(newPos);
                discs2_.push_back(discs1_.back());
                discWidths2_.push_back(discTopWidth1_);
            } else {
                // Illegal move, the disc stays where it started.
                discs1_.push_back(discs1_.back());
                discWidths1_.push_back(discTopWidth1_);
                destination = '1';
                illegalMovesLeft_--;
            }
        } else if (destination == '3') {
            if (discTopWidth3_ > discTopWidth1_ || discs3_.size() == 0) {
                discs1_.back()->setPos(newPos);
                discs3_.push_back(discs1_.back());
                discWidths3_.push_back(discTopWidth1_);
            } else {
                discs1_.push_back(discs1_.back());
                discWidths1_.push_back(discTopWidth1_);
                destination = '1';
                illegalMovesLeft_--;
            }
        }
        // Movement completed, remove the disc from the previous rod's vector.
        discs1_.pop_back();
        discWidths1_.pop_back();

    } else if (startingPos_ == '2') {
        const QPointF oldPos = discs2_.back()->scenePos();
        const QPointF newPos = oldPos + QPoint(x, y);
        discs2_.back()->setPen(penDisc);

        if (destination == '1') {
            if (discTopWidth1_ > discTopWidth2_ || discs1_.size() == 0) {
                discs2_.back()->setPos(newPos);
                discs1_.push_back(discs2_.back());
                discWidths1_.push_back(discTopWidth2_);
            } else {
                discs2_.push_back(discs2_.back());
                discWidths2_.push_back(discTopWidth2_);
                destination = '1';
                illegalMovesLeft_--;
            }
        } else if (destination == '2') {
            discs2_.push_back(discs2_.back());
            discWidths2_.push_back(discTopWidth2_);
        } else if (destination == '3') {
            if (discTopWidth3_ > discTopWidth2_ || discs3_.size() == 0) {
                discs2_.back()->setPos(newPos);
                discs3_.push_back(discs2_.back());
                discWidths3_.push_back(discTopWidth2_);
            } else {
                discs2_.push_back(discs2_.back());
                discWidths2_.push_back(discTopWidth2_);
                destination = '2';
                illegalMovesLeft_--;
            }
        }
        // Movement completed, remove the disc from the previous rod's vector.
        discs2_.pop_back();
        discWidths2_.pop_back();

    } else if (startingPos_ == '3') {
        const QPointF oldPos = discs3_.back()->scenePos();
        const QPointF newPos = oldPos + QPoint(x, y);
        discs3_.back()->setPen(penDisc);

        if (destination == '1') {
            if (discTopWidth1_ > discTopWidth3_ || discs1_.size() == 0) {
                discs3_.back()->setPos(newPos);
                discs1_.push_back(discs3_.back());
                discWidths1_.push_back(discTopWidth3_);
            } else {
                discs3_.push_back(discs3_.back());
                discWidths3_.push_back(discTopWidth3_);
                destination = '3';
                illegalMovesLeft_--;
            }
        } else if (destination == '2') {
            if (discTopWidth2_ > discTopWidth3_ || discs2_.size() == 0) {
                discs3_.back()->setPos(newPos);
                discs2_.push_back(discs3_.back());
                discWidths2_.push_back(discTopWidth3_);
            } else {
                discs3_.push_back(discs3_.back());
                discWidths3_.push_back(discTopWidth3_);
                destination = '3';
                illegalMovesLeft_--;
            }
        } else if (destination == '3') {
            discs3_.push_back(discs3_.back());
            discWidths3_.push_back(discTopWidth3_);
        }
        // Movement completed, remove the disc from the previous rod's vector.
        discs3_.pop_back();
        discWidths3_.pop_back();
    }
    if (destination != startingPos_) {
        movesTaken_++;
        ui->movesLcdNumber->display(movesTaken_);

        if (minimumMovesToSolve_ == 0 ||
                ui->solveTextabel->text() == "Moves over the minimum:") {
            minimumMovesToSolve_++;
            ui->solveTextabel->setText("Moves over the minimum:");
        } else {
            minimumMovesToSolve_--;
        }
        ui->solveNumberLabel->setText(QString::number(minimumMovesToSolve_));
    }
    ui->illegalMovesLabel->setText(QString::number(illegalMovesLeft_));

    // Update each rod's topmost disc's width.
    if (discWidths1_.size() > 0) {
        discTopWidth1_ = discWidths1_.back();
    }
    if (discWidths2_.size() > 0) {
        discTopWidth2_ = discWidths2_.back();
    }
    if (discWidths3_.size() > 0) {
        discTopWidth3_ = discWidths3_.back();
    }

    // Check if the game is lost.
    gameLost();
    // Check if the game is won.
    gameWon();
}

void MainWindow::gameLost()
{
    if (illegalMovesLeft_ == 0) {
        ui->gameEndLabel->setText("YOU HAVE LOST!");
        gameEnd_ = true;

        ui->aButton->setDisabled(true);
        ui->sButton->setDisabled(true);
        ui->dButton->setDisabled(true);
    }
}

void MainWindow::gameWon()
{
    int discs3 = discs3_.size();
    if (discs3 == discsAmount_) {
        ui->gameEndLabel->setText("YOU HAVE WON!");
        gameEnd_ = true;

        ui->aButton->setDisabled(true);
        ui->sButton->setDisabled(true);
        ui->dButton->setDisabled(true);

        QPalette* palette = new QPalette();
        palette->setColor(QPalette::Window, Qt::green);

        // Check if the game was finished with minimum moves.
        if (minimumMovesToSolve_ == 0) {
            flashMoves_ = true;
        }

        // Check if a new best time was achieved.
        if (bestTime_ >= seconds_) {
            flashTime_ = true;
            bestTime_ = seconds_;
            ui->bestTimeLcdNumber->display(bestTime_);
        }
        ui->timeLcdNumber->setPalette(*palette);
        ui->bestTimeLcdNumber->setPalette(*palette);
        delete palette;
    }
}

void MainWindow::flashLights()
{
    QColor color(100, 0, 200);
    color.setAlphaF(0.3);

    QPalette* palette = new QPalette();
    palette->setColor(QPalette::Window, color);
    QPalette* palette2 = new QPalette();
    palette2->setColor(QPalette::Window, Qt::green);

    // Flash moves LCD number.
    if (flashMoves_) {
        // Switch between the palettes each time the function is called.
        if ((flashCount_ % 2) == 0) {
            ui->movesLcdNumber->setPalette(*palette2);
        } else {
            ui->movesLcdNumber->setPalette(*palette);
        }
    }
    // Flash bestTime LCD number.
    if (flashTime_) {
        if ((flashCount_ % 2) == 0) {
            ui->bestTimeLcdNumber->setPalette(*palette2);
        } else {
            ui->bestTimeLcdNumber->setPalette(*palette);
        }
    }
    delete palette;
    delete palette2;

    flashCount_++;
}

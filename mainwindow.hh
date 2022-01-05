#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QMainWindow>
#include <QGraphicsScene>
#include <QVector>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /* Construct a MainWindow.
     *
     * @param parent The parent widget of this MainWindow.
     */
    explicit MainWindow(QWidget *parent = 0);

    /* Destruct a MainWindow.
     */
    ~MainWindow();

private slots:
    /* Start the game.
     */
    void onPlayButtonClicked();

    /* Restart the game.
     */
    void onRestartButtonClicked();

    /* Select a disc from the first rod, or move a disc there.
     */
    void onAButtonClicked();

    /* Select a disc from the second rod, or move a disc there.
     */
    void onSButtonClicked();

    /* Select a disc from the thrid rod, or move a disc there.
     */
    void onDButtonClicked();

    /* Make the play field visible and fit it into the view.
     */
    void adjustSceneArea(int width, int height);

    /* Create rods.
     */
    void setupRods();

    /* Create discs based on the given amount of discs.
     *
     * @param discsAmount The amount of discs from a spin box.
     */
    void setupDiscs(int discsAmount);

    /* Count time when game is started.
     */
    void elapsedTime();

    /* Figure out the direction of the movable disc.
     */
    void discDirection();

    /* Acutally move the disc.
     *
     * @param x Determine how much to move the disc on x-axis.
     * @param y Determine how much to move the disc on y-axis.
     * @param destination The rod where to move the disc.
     */
    void moveDiscs(int x, int y, char destination);

    /* Called when the game is lost.
     */
    void gameLost();

    /* Called when the game is won.
     */
    void gameWon();

    /* Flash moves LCD number when the game is finished with minimum moves.
     */
    void flashLights();

private:
    Ui::MainWindow *ui;  // Access the UI widgets.
    QGraphicsScene scene_;  // Manage drawable objects.

    // Rectangular items of the game.
    QGraphicsRectItem* rod1_ = nullptr;
    QGraphicsRectItem* rod2_ = nullptr;
    QGraphicsRectItem* rod3_ = nullptr;
    QGraphicsRectItem* table_ = nullptr;
    QGraphicsRectItem* disc_ = nullptr;

    QTimer timeTimer_;  // Timer for counting elapsed time.
    QTimer flashingLightTimer_;  // Timer for flashing moves LCD number.

    // Game windwow's width and height.
    int width_ = 45;
    int height_ = 11;

    int rodHeight_ = 9;

    // The x-coordinates of each rod.
    int rod1pos_ = 7;
    int rod2pos_ = 22;
    int rod3pos_ = 37;

    int discsAmount_ = 0;  // The amount of discs in game.
    int y_ = 0;  // Determine how much to move the disc on y-axis.
    int discTopPosY_ = 0; // The y-coordinate of the topmost disc for selected rod.

    // The widths of the topmost disc for each rod.
    int discTopWidth1_ = 0;
    int discTopWidth2_ = 0;
    int discTopWidth3_ = 0;

    int movesTaken_ = 0;  // Moves taken in a single game.
    int seconds_ = 1;  // Elapsed time in seconds.
    int bestTime_ = 1000;  // Best time in the current game session.
    int flashCount_ = 0;  // Used to flash the moves LCD number.
    int illegalMovesLeft_ = 2;  // Amount of illegal moves in a single game.
    int minimumMovesToSolve_ = 0;  // Depends on the amount of discs.

    char rod_ = 'X';  // Selected rod based on user's key input.
    char startingPos_ = 'X';  // The starting rod of selected disc.

    bool discSelected_ = false;  // Determine if a disc is selected.
    bool gameEnd_ = false;  // Determine if the game has ended.
    bool flashMoves_ = false;  // Determine whether moves LCD number should flash.
    bool flashTime_ = false;  // Determine whether bestTime LCD number should flash.

    // All discs in each rod.
    QVector<QGraphicsRectItem*> discs1_;
    QVector<QGraphicsRectItem*> discs2_;
    QVector<QGraphicsRectItem*> discs3_;

    // All disc widths in each rod.
    std::vector<int> discWidths1_;
    std::vector<int> discWidths2_;
    std::vector<int> discWidths3_;
};

#endif // MAINWINDOW_HH

#include <iostream>
#include <cstdlib>
#include <raylib.h>
#include <string>
#include <vector>
#include <map>
#include <math.h>
#include <ctime>
#include <fstream>

using namespace std;

#define TARGETFPS 60 // Check for any update in the game for 60 times in a second.

class Colors
{
private:
    vector<Color> gameColors;
    vector<Color> getCellColors(); // By making this function private, we make sure that only member functions within the class or it's friend classes can access this function. Hence it cannot be directly called from main.
    friend class Grid;
    friend class Block;

public:
    Colors();
    Color getBlockColor(int blockId); // Getter for each individual color of a block [--------GETTER---------]
};

Colors::Colors()
{
    gameColors = getCellColors();
}
vector<Color> Colors::getCellColors()
{

    const Color Blue = {13, 64, 216, 255};
    const Color Cyan = {21, 204, 209, 255};
    const Color Purple = {166, 0, 247, 255};
    const Color Yellow = {237, 234, 4, 255};
    const Color Orange = {226, 116, 17, 255};
    const Color Red = {232, 18, 18, 255};
    const Color Green = {47, 230, 23, 255};
    const Color Grey = {26, 31, 40, 255};

    return {Grey, Purple, Blue, Green, Red, Cyan, Yellow, Orange};
}

Color Colors::getBlockColor(int blockId)
{
    return gameColors[blockId];
}

class Grid
{
    // vector<Color> colors;
    bool IsRowFull(int row);
    void ClearRow(int row);
    void MoveRowDown(int row, int numRows);
    int numRows;
    int numColumns;
    int cellSize;
    int **grid;

public:
    Grid();
    void setGrid();                                          // Initializes the Game Grid to 0  [---------SETTER-------]
    void setGridCell(int Xindex, int Yindex, int cellValue); // Sets the individual Cell Value [---------SETTER-------]
    void getGrid();                                          // Displaying the Grid on Console   [---------GETTER-------]
    void Draw();                                             // Draws Square on the the whole grid, and then assigns the color to each square based on the cell Value;
    bool isCellInsideGrid(int row, int column);
    bool IsCellEmpty(int row, int column);
    int ClearFullRows();
};

Grid::Grid()
{
    numRows = 20;
    numColumns = 10;
    cellSize = 40;

    // Dynamically allocating the Grid  [-------------DMA----------]
    grid = new int *[numRows];

    for (int i = 0; i < numRows; i++)
    {
        grid[i] = new int[numColumns];
    }

    setGrid(); // Calls the setGrid Function to initialize every cell value as 0 initially.

    Colors colors;
}

void Grid::setGrid() /// Initializes the whole Game Grid to 0  [---------SETTER-------]
{
    {
        for (int i = 0; i < numRows; i++)
        {
            for (int j = 0; j < numColumns; j++)
            {
                grid[i][j] = 0; // Initially all cell values are assigned ==0 , which means that the whole grid is Gret i.e empty
            }
        }
    }
}

void Grid::setGridCell(int Xindex, int Yindex, int cellValue)
{
    if (isCellInsideGrid(Xindex, Yindex))
    {
        grid[Xindex][Yindex] = cellValue;
    }
}

void Grid::getGrid() // Displaying the Grid on Console   [---------GETTER-------]
{
    for (int i = 0; i < numRows; i++)
    {
        for (int j = 0; j < numColumns; j++)
        {
            cout << grid[i][j] << "  ";
        }
        cout << endl;
    }
}

void Grid::Draw() // Draws Square on the the whole grid, and then assigns the color to each square based on the cell Value;
{
    for (int i = 0; i < numRows; i++)
    {
        for (int j = 0; j < numColumns; j++)
        {
            int CellValue = grid[i][j];
            Colors c;
            DrawRectangle((j * cellSize) + 33, (i * cellSize) + 33, cellSize - 1, cellSize - 1, c.gameColors[CellValue]); //  +1 Adds a 1 pixel offset in order to add the margin to make the border of each square visible. Secondly, -1 reduces the cell size to 29 pixels so that the overall grid size remains same.
        }
    }
}

bool Grid::isCellInsideGrid(int row, int column)
{
    if (row >= 0 and row < numRows and column >= 0 and column < numColumns)
    {
        return true;
    }
    return false;
}

bool Grid::IsCellEmpty(int row, int column)
{
    if (isCellInsideGrid(row, column))
    {
        return grid[row][column] == 0;
    }
    return false;
}

int Grid::ClearFullRows()
{
    int completed = 0;
    for (int row = numRows - 1; row >= 0; row--)
    {
        if (IsRowFull(row))
        {
            ClearRow(row);
            completed++;
        }
        else if (completed > 0)
        {
            MoveRowDown(row, completed);
        }
    }
    return completed;
}

bool Grid::IsRowFull(int row)
{
    for (int column = 0; column < numColumns; column++)
    {
        if (grid[row][column] == 0)
        {
            return false;
        }
    }
    return true;
}

void Grid::ClearRow(int row)
{
    for (int column = 0; column < numColumns; column++)
    {
        grid[row][column] = 0;
    }
}

void Grid::MoveRowDown(int row, int numRows)
{
    for (int column = 0; column < numColumns; column++)
    {
        grid[row + numRows][column] = grid[row][column];
        grid[row][column] = 0;
    }
}

class Position
{
public:
    int row;
    int column;
    Position();
    Position(int row, int column);
};

Position::Position() {}
Position ::Position(int row, int column) // [----------PARAMETERIZED CONSTRUCTOR-------------]
{
    this->row = row;
    this->column = column;
}

class Block
{
private:
    int uniqueId;
    int cellSize;
    int rotationState;
    int moveX; // Moves the block to X units left/right .
    int moveY; // Moves the block to Y units down. NOT UP!!

public:
    Position position; // [-----------FRIEND CLASS-----------]
    Colors color;
    // [-----------FRIEND CLASS-----------]
    map<int, vector<Position>> cells; // A map is a container that contains elements (these elements can be arrays) in which each element is associated with a unique key.

    Block();
    void setUniqueId(int uniqueId); // [----SETTER---] Setter for the id of each different type of block
    int getUniqueId();              // [----GETTER---] Getter for the id of each different type of block
    void Draw(int offsetX, int offsetY);
    vector<Position> getCellsPosition();
    void move(int moveX, int moveY);
    void initialOffset(); // As every block comes from the center but it has different dimensions, the offset will be different for each different type of tetramino. Also , we have to restrict the game to make the object of "Block Class" , therefore making it Abstract class by using a pure virtual function.
    void Rotate();
    void UndoRotation();
};

Block ::Block()
{
    cellSize = 40;
    rotationState = 0; // Current Rotation state.
    moveX = 0;
    moveY = 0;
}

void Block::Draw(int offsetX, int offsetY)
{
    vector<Position> tiles = getCellsPosition();
    for (Position item : tiles)
    {
        DrawRectangle(item.column * cellSize + offsetX, item.row * cellSize + offsetY, cellSize - 1, cellSize - 1, color.gameColors[uniqueId]);
    }
}

void Block::setUniqueId(int uniqueId)
{
    this->uniqueId = uniqueId;
}

int Block::getUniqueId()
{
    return uniqueId;
}

vector<Position> Block::getCellsPosition() // Calculates the new position that the block has to move.
{
    vector<Position> currentPosition = cells[rotationState]; // Gives Initial Position
    vector<Position> movedTiles;
    for (Position temp : currentPosition) // Loops through the current Position to reach the new (Required Position)...
    {
        Position newPosition = Position(temp.row + moveX, temp.column + moveY);
        movedTiles.push_back(newPosition);
    }
    return movedTiles;
}

void Block::Rotate()
{
    rotationState++;
    if (rotationState == (int)cells.size())
    {
        rotationState = 0;
    }
}

void Block::UndoRotation()
{
    rotationState--;
    if (rotationState == -1)
    {
        rotationState = cells.size() - 1;
    }
}

void Block::move(int rows, int columns)
{
    moveX += rows;
    moveY += columns;
}

class LBlock : public Block
{
public:
    LBlock();
    void initialOffset();
};

LBlock ::LBlock()
{
    setUniqueId(1);

    cells[0] = {Position(0, 2), Position(1, 0), Position(1, 1), Position(1, 2)};
    cells[1] = {Position(0, 1), Position(1, 1), Position(2, 1), Position(2, 2)};
    cells[2] = {Position(1, 0), Position(1, 1), Position(1, 2), Position(2, 0)};
    cells[3] = {Position(0, 0), Position(0, 1), Position(1, 1), Position(2, 1)};

    initialOffset();
}

void LBlock::initialOffset()
{
    move(0, 3);
}

class JBlock : public Block
{
public:
    JBlock();
    void initialOffset();
};

JBlock ::JBlock()
{
    setUniqueId(2);

    cells[0] = {Position(0, 0), Position(1, 0), Position(1, 1), Position(1, 2)};
    cells[1] = {Position(0, 1), Position(0, 2), Position(1, 1), Position(2, 1)};
    cells[2] = {Position(1, 0), Position(1, 1), Position(1, 2), Position(2, 2)};
    cells[3] = {Position(0, 1), Position(1, 1), Position(2, 0), Position(2, 1)};

    initialOffset();
}

void JBlock::initialOffset()
{
    move(0, 3);
}

class IBlock : public Block
{
public:
    IBlock();
    void initialOffset();
};

IBlock ::IBlock()
{
    setUniqueId(3);

    cells[0] = {Position(1, 0), Position(1, 1), Position(1, 2), Position(1, 3)};
    cells[1] = {Position(0, 2), Position(1, 2), Position(2, 2), Position(3, 2)};
    cells[2] = {Position(2, 0), Position(2, 1), Position(2, 2), Position(2, 3)};
    cells[3] = {Position(0, 1), Position(1, 1), Position(2, 1), Position(3, 1)};

    initialOffset();
}

void IBlock::initialOffset()
{
    move(-1, 3);
}

class OBlock : public Block
{
public:
    OBlock();
    void initialOffset();
};

OBlock ::OBlock()
{
    setUniqueId(4);

    cells[0] = {Position(0, 0), Position(0, 1), Position(1, 0), Position(1, 1)}; // Because square block is same for all rotations hence it has only one rotation state..

    initialOffset();
}

void OBlock::initialOffset()
{
    move(0, 4);
}

class SBlock : public Block
{
public:
    SBlock();
    void initialOffset();
};

SBlock ::SBlock()
{
    setUniqueId(5);

    cells[0] = {Position(0, 1), Position(0, 2), Position(1, 0), Position(1, 1)};
    cells[1] = {Position(0, 1), Position(1, 1), Position(1, 2), Position(2, 2)};
    cells[2] = {Position(1, 1), Position(1, 2), Position(2, 0), Position(2, 1)};
    cells[3] = {Position(0, 0), Position(1, 0), Position(1, 1), Position(2, 1)};

    initialOffset();
}

void SBlock::initialOffset()
{
    move(0, 3);
}

class TBlock : public Block
{
public:
    TBlock();
    void initialOffset();
};

TBlock ::TBlock()
{
    setUniqueId(6);

    cells[0] = {Position(0, 1), Position(1, 0), Position(1, 1), Position(1, 2)};
    cells[1] = {Position(0, 1), Position(1, 1), Position(1, 2), Position(2, 1)};
    cells[2] = {Position(1, 0), Position(1, 1), Position(1, 2), Position(2, 1)};
    cells[3] = {Position(0, 1), Position(1, 0), Position(1, 1), Position(2, 1)};

    initialOffset();
}

void TBlock::initialOffset()
{
    move(0, 3);
}

class ZBlock : public Block
{
public:
    ZBlock();
    void initialOffset();
};

ZBlock ::ZBlock()
{
    setUniqueId(7);

    cells[0] = {Position(0, 0), Position(0, 1), Position(1, 1), Position(1, 2)};
    cells[1] = {Position(0, 2), Position(1, 1), Position(1, 2), Position(2, 1)};
    cells[2] = {Position(1, 0), Position(1, 1), Position(2, 1), Position(2, 2)};
    cells[3] = {Position(0, 1), Position(1, 0), Position(1, 1), Position(2, 0)};

    initialOffset();
}

void ZBlock::initialOffset()
{
    move(0, 3);
}

class Game
{
private:
    Grid gameGrid; //[--------------HAS-A-RELATIONSHIP-----------(COMPOSITION)]

    Block getRandomBlock();
    vector<Block> getAllBlocks();
    void moveBlockLeft();
    void moveBlockRight();
    vector<Block> blocks; //[--------------HAS-A-RELATIONSHIP-----------(COMPOSITION)]
    Block currentBlock;
    Block nextBlock;
    bool isBlockOutside();
    void RotateBlock();
    void LockBlock();
    bool BlockFits();
    void Reset();
    void UpdateScore(int linesClear, int moveDownPoints);

public:
    Game();
    void Draw();
    void handleInput();
    void saveState();
    int loadState();

    void moveBlockDown();
    bool gameOver;
    int score;
    int level;
    double fallSpeed; // Speed at which blocks fall
    int linesToNextLevel;
    int linesCleared;
};

Game::Game()
{
    gameGrid = Grid();       // Creates a Grid.
    blocks = getAllBlocks(); // Creates the Game Blocks...
    currentBlock = getRandomBlock();
    nextBlock = getRandomBlock();
    gameOver = false;
    score = 0;
    level = 1;
    fallSpeed = 0.3; // Initial fall speed
    linesToNextLevel = 2;
}

void Game::saveState()
{
    fstream highScoreFile("highScore.bin", ios::in | ios::out | ios::binary);

    // Check if the file opened successfully
    if (highScoreFile.is_open())
    {
        int currentHighScore;
        highScoreFile >> currentHighScore;
        cout << "-----------------------" << currentHighScore << endl;
        if (this->score > currentHighScore)
        {
            cout << "-----------------------" << score << endl;
            highScoreFile.seekg(0);

            highScoreFile << score;
        }

        highScoreFile.close();
    }
    else
    {
        highScoreFile.open("highScore.bin", ios::out | ios::binary);
        if (highScoreFile.is_open())
        {
            highScoreFile << 0;
            highScoreFile.close();
        }
        else
        {
            cerr << "Error: Unable to create file." << endl;
        }
    }
}

int Game::loadState()
{
    ifstream highScoreFile("highScore.bin", ios::binary);

    if (highScoreFile.is_open())
    {
        int highScore;
        highScoreFile >> highScore;

        // Close the file
        highScoreFile.close();

        return highScore;
    }
    else
    {
        cerr << "Error: Unable to open file." << endl;
        return 0;
    }
}

Block Game::getRandomBlock()
{
    if (blocks.empty())
    {
        blocks = getAllBlocks();
    }
    srand(time(NULL));
    int randomIndex = rand() % blocks.size();
    Block randomBlock = blocks[randomIndex];
    blocks.erase(blocks.begin() + randomIndex);
    return randomBlock;
}

vector<Block> Game::getAllBlocks()
{
    vector<Block> temp;
    temp.push_back(LBlock());
    temp.push_back(IBlock());
    temp.push_back(JBlock());
    temp.push_back(SBlock());
    temp.push_back(OBlock());
    temp.push_back(ZBlock());
    temp.push_back(TBlock());
    return temp;
}

void Game::Draw()
{
    gameGrid.Draw();
    currentBlock.Draw(33, 33);
    switch (nextBlock.getUniqueId())
    {
    case 3:
        nextBlock.Draw(255 + 122, 280);
        break;
    case 4:
        nextBlock.Draw(255 + 115, 270);
        break;
    default:
        nextBlock.Draw(270 + 130, 270);
        break;
    }
}

void Game::handleInput()
{
    int keyPressed = GetKeyPressed();
    if (gameOver && keyPressed != 0)
    {
        gameOver = false;
        Reset();
    }
    switch (keyPressed)
    {
    case KEY_LEFT:
        moveBlockLeft();
        break;
    case KEY_RIGHT:
        moveBlockRight();
        break;
    case KEY_DOWN:
        moveBlockDown();
        UpdateScore(0, 1);
        break;
    case KEY_UP:
        RotateBlock();
        break;
    }
}

bool Game::isBlockOutside()
{
    vector<Position> tiles = currentBlock.getCellsPosition();
    for (Position item : tiles)
    {
        if (gameGrid.isCellInsideGrid(item.row, item.column))
        {
            return false;
        }
    }
    return true;
}

void Game::RotateBlock()
{
    if (!gameOver)
    {
        currentBlock.Rotate();
        if (isBlockOutside() || BlockFits() == false)
        {
            currentBlock.UndoRotation();
        }
    }
}

void Game::LockBlock()
{
    vector<Position> tiles = currentBlock.getCellsPosition();
    for (Position item : tiles)
    {
        gameGrid.setGridCell(item.row, item.column, currentBlock.getUniqueId());
    }
    currentBlock = nextBlock;
    if (BlockFits() == false)
    {
        gameOver = true;
    }
    nextBlock = getRandomBlock();
    int rowsCleared = gameGrid.ClearFullRows();
    UpdateScore(rowsCleared, 0);
}

bool Game::BlockFits()
{
    vector<Position> tiles = currentBlock.getCellsPosition();
    for (Position item : tiles)
    {
        if (gameGrid.IsCellEmpty(item.row, item.column) == false)
        {
            return false;
        }
    }
    return true;
}

void Game::Reset()
{
    gameGrid.setGrid();
    blocks = getAllBlocks();
    currentBlock = getRandomBlock();
    nextBlock = getRandomBlock();
    score = 0;
}

void Game::UpdateScore(int linesClear, int moveDownPoints)
{
    linesCleared = linesClear;
    switch (linesCleared)
    {
    case 1:
        score += 100;
        break;
    case 2:
        score += 300;
        break;
    case 3:
        score += 500;
        break;
    case 4:
        score += 700;
        break;
    default:
        break;
    }
    score += moveDownPoints;
    linesToNextLevel -= linesCleared;
    if (linesToNextLevel <= 0)
    {
        level++;
        // fallSpeed *= 0.8;                        // Increase the speed as the level increases
        linesToNextLevel = 10 + (level - 1) * 3; // Increase the lines required for next level
    }
}

void Game::moveBlockLeft()
{
    if (!gameOver)
    {
        currentBlock.move(0, -1);
        if (isBlockOutside() || BlockFits() == false)
        {
            currentBlock.move(0, 1);
        }
    }
}

void Game::moveBlockRight()
{
    if (!gameOver)
    {
        currentBlock.move(0, 1);
        if (isBlockOutside() || BlockFits() == false)
        {
            currentBlock.move(0, -1);
        }
    }
}

void Game::moveBlockDown()
{
    if (!gameOver)
    {
        currentBlock.move(1, 0);
        if (isBlockOutside() || BlockFits() == false)
        {
            currentBlock.move(-1, 0);
            LockBlock();
        }
    }
}

class gameLevel
{
public:
    virtual double DifficultyLevel() = 0;
};

class Level1 : public gameLevel
{
    double speed;

public:
    double DifficultyLevel()
    {
        speed = 0.4;
        return speed;
    }
};

class Level2 : public gameLevel
{
    double speed;

public:
    double DifficultyLevel()
    {
        speed = 0.35;
        return speed;
    }
};

class Level3 : public gameLevel
{
    double speed;

public:
    double DifficultyLevel()
    {
        speed = 0.3;
        return speed;
    }
};

class Level4 : public gameLevel
{
    double speed;

public:
    double DifficultyLevel()
    {
        speed = 0.28;
        return speed;
    }
};

class Level5 : public gameLevel
{
    double speed;

public:
    double DifficultyLevel()
    {
        speed = 0.25;
        return speed;
    }
};

class Level6 : public gameLevel
{
    double speed;

public:
    double DifficultyLevel()
    {
        speed = 0.24;
        return speed;
    }
};

double lastUpdateTime = 0;

bool EventTriggered(double interval)
{
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= interval)
    {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

int main()
{
    system("cls");
    InitWindow(700, 875, "TETRIS BY ZIA"); // Initial Game Screen Dimensions

    SetTargetFPS(TARGETFPS);

    Font font = LoadFontEx("Font/monogram.ttf", 64, 0, 0);

    Game game = Game();

    int highScore = game.loadState();

    double speed = 0.3;

    string playerName;

    fstream NameAndScoreFile("NameAndScoreFile.txt", ios::app);

    gameLevel *g[6];
    g[0] = new Level1();
    g[1] = new Level2();
    g[2] = new Level3();
    g[3] = new Level4();
    g[4] = new Level5();
    g[5] = new Level6();

    bool input = true;

    while (!WindowShouldClose())
    {
        if (input)
        {
            BeginDrawing();
            ClearBackground({221, 160, 221, 255});
            DrawText("Enter your name:", 115, 300, 50, WHITE);
            DrawText(playerName.c_str(), 115, 400, 50, WHITE);
            DrawText("Made with HEART by: ", 20, 700, 25, WHITE);
            DrawText("1. Ibad ur Rehman (23K-0517)", 20, 750, 20, WHITE);
            DrawText("2. Zubair Ahmed (23K-0907)", 20, 780, 20, WHITE);
            DrawText("3. Ahsan Faizan (23K-0615)", 20, 810, 20, WHITE);

            int key = GetCharPressed();
            if (key != 0)
            {
                playerName += (char)key;
            }
            if (IsKeyPressed(KEY_ENTER) && playerName.length() > 0)
                input = false;
            if (IsKeyPressed(KEY_BACKSPACE) && playerName.length() > 0)
                playerName.pop_back();
            EndDrawing();
        }
        else
        {

            if (game.level == 1)
                speed = g[0]->DifficultyLevel();
            else if (game.level == 2)
                speed = g[1]->DifficultyLevel();
            else if (game.level == 3)
                speed = g[2]->DifficultyLevel();
            else if (game.level == 4)
                speed = g[3]->DifficultyLevel();
            else if (game.level == 5)
                speed = g[4]->DifficultyLevel();
            else
                speed = g[5]->DifficultyLevel();
            game.handleInput();
            if (EventTriggered(speed))
            {
                game.moveBlockDown();
            }

            if (game.gameOver)
            {
                game.level = 1;
                DrawTextEx(font, "GAME OVER", {320 + 170, 450}, 25, 2, WHITE);
                if (game.score > highScore)
                {
                    highScore = game.score;
                    ofstream file("highscore.txt", ios::app);
                    if (file.is_open())
                    {
                        file << highScore << endl; // Writing the high score to the file
                        file.close();              // Closing the file stream
                    }
                    else
                    {
                        std::cerr << "Unable to open file for writing." << std::endl;
                    }
                }
            }

            BeginDrawing(); // Builds a blank canvas

            ClearBackground({221, 160, 221, 255});
            DrawTextEx(font, "Score", {365 + 155, 15}, 38, 2, WHITE);
            DrawTextEx(font, "High Score", {318 + 170, 500}, 32, 2, WHITE);
            DrawTextEx(font, "Next", {370 + 170, 175}, 38, 2, WHITE);
            DrawTextEx(font, "Level", {350 + 170, 600 + 70}, 38, 2, WHITE);
            DrawTextEx(font, "Welcome! ", {33, 10}, 20, 2, WHITE);
            DrawTextEx(font, playerName.c_str(), {33 + 100, 10}, 20, 2, WHITE);
            DrawTextEx(font, "Lines Cleared: ", {450 + 30, 800}, 20, 2, WHITE);
            string num = to_string(game.linesCleared);
            DrawTextEx(font, num.c_str(), {600 + 30, 800}, 20, 2, WHITE);

            // DrawTextEx(font, (char *)game.linesCleared, {330 + 80, 800}, 20, 2, WHITE);

            DrawRectangleRounded({320 + 170, 55, 170, 60}, 0.3, 6, {186, 85, 211, 255});  // For Score
            DrawRectangleRounded({320 + 170, 540, 170, 60}, 0.3, 6, {186, 85, 211, 255}); // For High Score
            DrawRectangleRounded({320 + 170, 640 + 70, 170, 60}, 0.3, 6, {186, 85, 211, 255});

            char scoreText[10];
            char highScoreText[10];
            char levelText[10];

            sprintf(scoreText, "%d", game.score);
            sprintf(highScoreText, "%d", highScore);
            sprintf(levelText, "%d", game.level);

            Vector2 textSize1 = MeasureTextEx(font, scoreText, 38, 2);
            Vector2 textSize2 = MeasureTextEx(font, highScoreText, 38, 2);
            Vector2 textSize3 = MeasureTextEx(font, levelText, 38, 2);

            DrawTextEx(font, scoreText, {320 + 170 + (170 - textSize1.x) / 2, 65}, 38, 2, WHITE);
            DrawTextEx(font, highScoreText, {320 + 170 + (170 - textSize2.x) / 2, 550}, 38, 2, WHITE);
            DrawTextEx(font, levelText, {320 + 170 + (170 - textSize3.x) / 2, 650 + 70}, 38, 2, WHITE);

            DrawRectangleRounded({320 + 170, 215, 170, 180}, 0.3, 6, {186, 85, 211, 255});

            game.Draw();

            EndDrawing();
        }
    }
    game.saveState();

    NameAndScoreFile << playerName << endl;
    NameAndScoreFile << game.score << endl;

    CloseWindow();

    return 0;
}

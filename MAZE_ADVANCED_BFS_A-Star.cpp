/*

This code generates a random maze with walls represented as black blocks, a surrounding wall, and entry and
exit points marked as red and green blocks. Adjust the density of walls by modifying the rand() % 3 == 0
condition to control the ratio of walls to open spaces.

MAZE SOLVER With BFS or A*
Alt-S -->  solve maze with plain BFS algorithm & SOLUTION is saved automatically
Ctrl-S --> solve maze with A* algorithm & SOLUTION is saved automatically

G --> Generate new maze
C --> clean maze, build walls
Alt-C --> Refresh current maze by cleaning Solution's Path and Field

S --> save maze

L --> load maze
Alt-L --> load last maze's solution saved


MOUSE: 
right-click ---> set black block. 
If dragged while pressed fills path with black blocks

right-double-click ----> set white block.
If dragged while pressed fills path with white blocks

left-click ---------> set RED/ENTRANCE block
left-double-click --> set GREEN/EXIT block


MAZE's WINDOW IS RESIZABLE at RUNTIME


*/



#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include <queue>
#include <map>
#include <algorithm>

#include <windows.h>

using namespace std;

const int M = 80; // Number of rows
const int N = 80; // Number of columns
const int blockSize = 7; // Block size

enum BlockType { EMPTY, WALL, START, END, RED, GREEN, PATH, VISITED };

struct Point 
{
    int r, c;
    bool operator<(const Point& other) const 
    {
        if (r != other.r) return r < other.r;
        return c < other.c;
    }
    bool operator==(const Point& other) const 
    {
        return r == other.r && c == other.c;
    }
};

sf::Clock algorithmTimer;


struct Node 
{
    Point pt;
    int g, f;
    bool operator>(const Node& other) const { return f > other.f; }
};

enum AlgorithmType { NONE, BFS_ALG, ASTAR_ALG };
AlgorithmType currentAlg = NONE;

// Heuristic: Manhattan Distance for A*
int heuristic(Point p1, Point p2) 
{
    return abs(p1.r - p2.r) + abs(p1.c - p2.c);
}

// Priority Queue for A*
priority_queue<Node, vector<Node>, greater<Node>> pq;
map<Point, int> gScore; 


// Global States
queue<Point> q;
map<Point, Point> parent;
vector<vector<bool>> visitedArr;
bool isSolving = false;
Point startNode = { -1, -1 }, endNode = { -1, -1 };


vector<vector<BlockType>> GenerateMaze() {
    vector<vector<BlockType>> maze(M, vector<BlockType>(N, EMPTY));
    srand(static_cast<unsigned>(time(nullptr)));

    for (int i = 1; i < M - 1; i++) {
        for (int j = 1; j < N - 1; j++) {
            if (rand() % 3 == 0) maze[i][j] = WALL;
        }
    }
    for (int i = 0; i < M; i++) { maze[i][0] = WALL; maze[i][N - 1] = WALL; }
    for (int j = 0; j < N; j++) { maze[0][j] = WALL; maze[M - 1][j] = WALL; }

    int entryX = 1 + rand() % (M - 2);
    int exitX = 1 + rand() % (M - 2);
    maze[entryX][1] = RED; // Start point
    maze[exitX][N - 2] = GREEN; // End point

    return maze;
}

void SaveMazeToFile(const vector<vector<BlockType>>& maze, string filename, bool isSolution = false) {
    ofstream outFile(filename);
    if (outFile.is_open()) {
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                char symbol;
                switch (maze[i][j]) {
                case WALL:   symbol = '0'; break;
                case RED:    symbol = 'R'; break;
                case GREEN:  symbol = 'G'; break;
                case PATH:   symbol = isSolution ? 'S' : '1'; break;
                default:     symbol = '1'; break;
                }
                outFile << symbol << (j < N - 1 ? ";" : "");
            }
            outFile << "\n";
        }
        outFile.close();
    }
}

void LoadMazeFromFile(vector<vector<BlockType>>& maze, string filename) {
    ifstream inFile(filename);
    if (!inFile.is_open()) return;
    string line; int row = 0;
    while (getline(inFile, line) && row < M) {
        stringstream ss(line); string cell; int col = 0;

        while (getline(ss, cell, ';') && col < N) {
            char val = cell[0];
            if (val == '0')      maze[row][col] = WALL;  // black block
            else if (val == '1') maze[row][col] = EMPTY; // white block
            else if (val == 'R') maze[row][col] = RED;   // entrance block
            else if (val == 'G') maze[row][col] = GREEN; // exit block
            else if (val == 'S') maze[row][col] = PATH;  // blue block -> solution path 
            col++;
        }

        row++;
    }
}

// BFS 
void StartBFS(vector<vector<BlockType>>& maze) 
{
    while (!q.empty()) q.pop();
    parent.clear();
    visitedArr.assign(M, vector<bool>(N, false));
    startNode = { -1, -1 }; endNode = { -1, -1 };

    for (int i = 0; i < M; i++) 
    {
        for (int j = 0; j < N; j++) 
        {
            if (maze[i][j] == RED) startNode = { i, j };
            if (maze[i][j] == GREEN) endNode = { i, j };
            if (maze[i][j] == PATH || maze[i][j] == VISITED) maze[i][j] = EMPTY;
        }
    }

    if (startNode.r != -1 && endNode.r != -1) 
    {
        algorithmTimer.restart(); 
      
        q.push(startNode);
        visitedArr[startNode.r][startNode.c] = true;
        isSolving = true;
    }
}

// A*
void StartAStar(vector<vector<BlockType>>& maze) {
    while (!pq.empty()) pq.pop();
    parent.clear();
    gScore.clear();
    visitedArr.assign(M, vector<bool>(N, false));
    startNode = { -1, -1 }; endNode = { -1, -1 };

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            if (maze[i][j] == RED) startNode = { i, j };
            if (maze[i][j] == GREEN) endNode = { i, j };
            if (maze[i][j] == PATH || maze[i][j] == VISITED) maze[i][j] = EMPTY;
        }
    }

    if (startNode.r != -1 && endNode.r != -1) 
    {
        algorithmTimer.restart(); 
        gScore[startNode] = 0;
        int h = heuristic(startNode, endNode);
        pq.push({ startNode, 0, h });
        isSolving = true;
        currentAlg = ASTAR_ALG;
    }
}

void UpdateBFS(vector<vector<BlockType>>& maze) 
{
    if (!isSolving || q.empty()) return;

    for (int speed = 0; speed < 10; speed++) 
    {
        if (q.empty()) 
        { 
            isSolving = false; return; 
        }

        Point curr = q.front(); q.pop();

        if (curr == endNode) 
        {
            isSolving = false;
            int steps = 0; // Solution's steps count
            Point step = endNode;
            if (parent.find(step) != parent.end()) 
            {
                step = parent[endNode];
                while (!(step == startNode)) 
                {
                    maze[step.r][step.c] = PATH;
                    step = parent[step];
                    steps++; 
                }
            }
            
            float duration = algorithmTimer.getElapsedTime().asSeconds();
            cout << "Shortest Path Found: " << steps << " steps (Algorithm: BFS) in " << duration << " seconds." << endl;

            SaveMazeToFile(maze, "myMaze_ExportSolution.txt", true);
            cout << "Solution saved in file: myMaze_ExportSolution.txt " << endl;

            return;
        }

        int dr[] = { -1, 1, 0, 0 }, dc[] = { 0, 0, -1, 1 };
        for (int i = 0; i < 4; i++) 
        {
            int nr = curr.r + dr[i], nc = curr.c + dc[i];
            if (nr >= 0 && nr < M && nc >= 0 && nc < N && !visitedArr[nr][nc] && maze[nr][nc] != WALL) 
            {
                visitedArr[nr][nc] = true;
                parent[{nr, nc}] = curr;
                q.push({ nr, nc });
                if (maze[nr][nc] == EMPTY) maze[nr][nc] = VISITED; // Wavefront (Light Blue)
            }
        }
    }
}

void UpdateAStar(vector<vector<BlockType>>& maze) 
{
    if (!isSolving || pq.empty()) return;

    for (int speed = 0; speed < 10; speed++) {
        if (pq.empty()) { isSolving = false; return; }
        Node current = pq.top(); pq.pop();
        Point curr = current.pt;

        if (visitedArr[curr.r][curr.c]) continue;
        visitedArr[curr.r][curr.c] = true;
        if (maze[curr.r][curr.c] == EMPTY) maze[curr.r][curr.c] = VISITED;

        if (curr == endNode) {
            isSolving = false;
            int steps = 0;
            Point step = endNode;
            if (parent.find(step) != parent.end()) {
                step = parent[endNode];
                while (!(step == startNode)) {
                    maze[step.r][step.c] = PATH;
                    step = parent[step];
                    steps++;
                }
            }
            
            float duration = algorithmTimer.getElapsedTime().asSeconds();
            cout << "Shortest Path Found: " << steps << " steps (Algorithm: A*) in " << duration << " seconds." << endl;
            
            SaveMazeToFile(maze, "myMaze_ExportSolution.txt", true);
            cout << "Solution saved in file: myMaze_ExportSolution.txt " << endl;

            return;
        }

        int dr[] = { -1, 1, 0, 0 }, dc[] = { 0, 0, -1, 1 };
        for (int i = 0; i < 4; i++) {
            int nr = curr.r + dr[i], nc = curr.c + dc[i];
            if (nr >= 0 && nr < M && nc >= 0 && nc < N && maze[nr][nc] != WALL) {
                int tentative_g = gScore[curr] + 1;
                Point next = { nr, nc };
                if (gScore.find(next) == gScore.end() || tentative_g < gScore[next]) {
                    gScore[next] = tentative_g;
                    int f = tentative_g + heuristic(next, endNode);
                    parent[next] = curr;
                    pq.push({ next, tentative_g, f });
                }
            }
        }
    }
}

void SetColor()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
   
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
       
}

void printHELP()
{
    system("cls");

    cout << endl << "Left-Click --> Set BLACK WALL-BLOCK " << endl;
    cout << "Left-Click + DRAG --> for continuous BLACKing " << endl;
    cout << endl << "Left-Double-Click --> Set WHITE PATH-BLOCK " << endl;
    cout << "Left-Double-Click + DRAG --> for continuous WHITEing " << endl;

    cout << endl << "Right-Click --> Set RED Entrance BLOCK " << endl;
    cout << "Right-Double-Click --> Set GREEN Exit BLOCK " << endl;

    cout << endl << "G --> Generate new MAZE " << endl;
    cout << "C --> Clean/Empty MAZE " << endl;
    cout << "Alt-C --> Clean SOLUTION PATH of MAZE " << endl;

    cout << endl << "S --> Save Current MAZE " << endl;
    cout << "L --> Load last MAZE saved " << endl;
    cout << "Alt-L --> Load last MAZE SOLUTION " << endl;

    cout << endl << "Alt-S --> Solve MAZE with BFS " << endl;
    cout << "Ctrl-S --> Solve MAZE with A* " << endl << endl;

    cout << endl << "  H --> Re-Print this HELP " << endl;
    cout << "Esc --> EXIT " << endl << endl;

}

int main() 
{
    SetColor();
    
    sf::RenderWindow window(sf::VideoMode(N * blockSize, M * blockSize), "MAZE Solver - BFS & A* - jopil 2026 FEB");
    window.setFramerateLimit(60);

    vector<vector<BlockType>> maze = GenerateMaze(); // Γεννήτρια τυχαίου maze

    bool isDrawing = false, isErasing = false;
    sf::Clock leftClickTimer, rightClickTimer;

    printHELP();

    while (window.isOpen()) 
    {
        sf::Event event;

        while (window.pollEvent(event)) 
        {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::MouseButtonPressed) 
            {
                
                // int bx = event.mouseButton.x / blockSize, by = event.mouseButton.y / blockSize;
                
                sf::Vector2f worldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                int bx = static_cast<int>(worldPos.x) / blockSize;
                int by = static_cast<int>(worldPos.y) / blockSize;
                
                if (bx >= 0 && bx < N && by >= 0 && by < M) 
                {
                    if (event.mouseButton.button == sf::Mouse::Left) 
                    {
                        if (leftClickTimer.getElapsedTime().asMilliseconds() < 300) 
                        { 
                            isErasing = true; isDrawing = false; 
                        }
                        else 
                        { isDrawing = true; isErasing = false; 
                        }
                        leftClickTimer.restart();
                    }
                    if (event.mouseButton.button == sf::Mouse::Right) 
                    {
                        if (rightClickTimer.getElapsedTime().asMilliseconds() < 300) maze[by][bx] = GREEN;
                        else maze[by][bx] = RED;
                        rightClickTimer.restart();
                    }
                }
            }
            
            if (event.type == sf::Event::MouseButtonReleased) 
            { 
                isDrawing = false; isErasing = false; 
            }

            if (event.type == sf::Event::KeyPressed) 
            {
                // if (event.key.code == sf::Keyboard::S && event.key.alt) StartBFS(maze);
                // if (event.key.code == sf::Keyboard::S) SaveMazeToFile(maze, "myMaze_Export.txt");

                if (event.key.code == sf::Keyboard::Escape)
                {
                  window.close();
                }

                if (event.key.code == sf::Keyboard::S) 
                {
                    if (event.key.alt) 
                    {
                        currentAlg = BFS_ALG;
                        StartBFS(maze);
                    }
                    else if (event.key.control) {
                        currentAlg = ASTAR_ALG;
                        StartAStar(maze);
                    }
                    else 
                    {
                        SaveMazeToFile(maze, "myMaze_Export.txt");
                        cout << "MAZE saved in file: myMaze_Export.txt " << endl;
                    }
                }
                
                
                if (event.key.code == sf::Keyboard::L)
                {
                    LoadMazeFromFile(maze, "myMaze_Export.txt");
                    cout << "Last build MAZE loaded from file: myMaze_Export.txt " << endl;
                }

                // ************************ HELP PRINT
                if (event.key.code == sf::Keyboard::H)
                {
                   printHELP();
                }

                // 'G' New maze
                if (event.key.code == sf::Keyboard::G) 
                {
                    isSolving = false; // Σταματάει τυχόν τρέχουσα επίλυση
                    maze = GenerateMaze();

                }

				// Clean solution path
                if (event.key.code == sf::Keyboard::C && event.key.alt) 
                {
                    isSolving = false; 
                    while (!q.empty()) q.pop(); 
                    for (int i = 0; i < M; i++) 
                    {
                        for (int j = 0; j < N; j++) 
                        {
                            if (maze[i][j] == PATH || maze[i][j] == VISITED) maze[i][j] = EMPTY;
                        }
                    }
                }
                else if (event.key.code == sf::Keyboard::C) 
                {
                    isSolving = false; 
                    while (!q.empty()) q.pop(); 
                    for (int i = 0; i < M; i++) 
                    {
                        for (int j = 0; j < N; j++) 
                        {
                            maze[i][j] = EMPTY;   
                            for (int i = 0; i < M; i++) { maze[i][0] = WALL; maze[i][N - 1] = WALL; }
                            for (int j = 0; j < N; j++) { maze[0][j] = WALL; maze[M - 1][j] = WALL; }
                        }
                    }
                }

                // LOAD Last Solution from file
                if (event.key.code == sf::Keyboard::L && event.key.alt) 
                {
                    isSolving = false; /
                    LoadMazeFromFile(maze, "myMaze_ExportSolution.txt");
                    cout << "Last MAZE SOLUTION loaded from file: myMaze_ExportSolution.txt " << endl;

                }

                if (event.type == sf::Event::Resized) 
                {
                    
                    sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                    window.setView(sf::View(visibleArea));
                }

            }
        }

       
        if (isDrawing || isErasing) 
        {
            sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
			
            // pixels to coordinates conversion
            sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);

            int bx = static_cast<int>(worldPos.x) / blockSize;
            int by = static_cast<int>(worldPos.y) / blockSize;

            if (bx >= 0 && bx < N && by >= 0 && by < M) 
            {
                maze[by][bx] = isDrawing ? WALL : EMPTY;
            }
        }


        //Update maze;
        if (currentAlg == BFS_ALG) 
        { 
            UpdateBFS(maze); 
        }
        else if (currentAlg == ASTAR_ALG) 
        { 
            UpdateAStar(maze); 
        }
        
        window.clear(sf::Color(100, 100, 100));

        sf::RectangleShape block(sf::Vector2f(blockSize - 1.0f, blockSize - 1.0f));
        for (int i = 0; i < M; i++) 
        {
            for (int j = 0; j < N; j++) 
            {
                if (maze[i][j] == WALL) block.setFillColor(sf::Color::Black);
                else if (maze[i][j] == RED) block.setFillColor(sf::Color::Red);
                else if (maze[i][j] == GREEN) block.setFillColor(sf::Color::Green);
                else if (maze[i][j] == PATH) block.setFillColor(sf::Color::Blue);
                else if (maze[i][j] == VISITED) block.setFillColor(sf::Color(173, 216, 230));
                else block.setFillColor(sf::Color::White);
                block.setPosition(j * blockSize, i * blockSize);
                window.draw(block);
            }
        }
        window.display();
    }
    return 0;
}
# MAZEs Creation & Solutions with BFS and A* algorithms
This is a C++ VS-2022 build console app, using SFML 2.6.0 x64, to randomly create and solve mazes using either BFS or A* algorithms.

Animation has been added to the solution part for understanding/looking at how the algorithms behave/work.

MAZE's window IS RESIZABLE at runtime

The app is based on the SFML library (Safe & Fast Multimedia Library) for creating and managing the maze's terrain window and basics. The maze can be selectably solved either with the BFS algoritm or with the A* algorithm (Alt-S, or Ctrl-S).
The app's keys' & mouse functionality are displayed in app's console when application starts. Can be re-displayed at any time by jsut pressing 'H'.

A new random maze is displayed in MAZE-Windo the moment application starts or can be randomly generated at any time by simply pressing 'G'.
In any maze, a RED - Entrance and GREEN - Exit blocks are randomly placed at the rim of the square maze. Any block of the maze, including RED & GREEN can be changed at will, 
R -> B or W, 
G -> B or W, 
B -> W -> B, etc.

With a Left-Mouse-Click any block can be converted into a black block (wall), or, with a Left-Mouse-DoubleClick to a white block (free-path).
If I keep the mouse button pressed after a Left-Mouse-Click or after a Left-Mouse-DoubleClick and then drag the mouse at any direction on the maze's terrain, black or white blocks are continuously dropped.

Any block on the maze's terrain can be set as a RED block (Entrance) with a Right-Mouse-Click or a GREEN block (EXIT) with a Right-Mouse-DoubleClick.

Every solution is automatically saved inside the runtime dir, as a ';' delimited csv file named "myMaze_ExportSolution.csv", where from can be loaded again pressing Ctrl-L for inspection and/or maze's modifications. [0 = block, 1 = free-path, S = solution path, R = Entrance, G = Exit].
Any randomly genarted maze can be also saved by pressing 'S'. It is saved in the runtime dir as "myMaze_Export.csv" file and can be also loaded again by pressing 'L'.

At any time, by pressing 'C' maze is cleaned completely but the rim wall.

At any time by pressing Alt-C only the solution's path is cleaned from maze's window.

By pressing 'Esc' or by clicking on the window's "X" we terminate application.


Importand Notes:

A)
In Project's properties we need to include in the C++ 'General' tab, in "Additional Include" dir, the "c:\SFML_version\include" , and in Linker's Additional Libraries Directory tab the "c:\SFML_version\lib". Also in Linker's Input/Additional Dependencies Libs we need to add the names sfml-graphics.lib;sfml-window.lib;sfml-system.lib, for the release x64 version compilation. Else, others relevant to your compilation, offered/found in the lib dir.

B) 
In the runtime dir, for Release/x64 compilation, the three dlls sfml-graphics-2.dll, sfml-system-2.dll, sfml-window-2.dll need to be pesent, taken from the bin directory of the SFML installation. Else, others relevant to your compilation, offered/found in the bin dir.

Minor pending additions:

a) Selectable names for saving mazes and solutions, 

b) Selectable maze's numbers of rows/cols and block size as well,

c) Not allowing adding a 2nd RED or GREEN block pair, if a pair already exist, before changing that pair into black or white.

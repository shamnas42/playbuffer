# README

## Shamnas Anil
## Enhancing a 2D Game 

--- 

### Instructions
The Leaderboard and Setting files start with no content when starting. The saved information will 
transfer over to the next game. 
To start the game, open the Source Folder, and on MainGame.cpp, press the Run button on top. You 
begin the game with a splash screen and will choose the options in the menu with the Up, Down, and
Enter Keys. You then play the game with the Up, Down, and Space keys. 

---

### Gameloop / Menus  

#### Feature Overview: 
I added a game loop to make the game more manageable. A splash screen starts at the beginning of 
the game for 4 seconds. The main menu then shows up to choose options such as playing the game, 
going to the leaderboard, turning off sound, and quitting the game. The leaderboard has its menu, 
which can return to the menu, reset the leaderboard, or exit the game. The game scene happens when 
you press play in the main menu. Once you lose, an end screen appears, which also has a menu to 
retry again, return to the menu, go to the leaderboard, or quit the game. The game loop makes the 
user feel more in control and allows the user to manage some settings for the game.  

#### Implementation Details: 

##### Implementation: 

A game enum can change through the splash screen, menu, leaderboard, and end screen. A switch 
statement can check the current game loop state and execute code. The game state held the game loop
state. 

<ins>Splash Screen Display</ins>

1. A splash screen usually has a certain number of seconds it appears on screen before it goes away. 
2. A timer in the game state made this work. 
3. A function displayed a picture of the Play Buffer title for 4 seconds. 

<ins>Menu, End Screen, and Leaderboard Display</ins>

1. The menu has a display function that will display all the options. A structure held the values of 
the menu options. The structure had the font size, content, and location. 
2. An array of this structure held all possible options.  
3. I iterated over the array to print each option. 
4. The end screen was a copy of the menu but with different content. 
5. The leaderboard was also a copy but with different content.

<ins>Menu, End Screen, and Leaderboard Choice</ins>

1. A number in the game state decides what option the player has chosen for each menu. 
2. Functions for each menu worked on the choice of options. 
3. A conditional decides whether to increase or decrease the choice number depending on which button 
the player has pressed. 
4. A sprite points to the choice depending on the choice number by changing its Y location. 
5. The game state changes to the corresponding option based on the enter key. 

<ins>Game Scene</ins>

1. The game scene remained the same but was given a function and implemented in the switch statement. 
2. When the player dies, the state changes to the end screen. 

<ins>Quitting</ins>

1. The functions for choice were booleans to return true for the update function to exit the game. 

##### Programming Concepts Used: 

**Variable** - Variables hold a type of data and its values. Variables were the game loop state, game 
enumerations, and printing of the menu options. Variables were throughout the entire program, but 
these were the main ones for this enhancement. 

**Function** - Functions are code blocks that do one thing to make the code more manageable. Functions 
repeat different aspects of the code, but for this program, it was for readability. Changing the 
game loop states invoked functions. 

**Enumeration** - Enumeration is when you give an integer value to a list of possibilities. Enumeration 
has allowed us to change game loop states. 

**Switch Statement** - Switch statements allow us to change the through enumerations. The switch 
statement checked the game loop state change and called different functions. 

**Structure** - Structures are user-defined data types. Structures made a way to make a menu option.

**Array** - An array is a container with many of the same data types. Arrays can hold a list of menu 
options.  

**For Loop** - For loops are to iterate over code blocks. Loops printed each menu structure in the 
array.  

**Conditional** - A conditional is a branching of code that depends on the boolean statement. 
Conditionals checked the pressed buttons. 

#### Difficulties: 

<ins>Challenges</ins>

I had many problems figuring out how to change between game loop states. At first, I was changing
the game scene based on the agent's state.  

I also had a problem figuring out how to choose between options.

I had a problem with a lot of repeated code with the menu. 

<ins>Solutions</ins>

I used a game enum and switch statements to change the game loop. 

At first, I was using separate prints in each conditional, but I realized that I could reduce code
by changing the location of the pointer by the choice number. 

I used a structure, array, and for loop to reduce the repeating code for the menu option. 

___ 
---

### Scoring and Leaderboard 

#### Feature Overview: 
I added a scoring system where every five successful attacks on tools without hitting a tool will 
increase all scores by a .5 multiplier. When attacking a coin, the streak will end. The Leaderboard
adds your score to the end of the game. The Leaderboard will transfer its data across all games. 
The Leaderboard also has a reset button that deletes all scores instead of deleting the text file. 

#### Implementation Details: 

##### Implementation: 

<ins>Scoring System</ins>

1. A game state variable kept the value of the score multiplier. Another one kept the streak value. 
2. When the tool and laser collide, the attack streak will increase by one. The action happens on 
1. the laser update function. 
3. When the attack streak reaches a multiple of 5, the score multiplier increases by .5. 
4. When the laser attacks the coin, the attack streak equals 0, and the score multiplier equals 1. 
5. The score multiplier is multiplied by the score when attacking a tool and colliding with loot. 
6. The score multiplier is displayed on top right.  

<ins>Leaderboard Score Set Up</ins>

1. One function takes care of the leaderboard data when opening the program. 
2. The first function that sets up the leaderboard uses a text file to open with previous scores.
3. A game state variable that holds the score vector then adds all the scores from the leaderboard.'
4. If there are less than eight scores, it will fill up with scores of 0 as needed.  
5. The vector is sorted from highest to lowest.
6. This function starts in the game entry. 

<ins>Leaderboard Score Upload</ins> 

1. When the player dies, a function will upload the score. 
2. The upload function will open a file and write data into the leaderboard. 
3. The function will also update the game state's vector and sort from highest to lowest. 


<ins>Leaderboard Score Score Display</ins>

1. A for loop prints eight scores from the vector in the game state. 
2. We print the first eight indexes because the vector sorts itself from highest to lowest. 
3. The Y location of the score changes on the index of the score. 

<ins>Leaderboard Reset</ins>

1. A reset button will delete all data from the text file and set eight 0s into the vector. 

##### Programming Concepts Used

**FStream** - An FStream allows one to write and read data on a text file. We read and wrote scores
on a text file to save data.  

**Vector** - A vector is a dynamic container that can change size depending on the content. A 
vector kept track of all the scores from the previous and current games.

**Algorithms** - Algorithms are steps in a process that manipulate data. Algorithms sorted the 
vectors from highest to lowest. 

**Structure** - A structure is a user-defined data type. The game state structures kept track of 
the vector throughout the game. 

**For Loop** - A for loop allows for iteration and repeated code blocks. A for loop displayed the 
scores from the vectors. 

**Variables and Functions** - Variables and functions are the backbone of most processes in 
programming. Variables saved the vector, opened the file, changed the score multiplier, and set 
the attack streak. Functions make code more readable and manageable, calling them when needed. 

#### Difficulties: 

<ins>Challenges</ins>

I had a problem where I was using an array but didn't know how to change the size of it. 
I had an issue with making a sorting function to set up the leaderboard. 
I had an issue with the scoreboard less than eight scores. It kept crashing. 


<ins>Solutions</ins>

I realized arrays can't change size, so I switched to a vector. 
I looked online for an easy way to sort the vector and found the algorithm header with the sort option. 
I made a condition so if there are fewer than eight scores- the vector will add enough scores of 0. 

___ 
---

### Coin Frenzy

#### Feature Overview: 

A coin frenzy is when a silver coin drops with a low chance - and when caught, increases the chance 
of gold coins dropping for 5 seconds. 

#### Implementation Details: 

##### Implementation

<ins>Game State</ins> 

1. A variable in the game state holds a boolean value if the coin frenzy is on. 
2. A timer in the game state counts the number of seconds the coin frenzy is on. 

<ins>Drops and Updates</ins>

1. I made a new game object type for a silver coin. 
2. In the fan function, there is a 1/5 chance of the coin drops turning into a silver coin. 
3. A function updates the animation and position of the silver coin. It's the same as the golden 
coin but with a different type and an animation instead of 1 frame. 
4. The laser function can destroy this coin. 
5. The game scene function calls the update for this mechanic. 

<ins>Mechanic</ins>

1. When the coin collides with the agent, the coin frenzy boolean turns on, and the random chance for coins 
to drop increases by three.
2. The game timer is passed through as a parameter to increase the coin frenzy timer. When the coin 
frenzy timer hits five seconds, the coin frenzy turns off. 
3. After death, the coin frenzy boolean turns off.

##### Programming Concepts Used

**Structure** -  The timer and boolean variables inside the game state structure kept track of this 
mechanic. 

**Parameters** - The elapsed timer in the game scene function updates the coin frenzy timer when the 
coin frenzy boolean variable is on. 

**Conditional** - Conditionals increase the chances of the coin drops. It checked whether the mechanic 
was on to increase the drop chance. 

**Variables and Functions** - Variables and functions kept track of most of the data in the program. 
Variables helped with the timer and the boolean variable. Functions helped update the animation, 
position, and state of the coin. 

#### Difficulties: 

<ins>Challenges</ins>
I had a problem with updating the animation of the coin. I had a problem figuring out how to animate 
the coin because it's a four-by-three but missing one frame. 
The other problem was figuring out how to update the location because the tutorial used a different 
function instead of animating it. Play Buffer used a rotation function. 

<ins>Solutions</ins>
I remembered that the tutorial used a module to skip frames, so I used that to skip every 12th frame, 
which was missing.  
I figured out I could update the coin's position by using the position member variable in the game 
object. 

---
___ 

### Shield

#### Feature Overview: 

A shield allows you to save yourself from dying one time as long as you have it on you at any moment. 

#### Implementation Details: 

##### Implementation

<ins>Game State</ins>

1. A variable in the game state holds a boolean value if the shield is on. 

<ins>Drops and Updates</ins>

1. I made a new game object type for a shield. 
2. There is a 1/2000 chance of dropping a shield every frame. 
3. A function updates the look and position of the shield. It's the same as the silver coin but with 
a different type. 
4. The laser function can destroy this coin. 
5. The game scene function calls the update for this mechanic. 

<ins>Mechanic</ins>

1. When the shield collides with the agent, the shield boolean turns on.
2. When the agent collides with a tool, it calls the reset function. The reset function only ends 
the game when the shield boolean is not on. 
3. If they have another shield, the game will continue when the player gets hit. 
4. Even keeping a shield, the streaks, multiplier, and coin frenzy reset when hit. 

##### Programming Concepts Used

**Structure** -  The boolean variable for the game keeps track of the shield. 
**Conditional** - Conditionals check whether or not to end the game or continue depending on the 
boolean variable for shield. 
**Variables and Functions** - Variables and functions kept track of most of the data in the 
program. Variables helped with the boolean variable. Functions help with the reset - and updating 
the look, position, and state of the shield. 

#### Difficulties: 

##### Challenges and Solution 

The silver coin was a more advanced feature than this, so I had no obstacle implementing this feature. 

---

### Bonus Sound 

I added a sound feature that doesn't start with the sound. If you start the sound, it saves that 
data and keeps it for future games. It's similar to how the leaderboard works. I added code 
throughout the program to check if the sound should play and only play when it's on. The music 
shouldn't replay itself, so I made another boolean to check if the sound was already playing. 
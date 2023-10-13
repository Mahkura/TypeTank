# Introduction: Type Tanks (working title)

Type tanks a is top down free-for-all multiplayer shooting game where the players control one or multiple tanks and compete to see who is last. The player characters are controlled via a specialized script in text file that the player may edit using whatever text editor they like.

Each tank is given a unique name such that the players may refer to them in the scripts, these names are visible on the screen.

## Definitions

- unit: player characters

## The script

The script defines the commands to control the game and the player character. The commands in the script are followed after the text file is saved. Commands related to units are followed from the beginning after each save, if the script file is modified while previous commands are still being followed, the previous commands are discarded and new commands are then followed.

### Basic structure

The script is defined as a series of blocks that start with a keyword, are followed by keywords, numbers or identifiers, which are the followed by curly braces. The commands related to the block are defined within curly braces.

Example script:

``` text
unit Tank1 {
    turn 45r
    move 10m
    turn west
    shoot
}
```

### Available commands

#### unit

Commands related to units are defined using the **unit** keyword, followed by the identifier of the unit the block targets. Only player characters are targetable with **unit**.

Commands:

- move {number}{'m'}: move the unit forward in its current direction by the amount of meters given **TODO: define meters somewhere**
- turn {{number}{'r'|'l'}} | {'west'|'east'|'north'|'south'}: rotates the unit by the amount of degrees given as a number, followed by a direction character (r for right, l for left), or by cardinal directions where 'west' indicates left side of the game view and 'north' the top side
- shoot: shoots in the current direction of the unit

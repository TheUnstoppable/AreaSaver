# AreaSaver
[![Jenkins Build](https://img.shields.io/jenkins/build?jobUrl=https%3A%2F%2Fci.unstoppable.work%2Fjob%2FDragonade%2520Plugins%2Fjob%2FAreaSaver%2F)](https://ci.unstoppable.work/job/Dragonade%20Plugins/job/AreaSaver/)
[![Discord](https://img.shields.io/discord/647431164138749966?label=support)](https://discord.gg/KjeQ7xv)
[![Buy me a Coffee](https://img.shields.io/badge/buy%20me%20a%20coffee-yellow)](https://buymeacoffee.com/theunstoppable)

## Overview
Saves objects in a specific region in any map into a file, and loads them.  
Before explaining the usage, let's talk about which commands does AreaSaver offer.


## Commands

### !firstpos
*Alias(es): !pos1*  
This command selects the first position to determine a region to save objects.  

### !secondpos
*Alias(es): !pos2*  
This command selects the second position to determine a region to save objects.  

### !selectmapbounds
*Alias(es): !smb*  
A shortcut to select the entire map as the region.  

### !ssave <file name>
*Alias(es): None*  
Saves the selected region (either by `!pos1` and `!pos2` or `!smb`) into `<file name>`.  

### !ssaveoverwrite <file name>
*Alias(es): None*  
`!ssave` command will fail if the file name specified already exists. This command overwrites the existing file.  

### !sload <file name>
*Alias(es): None*  
Loads a saved region from the `<file name>`.  

### !highlightbounds
*Alias(es): !highlightarea, !hlo, !hla*  
Creates markers at the corners and sides of the selected region.  

### !clearhighlight
*Alias(es): !chl*  
Removes the markers.  


## Usage
First, enable fly mode. Select the first position by using the `!pos1` command. If the object is grounded, it is preferred to fly a bit below the ground level to use the command. Then, fly up and select the second position using `!pos2` command such that the objects are covered within the selected region. To make sure objects are properly covered inside the region, `!hla` command can be used.  
Then, use the `!ssave` command to save the objects within the region. While saving objects, the position relative to the saver will be used as the position. So, the positioning of the saver is important before saving the objects. The file containing all the saved objects is called a "schematic" file, uses the `.sch` extension and saved under the "AreaSaverSchematics" folder in the FDS folder.  
To load the schematic, simply use `!sload` command. They will be re-created at the positions relative to the loader.
Note that AreaSaver can only save physical objects. (So, objects like script zones and building controllers cannot be saved.)  
Here is a list of properties that AreaSaver saves:  
- Object Definition ID
- Object Transform (contains Relative Position and Rotation)
- Object Model
- Collision Group
- Velocity[^1]
- Gravity Multiplier[^1]
- Health
- Armor
- Maximum Health
- Maximum Armor
- Health Skin
- Armor Skin
- Object Type (Team)
- Visibility
- Renderability[^2]
- Is Stealthed[^3]
- Movability[^1]
- Is Engine Enabled[^4]
- Innate Behavior[^5]
- Frozen[^5]
- Fly Mode[^5]
- Scale[^5]
- Damage Animations[^5]
- Skeleton Width & Height[^5]
- Bot Tag[^5]
- Scripts
- Weapons & Ammunition[^6]

[^1]: Only applicable for movable physics objects.
[^2]: Only applicable for objects with valid models.
[^3]: Only applicable for smart objects. (Soldiers & Vehicles)
[^4]: Only applicable for vehicles.
[^5]: Only applicable for soldiers.
[^6]: Only applicable for armed objects.


## Support
You will get the fastest response by joining in Discord server at https://discord.gg/KjeQ7xv, but be sure to read `#important` channel first.  
Feel free to join to suggest changes and request help.  

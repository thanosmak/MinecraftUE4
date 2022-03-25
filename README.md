# MinecraftUE4

Developed with Unreal Engine 4

## Possible crush problem
Most times after compiling, one parameter inside a component gets unset. Apparently it's a known bug of UE for some years now.

In order to avoid a crush you have to go to *FirstPersonCharacter_BP* in Unreal Engine Editor, find the *PlayerHandler* component and on the property Actor to Spawn use *Placeholder_Block_BP*

![alt text](https://github.com/thanosmak/MinecraftUE4/blob/master/ReadmeAssets/PlayerHandler.png?raw=true)

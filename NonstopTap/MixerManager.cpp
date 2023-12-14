#include <vector>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <cstdlib>
#include "MixerManager.h"

//Initialize music properties
mixerManager::mixerManager()
{
    menuGame = NULL;
    wrongNote = NULL;
    numRightNoteSound = 0;
}

//remove music properties
mixerManager::~mixerManager()
{
    free();
}

//remove music properties from memory
void mixerManager::free()
{
    //free the memory of menuGame 
    Mix_FreeMusic(menuGame); 
    menuGame = NULL;

    //free the memory of wrongNote
    Mix_FreeChunk(wrongNote); 
    wrongNote = NULL;
    
    //free the memory of all the element of rightNotes[]
    for (int i = 0; i < TOTAL_RIGHT_NOTE_SOUND; i++)
    {
        Mix_FreeChunk(rightNotes[i]); 
        rightNotes[i] = NULL;
    }
}

// Load music to pointer Mix_Music* menuGame from link: path
void mixerManager::loadMenuGameSound(std::string path)
{
    menuGame = Mix_LoadMUS(path.c_str());
}

// add a right-note-sound to array rightNotes from link: path
void mixerManager::addRightNoteSound(std::string path)
{
    rightNotes[numRightNoteSound++] = Mix_LoadWAV(path.c_str());
}

// load wrong-note-sound to pointer Mix_Chunk* wrongNote from link: path
void mixerManager::loadWrongNoteound(std::string path)
{
    wrongNote = Mix_LoadWAV(path.c_str());
}

// play the menu-music
void mixerManager::playMenuSound()
{
    //check if the sound is not playing
    if(Mix_PlayingMusic() == 0)
    {
        Mix_PlayMusic(menuGame, -1); // play the menu-music from menuGame infinite times until stopped
    }
}

// stop playing the menu-music
void mixerManager::stopPlayMenuSound()
{
    //Check if the sound is playing
    if (Mix_PlayingMusic() == 1)
    {
        Mix_HaltMusic(); // stop playing (menu) music that is playing
    }
}

// turn down the play-menu-sound
void mixerManager::lowerPlayMenuSound()
{
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2); // reduce volume 2 times
}

//reset the volume of play-menu-sound
void mixerManager::resetPlayMenuSound()
{
    Mix_VolumeMusic(MIX_MAX_VOLUME); // set the volume to default
}

//play the right-note-sound
void mixerManager::playRightNoteSound()
{
    int index = rand() % numRightNoteSound; // Select random the RightNoteSound
    Mix_PlayChannel(-1, rightNotes[index], 0); // select an empty audio channel and play the music rightNotes[index] once 
}
//play the wrong-note-sound
void mixerManager::playWrongNoteSound() 
{
    Mix_PlayChannel(-1, wrongNote, 0); // select an empty audio channel and play wrong-note once
}

// stop playing all the sound
void mixerManager::stopAnySound()
{
    stopPlayMenuSound(); //stop playing menu-music
    Mix_HaltChannel(-1); //stop playing all the audio channels
}

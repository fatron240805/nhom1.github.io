#include <vector>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <cstdlib>
#include "MixerManager.h"

mixerManager::mixerManager()
{
    menuGame = NULL;
    wrongNote = NULL;
    numRightNoteSound = 0;
}

mixerManager::~mixerManager()
{
    free();
}

void mixerManager::free()
{
    Mix_FreeMusic(menuGame);
    Mix_FreeChunk(wrongNote);
    for (int i = 0; i < TOTAL_RIGHT_NOTE_SOUND; i++)
    {
        Mix_FreeChunk(rightNotes[i]);
        rightNotes[i] = NULL;
    }
}

void mixerManager::loadMenuGameSound(std::string path)
{
    menuGame = Mix_LoadMUS(path.c_str());
}

void mixerManager::addRightNoteSound(std::string path)
{
    rightNotes[numRightNoteSound++] = Mix_LoadWAV(path.c_str());
}

void mixerManager::loadWrongNoteound(std::string path)
{
    wrongNote = Mix_LoadWAV(path.c_str());
}

void mixerManager::playMenuSound()
{
    if(Mix_PlayingMusic() == 0)
    {
        Mix_PlayMusic(menuGame, -1);
    }
}

void mixerManager::stopPlayMenuSound()
{
    if (Mix_PlayingMusic() == 1)
    {
        Mix_HaltMusic();
    }
}

void mixerManager::playRightNoteSound()
{
    int index = rand() % numRightNoteSound;
    Mix_PlayChannel(-1, rightNotes[index], 0);
}

void mixerManager::playWrongNoteSound()
{
    Mix_PlayChannel(-1, wrongNote, 0);
}

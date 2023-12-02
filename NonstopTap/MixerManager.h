#pragma once
#include <vector>
#include <string>
#include <assert.h>
#include <iostream>


#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <cstdlib>

const int TOTAL_RIGHT_NOTE_SOUND = 5;

class mixerManager
{
    public:
        mixerManager();

        ~mixerManager();

        void free();

        void loadMenuGameSound(std::string path);

        void addRightNoteSound(std::string path);

        void loadWrongNoteound(std::string path);

        void playMenuSound();

        void stopPlayMenuSound();

        void playRightNoteSound();

        void playWrongNoteSound();

        void stopAnySound();

    private:
        int numRightNoteSound;
        Mix_Music *menuGame;
        Mix_Chunk *rightNotes[TOTAL_RIGHT_NOTE_SOUND];
        Mix_Chunk *wrongNote;
};
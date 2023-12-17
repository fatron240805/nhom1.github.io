#pragma once
#include <vector>
#include <string>
#include <assert.h>
#include <iostream>


#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <cstdlib>

const int TOTAL_RIGHT_NOTE_SOUND = 5;

// manage the music
class mixerManager 
{
    public:
        // Initialize music properties
        mixerManager(); 

        // remove music properties
        ~mixerManager(); 

        // remove music properties from memory
        void free(); 

        // Load music to pointer Mix_Music* menuGame from link: path
        void loadMenuGameSound(std::string path); 

        // add a right-note-sound to array rightNotes from link: path
        void addRightNoteSound(std::string path); 

        // load wrong-note-sound to pointer Mix_Chunk* wrongNote from link: path
        void loadWrongNoteound(std::string path); 

        // play the menu-music
        void playMenuSound(); 

        // stop playing the menu-music
        void stopPlayMenuSound(); 

        // turn down the play-menu-sound
        void lowerPlayMenuSound(); 

        // reset the volume of play-menu-sound
        void resetPlayMenuSound(); 

        // play the right-note-sound
        void playRightNoteSound(); 

        // play the wrong-note-sound
        void playWrongNoteSound(); 
        
        // stop playing all the sound
        void stopAnySound(); 

    private:
        int numRightNoteSound; // numbers of right-note-sound
        Mix_Music *menuGame; // pointer to music of the menu
        Mix_Chunk *rightNotes[TOTAL_RIGHT_NOTE_SOUND]; // array of pointer to right-note-sound
        Mix_Chunk *wrongNote; // pointer to wrong-note
};
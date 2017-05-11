
#include <string>
#include <atlbase.h>
#include <iomanip>
#include <sstream>

#include "Dump.h"
#include "FileManager.h"
#include "Time.h"
#include "Logger.h"
#include "resource.h"

Dump::Dump(GFXs *g, InputHandler *h) {
    graphics = g;
    input = h;

    bufferpos = 0;
    displaypos = 0;
    filesize = 0;
    file_opened = false;

    curser.x = 2;
    curser.y = 3;

    rows.clear();
    int r = graphics->getHeight();
    colwidth = graphics->getFontWidth() / 16;
    rowheight = graphics->getFontHeight() / 16;
    numcharswidth = (graphics->getWidth() - offset) / colwidth;
    numcharsheight = graphics->getHeight() / rowheight;
    for (int x = 0; x != r / rowheight; ++x) {
        rows.push_back(std::string(""));
    }

    rows[1] = "Open Wolf3d or SOD save file";
    rows[3] = "  Example: SAVEGAM0.WL6 / SAVEGAM0.SOD";

    texture = graphics->getTexture(graphics->getWidth(), graphics->getHeight());
    //texture = graphics->getTexture(graphics->SCREEN_WIDTH, graphics->SCREEN_HEIGHT);

    // Create screen with backup to clear with
    background = new BMP(graphics->getWidth(), graphics->getHeight());
    screen = new BMP(graphics->getWidth(), graphics->getHeight());

    for (size_t x = 0; x != background->width * background->height; ++x) {
        background->pixels[x] = graphics->BACKGROUND;
        background->pixels[x] = 0xff000000;
    }

    clearScreen();

    graphics->setFont(graphics->BITFONT);

    SDL_UpdateTexture(texture, NULL, screen->pixels, screen->width * sizeof(Uint32));  // Copy pixels on to window

    printRows();
}

Dump::~Dump() {
    if (fm != NULL) {
        delete fm;
    }
    SDL_DestroyTexture(texture);
}

void Dump::copy(BMP *dst, BMP *src, int xpos, int ypos) {
    if (xpos < 0 || ypos < 0) {
        return;
    }
    for (int y = 0; (y + ypos) < dst->height && y < src->height; ++y) {
        for (int x = 0; (x + xpos) < dst->width && x < src->width; ++x) {
            dst->pixels[xpos + x + ((ypos + y) * dst->width)] = src->pixels[x + y * src->width];
        }
    }
}

void Dump::printRows() {
    SDL_UpdateTexture(texture, NULL, screen->pixels, screen->width * sizeof(Uint32));
    for (int x = 0; x != rows.size(); ++x) {
        BMP line;
        graphics->buildString(rows[x], line, graphics->MEDIUMFONT);
        //copy(screen, &line, offset, 00 + x * line.height);
        SDL_Rect r;
        r.h = line.height;
        r.w = line.width;
        r.x = offset;
        r.y = x * line.height;
        SDL_UpdateTexture(texture, &r, line.pixels, line.width * sizeof(Uint32));
    }
}

void Dump::clearScreen() {
    memcpy(screen->pixels, background->pixels, background->width * background->height * sizeof(Uint32));
}

typedef struct
{
    char        header[32];
    int16_t     difficulty;
    int16_t     mapon;
    int32_t     oldscore, score, nextextra;
    int16_t     lives;
    int16_t     health;
    int16_t     ammo;
    int16_t     keys;
    int16_t     bestweapon, weapon, chosenweapon;

    int16_t     faceframe;
    int16_t     attackframe, attackcount, weaponframe;

    int16_t     episode, secretcount, treasurecount, killcount,
        secrettotal, treasuretotal, killtotal;
    int32_t     TimeCount;
    int32_t     killx, killy;
    char        victoryflag;        // set during victory animations
} gametype;

void Dump::update() {
    if (input->load_pressed()) {
        if (fm != NULL) {
            delete fm;
            fm = NULL;
        }
        fm = new FileManager();
        if (fm->succeeded()) {          // File is opened!
            char *difficulty[] = { "Can I play, Daddy?",
                "Don't hurt me.",
                "Bring 'em on!",
                "I am Death incarnate!" };
            for (int x = 0; x != rows.size(); ++x) {    // Clear all rows
                rows[x] = "";
            }
            file_opened = true;
            filesize = fm->getFilesize();
            clearScreen();
            gametype gamestate;
            fm->file.read((char *)&gamestate, sizeof(gametype));

            std::stringstream stream;

            if (gamestate.difficulty > 3 || gamestate.difficulty < 0) {
                stream << "Error: unable to open file " << fm->getfilename();
                rows[1] = stream.str();
            }
            else {
                stream << "Opened filename " << fm->getfilename();
                rows[0] = stream.str();
                stream.str("");
                stream.clear();
                stream << "Savename:            " << gamestate.header;
                rows[1] = stream.str();

                stream.str("");
                stream.clear();
                stream << "Difficulty:          " << difficulty[gamestate.difficulty];
                rows[2] = stream.str();

                stream.str("");
                stream.clear();
                stream << "Score:               " << gamestate.score;
                rows[3] = stream.str();

                stream.str("");
                stream.clear();
                stream << "Lives:               " << gamestate.lives;
                rows[4] = stream.str();
                stream.str("");
                stream.clear();

                stream << "Health:              " << gamestate.health;
                rows[5] = stream.str();
                stream.str("");
                stream.clear();

                stream << "Ammo:                " << gamestate.ammo;
                rows[6] = stream.str();
                stream.str("");
                stream.clear();

                stream << "Episode/Map:         " << gamestate.episode + 1 << "/" << gamestate.mapon + 1;
                rows[7] = stream.str();
                stream.str("");
                stream.clear();

                stream << "Secrets Found:       " << gamestate.secretcount << "/" << gamestate.secrettotal;
                rows[9] = stream.str();
                stream.str("");
                stream.clear();

                stream << "Treasures found:     " << gamestate.treasurecount << "/" << gamestate.treasuretotal;
                rows[10] = stream.str();
                stream.str("");
                stream.clear();

                stream << "Enemies killed:      " << gamestate.killcount << "/" << gamestate.killtotal;
                rows[11] = stream.str();
            }

            // set parms and print to screen
            printRows();
        }
    }

    graphics->draw(texture);
}
#ifndef __COMED_MOD_H__
#define __COMED_MOD_H__

extern bool guiisshowing();

namespace game
{   
    /* ammobar */
    void getammocolor(fpsent *d, int gun, int &r, int &g, int &b, int &a);
    void drawacoloredquad(float x, float y, float w, float h, uchar r, uchar g, uchar b, uchar a);
    void drawselectedammobg(float x, float y, float w, float h);
    void drawammobar(fpsent *d, int w, int h);
    
    /* hudscores */
    void drawscores(int w, int h);
    
    struct scoregroup : teamscore
    {
        vector<fpsent *> players;
    };
    extern int groupplayers();
    extern vector<scoregroup *> getscoregroups();
    
    /* frag messages */
    void drawfragmsg(fpsent *d, int w, int h);
    extern int fragmsg;
    extern int fragmsgdeaths;
    extern int fragmsgfade;
    extern int fragmsgname;
    extern int fragmsgsize;
    extern int fragmsgposy;

    /* game clock */
    void drawgameclock(int w, int h);

    /* hud names */
    void drawhudname(fpsent *d, int w, int h);
    extern int firstovermillis, lastovermillis;
    extern fpsent *lastoverplayer;
}
#endif
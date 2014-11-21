/*
 *  Community Edition 
 */
#include "game.h"
#include "engine.h"
#include "comed.h"

namespace game
{
    
    /*
     * ammo bar hud extension
     * credit goes to: ETERNALSOLSTICE / aaa
     * http://github.org/extra-a/sauer-sdl2
     */ 
    
    XIDENT(IDF_SWLACC, VARP, ammobarfilterempty, 0, 0, 1);
    XIDENT(IDF_SWLACC, VARP, ammobariconslast, 0, 0, 1);
    XIDENT(IDF_SWLACC, VARP, ammobarsize, 1, 5, 30);
    XIDENT(IDF_SWLACC, VARP, ammobaroffset_x, 0, 10, 1000);
    XIDENT(IDF_SWLACC, VARP, ammobaroffset_start_x, -1, -1, 1);
    XIDENT(IDF_SWLACC, VARP, ammobaroffset_y, 0, 500, 1000);
    XIDENT(IDF_SWLACC, VARP, ammobarhorizontal, 0, 0, 1);
    XIDENT(IDF_SWLACC, VARP, ammobarselectedcolor_r, 0, 100, 255);
    XIDENT(IDF_SWLACC, VARP, ammobarselectedcolor_g, 0, 200, 255);
    XIDENT(IDF_SWLACC, VARP, ammobarselectedcolor_b, 0, 255, 255);
    XIDENT(IDF_SWLACC, VARP, ammobarselectedcolor_a, 0, 150, 255);
    XIDENT(IDF_SWLACC, VARP, coloredammo, 0, 0, 1);
    
    float staticscale = 0.33;
    
    void getammocolor(fpsent *d, int gun, int &r, int &g, int &b, int &a) 
    {
        if(!d) return;
        if(gun == 0)
            r = 255, g = 255, b = 255, a = 255;
        else if(gun == 2 || gun == 6)
        {
            if(d->ammo[gun] > 10)
                r = 255, g = 255, b = 255, a = 255;
            else if(d->ammo[gun] > 5)
                r = 255, g = 127, b = 0, a = 255;
            else
                r = 255, g = 0, b = 0, a = 255;
        }
        else
        {
            if(d->ammo[gun] > 4)
                r = 255, g = 255, b = 255, a = 255;
            else if(d->ammo[gun] > 2)
                r = 255, g = 127, b = 0, a = 255;
            else
                r = 255, g = 0, b = 0, a = 255;
        }
    }
    
    void drawacoloredquad(float x, float y, float w, float h,
        uchar r, uchar g, uchar b, uchar a) 
    {
        holdscreenlock;

        glDisable(GL_TEXTURE_2D);
        notextureshader->set();
        glColor4ub(r, g, b, a);

        glBegin(GL_TRIANGLE_STRIP);
        glVertex2f(x, y);
        glVertex2f(x + w, y);
        glVertex2f(x, y + h);
        glVertex2f(x + w, y + h);
        glEnd();

        glColor4ub(255, 255, 255, 255);
        glEnable(GL_TEXTURE_2D);
        defaultshader->set();
    }
    
    void drawselectedammobg(float x, float y, float w, float h) 
    {
        drawacoloredquad(x, y, w, h,
                         (GLubyte)ammobarselectedcolor_r,
                         (GLubyte)ammobarselectedcolor_g,
                         (GLubyte)ammobarselectedcolor_b,
                         (GLubyte)ammobarselectedcolor_a);
    }

    static inline int limitscore(int s)
    {
        return s >= 0 ? min(9999, s) : max(-999, s);
    }

    static inline int limitammo(int s)
    {
        return s >= 0 ? min(999, s) : 0;
    }
    
    void drawammobar(fpsent *d, int w, int h) 
    {
        if(!d) return;
        if(scoreboard.showing || guiisshowing()) return;
        
        #define NWEAPONS 6
        float conw = w/staticscale, conh = h/staticscale;

        int icons[NWEAPONS] = {GUN_SG, GUN_CG, GUN_RL, GUN_RIFLE, GUN_GL, GUN_PISTOL};

        int r = 255, g = 255, b = 255, a = 255;
        char buff[10];
        float ammobarscale = (1 + ammobarsize/10.0)*h/1080.0;
        float xoff = 0.0;
        float yoff = ammobaroffset_y*conh/1000;
        float vsep = 10*ammobarscale*staticscale;
        float hsep = 60*ammobarscale*staticscale;
        float textsep = 20*ammobarscale*staticscale;
        int pw = 0, ph = 0, tw = 0, th = 0;

        holdscreenlock;
        glPushMatrix();
        glScalef(staticscale*ammobarscale, staticscale*ammobarscale, 1);
        draw_text("", 0, 0, 255, 255, 255, 255);

        int szx = 0, szy = 0, sz1 = 0;
        text_bounds("999", pw, ph);

        if(ammobarhorizontal)
        {
            szy = ph;
            szx = NWEAPONS * (ph + pw + 2.0 * textsep + hsep) - hsep;
            sz1 = ph + 2.0 * textsep + pw;
        }
        else
        {
            szx = ph + 2.0 * textsep + pw;
            szy = NWEAPONS * (ph + vsep + vsep) - 2*vsep;
            sz1 = szx;
        }

        if(ammobaroffset_start_x == 1)
            xoff = (1000-ammobaroffset_x)*conw/1000 - szx * ammobarscale;
        else if(ammobaroffset_start_x == 0)
            xoff = ammobaroffset_x*conw/1000 - szx/2.0 * ammobarscale;
        else
            xoff = ammobaroffset_x*conw/1000;

        yoff -= szy/2.0 * ammobarscale;

        for(int i = 0, xpos = 0, ypos = 0; i < NWEAPONS; i++)
        {
            snprintf(buff, 10, "%d", limitammo(d->ammo[i+1]));
            text_bounds(buff, tw, th);
            draw_text("", 0, 0, 255, 255, 255, 255);
            if(i+1 == d->gunselect)
            {
                drawselectedammobg(xoff/ammobarscale + xpos,
                                   yoff/ammobarscale + ypos - vsep/2.0,
                                   ph + pw + 2.0*textsep,
                                   ph + vsep);
            }

            if(ammobarfilterempty && d->ammo[i+1] == 0)
                draw_text("", 0, 0, 255, 255, 255, 85);
            if(ammobariconslast)
                drawicon(HICON_FIST+icons[i], xoff/ammobarscale + xpos + sz1 - th - textsep/2.0, yoff/ammobarscale + ypos, ph);
            else
                drawicon(HICON_FIST+icons[i], xoff/ammobarscale + xpos + textsep/2.0, yoff/ammobarscale + ypos, ph);
            if(coloredammo) getammocolor(d, i+1, r, g, b, a);
            if(!(ammobarfilterempty && d->ammo[i+1] == 0))
            {
                if(ammobariconslast)
                    draw_text(buff, xoff/ammobarscale + xpos + textsep/2.0 + (pw-tw)/2.0,
                              yoff/ammobarscale + ypos, r, g, b, a);
                else
                    draw_text(buff, xoff/ammobarscale + xpos + ph + 1.5*textsep + (pw-tw)/2.0,
                              yoff/ammobarscale + ypos, r, g, b, a);
            }
            if(ammobarhorizontal)
                xpos += ph + pw + 2.0 * textsep + hsep;
            else
                ypos += ph + vsep + vsep;
        }
        draw_text("", 0, 0, 255, 255, 255, 255);
        glPopMatrix();
        #undef NWEAPONS
    }
    
    /*
     *  hudscores extension
     *  credit goes to: ETERNALSOLSTICE / aaa
     *  http://github.org/extra-a/sauer-sdl2
     */ 
    
    XIDENT(IDF_SWLACC, VARP, hudscoressize, 1, 5, 30);
    XIDENT(IDF_SWLACC, VARP, hudscoresoffset_x, 0, 10, 1000);
    XIDENT(IDF_SWLACC, VARP, hudscoresoffset_start_x, -1, 1, 1);
    XIDENT(IDF_SWLACC, VARP, hudscoresoffset_y, 0, 350, 1000);
    XIDENT(IDF_SWLACC, VARP, hudscoresplayercolor_r, 0, 0, 255);
    XIDENT(IDF_SWLACC, VARP, hudscoresplayercolor_g, 0, 255, 255);
    XIDENT(IDF_SWLACC, VARP, hudscoresplayercolor_b, 0, 255, 255);
    XIDENT(IDF_SWLACC, VARP, hudscoresplayercolor_a, 0, 255, 255);
    XIDENT(IDF_SWLACC, VARP, hudscoresplayercolorbg_r, 0, 0, 255);
    XIDENT(IDF_SWLACC, VARP, hudscoresplayercolorbg_g, 0, 255, 255);
    XIDENT(IDF_SWLACC, VARP, hudscoresplayercolorbg_b, 0, 255, 255);
    XIDENT(IDF_SWLACC, VARP, hudscoresplayercolorbg_a, 0, 50, 255);
    XIDENT(IDF_SWLACC, VARP, hudscoresenemycolor_r, 0, 255, 255);
    XIDENT(IDF_SWLACC, VARP, hudscoresenemycolor_g, 0, 0, 255);
    XIDENT(IDF_SWLACC, VARP, hudscoresenemycolor_b, 0, 0, 255);
    XIDENT(IDF_SWLACC, VARP, hudscoresenemycolor_a, 0, 255, 255);
    XIDENT(IDF_SWLACC, VARP, hudscoresenemycolorbg_r, 0, 255, 255);
    XIDENT(IDF_SWLACC, VARP, hudscoresenemycolorbg_g, 0, 85, 255);
    XIDENT(IDF_SWLACC, VARP, hudscoresenemycolorbg_b, 0, 85, 255);
    XIDENT(IDF_SWLACC, VARP, hudscoresenemycolorbg_a, 0, 50, 255);
    
    /*
     extern vector<scoregroup *> getscoregroups();
     extern int groupplayers();
     extern void getbestplayers(vector<fpsent *> &best, bool fulllist);
     */
    
    void drawscores(int w, int h) 
    {
        if(scoreboard.showing || guiisshowing()) return;
        
        int conw = int(w/staticscale), conh = int(h/staticscale);

        holdscreenlock;

        vector<fpsent *> bestplayers;
        vector<scoregroup *> bestgroups;
        int grsz = 0;

        if(m_teammode) { grsz = groupplayers(); bestgroups = getscoregroups(); }
        else { getbestplayers(bestplayers,1); grsz = bestplayers.length(); }

        float scorescale = (1 + hudscoressize/10.0)*h/1080.0;
        float xoff = hudscoresoffset_start_x == 1 ? (1000 - hudscoresoffset_x)*conw/1000 : hudscoresoffset_x*conw/1000;
        float yoff = hudscoresoffset_y*conh/1000;
        float scoresep = 40*scorescale*staticscale;
        float borderx = scoresep/2.0;

        int r1, g1, b1, a1, r2, g2, b2, a2,
            bgr1, bgg1, bgb1, bga1, bgr2, bgg2, bgb2, bga2;
        int tw1=0, th1=0, tw2=0, th2=0;

        if(grsz) {
            char buff1[5], buff2[5];
            int isbest=1;
            fpsent *currentplayer = (player1->state == CS_SPECTATOR) ? followingplayer() : player1;
            if(!currentplayer) return;

            if(m_teammode) isbest = ! strcmp(currentplayer->team, bestgroups[0]->team);
            else isbest = currentplayer == bestplayers[0];

            glPushMatrix();
            glScalef(staticscale*scorescale, staticscale*scorescale, 1);
            draw_text("", 0, 0, 255, 255, 255, 255);

            if(isbest)
            {
                int frags=0, frags2=0;
                if(m_teammode) frags = bestgroups[0]->score;
                else frags = bestplayers[0]->frags;
                frags = limitscore(frags);

                snprintf(buff1, 5, "%d", frags);
                text_bounds(buff1, tw1, th1);

                if(grsz > 1)
                {
                    if(m_teammode) frags2 = bestgroups[1]->score;
                    else frags2 = bestplayers[1]->frags;
                    frags2 = limitscore(frags2);

                    snprintf(buff2, 5, "%d", frags2);
                    text_bounds(buff2, tw2, th2);
                }
                else
                {
                    snprintf(buff2, 5, " ");
                    text_bounds(buff2, tw2, th2);
                }

                r1 = hudscoresplayercolor_r;
                g1 = hudscoresplayercolor_g;
                b1 = hudscoresplayercolor_b;
                a1 = hudscoresplayercolor_a;

                r2 = hudscoresenemycolor_r;
                g2 = hudscoresenemycolor_g;
                b2 = hudscoresenemycolor_b;
                a2 = hudscoresenemycolor_a;

                bgr1 = hudscoresplayercolorbg_r;
                bgg1 = hudscoresplayercolorbg_g;
                bgb1 = hudscoresplayercolorbg_b;
                bga1 = hudscoresplayercolorbg_a;

                bgr2 = hudscoresenemycolorbg_r;
                bgg2 = hudscoresenemycolorbg_g;
                bgb2 = hudscoresenemycolorbg_b;
                bga2 = hudscoresenemycolorbg_a;
            }
            else
            {
                int frags=0, frags2=0;
                if(m_teammode) frags = bestgroups[0]->score;
                else frags = bestplayers[0]->frags;
                frags = limitscore(frags);

                snprintf(buff1, 5, "%d", frags);
                text_bounds(buff1, tw1, th1);

                if(m_teammode)
                {
                    loopk(grsz)
                    {
                        if(! strcmp(bestgroups[k]->team, currentplayer->team))
                            frags2 = bestgroups[k]->score;
                    }
                }
                else frags2 = currentplayer->frags;
                frags2 = limitscore(frags2);

                snprintf(buff2, 5, "%d", frags2);
                text_bounds(buff2, tw2, th2);

                r2 = hudscoresplayercolor_r;
                g2 = hudscoresplayercolor_g;
                b2 = hudscoresplayercolor_b;
                a2 = hudscoresplayercolor_a;

                r1 = hudscoresenemycolor_r;
                g1 = hudscoresenemycolor_g;
                b1 = hudscoresenemycolor_b;
                a1 = hudscoresenemycolor_a;

                bgr2 = hudscoresplayercolorbg_r;
                bgg2 = hudscoresplayercolorbg_g;
                bgb2 = hudscoresplayercolorbg_b;
                bga2 = hudscoresplayercolorbg_a;

                bgr1 = hudscoresenemycolorbg_r;
                bgg1 = hudscoresenemycolorbg_g;
                bgb1 = hudscoresenemycolorbg_b;
                bga1 = hudscoresenemycolorbg_a;
            }
            int fw = 0, fh = 0;
            text_bounds("00", fw, fh);
            fw = max(fw, max(tw1, tw2));

            float addoffset = 0.0;
            if(hudscoresoffset_start_x == 1)
                addoffset = 2.0 * fw + 2.0 * borderx + scoresep;
            else if(hudscoresoffset_start_x == 0)
                addoffset = (2.0 * fw + 2.0 * borderx + scoresep)/2.0;
            xoff -= addoffset*scorescale;

            drawacoloredquad(xoff/scorescale,
                             yoff/scorescale - th1/2.0,
                             fw + 2.0*borderx,
                             th1,
                             (GLubyte)bgr1,
                             (GLubyte)bgg1,
                             (GLubyte)bgb1,
                             (GLubyte)bga1);
            draw_text(buff1, xoff/scorescale + borderx + (fw-tw1)/2.0,
                      yoff/scorescale - th1/2.0, r1, g1, b1, a1);
            drawacoloredquad(xoff/scorescale + fw + scoresep,
                             yoff/scorescale - th2/2.0,
                             fw + 2.0*borderx,
                             th2,
                             (GLubyte)bgr2,
                             (GLubyte)bgg2,
                             (GLubyte)bgb2,
                             (GLubyte)bga2);
            draw_text(buff2, xoff/scorescale + fw + scoresep + borderx + (fw-tw2)/2.0,
                      yoff/scorescale - th2/2.0, r2, g2, b2, a2);

            draw_text("", 0, 0, 255, 255, 255, 255);
            glPopMatrix();
        }
    }

    /* frag messages function used in gameplayhud */

    XIDENT(IDF_SWLACC, VARP, fragmsgdeaths, 0, 1, 1);
    XIDENT(IDF_SWLACC, VARP, fragmsgfade, 0, 1200, 10000);
    XIDENT(IDF_SWLACC, VARP, fragmsgname, 0, 1, 1);
    XIDENT(IDF_SWLACC, VARP, fragmsgsize, 1, 4, 8);
    XIDENT(IDF_SWLACC, VARP, fragmsgposy, 0, 50, 1000);

    void drawfragmsg(fpsent *d, int w, int h)
    {   
        if(guiisshowing()) return;
        
        #define WEAP_ICON_SL 64
        #define WEAP_ICON_SPACE 20
        #define ICON_TEXT_DIFF 4

        string buf1, buf2;
        fpsent *att, *vic;
        int fragtime, weapon,
            msg1w, msg1h, msg2w, msg2h, total_width = 0,
            msg1posx, msgiconposx, msg2posx, msgxoffset,
            iconid;
        float alpha;
        bool suicide;

        const float fragmsgscale = 0.35 + fragmsgsize / 8.0;
        const float posy = fragmsgposy * (h / fragmsgscale - WEAP_ICON_SL) / 1000 - ICON_TEXT_DIFF;

        if(d->lastfragtime >= d->lastdeathtime)
        {
            att = d;
            vic = d->lastvictim;
            weapon = d->lastfragweapon;
            fragtime = d->lastfragtime;
        }
        else
        {
        	if(!fragmsgdeaths) return;
            att = d->lastkiller;
            vic = d;
            weapon = d->lastdeathweapon;
            fragtime = d->lastdeathtime;
        }

        suicide = (att == vic);
        if(!fragmsgdeaths && suicide) return;
        iconid = (weapon > -1) ? HICON_FIST + weapon : HICON_TOKEN;

        if(!suicide) {
            sprintf(buf1, "%s", teamcolorname(att, NULL, (!fragmsgname && att == d) ? "You" : att->name));
            text_bounds(buf1, msg1w, msg1h);
            total_width += msg1w + WEAP_ICON_SPACE;
        }

        sprintf(buf2, "%s", teamcolorname(vic, NULL, (!fragmsgname && vic == d) ? "You" : vic->name));
        text_bounds(buf2, msg2w, msg2h);
        total_width += msg2w + WEAP_ICON_SL + WEAP_ICON_SPACE;

        msgxoffset = (total_width * (1 - fragmsgscale));
        msg1posx = (w - total_width + msgxoffset) / (2 * fragmsgscale);
        msgiconposx = (suicide) ? msg1posx : msg1posx + msg1w + WEAP_ICON_SPACE;
        msg2posx = msgiconposx + WEAP_ICON_SL + WEAP_ICON_SPACE; 
        
        alpha = 255;
        if(lastmillis-fragtime > fragmsgfade)
            alpha = 255 - (lastmillis - fragtime) + fragmsgfade;
        alpha = max(alpha, 0.0f);
        
        glPushMatrix();
        glScalef(fragmsgscale, fragmsgscale, 1);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1, 1, 1, alpha/255);
        if(!suicide) draw_text(buf1, msg1posx, posy, 255, 255, 255, alpha);
        drawicon(iconid, msgiconposx, posy + ICON_TEXT_DIFF, WEAP_ICON_SL);
        draw_text(buf2, msg2posx, posy, 255, 255, 255, alpha);
        glPopMatrix();
    }

    /* game clock */

    XIDENT(IDF_SWLACC, VARP, gameclocksize, 1, 5, 30);
    XIDENT(IDF_SWLACC, VARP, gameclockturnredonlowtime, 0, 1, 1);
    XIDENT(IDF_SWLACC, VARP, gameclockcolor_r, 0, 255, 255);
    XIDENT(IDF_SWLACC, VARP, gameclockcolor_g, 0, 255, 255);
    XIDENT(IDF_SWLACC, VARP, gameclockcolor_b, 0, 255, 255);
    XIDENT(IDF_SWLACC, VARP, gameclockcolor_a, 0, 255, 255);
    XIDENT(IDF_SWLACC, VARP, gameclockoffset_x, 0, 900, 1000);
    XIDENT(IDF_SWLACC, VARP, gameclockoffset_y, 0, 5, 1000);
    XIDENT(IDF_SWLACC, VARP, gameclockoffset_x_withradar, 0, 765, 1000);
    XIDENT(IDF_SWLACC, VARP, gameclockoffset_y_withradar, 0, 15, 1000);

    void drawgameclock(int w, int h)
    {           
        static char buf[16];
        const int millis = max(game::maplimit-lastmillis, 0);
        int secs = millis/1000;
        int mins = secs/60;
        secs %= 60;
        sprintf(buf, "%d:%02d", mins, secs);
        
        const float conscale = 0.33f;
        const int conw = int(w/conscale), conh = int(h/conscale);
        
        int r = gameclockcolor_r,
            g = gameclockcolor_g,
            b = gameclockcolor_b,
            a = gameclockcolor_a;
        
        if (mins < 1 && gameclockturnredonlowtime) {
            r = 255;
            g = 0;
            b = 0;
            a = 255;
        }
        
        const float gameclockscale = 1 + gameclocksize/10.0;
        const bool radar = (m_ctf || m_capture);
        const float xoff = ((radar ? gameclockoffset_x_withradar : gameclockoffset_x)*conw/1000);
        const float yoff = ((radar ? gameclockoffset_y_withradar : gameclockoffset_y)*conh/1000);
        
        glPushMatrix();
        glScalef(conscale*gameclockscale, conscale*gameclockscale, 1);
        draw_text(buf,
                  xoff/gameclockscale,
                  yoff/gameclockscale,
                  r, g, b, a);
        glPopMatrix();
    }

    #define MINTIMEOVER 250.0
    #define MAXTIMEASIDE 1000.0
    #define HN_FADEDUR 120.0

    int firstovermillis = 0-MINTIMEOVER, lastovermillis = 0-MAXTIMEASIDE;
    fpsent *lastoverplayer = NULL;

    void drawhudname(fpsent *d, int w, int h)
    {
        string name = "";
        float alpha;
        dynent *o = intersectclosest(d->o, worldpos, d);

        if(o && (o->type == ENT_PLAYER || o->type == ENT_AI) && !guiisshowing())
        {
            if((fpsent *)o != lastoverplayer || lastmillis > lastovermillis + MAXTIMEASIDE) firstovermillis = lastmillis;
            lastovermillis = lastmillis;
            lastoverplayer = (fpsent *)o;
        }

        if(lastoverplayer && (lastoverplayer->type==ENT_PLAYER || lastoverplayer->type==ENT_AI) && lastmillis > firstovermillis + MINTIMEOVER && lastmillis < lastovermillis + MAXTIMEASIDE)
            sprintf(name, "%s", lastoverplayer->name);
        else return;

        alpha = 255;
        if(lastmillis > lastovermillis + MAXTIMEASIDE - HN_FADEDUR)
            alpha = (-lastmillis + lastovermillis + MAXTIMEASIDE) * (255 / HN_FADEDUR);
        alpha = max(alpha, 0.0f);

        glPushMatrix();
        glScalef(1, 1, 1);
        draw_text(name, 200, 400, 255, 255, 255, alpha);
        glPopMatrix();
    }
}

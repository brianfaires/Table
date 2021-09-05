#pragma once
#include "LEDLoop/PaletteManager.h"

enum class PaletteIndex : uint8_t {
	analagousRed, analagousRedOrange, analagousOrange, analagousYellow, analagousGreen, analagousSeaGreen, analagousCyan, analagousBlue, analagousPurple, analagousFusia, analagousPink, analagousPaleRed1, analagousPaleRed2, analagousPaleRed3,
	Orange, 			  Green, 		   Blue,			Pink,
	triadRed,     triadRedOrange,     triadOrange,     triadYellow,     triadLimeGreen,    triadGreen,     triadSeaGreen,     triadCyan,     triadBlue,     triadPurple,     triadFusia,     triadPink,     triadPaleOrange,   triadPalePink, 	   Count
};
#define NUM_PALETTES 3 //uint8_t(PaletteIndex::Count)


static const PROGMEM CHSV allPalettes[NUM_PALETTES][PALETTE_SIZE] = {
{ CHSV(0,255,170), CHSV(0,255,170), CHSV(0,255,170), CHSV(0,255,170), CHSV(0,255,170), CHSV(0,255,170) }, // Pure Red
//{ CHSV(14,255,255), CHSV(0,255,255), CHSV(0,255,255), CHSV(0,255,255), CHSV(0,255,255), CHSV(0,255,255) }, // Pure Orange
//{ CHSV(48,255,255), CHSV(0,255,255), CHSV(0,255,255), CHSV(0,255,255), CHSV(0,255,255), CHSV(0,255,255) }, // Pure Yellow
{ CHSV(99,255,150), CHSV(99,255,150), CHSV(99,255,150), CHSV(99,255,150), CHSV(99,255,150), CHSV(99,255,150) }, // Pure Green
//{ CHSV(0,255,255), CHSV(0,255,255), CHSV(0,255,255), CHSV(0,255,255), CHSV(0,255,255), CHSV(0,255,255) }, // Pure Cyan
{ CHSV(155,255,255), CHSV(155,255,255), CHSV(155,255,255), CHSV(155,255,255), CHSV(155,255,255), CHSV(155,255,255) } // Pure Blue
//{ CHSV(0,255,255), CHSV(0,255,255), CHSV(0,255,255), CHSV(0,255,255), CHSV(0,255,255), CHSV(0,255,255) }, // Pure Purple
//{ CHSV(0,255,255), CHSV(0,255,255), CHSV(0,255,255), CHSV(0,255,255), CHSV(0,255,255), CHSV(0,255,255) }, // Pure Fusia

/*
	{ CHSV(255,255,255), CHSV(  0,200,255), CHSV(246,185,255), CHSV( 45,230,255), CHSV(155,230,255), CHSV( 96, 90,255) }, // analag Red
	{ CHSV(  7,255,255), CHSV( 26,255,255), CHSV( 16,255,255), CHSV(  1,255,255), CHSV(  7,235,255), CHSV( 50,180,255) }, // analag RedOrange
	{ CHSV( 14,255,255), CHSV( 28,255,255), CHSV(  6,240,255), CHSV( 10,220,255), CHSV(137,220,255), CHSV(137,100,255) }, // analag Orange
	{ CHSV( 48,255,255), CHSV( 30,255,255), CHSV( 69,255,255), CHSV( 90,200,255), CHSV(  0,150,255), CHSV( 57,150,255) }, // analag Yellow
	{ CHSV( 99,255,255), CHSV( 85,225,255), CHSV( 67,245,255), CHSV( 95,237,255), CHSV(  2,180,255), CHSV( 32,100,255) }, // analag Green
	{ CHSV(111,255,255), CHSV(130,255,255), CHSV( 98,220,255), CHSV( 71,235,255), CHSV( 40,255,255), CHSV( 64,180,255) }, // analag SeaGreen
	{ CHSV(131,255,255), CHSV(115,220,255), CHSV(150,195,255), CHSV(165,255,255), CHSV( 64,210,255), CHSV(128,115,255) }, // analag Cyan
	{ CHSV(155,255,255), CHSV(173,255,255), CHSV(127,220,255), CHSV(145,190,255), CHSV( 52,210,255), CHSV( 63,100,255) }, // analag Blue
	{ CHSV(192,255,255), CHSV(174,255,255), CHSV(210,255,255), CHSV(254,235,255), CHSV(161,235,255), CHSV(  0,  0,255) }, // analag Purple
	{ CHSV(220,255,255), CHSV(195,255,255), CHSV(240,195,255), CHSV(255,200,255), CHSV(142,255,255), CHSV(  0,  0,255) }, // analag Fusia
	{ CHSV(245,255,255), CHSV(210,255,255), CHSV(235,185,255), CHSV(230,255,255), CHSV(105,200,255), CHSV(120, 70,255) }, // analag Pink
	{ CHSV(  1,230,255), CHSV(  9,220,255), CHSV( 19,230,255), CHSV(  8,205,255), CHSV( 25,205,255), CHSV(  5,125,255) }, // anal PaleRed1
	{ CHSV(255,210,255), CHSV(230,200,255), CHSV(195,230,255), CHSV(160,160,255), CHSV(115,150,255), CHSV(200, 80,255) }, // anal PaleRed2
	{ CHSV(  0,180,255), CHSV( 40,220,255), CHSV( 20,220,255), CHSV(138,160,255), CHSV(230,180,255), CHSV(120,100,255) }, // anal PaleRed3
	{ CHSV(  9,225,255), CHSV( 28,215,255), CHSV( 16,255,255), CHSV( 32,235,255), CHSV(  7,240,255), CHSV( 60,120,255) }, // anal Orange
	{ CHSV( 96,190,255), CHSV( 69,210,255), CHSV(115,205,255), CHSV( 74,235,255), CHSV( 64,255,255), CHSV(  0,  0,255) }, // anal Green
	{ CHSV(160,180,255), CHSV(132,210,255), CHSV(116,210,255), CHSV(200,170,255), CHSV(  2,150,255), CHSV(  0,  0,255) }  // anal Blue
	{ CHSV(234,190,255), CHSV(196,210,255), CHSV(175,230,255), CHSV(255,200,255), CHSV( 13,190,255), CHSV(  5,100,255) }, // anal Pink
	{ CHSV(255,255,255), CHSV(162,255,255), CHSV( 71,255,255), CHSV(255,200,255), CHSV(225,220,255), CHSV(  0,  0,255) }, // triad Red
	{ CHSV(  8,255,255), CHSV( 87,255,255), CHSV(155,255,255), CHSV( 24,255,255), CHSV(122,240,255), CHSV(  0,  0,255) }, // triad RedOrange
	{ CHSV( 16,255,255), CHSV(138,255,255), CHSV( 80,255,255), CHSV( 32,200,255), CHSV(  9,200,255), CHSV(  0,  0,255) }, // triad Orange
	{ CHSV( 56,255,255), CHSV(132,255,255), CHSV(248,235,255), CHSV(170,255,255), CHSV(208,220,255), CHSV(  0,  0,255) }, // triad Yellow
	{ CHSV( 80,255,255), CHSV(  2,220,255), CHSV(144,255,255), CHSV( 27,220,255), CHSV( 63,220,255), CHSV(  0,  0,255) }, // triad LimeGreen
	{ CHSV( 92,235,255), CHSV(162,215,255), CHSV( 18,255,255), CHSV( 90,205,255), CHSV(  0,180,255), CHSV(  0,  0,255) }, // triad Green
	{ CHSV(112,255,255), CHSV(218,255,255), CHSV( 54,235,255), CHSV(148,255,255), CHSV(  0,200,255), CHSV(  0,  0,255) }, // triad SeaGreen
	{ CHSV(128,255,255), CHSV( 59,255,255), CHSV(235,200,255), CHSV( 35,255,255), CHSV(  6,230,255), CHSV(  0,  0,255) }, // triad Cyan
	{ CHSV(161,255,255), CHSV( 80,255,255), CHSV( 14,255,255), CHSV(144,200,255), CHSV( 40,225,255), CHSV(  0,  0,255) }, // triad Blue
	{ CHSV(192,255,255), CHSV(100,175,255), CHSV( 28,255,255), CHSV(224,255,255), CHSV(130,220,255), CHSV(  0,  0,255) }, // triad Purple
	{ CHSV(224,255,255), CHSV(110,255,255), CHSV( 70,255,255), CHSV(130,255,255), CHSV(254,190,255), CHSV(  0,  0,255) }, // triad Fusia
	{ CHSV(244,255,255), CHSV(135,255,255), CHSV( 56,255,255), CHSV(  0,235,255), CHSV(104,200,255), CHSV(  0,  0,255) }, // triad Pink
	{ CHSV(  8,220,255), CHSV(151,255,255), CHSV( 78,230,255), CHSV(230,255,255), CHSV(100,255,255), CHSV(  0,  0,255) }, // triad PaleOrange
	{ CHSV(240,190,255), CHSV(133,255,255), CHSV( 66,230,255), CHSV(252,230,255), CHSV(165,215,255), CHSV(  0,  0,255) }  // triad PalePink

*/
};


/*
static CHSV bhw239[PALETTE_SIZE] =    { CHSV(148,255,255), CHSV(195,255,255), CHSV( 64,255,255), CHSV( 32,255,255), CHSV( 14,255,255), CHSV(238,120,255) };
static CHSV bhw238[PALETTE_SIZE] =    { CHSV(130,255,255), CHSV(224,255,255), CHSV( 64,255,255), CHSV(170,255,255), CHSV(192,255,255), CHSV(  0,  0,255) };
static CRGB ocean[PALETTE_SIZE] = { CRGB(0,0,255), CRGB(10,200,255), CRGB(90,50,255), CRGB(60,140,255), CRGB(90,150,255), CRGB(200,200,255) };
static CRGB lava[PALETTE_SIZE] = { CRGB(255,0,0), CRGB(255,100,0), CRGB(255,255,0), CRGB(255,150,50), CRGB(220,255,160), CRGB(255,200,100) };
static CHSV bhw106[PALETTE_SIZE] =    { CHSV(235,255,255), CHSV(200,255,255), CHSV(122,225,255), CHSV(175,255,255), CHSV(145,255,255), CHSV(160,120,255) };
static CHSV bhw332[PALETTE_SIZE] =    { CHSV( 48,255,255), CHSV( 18,255,255), CHSV(164,240,255), CHSV(233,210,255), CHSV(196,220,255), CHSV(  0,  0,255) };
static CHSV fib5306[PALETTE_SIZE] =   { CHSV(128,255,255), CHSV(224,255,255), CHSV(192,255,255), CHSV( 10,210,255), CHSV(250,160,255), CHSV(128,100,255) };
static CHSV qualDark[PALETTE_SIZE] =  { CHSV(170,255,255), CHSV( 28,245,255), CHSV( 50,255,255), CHSV(255,236,255), CHSV(112,235,255), CHSV( 92,120,255) };


static CHSV ocean[PALETTE_SIZE] =     { CHSV(160,255,255), CHSV(135,200,255), CHSV(145,180,255), CHSV(155,155,255), CHSV(140,255,255), CHSV(145,110,255) };
static CHSV fire[PALETTE_SIZE] =      { CHSV(  8,255,255), CHSV( 16,245,255), CHSV( 25,255,255), CHSV( 48,255,255), CHSV( 34,150,255), CHSV(160,120,255) };
static CHSV icyBright[PALETTE_SIZE] = { CHSV(120,135,255), CHSV(210,160,255), CHSV( 64,190,255), CHSV(135,180,255), CHSV(210,150,255), CHSV(  0,  0,255) };
static CHSV watermelon[PALETTE_SIZE] ={ CHSV(220,255,255), CHSV( 79,255,255), CHSV(240,255,255), CHSV( 90,180,255), CHSV(192,255,255), CHSV(190,100,255) };
static CHSV fruitLoop[PALETTE_SIZE] = { CHSV(224,255,255), CHSV(175,255,255), CHSV( 80,235,255), CHSV(140,255,255), CHSV(190,255,255), CHSV( 64,170,255) };
static CHSV love[PALETTE_SIZE] =      { CHSV(250,255,255), CHSV(224,255,255), CHSV(197,255,255), CHSV(224,200,255), CHSV(250,175,255), CHSV(160, 65,255) };
static CHSV rave[PALETTE_SIZE] =      { CHSV(160,255,255), CHSV(208,255,255), CHSV(  0,255,255), CHSV( 53,255,255), CHSV( 96,255,255), CHSV(128,100,255) };
static CHSV pastel[PALETTE_SIZE] =    { CHSV(132,200,255), CHSV( 32,210,255), CHSV(200,165,255), CHSV(  5,180,255), CHSV(220,190,255), CHSV( 64,150,255) };
static CHSV strobe[PALETTE_SIZE] =    { CHSV(160,100,255), CHSV(  5,100,255), CHSV(  0,255,255), CHSV( 32,185,255), CHSV(160,145,255), CHSV(  0,  0,255) };
static CHSV lava[PALETTE_SIZE] =      { CHSV(  3,255,255), CHSV( 28,255,255), CHSV( 40,255,255), CHSV( 18,255,255), CHSV(255,255,255), CHSV(128,150,255) };
static CHSV forest[PALETTE_SIZE] =    { CHSV(  0,255,255), CHSV( 32,255,255), CHSV( 64,255,255), CHSV( 48,200,255), CHSV( 70,160,255), CHSV( 50,100,255) };
static CHSV rainbow[PALETTE_SIZE] =   { CHSV(  0,255,255), CHSV( 32,255,255), CHSV( 64,255,255), CHSV( 48,200,255), CHSV( 70,160,255), CHSV( 50,100,255) };

static CRGB ocean[PALETTE_SIZE] =     { CRGB(  0,  0,255), CRGB(  0,130,210), CRGB( 25,200,160), CRGB( 30, 45,230), CRGB( 55,  7,255), CRGB(200,200, 60) };
static CRGB fire[PALETTE_SIZE] =      { CRGB(255,  6,  0), CRGB(255, 52,  0), CRGB(240,130,  0), CRGB(220,197,  0), CRGB( 32, 26,255), CRGB(255,170, 85) };
static CRGB icyBright[PALETTE_SIZE] = { CRGB( 35,255,180), CRGB(255,138,185), CRGB(255,255, 70), CRGB( 55,135,255), CRGB(190, 95,255), CRGB(255,255,255) };
static CRGB watermelon[PALETTE_SIZE] ={ CRGB(255,  0,165), CRGB(160, 26,255), CRGB(170,255,  8), CRGB(255, 20, 70), CRGB(100,255,  0), CRGB(200,150,255) };
static CRGB fruitLoop[PALETTE_SIZE] = { CRGB(255,105,  0), CRGB( 40,  0,255), CRGB( 80,255, 14), CRGB(  0, 80,255), CRGB(255,  0,120), CRGB(255,255, 60) };
static CRGB love[PALETTE_SIZE] =      { CRGB(255,  3,  7), CRGB(255,  0,200), CRGB(135, 15,255), CRGB(130,  0,  0), CRGB( 50, 24,255), CRGB(100,100,255) };
static CRGB rave[PALETTE_SIZE] =      { CRGB(  0,  0,255), CRGB(255,  0,  0), CRGB(255,255,  0), CRGB(255,  0,255), CRGB(  0,255,  0), CRGB(100,255,255) };
static CRGB pastel[PALETTE_SIZE] =    { CRGB( 30,205,255), CRGB(255,125, 20), CRGB(120,255,130), CRGB(210, 38, 32), CRGB(110, 40,255), CRGB(255,255, 80) };
static CRGB strobe[PALETTE_SIZE] =    { CRGB(185,172,255), CRGB( 50, 40, 30), CRGB(255,  0,  0), CRGB(200,200, 35), CRGB(  0,  0,255), CRGB(255,255,255) };
*/


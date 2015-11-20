#ifndef TRIANGLE_WEB_H__
#define TRIANGLE_WEB_H__

#include <Arduino.h>
#include <math.h>
#include "ILI9341_t3.h"

struct TriangleWebVars {
  float _phase;
};
TriangleWebVars triWeb = (TriangleWebVars){0};

void triangleWeb_start( ILI9341_t3 tft ) {
  uint_fast16_t w = tft.width();
  uint_fast16_t h = tft.height();
  tft.fillRect( 0, 0, w, h, 0x0 );
}

Point getWebPoint( uint_fast8_t i, uint_fast8_t j, float phase ) {
  uint_fast8_t idx = j*4 + i;
  uint_fast8_t rando = (idx ^ 37);
  float angle = rando * phase;
  return (Point){ 
    (uint_fast16_t)( (i*64) + (cos(angle)*20.0f) ),
    (uint_fast16_t)( (j*64) + (sin(angle)*20.0f) ) 
   };
}

uint_fast16_t scrollTest = 0;

void triangleWeb_perFrame( ILI9341_t3 tft, FrameParams frameParams ) {
  // Hmm
  tft.setScroll( scrollTest );
  scrollTest++;
  if( scrollTest >= 320 ) scrollTest = 0;
  
  //uint_fast16_t w = tft.width();
  //uint_fast16_t h = tft.height();
  
  // fillRect: flickers pretty bad
  //tft.fillRect( 0, 0, w, h, LV_RED );

  triWeb._phase += frameParams.timeMult * 0.0005 * (frameParams.audioMean*3.0f + 1.0f);
  
  // Prepare web color
  uint_fast8_t bright = (frameParams.audioPeak >> 1); // 0..512 -> 0..255
  uint_fast16_t color = tft.color565( bright, bright, bright );

  // TODO: It is possible to re-use some Point's here
  Point nw, ne, sw;
  for( uint_fast8_t i=0; i<4; i++ ) { // step across
    nw = getWebPoint( i, 0, triWeb._phase );

    for( uint_fast8_t j=0; j<6; j++ ) { // step down
      ne = getWebPoint( i+1, j, triWeb._phase );
      sw = getWebPoint( i, j+1, triWeb._phase );

      tft.drawLine( nw.x, nw.y, ne.x, ne.y, color );
      tft.drawLine( nw.x, nw.y, sw.x, sw.y, color );
      tft.drawLine( ne.x, ne.y, sw.x, sw.y, color );

      // Re-use sw point as new nw
      nw = sw;
    }
  }
  
}

#endif
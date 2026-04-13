#include "drive.h"

/* ══════════════════════════════════════════════════════════════════
   HELPER FUNCTIONS
   ══════════════════════════════════════════════════════════════════ */

/* ── Car ─────────────────────────────────────────────────────────── */


/* ── Mountain fill ───────────────────────────────────────────────── */
void mtnFill(float l, float px, float py, float rr, float by,
             float cr, float cg, float cb,
             float fogr, float fogg, float fogb, float fogStart)
{
  for (float x = l; x <= rr; x += 0.5f)
  {
    float ty;
    if (x <= px) {
      float d = px - l;
      ty = (fabsf(d) > 1e-6f) ? (by + (x - l) / d * (py - by)) : by;
    } else {
      float d = rr - px;
      ty = (fabsf(d) > 1e-6f) ? (py + (x - px) / d * (by - py)) : py;
    }
    float fogT = cl((ty - fogStart) / (by - fogStart));
    col3(lp(cr, fogr, fogT * 0.6f),
         lp(cg, fogg, fogT * 0.6f),
         lp(cb, fogb, fogT * 0.6f));
    dda(x, by, x, ty);
  }
}

/* ── Snow cap ────────────────────────────────────────────────────── */
void snowCap(float px, float py, float sp)
{
  for (float x = px - sp; x <= px + sp; x += 0.5f)
  {
    float t   = fabsf(x - px) / sp;
    float bot = py - (1 - t) * sp * 0.65f;
    for (float yy = bot; yy <= py; yy += 0.5f)
    {
      float s = 0.75f + 0.25f * (yy - bot) / (py - bot + 0.01f);
      col3(0.88f * s, 0.90f * s, 0.98f * s);
      glBegin(GL_POINTS); glVertex2f(x, yy); glEnd();
    }
  }
}

/* ── Pine Tree ───────────────────────────────────────────────────── */
void drawPine(float x, float groundY, float h, float fogT)
{
  /* Trunk */
  float tw = h * 0.06f;
  col3(lp(0.28f, 0.55f, fogT), lp(0.18f, 0.48f, fogT), lp(0.10f, 0.42f, fogT));
  fillRect(x - tw * 0.5f, groundY, tw, h * 0.22f);

  /* 4 tiers of foliage, bottom to top */
  float tierW[] = { 0.58f, 0.44f, 0.30f, 0.17f };
  float tierH[] = { 0.30f, 0.24f, 0.19f, 0.13f };
  float ys = groundY + h * 0.14f;
  for (int t = 0; t < 4; t++)
  {
    float tw2 = tierW[t] * h;
    float th  = tierH[t] * h;
    for (float yy = ys; yy <= ys + th; yy += 0.5f)
    {
      float frac = (yy - ys) / th;
      float half = tw2 * (1.0f - frac);
      float br   = 0.85f + 0.15f * (1.0f - frac);
      col3(lp(0.10f * br, 0.50f, fogT),
           lp(0.35f * br, 0.62f, fogT),
           lp(0.10f * br, 0.45f, fogT));
      dda(x - half, yy, x + half, yy);
    }
    /* Dark underside shadow line */
    col3(lp(0.06f, 0.40f, fogT), lp(0.20f, 0.52f, fogT), lp(0.06f, 0.38f, fogT));
    dda(x - tw2, ys + th * 0.93f, x + tw2, ys + th * 0.93f);
    ys += th * 0.72f;
  }
}

/* ── Palm Tree ───────────────────────────────────────────────────── */
void drawPalmTree(float x, float groundY, float h, float fogT)
{
  for (float yy = groundY; yy < groundY + h; yy += 0.5f)
  {
    float frac  = (yy - groundY) / h;
    float lean  = frac * frac * 14.0f;
    float width = lp(7.0f, 2.5f, frac);
    float shade = 0.75f + 0.25f * fmodf(frac * 18.0f, 1.0f);
    col3(lp(0.36f * shade, 0.55f, fogT),
         lp(0.22f * shade, 0.48f, fogT),
         lp(0.10f * shade, 0.38f, fogT));
    dda(x + lean - width, yy, x + lean + width, yy);
  }
  float cx = x + 14.0f, cy = groundY + h;
  float angles[]  = { 90,   130,  160,  200,  230,  50,   20  };
  float lengths[] = { 55,   50,   45,   45,   50,   48,   42  };
  float droops[]  = { 0.25f,0.35f,0.40f,0.40f,0.35f,0.30f,0.38f };
  for (int i = 0; i < 7; i++)
  {
    float ang = angles[i] * 3.14159f / 180.0f;
    float len = lengths[i];
    float px0 = cx, py0 = cy;
    for (int s = 1; s <= 10; s++)
    {
      float t    = (float)s / 10.0f;
      float drop = droops[i] * len * t * t;
      float px1  = cx + cosf(ang) * len * t;
      float py1  = cy + sinf(ang) * len * t - drop;
      float fw   = lp(4.5f, 0.5f, t);
      float shade2 = 0.7f + 0.3f * (1.0f - t);
      col3(lp(0.10f * shade2, 0.55f, fogT),
           lp(0.42f * shade2, 0.65f, fogT),
           lp(0.08f * shade2, 0.45f, fogT));
      for (float w = -fw; w <= fw; w += 0.5f) {
        float ox = -sinf(ang) * w, oy = cosf(ang) * w;
        dda(px0+ox, py0+oy, px1+ox, py1+oy);
      }
      px0 = px1; py0 = py1;
    }
  }
  col3(lp(0.30f, 0.55f, fogT), lp(0.18f, 0.45f, fogT), lp(0.05f, 0.30f, fogT));
  fc(cx - 4, cy - 4, 4);
  fc(cx + 3, cy - 6, 3);
  fc(cx,     cy - 2, 3);
}

/* ── Bird ────────────────────────────────────────────────────────── */
void drawBird(float x, float y, float flapT)
{
  float flap = sinf(flapT) * 6.0f;
  col3(0.06f, 0.05f, 0.05f);
  dda(x,      y,      x - 10, y + flap);
  dda(x - 10, y+flap, x - 18, y + flap - 2);
  dda(x,      y,      x + 10, y + flap);
  dda(x + 10, y+flap, x + 18, y + flap - 2);
  fc(x, y, 2);
}

/* ══════════════════════════════════════════════════════════════════
   SCENE 2 — MOUNTAIN ROAD AT DAWN
   ══════════════════════════════════════════════════════════════════ */
void scene2()
{
  float speed  = 120.0f;
  float scroll = fmodf(T * speed, W + 240.0f);

  /* [LAYER 1] SKY */
  Stop sky2[] = {
    {0.0f,  0.02f, 0.01f, 0.14f},
    {0.18f, 0.06f, 0.04f, 0.24f},
    {0.35f, 0.30f, 0.12f, 0.24f},
    {0.52f, 0.70f, 0.38f, 0.16f},
    {0.68f, 0.88f, 0.62f, 0.20f},
    {0.82f, 0.95f, 0.82f, 0.55f},
    {1.0f,  0.98f, 0.93f, 0.80f}
  };
  skyGrad(sky2, 7);

  float fogr = 0.82f, fogg = 0.75f, fogb = 0.84f;

  /* [LAYER 2] FAR BACKGROUND MOUNTAINS — very hazy, low peaks */
  mtnFill(  0, 150, 280, 320, 110, 0.55f, 0.48f, 0.62f, fogr, fogg, fogb, 160);
  mtnFill(160, 340, 265, 510, 110, 0.52f, 0.46f, 0.60f, fogr, fogg, fogb, 160);
  mtnFill(470, 640, 272, 820, 110, 0.53f, 0.47f, 0.61f, fogr, fogg, fogb, 160);

  /* [LAYER 3] MID MOUNTAINS — medium darkness, taller */
  mtnFill(  0, 140, 340, 310, 110, 0.36f, 0.30f, 0.46f, fogr, fogg, fogb, 200);
  mtnFill(280, 460, 360, 640, 110, 0.34f, 0.28f, 0.44f, fogr, fogg, fogb, 200);
  mtnFill(590, 720, 350, 820, 110, 0.35f, 0.29f, 0.45f, fogr, fogg, fogb, 200);

  /* [LAYER 4] NEAR MOUNTAINS — darkest, tallest, snow capped */
  mtnFill(  0, 175, 405, 370, 110, 0.24f, 0.18f, 0.34f, fogr, fogg, fogb, 230);
  mtnFill(340, 560, 420, 760, 110, 0.26f, 0.20f, 0.36f, fogr, fogg, fogb, 230);
  snowCap(175, 405, 48);
  snowCap(560, 420, 42);
  /* Extra snow detail lines */
  col3(0.80f, 0.84f, 0.94f);
  dda(140, 360, 175, 405);
  dda(590, 375, 560, 420);

  /* [LAYER 5] ROAD */
  col3(0.15f, 0.15f, 0.15f);
  fillRect(0, 0, W, 110);
  /* Edge lines */
  col3(0.82f, 0.82f, 0.82f);
  fillRect(0, 105, W, 3);
  fillRect(0,   3, W, 3);
  /* Centre dashes */
  col3(0.90f, 0.82f, 0.20f);
  for (float rx = -120; rx < W + 120; rx += 140) {
    float lx = fmodf(rx + scroll, W + 240.0f) - 120.0f;
    fillRect(lx, 52, 65, 6);
  }
  /* Road texture */
  col3(0.13f, 0.13f, 0.13f);
  for (float rx = -60; rx < W + 60; rx += 70) {
    float lx = fmodf(rx + scroll * 0.5f, W + 120.0f) - 60.0f;
    fillRect(lx,      22, 28, 2);
    fillRect(lx + 12, 82, 22, 2);
  }

  /* [LAYER 6] BACKGROUND PINE TREES (small, behind big trees) */
  drawPine(  40, 110,  80, 0.35f);
  drawPine( 120, 110,  90, 0.28f);
  drawPine( 680, 110,  85, 0.28f);
  drawPine( 760, 110,  78, 0.35f);


  /* [LAYER 8] PALM TREES (foreground edges) */
  drawPalmTree( 30, 110, 130, 0.08f);
  drawPalmTree(155, 110, 115, 0.16f);
  drawPalmTree(645, 110, 118, 0.16f);
  drawPalmTree(762, 110, 128, 0.08f);

  /* [LAYER 9] FOREGROUND PINE (tall, dark, very close) */
  drawPine(  0, 110, 135, 0.05f);
  drawPine(800, 110, 130, 0.05f);

  /* [LAYER 10] BIRDS — two flocks at different heights/speeds */
  float bd1 = fmodf(T * 18.0f, W + 80.0f) - 40.0f;
  drawBird(bd1 + 100, 340, T * 4.0f);
  drawBird(bd1 + 132, 352, T * 4.0f + 1.1f);
  drawBird(bd1 + 158, 342, T * 4.0f + 2.3f);

  float bd2 = fmodf(T * 12.0f, W + 80.0f) - 40.0f;
  drawBird(bd2 + 320, 305, T * 3.5f);
  drawBird(bd2 + 350, 292, T * 3.5f + 0.9f);
  drawBird(bd2 + 378, 310, T * 3.5f + 1.8f);
  drawBird(bd2 + 480, 318, T * 4.2f + 0.5f);

  /* [LAYER 11] CAR — frontmost */
  float carX = fmodf(scroll, W + 200.0f) - 100.0f;
  drawCar(carX, 32);
}

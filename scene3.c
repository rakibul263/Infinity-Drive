#include "drive.h"

/* ══════════════════════════════════════════════════════════════════
   SCENE 3 — VILLAGE AFTERNOON
   Subsurface-scatter stone walls, tile roof shading, chimney smoke
   physics, realistic foliage with layered light, cobblestones
   ══════════════════════════════════════════════════════════════════ */

/* Smoke particle system */
typedef struct
{
  float x, y, vx, vy, life, r;
} Smoke;
static Smoke smk[60];
static int smkInit = 0;
void initSmoke()
{
  if (smkInit)
    return;
  smkInit = 1;
  for (int i = 0; i < 60; i++)
  {
    smk[i].x = 0;
    smk[i].y = 0;
    smk[i].life = 0;
  }
}
void updateSmoke(float cx, float cy)
{
  for (int i = 0; i < 60; i++)
  {
    if (smk[i].life <= 0)
    {
      smk[i].x = cx + (float)(rand() % 5 - 2);
      smk[i].y = cy;
      smk[i].vx = (float)(rand() % 7 - 3) * 0.15f;
      smk[i].vy = 0.45f + rand() % 10 * 0.06f;
      smk[i].life = 1.0f;
      smk[i].r = 4.0f + rand() % 6;
      break;
    }
  }
  for (int i = 0; i < 60; i++)
  {
    if (smk[i].life > 0)
    {
      smk[i].x += smk[i].vx;
      smk[i].y += smk[i].vy;
      smk[i].r += 0.08f;
      smk[i].life -= 0.012f;
    }
  }
}
void drawSmoke()
{
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  for (int i = 0; i < 60; i++)
  {
    if (smk[i].life > 0)
    {
      float a = smk[i].life * 0.35f;
      float c = 0.65f + smk[i].life * 0.20f;
      glColor4f(c, c, c + 0.05f, a);
      fc(smk[i].x, smk[i].y, smk[i].r);
    }
  }
  glDisable(GL_BLEND);
}

/* Stone wall — subsurface scatter approximation */
void stoneWall(float x, float y, float w, float h, float lr, float lg, float lb)
{
  /* base colour gradient (lit top, shadow bottom) */
  for (float yy = y; yy < y + h; yy += 0.5f)
  {
    float sh = 0.72f + 0.28f * (yy - y) / h;
    col3(lr * sh, lg * sh, lb * sh);
    dda(x, yy, x + w, yy);
  }
  /* mortar lines horizontal */
  float mCol = lr * 0.52f, mColG = lg * 0.52f, mColB = lb * 0.52f;
  for (float yy = y + 14; yy < y + h; yy += 14)
  {
    col3(mCol, mColG, mColB);
    dda(x, yy, x + w, yy);
  }
  /* mortar lines vertical (offset per row) */
  int row = 0;
  for (float yy = y; yy < y + h; yy += 14)
  {
    float off = (row % 2) * 22;
    for (float xx = x + off; xx < x + w; xx += 44)
    {
      col3(mCol, mColG, mColB);
      dda(xx, yy, xx, yy + 14);
    }
    row++;
  }
  /* left edge AO */
  for (float yy = y; yy < y + h; yy += 0.5f)
  {
    float sh = 0.68f;
    col3(lr * sh, lg * sh, lb * sh);
    dda(x, yy, x + 8, yy);
  }
  /* top edge light */
  col3(cl(lr + 0.12f), cl(lg + 0.12f), cl(lb + 0.12f));
  dda(x, y + h - 1, x + w, y + h - 1);
}

/* Tile roof */
void tileRoof(float x, float y, float w, float h, float rr, float rg, float rb)
{
  /* base shape */
  glBegin(GL_TRIANGLES);
  col3(rr, rg, rb);
  glVertex2f(x + w / 2, y + h);
  col3(rr * 0.82f, rg * 0.82f, rb * 0.82f);
  glVertex2f(x - 10, y);
  glVertex2f(x + w + 10, y);
  glEnd();
  /* tile rows */
  for (int row = 0; row < 7; row++)
  {
    float ry = y + row * h / 7.0f;
    float hw = (w / 2 + 10) * (1.0f - row / 7.0f);
    col3(rr * 0.70f, rg * 0.70f, rb * 0.70f);
    dda(x + w / 2 - hw, ry, x + w / 2 + hw, ry);
    /* tile highlights */
    col3(cl(rr + 0.08f), cl(rg + 0.05f), cl(rb + 0.02f));
    for (float tx = x + w / 2 - hw; tx < x + w / 2 + hw; tx += 16)
      dda(tx, ry, tx + 8, ry + h / 7.0f * 0.55f);
  }
  /* shadow under eaves */
  col3(rr * 0.42f, rg * 0.42f, rb * 0.42f);
  dda(x - 10, y, x + w + 10, y);
  dda(x - 11, y - 1, x + w + 11, y - 1);
}

/* Window with interior glow */
void villageWindow(float x, float y, float w, float h)
{
  /* warm interior */
  col3(0.98f, 0.88f, 0.52f);
  fillRect(x, y, w, h);
  /* curtains */
  col3(0.75f, 0.40f, 0.30f);
  fillRect(x, y, w * 0.22f, h);
  fillRect(x + w * 0.78f, y, w * 0.22f, h);
  /* cross pane */
  col3(0.30f, 0.18f, 0.08f);
  dda(x, y + h / 2, x + w, y + h / 2);
  dda(x + w / 2, y, x + w / 2, y + h);
  /* frame */
  col3(0.22f, 0.13f, 0.06f);
  dda(x - 2, y - 2, x + w + 2, y - 2);
  dda(x - 2, y + h + 2, x + w + 2, y + h + 2);
  dda(x - 2, y - 2, x - 2, y + h + 2);
  dda(x + w + 2, y - 2, x + w + 2, y + h + 2);
  /* glass reflection */
  col3(0.85f, 0.90f, 0.98f);
  dda(x + 3, y + h - 4, x + w * 0.42f, y + 4);
  /* exterior glow */
  glow(x + w / 2, y + h / 2, 0, 22, 0.95f, 0.82f, 0.35f);
}

/* Realistic cottage */
void cottage3(float x, float y, float w, float h,
              float wr, float wg, float wb, float rr, float rg, float rb,
              float chimX)
{
  stoneWall(x, y, w, h, wr, wg, wb);
  tileRoof(x, y + h, w, h * 0.62f, rr, rg, rb);
  /* chimney */
  col3(wr * 0.68f, wg * 0.65f, wb * 0.60f);
  fillRect(chimX, y + h, 14, h * 0.42f);
  /* chimney cap */
  col3(wr * 0.50f, wg * 0.48f, wb * 0.44f);
  fillRect(chimX - 2, y + h + h * 0.42f, 18, 4);
  /* smoke */
  updateSmoke(chimX + 7, y + h + h * 0.44f);
  drawSmoke();
  /* arched door */
  float dw = w * 0.26f, dh = h * 0.48f, dx = x + w * 0.37f;
  col3(0.28f, 0.15f, 0.07f);
  fillRect(dx, y, dw, dh * 0.68f);
  col3(0.22f, 0.11f, 0.05f);
  fc(dx + dw / 2, y + dh * 0.68f, dw / 2);
  /* door planks */
  col3(0.32f, 0.18f, 0.08f);
  for (int p = 1; p < 4; p++)
    dda(dx, y + dh * 0.18f * p, dx + dw, y + dh * 0.18f * p);
  /* door knob */
  col3(0.75f, 0.62f, 0.10f);
  fc(dx + dw * 0.82f, y + dh * 0.32f, 2.5f);
  /* windows */
  villageWindow(x + w * 0.08f, y + h * 0.42f, w * 0.22f, h * 0.30f);
  villageWindow(x + w * 0.70f, y + h * 0.42f, w * 0.22f, h * 0.30f);
}

/* Detailed windmill */
void windmill3(float x, float y, float ang)
{
  /* stone tower — tapered */
  for (float yy = y; yy < y + 105; yy += 0.5f)
  {
    float f = (yy - y) / 105.0f;
    float hw = 18 - f * 7;
    float sh = 0.68f + 0.22f * (1 - f);
    col3(0.60f * sh, 0.56f * sh, 0.48f * sh);
    dda(x - hw, yy, x + hw, yy);
    /* mortar */
    if (fmod(yy - y, 13) < 0.7f)
    {
      col3(0.38f, 0.34f, 0.28f);
      dda(x - hw, yy, x + hw, yy);
    }
  }
  /* conical cap */
  for (float yy = y + 105; yy < y + 128; yy += 0.5f)
  {
    float f = (yy - y - 105) / 23.0f;
    float hw = (1 - f) * 13;
    col3(0.30f, 0.20f, 0.10f);
    dda(x - hw, yy, x + hw, yy);
  }
  /* hub with rivets */
  col3(0.48f, 0.44f, 0.40f);
  fc(x, y + 105, 9);
  col3(0.28f, 0.24f, 0.20f);
  mca(x, y + 105, 9);
  /* 4 sails — canvas on wooden frame */
  for (int b = 0; b < 4; b++)
  {
    float rad = (ang + b * 90.0f) * 3.14159f / 180.0f;
    float pr = (ang + b * 90 + 90) * 3.14159f / 180.0f;
    float ex = x + 52 * cosf(rad), ey = y + 105 + 52 * sinf(rad);
    /* wood spar */
    col3(0.38f, 0.24f, 0.10f);
    dda(x, y + 105, ex, ey);
    /* canvas fill */
    col3(0.88f, 0.84f, 0.72f);
    glBegin(GL_POLYGON);
    glVertex2f(x + 9 * cosf(pr), y + 105 + 9 * sinf(pr));
    glVertex2f(x - 9 * cosf(pr), y + 105 - 9 * sinf(pr));
    glVertex2f(ex - 8 * cosf(pr), ey - 8 * sinf(pr));
    glVertex2f(ex + 8 * cosf(pr), ey + 8 * sinf(pr));
    glEnd();
    /* canvas shadow */
    col3(0.65f, 0.60f, 0.48f);
    dda(x - 9 * cosf(pr), y + 105 - 9 * sinf(pr), ex - 8 * cosf(pr), ey - 8 * sinf(pr));
  }
}

/* Stone well with rope */
void stoneWell3(float x, float y)
{
  /* barrel */
  for (float yy = y; yy < y + 34; yy += 0.5f)
  {
    float c = 0.52f + 0.08f * sinf((yy - y) * 0.5f);
    col3(c + 0.04f, c, c - 0.06f);
    dda(x - 20, yy, x + 20, yy);
    if (fmod(yy - y, 9) < 0.7f)
    {
      col3(0.34f, 0.30f, 0.26f);
      dda(x - 20, yy, x + 20, yy);
    }
  }
  mca(x, y, 20);
  mca(x, y + 34, 20);
  /* posts */
  col3(0.32f, 0.20f, 0.10f);
  fillRect(x - 24, y + 32, 7, 38);
  fillRect(x + 17, y + 32, 7, 38);
  /* cross beam */
  fillRect(x - 24, y + 68, 48, 6);
  /* rope */
  col3(0.60f, 0.48f, 0.22f);
  for (float yy = y + 45; yy < y + 68; yy += 3)
    dda(x - 1, yy, x + 1, yy + 3);
  /* bucket */
  col3(0.28f, 0.22f, 0.16f);
  glBegin(GL_POLYGON);
  glVertex2f(x - 7, y + 28);
  glVertex2f(x + 7, y + 28);
  glVertex2f(x + 5, y + 44);
  glVertex2f(x - 5, y + 44);
  glEnd();
  /* water */
  col3(0.25f, 0.50f, 0.72f);
  dda(x - 4, y + 42, x + 4, y + 42);
}

/* Foliage tree — realistic multilayer canopy */
void realTree(float x, float by, float h)
{
  /* trunk */
  for (float yy = by; yy < by + h * 0.38f; yy += 0.5f)
  {
    float hw = 5 - 3 * (yy - by) / (h * 0.38f);
    float sh = 0.55f + 0.20f * (yy - by) / (h * 0.38f);
    col3(0.28f * sh, 0.16f * sh, 0.07f * sh);
    dda(x - hw, yy, x + hw, yy);
  }
  /* bark texture lines */
  col3(0.18f, 0.10f, 0.05f);
  for (float yy = by + 6; yy < by + h * 0.35f; yy += 10)
    dda(x - 4, yy, x - 2, yy + 8);
  /* layered canopy */
  float cy = by + h * 0.35f;
  /* deep shadow layer */
  col3(0.04f, 0.28f, 0.06f);
  fc(x, cy, h * 0.36f);
  /* mid layer */
  col3(0.08f, 0.42f, 0.10f);
  fc(x + h * 0.08f, cy + h * 0.08f, h * 0.28f);
  fc(x - h * 0.10f, cy + h * 0.06f, h * 0.22f);
  /* lit upper layer */
  col3(0.14f, 0.58f, 0.16f);
  fc(x + h * 0.05f, cy + h * 0.18f, h * 0.18f);
  /* specular highlight */
  col3(0.28f, 0.72f, 0.22f);
  fc(x + h * 0.10f, cy + h * 0.26f, h * 0.08f);
  /* fruit dots */
  col3(0.85f, 0.18f, 0.08f);
  fc(x - h * 0.14f, cy + h * 0.05f, 3);
  fc(x + h * 0.12f, cy - h * 0.02f, 3);
}

/* Detailed fence */
void realFence(float x, float y, float len)
{
  /* rail */
  col3(0.45f, 0.30f, 0.12f);
  fillRect(x, y + 6, len, 5);
  /* posts every 22px */
  for (float px = x; px < x + len; px += 22)
  {
    col3(0.40f, 0.26f, 0.10f);
    fillRect(px - 2, y - 2, 5, 22);
    /* post cap */
    col3(0.55f, 0.38f, 0.16f);
    glBegin(GL_TRIANGLES);
    glVertex2f(px - 3, y + 20);
    glVertex2f(px + 4, y + 20);
    glVertex2f(px + 0.5f, y + 26);
    glEnd();
  }
  /* picket infill */
  col3(0.52f, 0.36f, 0.14f);
  for (float px = x + 5; px < x + len - 4; px += 8)
    dda(px, y - 2, px, y + 16);
}

/* Cobblestone path */
void cobblePath(float x, float y, float w, float h2)
{
  col3(0.44f, 0.38f, 0.30f);
  fillRect(x, y, w, h2);
  /* individual stones */
  srand(13);
  for (float sy = y + 4; sy < y + h2 - 4; sy += 16)
  {
    float off = (rand() % 2) * 11;
    for (float sx = x + off + 4; sx < x + w - 4; sx += 22)
    {
      float sw2 = 14 + rand() % 6, sh = 10 + rand() % 4;
      float sr = 0.48f + rand() % 20 * 0.01f, sg = 0.42f + rand() % 15 * 0.01f, sb = 0.34f + rand() % 12 * 0.01f;
      col3(sr, sg, sb);
      fillRect(sx, sy, sw2, sh);
      col3(sr * 0.70f, sg * 0.70f, sb * 0.70f);
      dda(sx, sy, sx + sw2, sy);
      dda(sx, sy, sx, sy + sh);
      col3(sr + 0.08f, sg + 0.06f, sb + 0.04f);
      dda(sx, sy + sh, sx + sw2, sy + sh);
      dda(sx + sw2, sy, sx + sw2, sy + sh);
    }
  }
}

/* Fluffy cloud — multi-layer */
void fluffyCloud(float x, float y, float s)
{
  /* shadow underside */
  col3(0.78f, 0.78f, 0.84f);
  fc(x, y - s * 0.2f, s * 0.55f);
  fc(x + s * 0.7f, y - s * 0.15f, s * 0.42f);
  fc(x - s * 0.7f, y - s * 0.12f, s * 0.38f);
  /* main body */
  col3(1.0f, 1.0f, 1.0f);
  fc(x, y, s);
  fc(x + s * 0.8f, y + s * 0.28f, s * 0.72f);
  fc(x - s * 0.8f, y + s * 0.22f, s * 0.62f);
  fc(x + s * 0.35f, y + s * 0.48f, s * 0.52f);
  fc(x - s * 0.30f, y + s * 0.42f, s * 0.48f);
  /* bright top */
  col3(1.0f, 1.0f, 1.0f);
  fc(x + s * 0.1f, y + s * 0.52f, s * 0.32f);
}

void scene3()
{
  /* 1. Car Movement Logic */
  // The car moves from left to right. Speed 120.0f makes it feel steady.
  float carX = (T * 120.0f);

  // This makes the car wrap back to the left side once it leaves the screen
  // (W + 200) ensures it fully disappears before reappearing
  carX = fmodf(carX, W + 200.0f) - 100.0f;

  /* 2. Sky & Atmosphere (Static) */
  Stop sky3[] = {{0.0f, 0.42f, 0.72f, 0.96f}, {0.55f, 0.65f, 0.85f, 0.98f}, {1.0f, 0.90f, 0.95f, 1.0f}};
  skyGrad(sky3, 3);

  glow(680, 560, 30, 90, 1.0f, 0.92f, 0.45f);
  col3(1.0f, 0.97f, 0.72f); fc(680, 560, 26);
  fluffyCloud(90, 510, 24);
  fluffyCloud(340, 535, 19);

  /* 3. Background Scenery (Static) */
  for (float yy = 0; yy < 225; yy += 0.5f) {
    float t = yy / 225.0f;
    col3(lp(0.12f, 0.20f, t), lp(0.45f, 0.58f, t), lp(0.10f, 0.14f, t));
    dda(0, yy, W, yy);
  }

  // Houses and Trees are now at fixed positions
  initSmoke();
  cottage3(50,  222, 110, 100, 0.76f, 0.70f, 0.60f, 0.50f, 0.20f, 0.16f, 110);
  realTree(180, 222, 90);

  cottage3(300, 222, 118, 104, 0.68f, 0.74f, 0.62f, 0.22f, 0.45f, 0.20f, 380);
  realTree(450, 222, 110);

  cottage3(600, 222, 100, 92, 0.70f, 0.74f, 0.64f, 0.26f, 0.42f, 0.22f, 660);
  windmill3(750, 222, T * 40.0f);

  /* 4. Road (Static) */
  col3(0.30f, 0.28f, 0.25f);
  fillRect(0, 0, W, 222);

  // Center lane markings (Static cobblestones)
  col3(0.40f, 0.38f, 0.35f);
  for (int i = 0; i < W; i += 100) {
    fillRect(i + 20, 108, 40, 6);
  }

  /* 5. Draw the Moving Car */
  // carX is the only thing changing over time here
  drawCar(carX, 72);
}

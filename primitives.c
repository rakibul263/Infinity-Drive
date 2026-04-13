#include "drive.h"

void initStars()
{
  if (stInit)
    return;
  stInit = 1;
  srand(42);
  for (int i = 0; i < 120; i++)
  {
    stX[i] = (float)(rand() % 800);
    stY[i] = (float)(280 + rand() % 320);
    stS[i] = 0.8f + 1.4f * (rand() % 100) / 100.0f;
    stTw[i] = (float)(rand() % 628) / 100.0f;
  }
}

/* ══════════════════════════════════════════════════════════════════
   CORE DRAWING PRIMITIVES
   ══════════════════════════════════════════════════════════════════ */
void dda(float x1, float y1, float x2, float y2)
{
  float dx = x2 - x1, dy = y2 - y1;
  float s = fabsf(dx) > fabsf(dy) ? fabsf(dx) : fabsf(dy);
  if (s < 0.5f)
  {
    glBegin(GL_POINTS);
    glVertex2f(x1, y1);
    glEnd();
    return;
  }
  float xi = dx / s, yi = dy / s, x = x1, y = y1;
  glBegin(GL_POINTS);
  for (int i = 0; i <= (int)s; i++)
  {
    glVertex2f(x, y);
    x += xi;
    y += yi;
  }
  glEnd();
}
static void pp8(float cx, float cy, float x, float y)
{
  glVertex2f(cx + x, cy + y);
  glVertex2f(cx - x, cy + y);
  glVertex2f(cx + x, cy - y);
  glVertex2f(cx - x, cy - y);
  glVertex2f(cx + y, cy + x);
  glVertex2f(cx - y, cy + x);
  glVertex2f(cx + y, cy - x);
  glVertex2f(cx - y, cy - x);
}
void mca(float cx, float cy, float r)
{
  if (r < 1)
  {
    glBegin(GL_POINTS);
    glVertex2f(cx, cy);
    glEnd();
    return;
  }
  float x = 0, y = r, d = 1 - r;
  glBegin(GL_POINTS);
  pp8(cx, cy, x, y);
  while (x < y)
  {
    if (d < 0)
      d += 2 * x + 3;
    else
    {
      d += 2 * (x - y) + 5;
      y--;
    }
    x++;
    pp8(cx, cy, x, y);
  }
  glEnd();
}
void fc(float cx, float cy, float r)
{
  for (float ri = 0.5f; ri <= r; ri += 0.5f)
    mca(cx, cy, ri);
}

/* ── Colour helpers ─────────────────────────────────────────────── */
void col3(float r, float g, float b) { glColor3f(r, g, b); }
float cl(float v) { return v < 0 ? 0 : (v > 1 ? 1 : v); }
float lp(float a, float b, float t) { return a + (b - a) * t; }

/* ── Gradient horizontal band ───────────────────────────────────── */
void gradBand(float x, float y, float w, float h,
              float r0, float g0, float b0, float r1, float g1, float b1)
{
  glBegin(GL_QUADS);
  col3(r0, g0, b0);
  glVertex2f(x, y + h);
  glVertex2f(x + w, y + h);
  col3(r1, g1, b1);
  glVertex2f(x + w, y);
  glVertex2f(x, y);
  glEnd();
}
/* ── Sky scan-line gradient ─────────────────────────────────────── */
void skyGrad(Stop *stops, int n)
{
  if (n < 2)
    return;
  for (int y = 0; y < H; y++)
  {
    float t = (float)y / H;
    int lo = 0;
    if (t <= stops[0].yf)
      lo = 0;
    else if (t >= stops[n - 1].yf)
      lo = n - 2;
    else
    {
      for (int i = 0; i < n - 1; i++)
      {
        if (t >= stops[i].yf && t < stops[i + 1].yf)
        {
          lo = i;
          break;
        }
      }
    }
    float span = stops[lo + 1].yf - stops[lo].yf;
    float f = (span > 0) ? (t - stops[lo].yf) / span : 0;
    if (f < 0)
      f = 0;
    if (f > 1)
      f = 1;
    col3(lp(stops[lo].r, stops[lo + 1].r, f),
         lp(stops[lo].g, stops[lo + 1].g, f),
         lp(stops[lo].b, stops[lo + 1].b, f));
    dda(0, y, W, y);
  }
}

/* ── Glow halo around a point ───────────────────────────────────── */
void glow(float cx, float cy, float r, float R, float gr, float gg, float gb)
{
  if (R <= r + 1e-6f)
    return;
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  for (float ri = r; ri < R; ri += 1.5f)
  {
    float a = powf((R - ri) / (R - r), 2) * 0.35f;
    glColor4f(gr, gg, gb, a);
    mca(cx, cy, ri);
  }
  glDisable(GL_BLEND);
}

/* ── Filled rect ────────────────────────────────────────────────── */
void fillRect(float x, float y, float w, float h)
{
  glBegin(GL_QUADS);
  glVertex2f(x, y);
  glVertex2f(x + w, y);
  glVertex2f(x + w, y + h);
  glVertex2f(x, y + h);
  glEnd();
}

/* ── Stars ──────────────────────────────────────────────────────── */
void drawStars(float brightness)
{
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  for (int i = 0; i < 120; i++)
  {
    float tw = 0.55f + 0.45f * sinf(T * 1.8f + stTw[i]);
    float b = stS[i] * tw * brightness * 0.95f;
    glColor4f(0.92f, 0.93f, 1.0f, b);
    fc(stX[i], stY[i], stS[i] * 1.1f);
  }
  glDisable(GL_BLEND);
}

/* ── Moon ───────────────────────────────────────────────────────── */
void drawMoon(float x, float y, float skyr, float skyg, float skyb)
{
  glow(x, y, 24, 65, 0.90f, 0.90f, 0.70f);
  col3(0.955f, 0.952f, 0.880f);
  fc(x, y, 23);
  col3(0.860f, 0.855f, 0.780f);
  fc(x + 5, y - 4, 18);
  col3(skyr, skyg, skyb);
  fc(x + 10, y + 6, 18);
  col3(0.820f, 0.818f, 0.750f);
  fc(x - 5, y + 6, 7);
  fc(x + 6, y - 2, 5);
}

/* ── Realistic stick person with shadow ────────────────────────── */
void drawPerson(float x, float y, float shR, float shG, float shB)
{
  float sw = sinf(T * 4.2f) * 9.0f;
  col3(0, 0, 0);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4f(0, 0, 0, 0.28f);
  for (float r = 0; r < 12; r += 0.5f)
    mca(x, y - 14, r);
  glDisable(GL_BLEND);
  col3(shR, shG, shB);
  dda(x, y + 20, x, y + 2);
  dda(x, y + 15, x - 10, y + 5 + sw);
  dda(x, y + 15, x + 10, y + 5 - sw);
  dda(x, y + 2, x - 8, y - 14 + sw * 0.4f);
  dda(x, y + 2, x + 8, y - 14 - sw * 0.4f);
  col3(0.88f, 0.70f, 0.52f);
  fc(x, y + 27, 7);
  col3(0.15f, 0.09f, 0.04f);
  fc(x, y + 33, 5);
  col3(0.12f, 0.08f, 0.05f);
  fc(x - 2.5f, y + 28, 1.2f);
  fc(x + 2.5f, y + 28, 1.2f);
}

/* Inside primitives.c */

void drawCar(float x, float y)
{
  /* Shadow */
  col3(0.10f, 0.10f, 0.10f);
  for (float i = 0; i < 5; i++)
    fillRect(x - 44 + i, y - 4, 88 - i * 2, 3);

  /* Wheels */
  col3(0.08f, 0.08f, 0.08f);
  fc(x - 26, y, 10);
  fc(x + 26, y, 10);
  col3(0.35f, 0.35f, 0.35f);
  fc(x - 26, y, 5);
  fc(x + 26, y, 5);

  /* Chassis */
  col3(0.75f, 0.08f, 0.08f);
  fillRect(x - 46, y + 4, 92, 20);

  /* Roof */
  col3(0.65f, 0.06f, 0.06f);
  fillRect(x - 28, y + 24, 56, 18);

  /* Windows */
  col3(0.55f, 0.80f, 0.95f);
  fillRect(x - 24, y + 26, 20, 12);
  fillRect(x + 4, y + 26, 20, 12);

  /* Headlight & Tail light */
  col3(1.0f, 0.95f, 0.60f); fillRect(x + 40, y + 8, 7, 6);
  col3(0.95f, 0.10f, 0.10f); fillRect(x - 47, y + 8, 6, 6);
}

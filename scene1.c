#include "drive.h"

/* ══════════════════════════════════════════════════════════════════
   SCENE 1 — CITY NIGHT  (rain-wet neon city)
   Photorealistic: wet asphalt reflections, neon bloom, layered fog,
   building material variety, detailed car geometry
   ══════════════════════════════════════════════════════════════════ */

/* Neon sign with full bloom */
void neonSign(float x, float y, float r, float g, float b, float radius)
{
  glow(x, y, radius, radius * 3.5f, r, g, b);
  col3(r, g, b);
  fc(x, y, radius);
  col3(cl(r + 0.3f), cl(g + 0.3f), cl(b + 0.3f));
  mca(x, y, radius + 1);
}

/* Realistic building with glass curtain wall */
void building(float x, float y, float w, float h,
              float mr, float mg, float mb, /* material base */
              int floors, int cols, float glassR, float glassG, float glassB)
{
  /* ambient occlusion base — darker bottom */
  for (float yy = y; yy < y + h; yy += 0.5f)
  {
    float sh = 0.7f + 0.3f * (yy - y) / h;
    col3(mr * sh, mg * sh, mb * sh);
    dda(x, yy, x + w, yy);
  }
  /* left edge AO */
  for (float yy = y; yy < y + h; yy += 0.5f)
  {
    float sh = 0.78f;
    col3(mr * sh, mg * sh, mb * sh);
    dda(x, yy, x + w * 0.06f, yy);
  }
  /* right highlight */
  col3(cl(mr + 0.08f), cl(mg + 0.08f), cl(mb + 0.10f));
  dda(x + w - 1, y, x + w - 1, y + h);
  /* roof edge */
  col3(cl(mr + 0.12f), cl(mg + 0.12f), cl(mb + 0.12f));
  dda(x, y + h, x + w, y + h);

  /* windows */
  float ww = w / cols, wh = h / floors;
  for (int f = 0; f < floors; f++)
  {
    for (int c = 0; c < cols; c++)
    {
      float wx = x + ww * c + ww * 0.15f, wy = y + wh * f + wh * 0.12f;
      float wp = ww * 0.70f, hp = wh * 0.65f;
      int lit = (rand() % (f / 2 + 2) != 0);
      /* glass base */
      float gc = (lit) ? 1.0f : 0.18f;
      col3(glassR * gc, glassG * gc, glassB * gc);
      fillRect(wx, wy, wp, hp);
      /* window frame */
      col3(mr * 0.55f, mg * 0.55f, mb * 0.55f);
      dda(wx, wy, wx + wp, wy);
      dda(wx, wy + hp, wx + wp, wy + hp);
      dda(wx, wy, wx, wy + hp);
      dda(wx + wp, wy, wx + wp, wy + hp);
      /* reflection sheen on glass */
      if (lit)
      {
        col3(cl(glassR + 0.15f), cl(glassG + 0.15f), cl(glassB + 0.05f));
        dda(wx + wp * 0.1f, wy + hp * 0.8f, wx + wp * 0.4f, wy + hp * 0.6f);
        /* bloom on wet road below */
        float ry = y - (wy - y) - hp * 0.5f;
        if (ry > 0 && ry < y)
        {
          glEnable(GL_BLEND);
          glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
          glColor4f(glassR, glassG, glassB, 0.18f);
          fillRect(wx, ry - hp * 0.3f, wp, hp * 0.35f);
          glDisable(GL_BLEND);
        }
      }
    }
  }
  /* antenna */
  col3(0.55f, 0.55f, 0.58f);
  dda(x + w * 0.6f, y + h, x + w * 0.6f, y + h + 22);
  col3(0.8f, 0.15f, 0.10f);
  fc(x + w * 0.6f, y + h + 22, 3); /* red warning light */
}


/* Realistic traffic light with pole box */
void trafficLight(float x, float y)
{
  /* shadow */
  col3(0.08f, 0.08f, 0.10f);
  dda(x + 3, y - 2, x + 3, y + 92);
  /* pole */
  col3(0.22f, 0.24f, 0.26f);
  dda(x, y, x, y + 92);
  col3(0.28f, 0.30f, 0.32f);
  dda(x + 1, y, x + 1, y + 92);
  /* housing box with gradient */
  for (float yy = y + 58; yy < y + 95; yy += 0.5f)
  {
    float sh = 0.7f + 0.3f * (yy - y - 58) / 37.0f;
    col3(0.10f * sh, 0.10f * sh, 0.12f * sh);
    dda(x - 9, yy, x + 9, yy);
  }
  col3(0.16f, 0.16f, 0.18f);
  dda(x - 9, y + 58, x + 9, y + 58);
  dda(x - 9, y + 95, x + 9, y + 95);
  /* lights */
  float pulse = 0.85f + 0.15f * sinf(T * 1.5f);
  glow(x, y + 88, 5, 18, 0.95f, 0.10f, 0.10f);
  col3(0.95f * pulse, 0.08f, 0.08f);
  fc(x, y + 88, 5);
  col3(0.90f, 0.70f, 0.02f);
  fc(x, y + 78, 5);
  glow(x, y + 68, 5, 16, 0.10f, 0.90f, 0.15f);
  col3(0.10f, 0.92f * pulse, 0.12f);
  fc(x, y + 68, 5);
}

void scene1()
{
  /* ── 1. Scoped Variables for Movement (From Scene 2) ── */
  float speed  = 120.0f;
  float scroll = fmodf(T * speed, W + 240.0f);

  /* ── 2. Sky & Atmosphere (Original Scene 1) ── */
  Stop sky1[] = {{0.0f, 0.02f, 0.02f, 0.10f}, {0.45f, 0.04f, 0.04f, 0.18f}, {0.75f, 0.06f, 0.06f, 0.22f}, {1.0f, 0.08f, 0.08f, 0.26f}};
  skyGrad(sky1, 4);
  drawStars(1.0f);
  drawMoon(720, 535, 0.06f, 0.06f, 0.22f);

  /* ── 3. City Background (Original Scene 1) ── */
  srand(7);
  building(0, 222, 68, 290, 0.12f, 0.13f, 0.17f, 6, 2, 0.9f, 0.85f, 0.55f);
  building(130, 222, 88, 265, 0.10f, 0.14f, 0.16f, 6, 3, 0.90f, 0.88f, 0.60f);
  building(520, 222, 80, 308, 0.09f, 0.11f, 0.20f, 7, 3, 0.55f, 0.82f, 0.98f);
  building(740, 222, 56, 278, 0.16f, 0.11f, 0.16f, 6, 2, 0.90f, 0.70f, 0.95f);
  neonSign(110, 418, 1.0f, 0.08f, 0.45f, 5);
  neonSign(595, 385, 0.08f, 0.85f, 1.0f, 5);

  /* ── 4. Road Logic (Integrated from Scene 2) ── */
  // Base asphalt
  col3(0.12f, 0.12f, 0.14f);
  fillRect(0, 0, W, 222);

  // Kerb lines (Modified Scene 1)
  col3(0.48f, 0.48f, 0.52f);
  dda(0, 55, W, 55); dda(0, 167, W, 167);

  // Scrolling Centre dashes (From Scene 2 Logic)
  col3(0.78f, 0.68f, 0.02f);
  for (float rx = -120; rx < W + 120; rx += 140) {
    float lx = fmodf(rx + scroll, W + 240.0f) - 120.0f;
    // Glow effect from scene 1 applied to scrolling dashes
    glow(lx + 18, 111, 0, 12, 0.75f, 0.65f, 0.0f);
    fillRect(lx, 110, 36, 3);
  }

  /* ── 5. Static Props (Original Scene 1) ── */
  trafficLight(288, 167);
  trafficLight(508, 167);

  /* ── 6. The Car (Integrated from Scene 2) ── */
  // carX uses the same scroll calculation as Scene 2
  float carX = fmodf(scroll, W + 200.0f) - 100.0f;
  drawCar(carX, 72); // Positioned on the lower lane

  /* ── 7. Fog (Original Scene 1) ── */
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  for (float yy = 0; yy < 35; yy += 2) {
    float a = (35 - yy) / 35.0f * 0.22f;
    glColor4f(0.18f, 0.18f, 0.28f, a);
    dda(0, yy, W, yy);
  }
  glDisable(GL_BLEND);
}

# PR7-N's body: traced off a product photo of a 5-string bass the owner chose (straight on, neck up, white body on a
# white background, a shop's badge top right, fanned frets with a slanted fretboard end). The body is everything not
# pure white (the background is exactly 255); the neck is measured from its maple board; the strap buttons (dark blobs
# touching the background) come off; the badge is left out. Never commits the photo; no maker's name or logo used.
#   python3 trace_ng3.py <photo> <out folder> [neck width at the board's end, mm] [smoothing px]
import cv2, numpy as np, json, sys
im = cv2.imread(sys.argv[1]); out = sys.argv[2]
neck_mm = float(sys.argv[3]) if len(sys.argv) > 3 else 72.0
smooth = int(sys.argv[4]) if len(sys.argv) > 4 else 5
h, w = im.shape[:2]
g = cv2.cvtColor(im, cv2.COLOR_BGR2GRAY).astype(int)
B, G, R = [im[:, :, i].astype(int) for i in range(3)]
maple = (R - B > 25) & (R > 150)
rows = list(range(100, 561, 10))
L = [np.nonzero(maple[y, 550:900])[0].min() + 550 for y in rows]
Rt = [np.nonzero(maple[y, 550:900])[0].max() + 550 for y in rows]
def robust(ys, xs):
    ys, xs = np.array(ys, float), np.array(xs, float); keep = np.ones(len(ys), bool)
    for _ in range(5):
        f = np.polyfit(ys[keep], xs[keep], 1); res = np.abs(np.polyval(f, ys) - xs); keep = res < max(1.5, 2.5 * np.median(res[keep]))
    return f
fl, fr = robust(rows, L), robust(rows, Rt)
centre_x = int(round((np.polyval(fl, 600) + np.polyval(fr, 600)) / 2))
board_end = int(np.nonzero(maple[300:800, centre_x])[0].max()) + 300   # where the board ends on the neck's centre line
# the body: not the background
gs = cv2.medianBlur(cv2.cvtColor(im, cv2.COLOR_BGR2GRAY), 5).astype(int)   # (JPEG speckle on the white rim)
fg = (gs < 251).astype(np.uint8)
fg[:540, 1040:] = 0                                                     # the badge
# strap buttons: dark blobs touching the background
dark = cv2.dilate((gs < 150).astype(np.uint8), np.ones((5, 5), np.uint8))   # (their highlights with them)
n, lab, stats, _ = cv2.connectedComponentsWithStats(dark)
bg = cv2.dilate((gs >= 251).astype(np.uint8), np.ones((7, 7), np.uint8))
for i in range(1, n):
    x, y, ww, hh, area = stats[i]
    if area < 4000 and (bg[lab == i] > 0).any():
        fg[cv2.dilate((lab == i).astype(np.uint8), np.ones((9, 9), np.uint8)) > 0] = 0
# the neck cut away down to the board's end (our 3D neck covers it), the slanted end included
for y in range(0, board_end + 25):
    a, b = int(np.polyval(fl, y)) - 9, int(np.polyval(fr, y)) + 9   # (the neck's edge isn't maple-coloured)
    if y > board_end:   # below the centre line's end, only where the maple still is (the slant)
        xs = np.nonzero(maple[y, a:b + 1])[0]
        if len(xs) == 0: continue
        a, b = a + xs.min() - 6, a + xs.max() + 6
    fg[y, max(0, a):min(w, b + 1)] = 0
fg = cv2.morphologyEx(fg, cv2.MORPH_CLOSE, cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (7, 7)))
cnts, _ = cv2.findContours(fg, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
fill = np.zeros_like(fg); cv2.drawContours(fill, [max(cnts, key=cv2.contourArea)], -1, 1, -1)
fill = cv2.morphologyEx(fill, cv2.MORPH_OPEN, cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (5, 5)))
cnts, _ = cv2.findContours(fill, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
c = max(cnts, key=cv2.contourArea)[:, 0, :].astype(float)
wrap = np.concatenate([c[-smooth:], c, c[:smooth]]); k = np.ones(smooth) / smooth
c = np.stack([np.convolve(wrap[:, 0], k, "same"), np.convolve(wrap[:, 1], k, "same")], 1)[smooth:-smooth]
simple = cv2.approxPolyDP(np.round(c).astype(np.int32).reshape(-1, 1, 2), 0.8, True)[:, 0, :]
neck_px = np.polyval(fr, board_end) - np.polyval(fl, board_end); centre = (np.polyval(fl, board_end) + np.polyval(fr, board_end)) / 2
kmm = neck_mm / neck_px
pts = [[round((board_end - py) * kmm, 2), round((centre - px) * kmm, 2)] for px, py in simple]
xs, ys = [p[0] for p in pts], [p[1] for p in pts]
print(f"neck edges x = {fl[0]:.4f} y + {fl[1]:.1f} / {fr[0]:.4f} y + {fr[1]:.1f}; board end row {board_end}; {neck_px:.0f} px = {neck_mm} mm there")
print(f"{len(pts)} points; {min(xs):.0f} to {max(xs):.0f} mm along (from the board's end), {min(ys):.0f} to {max(ys):.0f} across "
      f"(body {max(xs)-min(xs):.0f} x {max(ys)-min(ys):.0f} mm)")
json.dump({"about": "The body's outline in millimetres: x from where the fretboard ends (toward the neck +), y from the neck's "
                    "centre line (toward the low strings +). Traced off a product photo of a 5-string bass the owner chose "
                    f"(straight on, neck up; scaled by an assumed {neck_mm:g} mm board width at its end); no maker's name or logo used.",
           "points": pts}, open(out + "/body-outline.json", "w"))
vis = im.copy(); cv2.polylines(vis, [simple.reshape(-1, 1, 2)], True, (0, 0, 255), 3)
cv2.line(vis, (0, board_end), (w, board_end), (0, 200, 0), 1)
cv2.imwrite(out + "/traced-small.png", cv2.resize(vis, None, fx=0.5, fy=0.5))

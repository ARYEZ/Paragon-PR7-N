# PR7-N's headstock: traced off the owner's photo of the 5-string bass's headstock (neck up, the headstock orange on a
# pale brick wall, a shop's badge right; no maker's name or logo used; the photo isn't committed). Every point in mm:
# x along the neck from the slanted nut line (the fanned nut), y across (+ the low strings' side, the photo's left),
# scaled so the neck's width at the nut is our neck's (59 mm). Posts: each string's tuner post, low string first.
#   python3 trace_head.py <photo> <out.json> [nut width mm]
import cv2, numpy as np, json, sys
im = cv2.imread(sys.argv[1]); out = sys.argv[2]
nut_mm = float(sys.argv[3]) if len(sys.argv) > 3 else 59.0
hsv = cv2.cvtColor(im, cv2.COLOR_BGR2HSV)
sat, val = hsv[:, :, 1].astype(int), hsv[:, :, 2].astype(int)
# the photo's own numbers (read off it): the nut's line (low side, high side), the neck below it, the posts
nut_a, nut_b = (394.0, 1183.0), (513.0, 1258.0)
nut_row = lambda px: nut_a[1] + (nut_b[1] - nut_a[1]) * (px - nut_a[0]) / (nut_b[0] - nut_a[0])
neck_l, neck_r = 391.0, 516.0                       # the neck's edges just under the nut
posts_px = [(391, 1061), (418, 955), (463, 905), (488, 1016), (513, 1122)]   # low string first (followed up from the nut)
k = nut_mm / (neck_r - neck_l)
centre = (neck_l + neck_r) / 2
to_mm = lambda px, py: [round((nut_row(px) - py) * k, 2), round((centre - px) * k, 2)]
# the headstock: everything tinted (orange, its pale edge) above the nut, the badge and the wall left out
m = ((sat > 30) & (val > 60)).astype(np.uint8)
m[:, 640:] = 0
h, w = m.shape
for px in range(w):
    m[int(nut_row(min(max(px, nut_a[0]), nut_b[0]))) + 1:, px] = 0   # nothing below the nut's line
m[:760, :] = 0
m = cv2.morphologyEx(m, cv2.MORPH_CLOSE, cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (9, 9)))
cnts, _ = cv2.findContours(m, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
fill = np.zeros_like(m); cv2.drawContours(fill, [max(cnts, key=cv2.contourArea)], -1, 1, -1)
fill = cv2.morphologyEx(fill, cv2.MORPH_OPEN, cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (13, 13)))   # the tuner shafts' stubs
cnts, _ = cv2.findContours(fill, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
c = max(cnts, key=cv2.contourArea)[:, 0, :].astype(float)
n = 5; wrap = np.concatenate([c[-n:], c, c[:n]]); kk = np.ones(n) / n
c = np.stack([np.convolve(wrap[:, 0], kk, "same"), np.convolve(wrap[:, 1], kk, "same")], 1)[n:-n]
simple = cv2.approxPolyDP(np.round(c).astype(np.int32).reshape(-1, 1, 2), 0.8, True)[:, 0, :]
pts = [to_mm(px, py) for px, py in simple]
posts = [to_mm(px, py) for px, py in posts_px]
xs, ys = [p[0] for p in pts], [p[1] for p in pts]
print(f"{len(pts)} points; {min(xs):.0f} to {max(xs):.0f} mm from the nut, {min(ys):.0f} to {max(ys):.0f} across; posts {posts}")
json.dump({"note": "PR7-N: the 5-string bass's headstock in mm (x along the neck from the slanted nut line, y across, + the low "
                   "strings' side), traced off the owner's photo (no maker's name or logo; the photo isn't committed); posts: the "
                   "tuners' string posts, low string first; keys: which side each tuner's key sticks out (+1 the low side)",
           "outline": pts, "posts": posts, "keys": [1 if p[1] > 0 else -1 for p in posts], "fanned": True}, open(out, "w"))
vis = im.copy(); cv2.polylines(vis, [simple.reshape(-1, 1, 2)], True, (0, 0, 255), 2)
for px, py in posts_px: cv2.circle(vis, (px, py), 6, (0, 200, 0), 2)
cv2.line(vis, tuple(map(int, nut_a)), tuple(map(int, nut_b)), (255, 0, 0), 2)
cv2.imwrite(out.replace(".json", ".png"), vis[740:1320, 200:700])

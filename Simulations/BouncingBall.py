g = 9.80665
deltaT_ms = 10
v_mm_s = 0
s_um = 0

def tick(t):
    global v_mm_s
    global s_um
    s_um += v_mm_s*(t) + 1/2 * g * (t)**2
    v_mm_s += g * (t)

t = 0
while t < 450:
    t += deltaT_ms
    tick(deltaT_ms)
    print(t)
    print(s_um / 1000000)
    print(v_mm_s / 1000)
    print("---")
    

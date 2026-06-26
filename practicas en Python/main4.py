from secrets import (
    randbelow,
    randbits,
)

from Crypto.Util.number import getPrime

p1 = 36457
g1 = 5

p2 = 348261019
g2 = 2

p3 = 66469876885664026651027094563307379287663807348031128100844070269546283933311
g3 = 2

def random_a(g, p) -> None:
    print(f"Mis datos: ")
    
    xA = randbelow(p - 1) + 2
    yA = pow(g, xA, p)

    print(f"xA = {xA}")
    print(f"yA = {yA}")

def main(yb, xa, p) -> None:
    print(f"yA mia = {pow(yb, xa, p)}")

def brute_force_discrete_log(
        base: int,
        target: int,
        modulus: int
) -> int | None:
    solution = None
    
    for x in range(modulus):
        if (base * x) % modulus == target:
            solution = x
            break

    return solution

if __name__ == "__main__":
    #random_a(g3, p3)
    #print(f"\nResultado: ")
    #main(
    #    yb=28661128006258900229952663279682620746694835602493589843615757982302291195969,
    #    xa=7793123566705845086975971845308234916399347786774918963503903761728738814302,
    #    p=p3,
    #)

    xA = brute_force_discrete_log(
        base=2, # g
        target=1693005907646833799, # ya
        modulus=3304471280644310347 # p
    )
    print(f"xA = {xA}")
    xB = brute_force_discrete_log(
        base=2, # g
        target=418496310744558233, # yb
        modulus=3304471280644310347 # p
    )
    print(f"\nxB = {xB}")
    pass
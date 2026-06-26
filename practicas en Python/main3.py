def brute_force_discrete_log(
        base: int,
        target: int,
        modulus: int
) -> int | None:
    for x in range(modulus):
        if pow(base, x, modulus) == target:
            return x
        
    return None

if __name__ == "__main__":
    xA = brute_force_discrete_log(
        base=2, # g
        target=362628610851685233068842194776, # ya
        modulus=1267650600228229401496703205653 # p
    )
    print(f"xA = {xA}")
    xB = brute_force_discrete_log(
        base=2, # g
        target=998637156729697830623291901091, # yb
        modulus=1267650600228229401496703205653 # p
    )
    print(f"\nxB = {xB}")
    pass
def main() -> None:
    binary = input("Ingresa un número binario: ")
    binary_lenght = len(binary) - 1
    exponentes: list[str] = []


    for i in binary:
        if i == '1':
            exponentes.append(f"x^{binary_lenght}")

        binary_lenght -= 1

    print(exponentes)
    print(f"{" + ".join(exponentes)}")
        


if __name__ == "__main__":
    main()
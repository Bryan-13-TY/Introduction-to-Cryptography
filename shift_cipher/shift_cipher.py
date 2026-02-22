def encrypt_shift(
        key: int,
        unencrypted_text: str,
        alphabet: str,
    ) -> str:
    encrypted_text = ""
    m = len(alphabet)
    for letter in unencrypted_text:
        if letter in alphabet:
            residue = (alphabet.find(letter) + key) % m
            encrypted_text += alphabet[residue]
        else:
            encrypted_text += letter
    return encrypted_text


def decrypt_shift(
        key: int,
        encrypted_text: str,
        alphabet: str,
    ) -> str:
    unencrypted_text = ""
    m = len(alphabet)
    for letter in encrypted_text:
        if letter in alphabet:
            residue = (alphabet.find(letter) - key) % m
            unencrypted_text += alphabet[residue]
        else:
            unencrypted_text += letter
    return unencrypted_text

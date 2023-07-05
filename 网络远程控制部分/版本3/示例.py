def encrypt_data(data, key):
    encrypted_data = ""
    for char in data:
        encrypted_char = chr(ord(char) ^ key)
        encrypted_data += encrypted_char
    return encrypted_data


data = "Hello, world!"
key = 42
encrypted_data = encrypt_data(data, key)
print("加密后的数据:", encrypted_data)

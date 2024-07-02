import sys
from PIL import Image
import numpy as np

# pip install pillow

# Per questo programma va bene anche un png. Si può usare affinity designer utilizzando solo bianco o nero (In realtà quando si scala l'immagine a dimensioni piccole
# (il display è 128x64) crea delle zone sfumate. Questo programma permette di impostare una soglia oltre la quale considera il pixel nero perché servono immagini
# ad un bit. Poi codifica l'array come richiesto dalla libreria openGLCD in cui si scende in verticale a considerare otto pixel (che fanno un byte) e poi si considera
# il successivo in orizzontale.
#
# Ipotizzando un a immagine che copre l'instero schermo da 128x64:
#
#    Byte 0   Byte 1   Byte 2        ...         Byte 127
#    |        |        |             ...         |
#    |        |        |             ...         |
#    |        |        |             ...         |
#    |        |        |             ...         |
#    |        |        |             ...         |
#    |        |        |             ...         |
#    |        |        |             ...         |
#    |        |        |             ...         |
#
#    Byte 128    ...
#    |           ...
#    |           ...
#    ...
#
# python cv.py <nome file> <soglia di nero> <invertire>

def convert_image_to_bitmap(image_path, output_path, threshold=200, invert_colors=True):
    # Open the image file
    img = Image.open(image_path).convert('L')  # Convert image to grayscale

    # Apply threshold
    img = img.point(lambda p: 255 if p > threshold else 0)

    # Invert colors if needed
    if invert_colors:
        img = Image.fromarray(255 - np.array(img))

    # Get image dimensions
    width, height = img.size

    # Get image data
    img_data = np.array(img, dtype=np.uint8)

    # Convert image data to bytes (scanning 8 pixels vertically, then moving horizontally)
    byte_array = []
    for y in range(0, height, 8):
        for x in range(width):
            byte = 0
            for bit in range(8):
                if y + bit < height:
                    byte |= (img_data[y + bit, x] // 255) << bit
            byte_array.append(byte)

    # Generate the header file content
    header_content = "GLCDBMAPDECL(bitmap) = {\n"
    header_content += f"    {width}, // width\n"
    header_content += f"    {height}, // height\n"

    line_length = 16

    for i in range(0, len(byte_array), line_length):
        line_data = ', '.join(hex(byte) for byte in byte_array[i:i+line_length])
        header_content += f"    {line_data},\n"

    header_content = header_content.rstrip(",\n") + "\n};\n"  # Remove the last comma and add closing brace

    # Save the header file
    with open(output_path, 'w') as f:
        f.write(header_content)

    print(f'Bitmap header file saved to {output_path}')


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python convert_image.py <image_path> [threshold] [invert]")
        sys.exit(1)

    image_path = sys.argv[1]
    threshold = int(sys.argv[2]) if len(sys.argv) > 2 else 200
    invert_colors = bool(int(sys.argv[3])) if len(sys.argv) > 3 else False
    output_path = 'bitmap.h'

    byte_array_str = convert_image_to_bitmap(image_path, output_path, threshold, invert_colors)

    print(f"Byte array successfully generated and saved to {output_path}")

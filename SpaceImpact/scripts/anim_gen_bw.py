#!/usr/bin/python

import getopt
import numpy as np
import os
import sys

from PIL import Image

allowed_extensions  = ['bmp', 'jpg', 'jpeg', 'png']

def generate_bmp_data(filepath):
    image = Image.open(filepath)
    width, height = image.size

    # Threshold grayscale image by given value
    image = np.array(image)
    image = image[:,:,0:3].mean(axis=2)
    image = 1 - image / (image.max() + 0.01)
    image_bw = image > 0.5;
    
    # Convert image to hex data
    px_count   = 0
    byte_count = 0
    px_str     = f"{'{'}0x{width:02x}, 0x{height:02x}, "
    for byte in range(np.ceil(height / 8).astype(np.int32)): 
        for col in range(width):
            num_bits = min(8, height - 8 * byte)
            for bit in range(num_bits):
                row = 8*byte + bit
                
                # 0s will be lit in the LCD
                px_bw = image_bw[row, col]
                
                # Pack 8 pixel bits into bytes
                if ((px_count % num_bits) == 0):
                    px_write = px_bw * (2 ** (num_bits - 1))
                else:
                    px_write = px_write // 2 + px_bw * (2 ** (num_bits - 1))
                    
                # Update pixel and byte counts
                px_count   += 1
                byte_count = px_count // num_bits
                
                # Add byte to string of hexs after done scanning 8 pixels
                if ((px_count % num_bits) == 0):
                    px_str += f"0x{px_write:02x}, " + ("\n\t" if ((byte_count % 32) == 0) else "")
                
    px_str = px_str[:-2] + "}"
    return px_str, byte_count + 2

def generate_animation_header(dirpath):
    filenames = [f for f in os.listdir(dirpath) if f.endswith(tuple(allowed_extensions))]
    filenames.sort()
    
    dirname = dirpath.replace('\\', '/').split('/')
    dirname = dirname[-1] if len(dirname[-1]) else dirname[-2]
    num_bytes = 10
    
    str_sprite = ""
    str_obj  = ""
    for f in filenames:
        str_hex, count_byte = generate_bmp_data(os.path.join(dirpath, f))
        if (len(str_sprite) == 0):
            str_sprite = f"static const unsigned char Sprite_{dirname}[][{count_byte}] = {'{'}\n\t"
            str_timing = f"static const float Times_{dirname}[] = {'{'}"
        str_sprite += str_hex + ",\n\t"
        str_timing += "1, "
    
    str_obj = (
        f"static Object_t {dirname} = {'{'}\n"
        + "\t0, 0, 1, 1,\n"
        + "\t{\n"
         "\t\t// Idle animation\n"
        + f"\t\t(unsigned char *) Sprite_{dirname},\n"
        + f"\t\t(float *) Times_{dirname}, {count_byte}, {len(filenames)}, 0, 0\n"
        + "\t},\n"
        + "\t{\n"
        + "\t\t// Default death animation\n"
        + f"\t\t(unsigned char *) Sprite_Explosion,\n"
        + f"\t\t(float *) Times_Explosion, 10, 6, 0, 0\n"
        + "\t}\n"
        + "};"
    )
    
    str_file = (
        "// Code to be inserted in Graphics.h\n"
        + str_sprite[:-3] + "\n};\n\n"
        + str_timing[:-2] + "};\n\n\n"
        +"// Code to be inserted in Objects.h\n"
        + str_obj
    )
    
    with open(os.path.join(dirpath, f"{dirname}.h"), "w") as f:
        f.write(str_file)

def usage():
    print(f"USAGE: bmp_gen.py -i <directory>")

def main(argv):
    if (len(argv) < 1 or len(argv) > 2):
        usage()
        sys.exit(1)
    
    inputdir = ''
    try:
        opts, args = getopt.getopt(argv, "hi:",["ifile="])
    except getopt.GetoptError:
        usage()
        sys.exit(2)
        
    if (not opts):
        usage()
        sys.exit(3)
        
    for opt, arg in opts:
        if opt == '-h':
            usage()
            sys.exit(4)
        elif opt in ("-i", "--ifile"):
            inputdir = arg
    
    if (len(inputdir) > 0):
        print(f"Input directory  : {inputdir}")
        if (not os.path.isdir(inputdir)):
            print(f"Error            : Directory {inputdir} does not exist!")
        else:
            generate_animation_header(inputdir)
            print(f"Output file      : {inputdir}.h")
    else:
        usage()
        sys.exit(5)

if __name__ == "__main__":
    main(sys.argv[1:])
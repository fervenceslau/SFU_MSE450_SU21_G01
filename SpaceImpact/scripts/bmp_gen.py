#!/usr/bin/python

import getopt
import numpy as np
import os
import sys

from PIL import Image

def generate_bmp_header(inputfile):
    filepath, _ = os.path.splitext(inputfile)
    filename    = filepath.replace('\\', '/').split('/')[-1]
    
    image = Image.open(inputfile)
    image.load()

    width, height = image.size
    round_width   = np.ceil(width / 2).astype(np.int32) * 2

    def pixel2String(image):
        width, height = image.size
        byte_count = 22
        px_str     = ""
        
        # Loop through pixel positions (bottom left to top right)
        for row in range(height - 1, -1, -1):
            for col in range(np.ceil(width / 8).astype(np.int32) * 8):
                
                # Fit pixels into 4 bytes = 8 nibbles
                if (col < width):
                    px_rgb  = image.getpixel((col, row))
                    px_gray = (255 - np.array(px_rgb).mean().astype(np.uint8)) // 16
                else:
                    px_gray = 0               
                
                # Sample left pixel
                if ((col % 2) == 0): 
                    px_write = px_gray * 16
                    
                # Sample right pixel, then convert both pixels to hex
                else:             
                    byte_count += 1
                    px_write   += px_gray
                    px_str     += f"0x{px_write:02x}, " + ("\n\t" if ((byte_count % 32) == 0) else "")
                        
        px_str += "0xFF\n};"
        return px_str

    with open(f"{filepath}.h", "w") as file:
        file.write(f"const unsigned char {filename}[] = {'{'}\n\t")
        file.write(f"0x42, 0x4D, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x{round_width:02x}, 0x00, 0x00, 0x00, 0x{height:02x}, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00, \n")
        file.write("\t0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80, \n")
        file.write("\t0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, \n")
        file.write("\t0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, ")
        file.write(pixel2String(image))

allowed_extensions  = ['bmp', 'jpg', 'jpeg', 'png']

def usage():
    print(f"USAGE: bmp_gen.py -i <bitmap_file> {allowed_extensions}")

def main(argv):
    if (len(argv) < 1 or len(argv) > 2):
        usage()
        sys.exit(1)
    
    inputfile = ''
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
            inputfile = arg
    
    if (len(inputfile) > 0):
        print(f"Input file  : {inputfile}")
        filepath, extension = os.path.splitext(inputfile)
        if (extension[1:] not in allowed_extensions):
            print(f"Error       : {extension} is not a valid extension {allowed_extensions}!")
        elif (not os.path.isfile(inputfile)):
            print(f"Error       : File {inputfile} does not exist!")
        else:
            generate_bmp_header(inputfile)
            print(f"Output file : {filepath}.h")
    else:
        usage()
        sys.exit(5)

if __name__ == "__main__":
    main(sys.argv[1:])
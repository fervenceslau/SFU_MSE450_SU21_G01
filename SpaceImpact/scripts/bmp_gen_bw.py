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
    width, height = image.size

    # Threshold grayscale image by given value
    image = np.array(image)   
    image = image[:,:,0:3].mean(axis=2)
    image = 1 - image / (image.max() + 0.01)
    image_bw = image > 0.5;
       
    def pixel2String(image):
        px_count   = 0
        byte_count = 2
        px_str     = ""
        
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
                
        px_str = px_str[:-2] + "\n};"
        return px_str

    with open(f"{filepath}.h", "w") as file:
        file.write(f"const unsigned char {filename}[] = {'{'}\n\t")
        file.write(f"0x{width:02x}, 0x{height:02x}, ")
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
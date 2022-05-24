#!/usr/bin/python

import getopt
import matplotlib.pyplot as plt
import numpy as np
import os
import sys

from PIL import Image

def generate_bmp_header(inputfile, threshold, is_optional):
    filepath, _ = os.path.splitext(inputfile)
    filename    = filepath.replace('\\', '/').split('/')[-1]
    
    image = Image.open(inputfile)
    image = image.resize((84, 48))

    # Threshold grayscale image by given value
    image = np.array(image).mean(axis=2)
    image = image / image.max()
    image_bw = image > threshold/100
    
    # Save optional output file
    if (is_optional):
        opt_filepath = f"{filepath}_{threshold:03d}.bmp"
        Image.fromarray(image_bw).save(opt_filepath)
        #opt_image    = Image.fromarray(image_bw)
        #opt_image.save(opt_filepath)
    
    _, axs = plt.subplots(2, 1)
    axs[0].imshow(image, cmap='gray')
    axs[1].imshow(image_bw, cmap='gray')
    plt.show()
       
    def pixel2String(image, threshold):
        width      = 84
        height     = 48
        px_count   = 0
        byte_count = 0
        px_str     = ""
        
        for byte in range(height // 8): 
            for col in range(width):
                for bit in range(8):
                    row = 8*byte + bit
                    
                    # Invert to make 0s lit in the LCD
                    px_bw = 1 - image_bw[row, col]
                    
                    # Pack 8 pixel bits into bytes
                    if ((px_count % 8) == 0):
                        px_write = px_bw * 128
                    else:
                        px_write = px_write // 2 + px_bw * 128
                        
                    # Update pixel and byte counts
                    px_count   += 1
                    byte_count = px_count // 8
                    
                    # Add byte to string of hexs after done scanning 8 pixels
                    if ((px_count % 8) == 0):
                        px_str += f"0x{px_write:02x}, " + ("\n\t" if ((byte_count % 32) == 0) else "")
                
        px_str = px_str[:-2] + "\n};"
        return px_str
    
    with open(f"{filepath}.h", "w") as file:
        file.write(f"const char {filename}[] = {'{'}\n\t")
        file.write(pixel2String(image, threshold))

allowed_extensions  = ['bmp', 'jpg', 'jpeg', 'png']

def usage():
    print(f"USAGE: bmp_gen.py -i <bitmap_file> {allowed_extensions} -t <threshold> [0 - 100] -o [allow optional output file]")

def main(argv):
    if (len(argv) < 1 or len(argv) > 5):
        usage()
        sys.exit(1)
    
    inputfile = ''
    threshold = 50
    is_output = False
    try:
        opts, args = getopt.getopt(argv, "hi:t:o",["ifile=", "threshold=", "output"])
    except getopt.GetoptError:
        print('error')
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
        elif opt in ("-o", "--output"):
            is_output = True
        elif opt in ("-t", "--threshold"):
            value = int(arg)
            if (value > 100 or value < 0):
                print("Threshold outside the valid range (0, 100), using threshold = 50")
            else:
                threshold = int(arg)
        
    if (len(inputfile) > 0):
        print(f"Input file  : {inputfile}, threshold={threshold}")
        filepath, extension = os.path.splitext(inputfile)
        if (extension[1:] not in allowed_extensions):
            print(f"Error       : {extension} is not a valid extension {allowed_extensions}!")
        elif (not os.path.isfile(inputfile)):
            print(f"Error       : File {inputfile} does not exist!")
        else:
            generate_bmp_header(inputfile, threshold, is_output)
            print(f"Output file : {filepath}.h")
    else:
        usage()
        sys.exit(5)

if __name__ == "__main__":
    main(sys.argv[1:])
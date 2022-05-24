# Bitmap Generation Scripts (Python)

The files in this directory are used to facilitate the generation of bitmap codes for the project.

## Small Bitmap Generation

Specify the input image with -i option.  
The output will be a header file with the same name as the input file.

```
python bmp_gen.py -i ../bitmaps/Player/1.png
```

## Small Bitmap Generation (New version, to be used with Nokia5110_PrintBMP_BW() function)

Specify the input image with -i option.  
The output will be a header file with the same name as the input file.
The output will have less bytes than the original bitmap generation and only represent black-white pixels.

```
python bmp_gen_bw.py -i ../bitmaps/Player/1.png
```

## Full Image Generation

Specify the input image with -i option, a threshold (0 to 100) with -t option and optionally add the -o option.
The threshold is used to generate a black-white image using the full rage of the Nokia5110 LCD (84x48).  
The output will be a header file with the same name as the input file.
If the option -o was set, the black-white image is saved with the same name as the input file and threshold used.

```
python full_gen.py -i ../bitmaps/Title/Title_0.jpg -t 30 -o
```

## Animation Code Generation (includes in Graphics.h and Objects.h)

Specify the input folder with -i option.
The output will be a header file with the same name as the input file.
All images within input will be sorted and used to construct the codes.
The codes will follow the the definition used to run Nokia5110_PrintBMP_BW() function.

```
python anim_gen_bw.py -i ../bitmaps/Player
```

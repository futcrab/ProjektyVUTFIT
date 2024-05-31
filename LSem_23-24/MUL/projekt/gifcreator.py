import sys # For parsing arguments
from PIL import Image # For image manipulation and creating gifs
from numpy import zeros, uint8 # For image manipulation
from math import ceil, sqrt
import cv2 # For reading video

# Global variables
inFile = "" # For saving input file path
outFile = "" # For saving output file path
width = 0 # For width of the final gif
height = 0 # For height of the final gif
IorV = 2 # 0=image, 1=video, 2=not set
mode = 0 # 0=iterative, 1=blocking
dither = None # Dither mode None = None, Dither.FLOYDSTEINBERG = dithering is on
gifStart = "" # Start time of gif in video
duration = "" # Duration of gif in video
blocksSize = 0 # For setting size of blocks, blocks will be size blocksSize x blocksSize
fps = 0 # FPS of output GIF

#Function for printing help pannel
def printHelp():
    helpString = '''Gifcreator help pannel
Gifcreator can create true-color gifs from pictures or simple gifs from video. Gifs from video can be dithered or have individual palletes for every frame
    
Example usage:
Values in [] are optional...
Video:
    python[3] gifcreator.py -v -in video.mp4 -out outputGif[.gif] [-s 1:25:46] -dur 1:10 [-fps 10] [-d] [-w 1280] [-h 720]
Image:
    Blocks: python[3] gifcreator.py -i -in image.png -out outputGif[.gif] [-w 1280] [-h 720] -b [32]
    Iterative: python[3] gifcreator.py -i -in image.png -out outputGif[.gif] [-w 1280] [-h 720] [-iter]
If oposite arguments or same type of arguments are used (like -b and -iter) last one is used
Shared arguments:
    (-in or -input) inputFilePath - Path to input file (needs .type at end)
    (-out or -output) outputFilePath - Path to output file (if doesn't end on .gif it will be automatically added at end)
    [(-w or -width) wSize] - output width of gif
    [(-h or -height) hSize] - output height of gif
    
    Gifcreator supports outputs of max size 1920x1920 or other resolutions inside this box
    If only one (-w or -h) is provided, output resolution will be calculated to keep same ratio as input and to fit into box
    If both are provided image can stretch or fold based on given resolution (16:9 could be converted to 4:3 if 400x300 resolution is given)
Image arguments:
    (-i or -image) - sets mode to read image file
    (-b or -blocks) [bSize] - Resulting gif will be created block by block, if not provided bSize default one will be used (16x16 = 256 colors = all fit in pallete)
    [(-iter)] - Resulting gif will fill its colors iteratevally with most used colors added in first frames (This is default mode)
Video arguments:
    [(-s or -start) hh:mm:ss] - start time of gif in video (if not provided starts at beginning)
    (-dur or -duration) hh:mm:ss - duration of gif (ss must be provided, hh:mm is voluntary, hh, mm and ss must be divided by ':')
    [-fps FPS] - Output gif fps, if not given it will use video fps, output fps can be max 30 fps and will be auto adjusted if its more
    [-d or -dither] - Images in gif will be dithered, if not used every image will have its own pallete'''
    print(helpString)
    return False

# Function to automatically set width and height of output image
def setWH(image : Image.Image):
    global width, height

    imageW, imageH = image.size
    ratio = imageW / imageH

    if width == 0 and height == 0: #If width or height was not set by user
        if ratio >= 1.0: # If width is bigger than height
            if imageW > 1920:
                width = 1920
                height = int(width // ratio)
                print("Image resolution changed to", width, "x", height)
            else:
                width = imageW
                height = imageH
        else: # If height is bigger than width
            if imageH > 1920:
                height = 1920
                width = int(height * ratio)
                print("Image resolution changed to", width, "x", height)
            else:
                width = imageW
                height = imageH
    elif width == 0: # If only height was set by user
        if ratio >= 1.0: # If width is bigger than height
            if int(height * ratio) > 1920: # If given height would result in image being outside 1920x1920 box
                width = 1920
                height = int(width // ratio)
                print("Image resolution changed to", width, "x", height)
            else:
                width = int(height * ratio)
                print("Image resolution changed to", width, "x", height)
        else: # If height is bigger than width
            if height > 1920:
                height = 1920
                width = int(height * ratio)
                print("Image resolution changed to", width, "x", height)
            else:
                width = int(height * ratio)
                print("Image resolution changed to", width, "x", height)
    elif height == 0: # If only width was set by user
        if ratio >= 1.0: # If width is bigger than height
            if width > 1920: # If given width would result in image being outside 1920x1920 box
                width = 1920
                height = int(width // ratio)
                print("Image resolution changed to", width, "x", height)
            else:
                height = int(width // ratio)
                print("Image resolution changed to", width, "x", height)
        else: # If height is bigger than width
            if int(width // ratio) > 1920:
                height = 1920
                width = int(height * ratio)
                print("Image resolution changed to", width, "x", height)
            else:
                width = int(height * ratio)
                print("Image resolution changed to", width, "x", height)
    else: # If both height and width was given by user
        if height > 1920: # Set height inside box
            height = 1920
        if width > 1920: # Set width inside box
            width = 1920
        print("Image resolution changed to ", width, "x", height)
        
    return image.resize((width, height))

# Function for parsing HH:MM:SS format of time into number of seconds
def parseTime(timeString, dst):
    global gifStart, duration
    seconds = 0
    minutes = 0
    hours = 0
    
    acc_chars = set("0123456789:")
    validation = set(timeString)
    timeString = timeString.split(':')
    
    # Check if only HH:MM:SS is given and that only numbers and ':' is inside the string
    if len(timeString) >= 4 or (not validation.issubset(acc_chars)):
        return False
    
    # Save seconds, minutes and hours into designated variables
    if len(timeString[-1]) > 0:
        seconds = timeString[-1]
    if len(timeString) >= 2 and len(timeString[-2]) > 0:
        minutes = timeString[-2]
    if len(timeString) >= 3 and len(timeString[-3]) > 0:
        hours = timeString[-3]

    # Based on dst variable save time value
    if dst == 0:
        gifStart = int(seconds) + 60 * int(minutes) + 60*60* int(hours)
    else:
        duration = int(seconds) + 60 * int(minutes) + 60*60* int(hours)
    
    return True

#Function for checking required arguments and if they were inserted correctly
def checkGlobal():
    global inFile, outFile, IorV, mode, blocksSize, gifStart, duration

    if IorV == 0:
        if inFile == "" or outFile == "":
            return printHelp()
        if not outFile.endswith(".gif"):
            outFile = outFile + ".gif"
        if mode == 1:
            if blocksSize < 16:
                print("Blocks automatically adjusted to 16x16 pixels (256 pixels)")
                blocksSize = 16
    elif IorV == 1:
        if inFile == "" or outFile == "":
            return printHelp()
        if not outFile.endswith(".gif"):
            outFile = outFile + ".gif"
        if gifStart == "":
            gifStart = 0
        elif not parseTime(gifStart, 0):
            return printHelp()
        if duration == "":
            return printHelp()
        elif not parseTime(duration, 1):
            return printHelp()
    else:
        return printHelp()
    return True

# Function for parsing input arguments
# Unknown arguments are ignored, multiple arguments of same type can be given - last one will be saved
# If arguments are issued incorrectly program will print out help pannel and end itself
def parseArgs(args):
    global inFile, outFile, width, height, IorV, mode, dither, blocksSize, gifStart, duration, fps
    argc = len(args)
    
    for i, arg in enumerate(args):
        if arg == "-i" or arg == "-image":
            IorV = 0
        elif arg == "-v" or arg == "-video":
            IorV = 1
        elif arg == "-iter":
            mode = 0
        elif arg == "-d" or arg == "-dither":
            dither = Image.Dither.FLOYDSTEINBERG
        elif arg == "-in" or arg == "-input":
            if i+1 < argc:
                inFile = args[i+1]
            else:
                return printHelp()
        elif arg == "-out" or arg == "-output":
            if i+1 < argc:
                outFile = args[i+1]
            else:
                return printHelp()
        elif arg == "-w" or arg == "-width":
            if i+1 < argc:
                width = abs(int(args[i+1]))
            else:
                return printHelp()
        elif arg == "-h" or arg == "-height":
            if i+1 < argc:
                height = abs(int(args[i+1]))
            else:
                return printHelp()
        elif arg == "-b" or arg == "-blocks":
            mode = 1
            if i+1 < argc:
                blocksSize = int(args[i+1])
        elif arg == "-s" or arg == "-start":
            if i+1 < argc:
                gifStart = args[i+1]
            else:
                return printHelp()
        elif arg == "-dur" or arg == "-duration":
            if i+1 < argc:
                duration = args[i+1]
            else:
                return printHelp()
        elif arg == "-h" or arg == "-help":
            return printHelp()
        elif arg == "-fps":
            if i+1 < argc:
                fps = abs(int(args[i+1]))
            else:
                return printHelp()
    return checkGlobal()

# Function for dumping simple video info
def dumpVideoInfo(fps, vHeight, vWidth, bitrate):
    global inFile
    print("Input video info dump:")
    print("Filename:", inFile)
    print("Width:", vWidth)
    print("Height:", vHeight)
    print("FPS: ", round(fps, 3))
    print("Bitrate:", bitrate, "kbits/s")
    print()

def fromVideo():
    global inFile, height, width, fps, gifStart, duration, dither
    
    # It will try to open the video with given inputFile path, if unsuccessfull print error and end
    try:
        cap = cv2.VideoCapture(inFile)
    except:
        print("Error while opening video file - make sure input file is written correctly")
        return
    
    # Get video info from container
    videoFPS = cap.get(cv2.CAP_PROP_FPS)
    videoHeight = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
    videoWidth = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
    frameCount = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
    bitrate = cap.get(cv2.CAP_PROP_BITRATE)
    videoLen = frameCount / videoFPS
    dumpVideoInfo(videoFPS, videoHeight, videoWidth, bitrate)

    # Set output width and height based on video resolution and users input
    setWH(Image.new("L", (videoWidth, videoHeight), 0))

    # Figure out output FPS (It finds how how many frames to skip)
    # Max allowed FPS is 30
    divider = 1
    if (fps == 0 or fps > 30) and videoFPS > 30:
        fps = videoFPS
        while fps > 30:
            divider += 1
            fps = videoFPS / divider
    elif fps > 30 and videoFPS <= 30:
        fps = videoFPS
    elif fps == 0 and videoFPS <= 30:
        fps = videoFPS
    elif fps <= 30:
        divider = round(videoFPS / fps)
        fps = videoFPS / divider
        while fps > 30:
            divider += 1
            fps = videoFPS / divider
    if gifStart + duration > round(videoLen):
        duration = round(videoLen) - gifStart
    
    # Calculate max number of frames possible (When creating gif with pillow all frames needs to be saved in memory)
    # This is calibrated to 16 GB RAM (Not really, just by eye. Probably could work on 8 GB as well)
    nOfFrames = round(duration * fps)
    maxNOfFrames = (1920*1920) / (width * height) * 1250

    # Change output fps to prevent memory problems
    if nOfFrames > maxNOfFrames:
        while nOfFrames > maxNOfFrames:
            divider += 1
            fps = videoFPS / divider
            nOfFrames = round(duration * fps)
        print("Output FPS needed to be changed to prevent memory problems")
    print("Output FPS:", round(fps, 3))

    # Skip forward in video to selected start time
    cap.set(cv2.CAP_PROP_POS_MSEC, gifStart * 1000)
    renderedFrame = 1
    frameCounter = 0
    # Calculate duration between output gif frames
    durationBetweenFrames = (1000 / fps)
    frames = []


    # while cap is opened and not enough frames are added to gif 
    while(cap.isOpened() and renderedFrame - 1 != nOfFrames):
        # Read image from cap, ret variable is false if on end of file
        ret, cv2_image = cap.read()

        # If successfully read image from video and frame is not being skipped
        if ret and frameCounter % divider == 0:
            print("Frame", int(frameCounter / divider) + 1, "of cca.", nOfFrames, "is being loaded")
            # Convert image from BGR to RGB (BGR is used in OpenCV)
            rgb_cv2_image = cv2.cvtColor(cv2_image, cv2.COLOR_BGR2RGB)
            # create and resize image into Pillow
            image = Image.fromarray(rgb_cv2_image).resize((width, height))
            # If user selected dither use Dither.FLOYDSTEINBERG when converting (it is saved in dither variable)
            if dither != None:
                image = image.convert("P", dither=dither)
            else: # Else create new palette based on image
                image = image.convert("P", dither=None, palette=Image.Palette.ADAPTIVE, colors=256)

            frames.append(image)
            renderedFrame += 1
        elif not ret: # If all images from video are read release cap
            cap.release()
        frameCounter += 1

    # If none frames are loaded print error and end
    if len(frames) == 0:
        print("No frames loaded - check if start point is in video")
        return

    print("Saving Gif")
    # Try to save gif onto outputFile, if unsuccessfull print error and end
    try:
        frames[0].save(outFile, format="GIF", save_all=True, append_images=frames[1:], duration=int(durationBetweenFrames), optimize=True, include_color_table=True, loop = 0)
    except:
        print("Error while saving file - make sure output file is written correctly")
        return
    print("Your gif", outFile, "from video", inFile ,"is created !")

# Function to create true-color gif by iterratively adding colors to image with every frame
def imageIter(image : Image.Image):
    pixels = list(image.getdata())
    color_count = {}

    # Create dictionary with every color in image, its positions on image and its count
    print("Sorting colors in image")
    for pos, pixel in enumerate(pixels):
        # White color is used as transparent color
        # Change pure white slightly to see it
        if pixel == (255,255,255):
            color = (255,255,254)
            pixels[pos] = color
        else:
            color = pixel
        if color not in color_count:
            color_count[color] = [1, [pos]]
        else:
            color_count[color][0] += 1
            color_count[color][1].append(pos)

    image = Image.new(image.mode, image.size)
    image.putdata(pixels)

    # Sort colors with most used being first
    sorted_colors = sorted(color_count.items(), key=lambda x: x[1][0], reverse=True)
    nOfColors = len(sorted_colors)
    colorsInPalette = 255
    nOfFrames = nOfColors // colorsInPalette
    maxNOfFrames = (1920*1920) / (width * height) * 1250

    # Calculate max number of frames possible (When creating gif with pillow all frames needs to be saved in memory)
    # This is calibrated to 16 GB RAM (Not really, just by eye. Probably could work on 8 GB as well) 
    if nOfFrames > maxNOfFrames:
        print("Too many colors in picture - possible memory problems, try to lower output resolution")
        return

    frames = []

    for x in range(0, nOfColors, colorsInPalette):
        print("Pallete: " + str(x // colorsInPalette) + " of " + str(nOfFrames))
        paletteColors = sorted_colors[x:x+colorsInPalette]
        
        # Create empty mask
        mask = zeros(width * height, dtype=bool)
        for color in paletteColors:
            # Set true every position in image where color in palette is
            mask[color[1][1]] = True
        # Reshape mask to fit image and change it to uint_8
        mask = mask.reshape((height, width))
        mask = (mask * 255).astype(uint8)

        # Paste from image onto new_image only values that are in mask
        new_image = Image.new("RGB", image.size, (255, 255, 255))
        new_image.paste(image, mask=Image.fromarray(mask))
        # Convert new_image into Palette mode
        new_image = new_image.convert("P", palette=Image.Palette.ADAPTIVE, colors=256)

        frames.append(new_image)
    
    print("Saving Gif")
    # Try to save gif onto outputFile, if unsuccessfull print error and end
    try:
        frames[0].save(outFile, format="GIF", save_all=True, append_images=frames[1:], duration=20, optimize=True, include_color_table=True, transparency=0)
    except:
        print("Error while saving file - make sure output file is written correctly")
        return
    print("Your gif", outFile, "from image", inFile ,"is created !")

# Function to check number of frames based on blockSize and resolution of image
def checkNOfFrames(blocksize):
    global width, height
    vBlocks = ceil(height / blocksize)
    hBlocks = ceil(width / blocksize)
    return vBlocks * hBlocks

# Function for true-color gifs using smaller blocks for every frame
def imageBlock(image : Image.Image):
    global width, height, blocksSize
    
    # White color (255,255,255) is used as transparent color
    # So every pure white is slightly changed
    pixels = list(image.getdata())
    for pos, pixel in enumerate(pixels):
        if pixel == (255,255,255):
            pixels[pos] = (255,255,254)
    image = Image.new(image.mode, image.size)
    image.putdata(pixels)

    # Calculate max number of frames possible (When creating gif with pillow all frames needs to be saved in memory)
    # This is calibrated to 16 GB RAM (Not really, just by eye. Probably could work on 8 GB as well)
    maxNOfFrames = (1920*1920) / (width * height) * 1250
    # Check nOfFrames based on blocksSize
    nOfFrames = checkNOfFrames(blocksSize)
    
    # If its more than maxNOfFrames it will auto adjust
    if nOfFrames > maxNOfFrames:
        blocksSize = ceil(sqrt((width*height)/1500))
        nOfFrames = checkNOfFrames(blocksSize)
        while nOfFrames > maxNOfFrames:
            blocksSize += 1
            nOfFrames = checkNOfFrames(blocksSize)
        print("Block size needed to be changed to prevent memory problems")
        print("New Size:", blocksSize, "x", blocksSize)
    
    vBlocks = ceil(height / blocksSize)
    hBlocks = ceil(width / blocksSize)

    frames = []

    for y in range(vBlocks):
        for x in range(hBlocks):
            print("Block: " + str((y*hBlocks) + x + 1) + " of " + str(hBlocks*vBlocks))

            # Calculates position of block in image
            left = x * blocksSize
            top = y * blocksSize
            right = min(left + blocksSize, width)
            bottom = min(top + blocksSize, height)

            # Crops the image into separate block
            block = image.crop((left, top, right, bottom))

            # Paste the block onto separate image and convert it to Palette mode
            new_image = Image.new("RGB", image.size, (255,255,255))
            new_image.paste(block, (left, top))
            new_image = new_image.convert("P", palette=Image.Palette.ADAPTIVE, colors=256)
            
            frames.append(new_image)

    print("Saving Gif")
    # Try to save gif onto outputFile, if unsuccessfull print error and end
    try:
        frames[0].save(outFile, format="GIF", save_all=True, append_images=frames[1:], duration=20, optimize=True, include_color_table=True, transparency=0)
    except:
        print("Error while saving file - make sure output file is written correctly")
        return
    print("Your gif", outFile, "from image", inFile ,"with blockSize", blocksSize, "x", blocksSize, "is created !")

# Funtion to create true-color gif from image
def fromImage():
    # It will try to open the image with given inputFile path, if unsuccessfull print error and end
    try:
        image = Image.open(inFile).convert("RGB")
    except:
        print("Error while opening image file - make sure input file is written correctly")
        return
    # Sets width and height given by user and resizes image
    image = setWH(image)

    #Based on mode create gif either by iteration or blocks
    if mode == 0:
        imageIter(image)
    else:
        imageBlock(image)

# Main function: if arguments are issued correctly it will start either fromImage or fromVideo
if __name__ == "__main__":
    if(parseArgs(sys.argv)):
        if IorV == 0:
            fromImage()
        else:
            fromVideo()
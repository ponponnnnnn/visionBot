# According to the official manual, INTER_AREA interpolation is the best for shrinking videos.
import sys
import cv2
from os.path import splitext

def cropBorder(srcLength, dstLength):
    center = srcLength//2
    half = divmod(dstLength, 2)
    begin = center - half[0]
    end = center + half[0]
    if half[1]:
        end += 1

    return begin, end

inFile = str( sys.argv[1] )
outFileName = splitext(inFile)[0] + "_norm.avi"
src = cv2.VideoCapture(inFile)
fourcc = cv2.VideoWriter_fourcc(*'MJPG')

fps = int( src.get(cv2.CAP_PROP_FPS) )
srcHW = tuple([int(src.get(cv2.CAP_PROP_FRAME_HEIGHT)), int(src.get(cv2.CAP_PROP_FRAME_WIDTH))])
dstHW = tuple([64, 64])
print("Input file: {} with {} fps, {} pixels high, {} pixels wide".format(inFile, fps, srcHW[0], srcHW[1]))
if srcHW[0] < dstHW[0] or srcHW[1] < dstHW[1]:
    print('\033[93m' + "Warning: Original video resolution is lower than the target resolution." + '\033[0m')
print("Processing...")

if srcHW[0] >= srcHW[1]:
    border = cropBorder(srcHW[0], srcHW[1])
    dst = cv2.VideoWriter(outFileName, fourcc, fps, dstHW)
    ret, frame = src.read()
    while ret:
        cropframe = frame[border[0]:border[1], 0:]
        dst.write(cv2.resize(cropframe, dsize=dstHW, interpolation=cv2.INTER_AREA))
        ret, frame = src.read()

elif srcHW[0] < srcHW[1]:
    border = cropBorder(srcHW[1], srcHW[0])
    dst = cv2.VideoWriter(outFileName, fourcc, fps, dstHW)
    ret, frame = src.read()
    while ret:
        cropframe = frame[0:, border[0]:border[1]]
        dst.write(cv2.resize(cropframe, dsize=dstHW, interpolation=cv2.INTER_AREA))
        ret, frame = src.read()
else:
    dst = cv2.VideoWriter(outFileName, fourcc, fps, dstHW)
    print("Do nothing")

print("Output file: {} with {} fps, {} pixels high, {} pixels wide".format(outFileName, fps, dstHW[0], dstHW[1]))
src.release()
dst.release()

from socket import *
import time
import math
import sys
import pygame
import colorsys
from pygame.locals import *

HEIGHT = 18
WIDTH = 10
PIXEL_SIZE = 50

def xyToIndex(x, y):
    return y * WIDTH + x
    """
    leftToRight = True#y % 2 == 0
    if leftToRight:
        return y * WIDTH + x
    else:
        return y * WIDTH + (WIDTH - x - 1)
    """

message = [0] * 180

#sock = socket(AF_INET, SOCK_DGRAM)
#sock.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
#sock.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)

def plasma(w, h, t):
    hue = 4.0 + math.sin(x / 19.0 + t) + math.sin(y / 9.0 + t) \
	    + math.sin((x + y) / 25.0 + t) + math.sin(math.sqrt(x**2.0 + y**2.0) / 8.0)
    hsv = colorsys.hsv_to_rgb(hue/8.0, 1, 1)
    return tuple([int(round(c * 255.0)) for c in hsv])

screen = pygame.display.set_mode((WIDTH * PIXEL_SIZE, HEIGHT * PIXEL_SIZE))
screen.fill((255, 255, 255))

t = 0.0

def render_message(message):
    for y in range(0, HEIGHT):
        for x in range(0, WIDTH):
            brightness = message[xyToIndex(x, y)]
            color = (brightness, brightness, brightness)
            pygame.draw.rect(screen, color, (x * PIXEL_SIZE, y * PIXEL_SIZE, PIXEL_SIZE, PIXEL_SIZE))

while True:
    '''
    for y in range(0, HEIGHT):
        for x in range(0, WIDTH):
            r, g, b = plasma(x, y, t)
            brightness = (r + g + b) / 3.0
            #if brightness > 128:
                #brightness = 255
            #else:
            #    brightness = 0

            index = xyToIndex(x, y)
            message[index] = int(brightness)
    '''
    #for i in range(0, int(t) % len(message)):
    #    message[i] = 0xff
    for i in range(0, len(message)):
        message[i] = int(t) % 16

    render_message(message)
    pygame.display.flip()

    for events in pygame.event.get():
        if events.type == QUIT:
            sys.exit(0)

    bmessage = bytes(message)
    #print(len(bmessage))
    #sock.sendto(bmessage, ('255.255.255.255', 4210))

    time.sleep(1.0/100.0)
    t += 0.2


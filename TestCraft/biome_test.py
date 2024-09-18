import os
import sys
import numpy as np

import matplotlib
import matplotlib.pyplot as plt

colors = \
    [
        [235, 215, 66], #desert 
        [4, 145, 58],#rainforest
        [178, 214, 92],#shrubland
        [55, 222, 80], #grassland
        [132, 181, 167],#tundra
        [225, 227, 226],#snowland
        [8, 147, 207],#shallow
        [4, 46, 64],#deep
        [255, 0, 0],#none
    ]

sz = 512
def to_np_img(title: str, dtype = int):
    with open(title, 'r') as file:
        strinput = file.read()
        llinput = strinput.split(' ')[:-1] #exclude last space
        mp = np.array([dtype(x) for x in llinput])
        mp = mp.reshape([sz+1, sz+1])

        return mp

def assign_biome_colors(mp):
    img = np.zeros((sz, sz, 3), dtype=int)
    for i in range(sz):
        for j in range(sz):
            img[i,j] = colors[mp[i,j]]

    return img

def test_biome_cat():
    img0_0 = assign_biome_colors(to_np_img("map0_0.txt"))
    img1_0 = assign_biome_colors(to_np_img("map0_4096.txt"))

    img = np.concatenate([img0_0, img1_0], axis = 1)

    plt.imshow(img)
    plt.show()

def test_landscape():
    scimg = to_np_img("sc_landscape_0.txt")
    rghimg = to_np_img("rgh_landscape_0.txt", dtype=float)
    fig, ax = plt.subplots(1, 2)
    ax[0].pcolor(scimg)
    ax[1].pcolor(rghimg)
    plt.show()
test_landscape()
        


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

def to_np_img(title: str, dtype = int, sz : int = 512, pad : int = 2):
    with open(title, 'r') as file:
        strinput = file.read()
        llinput = strinput.split(' ')[:-1] #exclude last space
        mp = np.array([dtype(x) for x in llinput])
        mp = mp.reshape([sz+2*pad, sz+2*pad])

        return mp

def assign_biome_colors(mp, sz:int = 512, pad:int = 2):
    img = np.zeros((sz, sz, 3), dtype=int)
    for i in range(sz):
        for j in range(sz):
            img[i,j] = colors[mp[pad + i, pad +j]]

    return img

check = False
def test_biome_cat():
    sz=512
    map0 = to_np_img(f"biome{sz}_-4096.txt", sz=sz, pad = 2)
    map1 = to_np_img(f"biome{sz}_0.txt", sz=sz, pad = 2)
    img0_0 = assign_biome_colors(map0, sz=sz, pad = 2)
    img1_0 = assign_biome_colors(map1, sz=sz, pad = 2)
    if check:
        for j in range(map0.shape[1]):
            assert map0[-1][j] == map1[0][j], f"value mismatch at column {j}"
    img = np.concatenate([img0_0, img1_0], axis = 1)

    plt.imshow(img)
    plt.show()

def test_landscape():
    scimg = to_np_img("sc_landscape_0.txt", sz = 512, pad = 1)
    rghimg = to_np_img("rgh_landscape_0.txt", dtype=float, pad = 1)
    fig, ax = plt.subplots(1, 2)
    ax[0].pcolor(scimg)
    ax[1].pcolor(rghimg)
    plt.show()

def test_landscape_cat():
    scimg0 = to_np_img("sc_landscape_-4096.txt", sz=512, pad=1)
    scimg1 = to_np_img("sc_landscape_0.txt", sz=512, pad=1)
    
    img = np.concatenate([scimg0, scimg1], axis = 1)
    
    plt.imshow(img)
    plt.colorbar()
    plt.show()


test_landscape_cat()
        


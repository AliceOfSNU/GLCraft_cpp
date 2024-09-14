import numpy as np
import matplotlib
import matplotlib.pyplot as plt
LandscapeL1Map = [['X' for _ in range(9)] for _ in range(9)]
LandscapeL2Map = [['X' for _ in range(17)] for _ in range(17)]
LandscapeL3Map = [['X' for _ in range(33)] for _ in range(33)]
LandscapeL4Map = [['X' for _ in range(65)] for _ in range(65)]
def generate_land_oceans(layer):
    for i in range(9):
        for j in range(9):
            layer[i][j] = 'O' if np.random.rand() > 0.5 else 'L'

def mix(*inputs):
    arr = [*inputs]
    n = len(arr)
    x = int(np.random.rand() * n)
    return arr[x]

def zoom(sz, layer1, layer2):
    for i in range(sz):
        for j in range(sz):
            layer2[2*i][2*j] = layer1[i][j]
            layer2[2*i][2*j+1] = mix(layer1[i][j], layer1[i][j+1])
            layer2[2*i+1][2*j] = mix(layer1[i+1][j], layer1[i][j])
            layer2[2*i+1][2*j+1]= mix(layer1[i][j], layer1[i+1][j], layer1[i][j+1], layer1[i+1][j+1])
        layer2[2*i][2*sz] = layer1[i][sz]
        layer2[2*i+1][2*sz] = mix(layer1[i][sz], layer1[i+1][sz])
    for j in range(sz):
        layer2[2*sz][2*j] = layer1[sz][j]
        layer2[2*sz][2*j+1] = mix(layer1[sz][j], layer1[sz][j+1])
    layer2[2*sz][2*sz] = layer1[sz][sz]
def map2img(sz, layer):
    img = np.zeros((sz, sz, 3))
    colmap = {'O': [0, 0, 220], 'L': [255, 255, 0], 'X': [0, 0, 0]}
    for i in range(sz):
        for j in range(sz):
            img[i][j] = colmap[layer[i][j]]
    plt.imshow(img)
    plt.show()

generate_land_oceans(LandscapeL1Map)
print(LandscapeL1Map)
zoom(8, LandscapeL1Map, LandscapeL2Map)
zoom(16, LandscapeL2Map, LandscapeL3Map)
zoom(32, LandscapeL3Map, LandscapeL4Map)

map2img(65, LandscapeL4Map)
import sys
import os
from PIL import Image

BASE_DIR = '../craft'
filenames = ['dirt_bottom_x64.png', 'dirt_side_x64.png', 'dirt_top_x64.png',
             'sand_x64.png','snowdirt_side_x64.png', 'snow_x64.png', 'granite_x64.png', 'water_x64.png', 'birch_top.png', 'birch_side.png',
              'elm_top.png', 'elm_side.png', 'foilage.png']
filenames = [os.path.join(BASE_DIR, x) for x in filenames]
images = [Image.open(x) for x in filenames]
widths, heights = zip(*(i.size for i in images))
print(widths, heights)

total_height = sum(heights)
max_width = max(widths)

new_im = Image.new('RGBA', (max_width, total_height))

y_offset = 0
for im in images:
  new_im.paste(im, (0, y_offset))
  y_offset += im.size[0]

new_im.save('atlas.png')
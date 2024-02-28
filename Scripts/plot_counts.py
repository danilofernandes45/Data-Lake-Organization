import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

plt.rcParams.update({'font.size': 35, 'lines.markersize': 15, 'lines.linewidth': 3})

ntags_raw = [
                380, 456, 456, 472, 399, 670, 419, 732, 500, 510,
                1092, 1349, 1171, 1304, 1194, 1279, 1195, 1250, 1200, 1210,
                1613, 1851, 1891, 1782, 1920, 1756, 1724, 1941, 1897, 1956
            ]
ntags_pros = [
                107, 188, 143, 88, 120, 211, 116, 179, 169, 112,
                314, 392, 419, 329, 389, 481, 344, 371, 375, 379,
                483, 506, 611, 500, 574, 584, 547, 563, 625, 583
             ]
ncols = [
                462, 603, 569, 683, 984, 1042, 595, 871, 790, 713,
                1934, 2258, 2228, 2461, 2903, 2784, 2456, 2144, 2344, 2187,
                3466, 3628, 3896, 4572, 4303, 4255, 4453, 3613, 4865, 3538
        ]

x = np.arange(1, 31)
plt.bar(x, ncols, color = '#DD2800')
plt.xlabel("Data Lake ID")
plt.ylabel("#Columns")
plt.show()

width = 0.4
# plot data in grouped manner of bar type
plt.bar(x-0.2, ntags_raw, width)
plt.bar(x+0.2, ntags_pros, width)
plt.legend(["Raw", "Normalized"])
plt.xlabel("Data Lake ID")
plt.ylabel("#Tags")
plt.show()
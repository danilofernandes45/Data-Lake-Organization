import pandas as pd
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
from statsmodels.distributions.empirical_distribution import ECDF

plt.rcParams.update({'font.size': 35, 'lines.markersize': 15, 'lines.linewidth': 3})

def plotECDF(data1, data2):
    ecdf = ECDF(data1[0])
    data1 = pd.DataFrame(np.array([ecdf.x, ecdf.y]).T)
    data1 = data1.groupby([0]).max().reset_index()
    data1.iloc[0, 0] = 0

    ecdf = ECDF(data2[0])
    data2 = pd.DataFrame(np.array([ecdf.x, ecdf.y]).T)
    data2 = data2.groupby([0]).max().reset_index()
    data2.iloc[0, 0] = 0

    red = sns.color_palette()[3]
    green = sns.color_palette()[2]

    plt.plot(data1[0], data1[1], '--o', color = green)
    plt.plot(data2[0], data2[1], '-^', color = red)

    # sns.color_palette(["green", "red"])
    # g = sns.lineplot(data=probs, x = 'x', y = 'y', hue = 'approach', legend = False)
    plt.legend(title=None, loc='upper left', labels=['Nargesian', 'SA'])

    plt.xlabel("Time to target (seconds)")
    plt.ylabel("Cumulative probability")
    plt.grid()

    plt.show()


# data1 = pd.read_csv("../Data/Performance/Socrata/tttplot/nargesian-tttplot-100-3.txt", header = None)
# data2 = pd.read_csv("../Data/Performance/Socrata/tttplot/sa-tttplot-100-3.txt", header = None)
# plotECDF(data1, data2)

data1 = pd.read_csv("../Data/Performance/Socrata/tttplot/nargesian-tttplot-100-5.txt", header = None)
data2 = pd.read_csv("../Data/Performance/Socrata/tttplot/sa-tttplot-100-5.txt", header = None)
plotECDF(data1, data2)

data1 = pd.read_csv("../Data/Performance/Socrata/tttplot/nargesian-tttplot-300-6.txt", header = None)
data2 = pd.read_csv("../Data/Performance/Socrata/tttplot/sa-tttplot-300-6.txt", header = None)
plotECDF(data1, data2)

data1 = pd.read_csv("../Data/Performance/Socrata/tttplot/nargesian-tttplot-500-6.txt", header = None)
data2 = pd.read_csv("../Data/Performance/Socrata/tttplot/sa-tttplot-500-6.txt", header = None)
plotECDF(data1, data2)

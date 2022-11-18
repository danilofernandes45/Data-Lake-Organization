import pandas as pd
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
from statsmodels.distributions.empirical_distribution import ECDF

plt.rcParams.update({'font.size': 32})

def plotECDF(data1, data2):
    probs = pd.DataFrame()
    ecdf = ECDF(data1[0])
    ecdf = pd.DataFrame(np.array([ecdf.x, ecdf.y]).T)
    ecdf = ecdf.groupby([0]).max().reset_index()
    ecdf.iloc[0, 0] = 0
    ecdf['approach'] = 'Nargesian'
    probs = pd.concat([probs, ecdf])

    ecdf = ECDF(data2[0])
    ecdf = pd.DataFrame(np.array([ecdf.x, ecdf.y]).T)
    ecdf = ecdf.groupby([0]).max().reset_index()
    ecdf.iloc[0, 0] = 0
    ecdf['approach'] = 'SA'
    probs = pd.concat([probs, ecdf])

    probs = probs.reset_index(drop = True)
    probs.columns = ['x', 'y', 'approach']

    plt.plot(ecdf[0], ecdf[1], 'g--')
    plt.plot(ecdf[0], ecdf[1]-0.1, 'r-')
    # sns.color_palette(["green", "red"])
    # g = sns.lineplot(data=probs, x = 'x', y = 'y', hue = 'approach', legend = False)
    plt.legend(title=None, loc='lower right', labels=['Nargesian', 'SA'])
    plt.show()


data1 = pd.read_csv("../Data/Performance/Socrata/tttplot/nargesian-tttplot-100-3.txt", header = None)
data2 = pd.read_csv("../Data/Performance/Socrata/tttplot/sa-tttplot-100-3.txt", header = None)
plotECDF(data1, data2)
# ecdf = ECDF(data[0])
# plt.plot(ecdf.x, ecdf.y, '.:g') # --, -.
# plt.show()

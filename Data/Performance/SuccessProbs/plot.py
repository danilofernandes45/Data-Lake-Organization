import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd

def read(file):
    lines = open(file).readlines()
    probs = lines[1].replace(' \n', '')
    data = np.array(probs.split(" ")).astype('float64')
    return data

sa_50 = pd.DataFrame(read('sa_tg_50'), columns = ['Success Probability'])
sa_50['Desc'] = "Our proposal"
num_tables = sa_50.shape[0]
sa_50['Table ID'] = np.arange(1, num_tables + 1)

narge_50 = pd.DataFrame(read('nargesian_tg_50'), columns = ['Success Probability'])
narge_50['Desc'] = "State-of-art"
narge_50['Table ID'] = np.arange(1, num_tables + 1)

data = pd.concat([sa_50, narge_50]).reset_index(drop = True)
g = sns.lineplot(data=data, x = 'Table ID', y = 'Success Probability', hue = 'Desc')
g.legend_.set_title(None)

plt.show()

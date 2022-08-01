import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd

def read(file):
    lines = open(file).readlines()
    probs = lines[1].replace(' \n', '')
    data = np.array(probs.split(" ")).astype('float64')
    return data

sa = pd.DataFrame(read('sa_tg_150'), columns = ['Success Probability'])
sa['Desc'] = "Our proposal"
num_tables = sa.shape[0]
sa['Table ID'] = np.arange(1, num_tables + 1)

narge = pd.DataFrame(read('nargesian_tg_150'), columns = ['Success Probability'])
narge['Desc'] = "State-of-art"
narge['Table ID'] = np.arange(1, num_tables + 1)

data = pd.concat([sa, narge]).reset_index(drop = True)
g = sns.lineplot(data=data, x = 'Table ID', y = 'Success Probability', hue = 'Desc')
g.legend_.set_title(None)

plt.show()

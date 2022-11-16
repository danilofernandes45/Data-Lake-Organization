import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd

plt.rcParams.update({'font.size': 32})

def plotSuccessProbs(data):
    success_probs = pd.DataFrame()

    for i in range(data.shape[0]):
        row = data.iloc[i]
        probs = np.array(row['SuccessProbs'].split(' '), dtype = 'float')
        probs = pd.DataFrame(probs, columns = ['Success Probability'])
        probs['Desc'] = row['Approach']
        probs['Table ID'] = np.arange(1, probs.shape[0] + 1)
        success_probs = pd.concat([success_probs, probs])

    success_probs = success_probs.reset_index(drop = True)

    g = sns.lineplot(data=success_probs, x = 'Table ID', y = 'Success Probability', hue = 'Desc')
    g.legend_.set_title(None)
    plt.show()

data = pd.read_csv('../Data/Performance/Socrata/SuccessProbs/success-100-7.csv', sep = '|')
plotSuccessProbs(data)

data = pd.read_csv('../Data/Performance/Socrata/SuccessProbs/success-300-1.csv', sep = '|')
plotSuccessProbs(data)

data = pd.read_csv('../Data/Performance/Socrata/SuccessProbs/success-300-6.csv', sep = '|')
plotSuccessProbs(data)

data = pd.read_csv('../Data/Performance/Socrata/SuccessProbs/success-500-6.csv', sep = '|')
plotSuccessProbs(data)

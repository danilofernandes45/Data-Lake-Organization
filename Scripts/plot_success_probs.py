import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd

plt.rcParams.update({'font.size': 35, 'lines.markersize': 15, 'lines.linewidth': 3})

def plotSuccessProbs(data, size):
    success_probs = pd.DataFrame()
    ids = np.arange(1, size + 1)
    marker_pos = np.arange(0, size, size // 10) + (size // 10)    
    line_type = [':s', '-.P' ,'--o', '-^']
    markers = ['s', 'P' ,'o', '^']

    for i in range(data.shape[0]):
        row = data.iloc[i]
        probs = np.array(row['SuccessProbs'].split(' '), dtype = 'float')
        plt.plot(ids, probs, line_type[i], markersize = 0.1)
        plt.scatter(marker_pos, probs[marker_pos-1], marker = markers[i])

    plt.legend(title=None, loc='upper left', labels=data['Approach'], markerscale = 150)
    plt.xlabel("Table ID")
    plt.ylabel("Success Probability")
    plt.grid()
    plt.show()

def find_best_plot(size):
    best_mean_diff = [0, 0]
    best_id = [-1, -1]

    for i in range(10):
        try:
            data = pd.read_csv('../Data/Performance/Socrata/SuccessProbs/success-'+str(size)+'-'+str(i)+'.csv', sep = '|')
            probs_narge = np.array(data.iloc[2]['SuccessProbs'].split(' '), dtype = 'float')
            probs_sa = np.array(data.iloc[3]['SuccessProbs'].split(' '), dtype = 'float')
            mean_diff = np.sum(probs_sa - probs_narge) / probs_sa.shape[0]

            if mean_diff > best_mean_diff[0] :
                best_mean_diff[1] = best_mean_diff[0]
                best_mean_diff[0] = mean_diff
                best_id[1] = best_id[0]
                best_id[0] = i
            elif mean_diff > best_mean_diff[1] :
                best_mean_diff[1] = mean_diff
                best_id[1] = i

        except:
            pass

    return best_id, best_mean_diff


best_id, best_mean_diff = find_best_plot(100)

print("Best mean diffs")
print(best_mean_diff)
print("Best IDs")
print(best_id)

data = pd.read_csv('../Data/Performance/Socrata/SuccessProbs/success-100-'+str(best_id[0])+'.csv', sep = '|')
plotSuccessProbs(data, 100)
data = pd.read_csv('../Data/Performance/Socrata/SuccessProbs/success-100-'+str(best_id[1])+'.csv', sep = '|')
plotSuccessProbs(data, 100)

best_id, best_mean_diff = find_best_plot(300)

print("Best mean diffs")
print(best_mean_diff)
print("Best IDs")
print(best_id)

# data = pd.read_csv('../Data/Performance/Socrata/SuccessProbs/success-100-7.csv', sep = '|')
# plotSuccessProbs(data, 100)

# data = pd.read_csv('../Data/Performance/Socrata/SuccessProbs/success-300-1.csv', sep = '|')
# plotSuccessProbs(data, 300)

# data = pd.read_csv('../Data/Performance/Socrata/SuccessProbs/success-300-2.csv', sep = '|')
# plotSuccessProbs(data, 300)

# data = pd.read_csv('../Data/Performance/Socrata/SuccessProbs/success-500-6.csv', sep = '|')
# plotSuccessProbs(data, 500)

# data = pd.read_csv('../Data/Performance/Socrata/SuccessProbs/success-500-10.csv', sep = '|')
# plotSuccessProbs(data, 500)
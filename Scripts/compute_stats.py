import pandas as pd
import numpy as np

for dl in [100, 300, 500]:
    for i in range(1, 11):
        org = pd.read_csv(f"Data/Performance/Socrata/Nargesian/{dl}/perform-{dl}-{i}.csv", header = None)
        sa = pd.read_csv(f"Data/Performance/Socrata/SA/{dl}/perform-{dl}-{i}.csv", header = None)
        fastsa = pd.read_csv(f"Data/Performance/Socrata/SA/{dl}/perform-{dl}-{i}-2.csv", header = None)
        print("Socrata-$%d$-$%d$ & $%.4f$ & $%.4f$ & $%.4f$ & $%.4f$ & $%.4f$ & $%.4f$ \\\\"%(dl, i, org[1].mean(), np.sqrt(org[1].var()), sa[1].mean(), np.sqrt(sa[1].var()), fastsa[1].mean(), np.sqrt(fastsa[1].var())))

    print('\n')
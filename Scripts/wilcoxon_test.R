library(readr)
library(BSDA)

best_ng <- c()
best_sa <- c()

for (i in c(100, 300, 500)){
    for (j in 1:10) {
        data_ng = read_csv(paste('../Data/Performance/Socrata/Nargesian/', toString(i), '/perform-', toString(i), '-', toString(j), '.csv', sep = ''), col_names = FALSE, show_col_types = FALSE)
        data_sa = read_csv(paste('../Data/Performance/Socrata/SA/', toString(i), '/perform-', toString(i), '-', toString(j), '.csv', sep = ''), col_names = FALSE, show_col_types = FALSE)

        best_ng <- append(best_ng, max(data_ng[2]))
        best_sa <- append(best_sa, max(data_sa[2]))
    }
}

print(wilcox.test(x = best_sa, y = best_ng, alternative = 'greater'))
print(SIGN.test(x = best_sa, y = best_ng, alternative = 'greater'))
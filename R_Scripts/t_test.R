library(readr)

for (i in c(100, 300, 500)){
    for (j in 1:10) {
        print(paste('Socrata-', toString(i), '-', toString(j), '\n', sep = ''))
        data_ng = read_csv(paste('../Data/Performance/Socrata/Nargesian/', toString(i), '/perform-', toString(i), '-', toString(j), '.csv', sep = ''), col_names = FALSE, show_col_types = FALSE)
        data_sa = read_csv(paste('../Data/Performance/Socrata/SA/', toString(i), '/perform-', toString(i), '-', toString(j), '.csv', sep = ''), col_names = FALSE, show_col_types = FALSE)
        print(t.test(x = data_sa[2], y = data_ng[2], alternative = 'greater'))
    }
}
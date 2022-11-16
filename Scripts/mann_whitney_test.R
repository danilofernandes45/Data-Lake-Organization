for (i in c(100, 300, 500)){
    for (j in 1:10) {
        print(paste('Socrata-', toString(i), '-', toString(j), '\n', sep = ''))
        data_ng = read.csv(paste('../Data/Performance/Socrata/Nargesian/', toString(i), '/perform-', toString(i), '-', toString(j), '.csv', sep = ''), header = FALSE)
        data_sa = read.csv(paste('../Data/Performance/Socrata/SA/', toString(i), '/perform-', toString(i), '-', toString(j), '.csv', sep = ''), header = FALSE)
        print(wilcox.test(x = data_sa$V2, y = data_ng$V2, paired = FALSE, alternative = 'greater'))
    }
}

#!/usr/bin/env Rscript
##      ---------------------------------------------------------------
##
##      tttplots: A R program for generating of time-to-target
##                plot of one/two/three algorithm(s).
##
##      usage: ./tttplots.R -a <input-file1> -b <input-file2>
##                -c <input-file3>
##
##             where <input-file> is the input file of time to
##                   target values (one per line).
##
##
##      author: Rian Gabriel S. Pinheiro <rian.gabriel@gmail.com>.
##
##      date: last modified at July 19, 2016.
##
##
##      note: Program tttplots.R requires that optparse package
##              be installed on your R system.
##      ---------------------------------------------------------------


library("optparse")

#Program options
option_list = list(
  make_option(c("-a", "--file1"), type="character", default=NULL,
              help="dataset file name", metavar="character"),
  make_option(c("-b", "--file2"), type="character", default=NULL,
              help="dataset file name", metavar="character"),
  make_option(c("-c", "--file3"), type="character", default=NULL,
              help="dataset file name", metavar="character"),
  make_option(c("-A", "--name1"), type="character", default="Alg1",
              help="algorithm1 name", metavar="character"),
  make_option(c("-B", "--name2"), type="character", default="Alg2",
              help="algorithm2 name", metavar="character"),
  make_option(c("-C", "--name3"), type="character", default="Alg3",
              help="algorithm3 name", metavar="character"),
  make_option(c("-m", "--main"), type="character", default=NA,
              help=" an overall title for the plot", metavar="character"),
  make_option(c("-o", "--output"), type="character", default="ttt.ps",
              help="output file name", metavar="character"),
  make_option(c("-x", "--xlegend"), type="character", default="time to target solution value (seconds)",
              help=" a title for the x axis", metavar="character"),
  make_option(c("-y", "--ylegend"), type="character", default="cumulative probability",
              help=" a title for the y axis", metavar="character"),
  make_option(c("-e", "--extension"), type="character", default="ps",
              help="extension file [ps, tikz, png, no (just plot)]"),
  make_option(c("-T", "--theoretical"), action="store_true", default=FALSE,
              help="plot the theoretical (estimative) distribution curve"),
  make_option(c("-p", "--nocolor"), action="store_true", default=FALSE,
              help="plots a black and white graph"),
  make_option(c("-t", "--type"), type="double", default=1,
              help=" typo of plot, 1 = ecdf plot, 2 = interpolated plot")
);

#Initializing the parser
opt_parser = OptionParser(option_list=option_list);
opt = parse_args(opt_parser);


if(is.null(opt$file1))
{
  print_help(opt_parser)
  stop("At least one argument must be supplied (input file1).\n", call.=FALSE)
}


#Legend and colors options
if(opt$nocolor)
{
  color1 <- color2 <- color3 <- "black"
}else
{
  color1 <- rgb(1,0,0)
  color2 <- rgb(0,1,0)
  color3 <- rgb(0,0,1)
}

#Point symbols
pch1 <- 1
pch2 <- 3
pch3 <- 8

#Lines types
lty1 <- 1
lty2 <- 3
lty3 <- 5

#Titles (main, x axis and y axis)
mainlegend <- opt$main
xlab <- opt$xlegend
ylab <- opt$ylegend

#Name of output file
output <- opt$output

#Reading algorithm1 data
alg1 = read.table(opt$file1)
alg1 = alg1$V1
alg1_ecdf = ecdf(alg1)
m = max(alg1)
names =  c(opt$name1)
colors = c(color1)
pch = c(pch1)
lty = c(lty1)

#Reading algorithm2 data
if(!is.null(opt$file2))
{
  alg2 = read.table(opt$file2)
  alg2 = alg2$V1
  alg2_ecdf = ecdf(alg2)
  m = max(m,alg2)
  names =  c(opt$name1,opt$name2)
  colors = c(color1, color2)
  pch = c(pch1,pch2)
  lty = c(lty1,lty2)
}

#Reading algorithm3 data
if(!is.null(opt$file3))
{
  alg3 = read.table(opt$file3)
  alg3 = alg3$V1
  alg3_ecdf = ecdf(alg3)
  m = max(m,alg3)
  names =  c(opt$name1,opt$name2,opt$name3)
  colors = c(color1, color2, color3)
  pch = c(pch1,pch2,pch3)
  lty = c(lty1,lty2,lty3)
}



# Create a single chart with all 3 CDF plots.
if(opt$extension == "ps")
{
  postscript(output)
}else if(opt$extension == "tikz")
{
  tikz(output)
}else if(opt$extension == "png")
{
  png(output)
}else
{
  X11()
}

#Plot type 1 graph
if(opt$type == 1)
{
  plot(alg1_ecdf, verticals=TRUE, col=color1, xlim=c(0,m), main=mainlegend, xlab=xlab, ylab=ylab, pch=pch1, lty=lty1)

  if (!is.null(opt$file2))
    plot(alg2_ecdf, verticals=TRUE, col=color2, add=T, pch=pch2, lty=lty2)

 if (!is.null(opt$file3))
    plot(alg3_ecdf, verticals=TRUE, col=color3, add=T, pch=pch3, lty=lty3)
}

#Plot type 2 graph
if(opt$type == 2)
{
  alg1_sor = unique(sort(alg1))
  plot(alg1_sor, alg1_ecdf(alg1_sor),type="o", cex = 1, col=color1, xlim=c(0,m), main=mainlegend, xlab=xlab, ylab=ylab, pch=pch1, lty=lty1)
  abline(h = 1,  col="gray70",lty = 2)
  abline(h = 0,  col="gray70",lty = 2)

  if(!is.null(opt$file2))
  {
    alg2_sor = unique(sort(alg2))
    lines(alg2_sor, alg2_ecdf(alg2_sor),type="o", cex = 1, col=color2, pch=pch2, lty=lty2)
  }

  if(!is.null(opt$file3))
  {
    alg3_sor = unique(sort(alg3))
    lines(alg3_sor, alg3_ecdf(alg3_sor),type="o", cex = 1, col=color3, pch=pch3, lty=lty3)
  }
}

# Add a legend to the chart.
legend('right', names, col=colors, border=NA, pch=pch, lty=lty)


if(opt$theoretical)
{
  require(MASS)
  fit <- fitdistr(alg1, "exponential")
  curve(pexp(x, rate = fit$estimate), col = color1, lty=lty1, add = TRUE)
  if(exists("alg2"))
  {
    fit <- fitdistr(alg2, "exponential")
    curve(pexp(x, rate = fit$estimate), col = color2, lty=lty2, add = TRUE)
  }
  if(exists("alg3"))
  {
    fit <- fitdistr(alg3, "exponential")
    curve(pexp(x, rate = fit$estimate), col = color3, lty=lty3, add = TRUE)
  }
}



#Close device
if(opt$extension == "ps"||  opt$extension == "tikz" ||  opt$extension == "png")
{
  dev.off()
}else
{
  message("Press Return To Continue")
  invisible(readLines("stdin", n=1))
}

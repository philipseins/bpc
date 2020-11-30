#!/bin/bash

tb_set="
400.perlbench
401.bzip2 
403.gcc 
410.bwaves
429.mcf
433.milc
435.gromacs
436.cactusADM
437.leslie3d
444.namd
445.gobmk
447.dealII
450.soplex
453.povray
454.calculix
456.hmmer
458.sjeng
459.GemsFDTD
462.libquantum
464.h264ref
465.tonto
470.lbm
471.omnetpp
473.astar
481.wrf
482.sphinx3
483.xalancbmk
999.specrand
"
for tb in $tb_set
do
    echo $tb
   # ./predictor /home/suchao/spec_trace/$tb/alpha-$tb-input0-201117-231907/runscript.log generated 1 1 > $tb.txt
   ./predictor /home/suchao/spec_trace/$tb/alpha-$tb-input0-201117-231907/runscript.log generated 1 2 >> $tb.txt
   # ./predictor /home/suchao/spec_trace/$tb/alpha-$tb-input0-201117-231907/runscript.log generated 1 4 >> $tb.txt
   # ./predictor /home/suchao/spec_trace/$tb/alpha-$tb-input0-201117-231907/runscript.log generated 1 0 >> $tb.txt
done


#### 400.perlbench
#./predictor /home/suchao/spec_trace/400.perlbench/alpha-400.perlbench-input0-201117-231907/runscript.log generated 0 1 > perlbench.txt
#./predictor /home/suchao/spec_trace/400.perlbench/alpha-400.perlbench-input0-201117-231907/runscript.log generated 0 2 >> perlbench.txt
#./predictor /home/suchao/spec_trace/400.perlbench/alpha-400.perlbench-input0-201117-231907/runscript.log generated 0 4 >> perlbench.txt
#./predictor /home/suchao/spec_trace/400.perlbench/alpha-400.perlbench-input0-201117-231907/runscript.log generated 0 0 >> perlbench.txt
#
#### bzip2
#./predictor /home/suchao/spec_trace/401.bzip2/alpha-401.bzip2-input0-201117-231907/runscript.log generated 0 1 > bzip2.txt
#./predictor /home/suchao/spec_trace/401.bzip2/alpha-401.bzip2-input0-201117-231907/runscript.log generated 0 2 >> bzip2.txt
#./predictor /home/suchao/spec_trace/401.bzip2/alpha-401.bzip2-input0-201117-231907/runscript.log generated 0 4 >> bzip2.txt
#./predictor /home/suchao/spec_trace/401.bzip2/alpha-401.bzip2-input0-201117-231907/runscript.log generated 0 0 >> bzip2.txt
#
#### gcc
#./predictor /home/suchao/spec_trace/403.gcc/alpha-403.gcc-input0-201117-231907/runscript.log generated 0 1 > gcc.txt
#./predictor /home/suchao/spec_trace/403.gcc/alpha-403.gcc-input0-201117-231907/runscript.log generated 0 2 >> gcc.txt
#./predictor /home/suchao/spec_trace/403.gcc/alpha-403.gcc-input0-201117-231907/runscript.log generated 0 4 >> gcc.txt
#./predictor /home/suchao/spec_trace/403.gcc/alpha-403.gcc-input0-201117-231907/runscript.log generated 0 0 >> gcc.txt
#
#### bwaves
#./predictor /home/suchao/spec_trace/410.bwaves/alpha-410.bwaves-input0-201117-231907/runscript.log generated 0 1 > bwaves.txt
#./predictor /home/suchao/spec_trace/410.bwaves/alpha-410.bwaves-input0-201117-231907/runscript.log generated 0 2 >> bwaves.txt
#./predictor /home/suchao/spec_trace/410.bwaves/alpha-410.bwaves-input0-201117-231907/runscript.log generated 0 4 >> bwaves.txt
#./predictor /home/suchao/spec_trace/410.bwaves/alpha-410.bwaves-input0-201117-231907/runscript.log generated 0 0 >> bwaves.txt
#
#### mcf
#./predictor /home/suchao/spec_trace/429.mcf/alpha-429.mcf-input0-201117-231907/runscript.log generated 0 1 > mcf.txt
#./predictor /home/suchao/spec_trace/429.mcf/alpha-429.mcf-input0-201117-231907/runscript.log generated 0 2 >> mcf.txt
#./predictor /home/suchao/spec_trace/429.mcf/alpha-429.mcf-input0-201117-231907/runscript.log generated 0 4 >> mcf.txt
#./predictor /home/suchao/spec_trace/429.mcf/alpha-429.mcf-input0-201117-231907/runscript.log generated 0 0 >> mcf.txt
#
#### milc
#./predictor /home/suchao/spec_trace/433.milc/alpha-433.milc-input0-201117-231907/runscript.log generated 0 1 > milc.txt
#./predictor /home/suchao/spec_trace/433.milc/alpha-433.milc-input0-201117-231907/runscript.log generated 0 2 >> milc.txt
#./predictor /home/suchao/spec_trace/433.milc/alpha-433.milc-input0-201117-231907/runscript.log generated 0 4 >> milc.txt
#./predictor /home/suchao/spec_trace/433.milc/alpha-433.milc-input0-201117-231907/runscript.log generated 0 0 >> milc.txt
#
#### gromacs
#./predictor /home/suchao/spec_trace/435.gromacs/alpha-435.gromacs-input0-201117-231907/runscript.log generated 0 1 > gromacs.txt
#./predictor /home/suchao/spec_trace/435.gromacs/alpha-435.gromacs-input0-201117-231907/runscript.log generated 0 2 >> gromacs.txt
#./predictor /home/suchao/spec_trace/435.gromacs/alpha-435.gromacs-input0-201117-231907/runscript.log generated 0 4 >> gromacs.txt
#./predictor /home/suchao/spec_trace/435.gromacs/alpha-435.gromacs-input0-201117-231907/runscript.log generated 0 0 >> gromacs.txt
#
#### cactusADM
#./predictor /home/suchao/spec_trace/436.cactusADM/alpha-436.cactusADM-input0-201117-231907/runscript.log generated 0 1 > cactusADM.txt
#./predictor /home/suchao/spec_trace/436.cactusADM/alpha-436.cactusADM-input0-201117-231907/runscript.log generated 0 2 >> cactusADM.txt
#./predictor /home/suchao/spec_trace/436.cactusADM/alpha-436.cactusADM-input0-201117-231907/runscript.log generated 0 4 >> cactusADM.txt
#./predictor /home/suchao/spec_trace/436.cactusADM/alpha-436.cactusADM-input0-201117-231907/runscript.log generated 0 0 >> cactusADM.txt
#
#### leslie3d
#./predictor /home/suchao/spec_trace/437.leslie3d/alpha-437.leslie3d-input0-201117-231907/runscript.log generated 0 1 > leslie3d.txt
#./predictor /home/suchao/spec_trace/437.leslie3d/alpha-437.leslie3d-input0-201117-231907/runscript.log generated 0 2 >> leslie3d.txt
#./predictor /home/suchao/spec_trace/437.leslie3d/alpha-437.leslie3d-input0-201117-231907/runscript.log generated 0 4 >> leslie3d.txt
#./predictor /home/suchao/spec_trace/437.leslie3d/alpha-437.leslie3d-input0-201117-231907/runscript.log generated 0 0 >> leslie3d.txt
#
#### namd
#./predictor 

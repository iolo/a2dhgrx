#!/bin/bash
DIMG="demo.po"
BAS_SRC=(demo demoamp slideshow)
C_SRC=(dhgrc democ)
ASM_SRC=(dhgrx dhgrxamp)

build_bas() {
	echo "Building $1.bas..."
	a2kit verify -t atxt < $1.bas
	a2kit delete -d ${DIMG} -f $1
	a2kit tokenize -t atxt -a 2049 < $1.bas | a2kit put -d ${DIMG} -t atok -f $1
}

build_c() {
	echo "Building $1.cc65..."
	cl65 -Oirs -t apple2enh -C apple2-hgr.cfg -l $1.lst -m $1.map -o $1 $1.c
	a2kit delete -d ${DIMG} -f $1
	a2kit put -d ${DIMG} -f $1 -t auto < $1
}

build_asm() {
	echo "Building $1.s..."
  cl65 -t apple2enh -C a2bin.cfg -l $1.lst -m $1.map -o $1 $1.s
	a2kit delete -d ${DIMG} -f $1
	a2kit put -d ${DIMG} -f $1 -t bin -a 6000 < $1
}

for f in "${BAS_SRC[@]}"; do
	build_bas ${f}
done


for f in "${C_SRC[@]}"; do
	build_c ${f}
done

for f in "${ASM_SRC[@]}"; do
	build_asm ${f}
done

a2kit catalog -d $DIMG
#sa2 --d1 $DIMG

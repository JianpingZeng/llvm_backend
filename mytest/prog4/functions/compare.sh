#!/bin/bash -x

iter=1

while [ $iter -lt 21 ]; do
	meld corei7_$iter cse502_$iter
	let iter=iter+1
done

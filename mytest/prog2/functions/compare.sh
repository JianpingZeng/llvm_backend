#!/bin/bash -x

iter=1

while [ $iter -lt 19 ]; do
	meld corei7$iter cse502$iter
	let iter=iter+1
done

#!/bin/bash
./find_non_ready_nodes.sh
./all_node_idgen.sh
./all_node_idcopy.sh
cat not_ready >> ready_nodes
if [ -f nodes ]
then
	rm nodes
fi

touch nodes
for machine in `cat pssh_nodes`
do
	scp -i ~/.ssh/id_rsa -o StrictHostKeyChecking=no ../ihostlist ../imonitorlist ../agent ../client ../config uwaterloo_pweb@$machine:~/
	echo "$machine 20000" >> nodes
done


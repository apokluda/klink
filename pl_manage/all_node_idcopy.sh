if [ -d pssh_keys ]
then
	rm -r pssh_keys
fi

parallel-ssh -h pssh_nodes -x "-l uwaterloo_pweb" -x "-i ~/.ssh/id_rsa" -o pssh_keys/ "cat ~/.ssh/id_rsa.pub"
cat pssh_keys/* > pssh_keys/keys

for i in 101 102 104 105 106 107 108 109 110
do
	if [ $i -eq 103 ]
	then
		continue
	fi
	host=cn$i.cs.uwaterloo.ca
	cat pssh_keys/keys | ssh pweb@$host "cat >> ~/.ssh/authorized_keys"
done
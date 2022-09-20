set -e

g++ -O2 A.cpp -o A
for i in $(seq 0 99)
do
  t=$(printf %04d $i)
  echo $t
  ./A < tools/in/${t}.txt > out/${t}.txt
done

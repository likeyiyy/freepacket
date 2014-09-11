files=`cat makeinfo | grep "unused" | awk -F ":" '{print $1}'`
lines=`cat makeinfo | grep "unused" | awk -F ":" '{print $2}'`
echo ${files}
echo $lines

Ai=1
for file in $files
do
line=`cat makeinfo | grep "unused" | awk -F ":" -v count="$Ai" '{if(NR == count)print $2}'`
Ai=`expr $Ai + 1`
echo $file
echo $line
spar=""$line"s/^/\/\//"
sed -i "$spar" $file
done

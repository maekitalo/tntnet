DEMOS=../../../cxxtools/demo

for a in $DEMOS/*.cpp
do
  F=`basename $a`

  (
    echo "/** \page cxxtools_$F $F

@htmlinclude sidebar.html

\htmlonly
    <div id="content">

<h2>Example: $F application</h2>
"

    sed '0,/\*\//d' <$a|source-highlight -s cpp -o STDOUT

    echo "

</div>

\endhtmlonly

*/"
  ) > cxxtools_$F.txt

done

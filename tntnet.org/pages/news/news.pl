#!/usr/bin/perl -w

=head1 NAME

=head1 SYNOPSIS

=head1 DESCRIPTION

=head1 OPTIONS

=head1 AUTHOR

Tommi ME<auml>kitalo, Tntnet.org

=cut

use strict;

local $/ = undef;

my $news = <>;
my %news;

while ($news =~ s/<h2>(.*?)<\/h2>(\s*<p>\s*((\d\d\d\d)-(\d\d)-(\d\d))\s*<\/p>)?\s*(.*?)(<h2>|$)/$8/s)
{
  my ($title, $date, $date_yyyy, $date_mm, $date_dd, $content) = ($1, $3, $4, $5, $6, $7);
  my $scontent = $content;
  $scontent = substr($content, 0, 20) . "..." if length($content) > 20;
  $scontent =~ s/\n/./g;
  $content =~ s/\.hms/.html/g;
  $news{$date} = [$title, $date_yyyy, $date_mm, $date_dd, $content];
  print "date:<$date>\ttitle:<$title>\tcontent:<$scontent>\n";
}

open M, ">main.txt" or die "cannot open main.txt: $!";
print M <<'EOF';
        <table class="news-table">
            <tr>
                <th>Date</th>
                <th>Headline</th>
            </tr>
EOF

open O, ">news.txt" or die "cannot open news.txt: $!";
print O <<'EOF';
/** \page news News

@htmlinclude sidebar.html

\htmlonly

<div id="content">
EOF

my @months = ('January', 'February', 'March', 'April', 'May', 'June', 'July', 'August', 'September', 'October', 'November', 'December');
my @monthsShort = ('Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec');
my $count = 0;
foreach (sort { $b cmp $a } keys (%news))
{
  my ($title, $date_yyyy, $date_mm, $date_dd, $content) = @{$news{$_}};
  my $month = $months[$date_mm - 1];
  my $monthShort = $monthsShort[$date_mm - 1];
  my $date = "$month $date_dd, $date_yyyy";
  my $dateShort = "$date_dd&nbsp;$monthShort&nbsp;$date_yyyy";
  print "$title, $date\n";

  if (++$count <= 4)
  {
    print M <<EOF;
            <tr>
                <td valign="top" class="news-table-date"><b>$dateShort</b></td>
                <td><a href="news.html">$title</a></td>
            </tr>

EOF
  }

  print O <<EOF;
    <table cellspacing="0" class="news-article">
        <tr>
            <th class="news-article-header" >
                <b>$title</b>
                <br/>
                <i>Posted on $month $date_dd, $date_yyyy</i>
            </th>
        </tr>
        <tr>
            <td class="news-article-body">
$content
            </td>
        </tr>
    </table>

    <br/>

EOF
}

print O <<'EOF';
</div>

\endhtmlonly

*/
EOF

print M <<'EOF';
        </table>
EOF

close O;
close M;

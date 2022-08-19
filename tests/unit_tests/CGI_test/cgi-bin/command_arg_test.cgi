#!/usr/bin/perl

sub html {
    $str = $_[0];
    $str =~ s/&/&amp;/g;
    $str =~ s/</&lt;/g;
    $str =~ s/>/&gt;/g;
    $str =~ s/"/&quot;/g;
    $str =~ s/'/&#x27;/g;
    return $str;
}

print <<EOF;
Content-Type: text/html

<!doctype html>
<html>
<head>
<meta charset="utf-8">
<title>CGI TEST</title>
</head>
<body>
<h1>CGI TEST</h1>
<pre>
EOF

#
# コマンド引数を書き出します
#
print "===\n";
print "Command Arguments\n";
print "===\n";

$query_string = $ENV{'QUERY_STRING'};
@a = split(/\+/, $query_string);
# それぞれの「変数名=値」について
foreach $a (@a) {
	# + や %8A などをデコードします
	$a =~ tr/+/ /;
	$a =~ s/%([0-9a-fA-F][0-9a-fA-F])/pack("C", hex($1))/eg;
	# 変数名と値を書き出します
	print "$a\n";
}
print "\n";

print <<EOF;
</pre>
</body>
</html>
EOF

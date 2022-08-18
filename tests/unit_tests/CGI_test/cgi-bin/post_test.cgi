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

print "===\n";
print "Form Variable\n";
print "===\n";
if ($ENV{'REQUEST_METHOD'} eq "POST") {
	# POSTであれば標準入力から読込みます
	read(STDIN, $query_string, $ENV{'CONTENT_LENGTH'});
} else {
	# GETであれば環境変数から読込みます
	$query_string = $ENV{'QUERY_STRING'};
}
# 「変数名1=値1&変数名2=値2」の形式をアンパサンド( & )で分解します
@a = split(/&/, $query_string);
# それぞれの「変数名=値」について
foreach $a (@a) {
	# イコール( = )で分解します
	($name, $value) = split(/=/, $a);
	# + や %8A などをデコードします
	$value =~ tr/+/ /;
	$value =~ s/%([0-9a-fA-F][0-9a-fA-F])/pack("C", hex($1))/eg;
	# 変数名と値を書き出します
	print "$name = [ " . html($value) . " ]\n";
	# 後で使用する場合は、$FORM{'変数名'} に代入しておきます
	$FORM{$name} = $value;
}

print <<EOF;
</pre>
</body>
</html>
EOF

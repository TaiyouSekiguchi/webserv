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

for ($i = 0; $i <= $#ARGV; $i++) {
	print "ARGV[$i] = [ " . html($ARGV[$i]) . " ]\n";
}
print "\n";

print <<EOF;
</pre>
</body>
</html>
EOF

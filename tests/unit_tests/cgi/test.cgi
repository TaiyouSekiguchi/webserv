#!/usr/bin/perl

print "Content-type: text/html\n\n";
print "<html>\n<body>\n<div>";
print "Hello, $ENV{NAME}!! Welcome CGI test page!! ;)";
print "</div>\n</body>\n</html>";
